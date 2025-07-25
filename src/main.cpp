#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <stack>
#include <memory>
#include <random>

class GameConfig {
public:
    using AspectRatioCallback = std::function<void (float)>;

    static auto getInstance() -> GameConfig& {
        static GameConfig instance;
        return instance;
    }

    [[nodiscard]] auto getWidth() const -> int {
        return width_;
    }

    [[nodiscard]] auto getHeight() const -> int {
        return height_;
    }

    [[nodiscard]] auto getNumGridCells() const -> int {
        return numGridCells_;
    }

    [[nodiscard]] auto getAspectRatio() const -> float {
        return static_cast<float>(width_) / static_cast<float>(height_);
    }

    auto setDimensions(int width, int height) -> void {
        if (width_ != width || height_ != height) {
            width_ = width;
            height_ = height;
            float newAspectRatio = getAspectRatio();
            for (const auto& callback : aspectRatioCallbacks_) {
                callback(newAspectRatio);
            }
        }
    }

    auto registerAspectRatioCallback(AspectRatioCallback callback) -> void {
        aspectRatioCallbacks_.push_back(std::move(callback));
    }

private:
    GameConfig() = default;
    int width_;
    int height_;
    const int numGridCells_ = 30;
    std::vector<AspectRatioCallback> aspectRatioCallbacks_;
};

class Window {
public:
    Window(std::string title) : title_(std::move(title)) {}

    ~Window() {
        if (window_ != nullptr) {
            glfwDestroyWindow(window_);
        }
    }

    auto init() -> int {
        if (glfwInit() != GLFW_TRUE) {
            std::cerr << "Failed to initialize GLFW\n";
            return GLFW_FALSE;
        }
        window_ = glfwCreateWindow(
            GameConfig::getInstance().getWidth(),
            GameConfig::getInstance().getHeight(),
            title_.c_str(),
            nullptr, nullptr);
        if (window_ == nullptr) {
            std::cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            return GLFW_FALSE;
        }
        glfwMakeContextCurrent(window_);
        setupCallbacks();
        return GLFW_TRUE;
    }

    [[nodiscard]] auto get() const ->GLFWwindow* {
        return window_;
    }

    [[nodiscard]] auto shouldClose() const -> int {
        return glfwWindowShouldClose(window_);
    }

    static auto pollEvents() -> void {
        glfwPollEvents();
    }

private:
    auto setupCallbacks() -> void {
        glfwSetWindowUserPointer(window_, this);
        glfwSetFramebufferSizeCallback(
            window_,
            []([[ maybe_unused ]] GLFWwindow* window, int width, int height) {
                glViewport(0, 0, width, height);
                GameConfig::getInstance().setDimensions(width, height);
            });
        glfwSetWindowSizeCallback(
            window_,
            []([[ maybe_unused ]] GLFWwindow* window, int width, int height) {
                GameConfig::getInstance().setDimensions(width, height);
            });
    }

    GLFWwindow* window_ = nullptr;
    std::string title_;
};

class Cell {
public:
    const float windowLength = 2.0F;
    Cell(float xPos, float yPos) : x_(xPos), y_(yPos) {
        Cell::length_ = windowLength / GameConfig::getInstance().getNumGridCells();
        static bool callbackRegistered = false;
        if (!callbackRegistered) {
            GameConfig::getInstance().registerAspectRatioCallback([](float newAspectRatio) {
                Cell::updateOffset(newAspectRatio);
            });
            callbackRegistered = true;
            Cell::updateOffset(GameConfig::getInstance().getAspectRatio());
        }
    }

    ~Cell() = default;

    auto draw() const -> void {
        glBegin(GL_QUADS);
        glVertex2f(x_, y_);
        glVertex2f(x_ + offset_, y_);
        glVertex2f(x_ + offset_, y_ - length_);
        glVertex2f(x_, y_ - length_);
        glEnd();
    }

    auto update(float xPos, float yPos) -> void {
        x_ = xPos;
        y_ = yPos;
    }

    auto getCellPos() -> std::vector<float> {
        return {x_, y_};
    }

    static inline float length_ = 0.0F;

private:
    static auto updateOffset(float aspectRatio) -> void {
        offset_ = length_ / aspectRatio;
    }

    float x_;
    float y_;
    static inline float offset_ = 0.0F;
};

class Snake {
public:
    enum class Direction : uint8_t {NORTH, EAST, SOUTH, WEST};

    Snake(float xInit, float yInit) : headPos_{xInit, yInit} {
        body_.emplace_back(headPos_[0], headPos_[1]);
        timeSinceLastMove = 0.0F;
        currentDirection_ = Direction::EAST;
    }

    ~Snake() = default;

    static auto turnRight(Direction dir) -> Direction {
        switch (dir) {
        case Direction::NORTH : return Direction::EAST;
        case Direction::EAST : return Direction::SOUTH;
        case Direction::SOUTH : return Direction::WEST;
        case Direction::WEST : return Direction::NORTH;
        }
        return Direction::EAST;
    }

    static auto turnLeft(Direction dir) -> Direction {
        switch (dir) {
        case Direction::NORTH : return Direction::WEST;
        case Direction::WEST : return Direction::SOUTH;
        case Direction::SOUTH : return Direction::EAST;
        case Direction::EAST : return Direction::NORTH;
        }
        return Direction::EAST;
    }

    auto changeCurrentDirection(Direction dir) -> void {
        currentDirection_  = dir;
    }

    auto getCurrentDirection() -> Direction {
        return currentDirection_;
    }

    auto getHeadPos() -> std::vector<float> {
        return headPos_;
    }

    auto draw() -> void {
        for (const Cell& cell : body_) {
            cell.draw();
        }
    }

    auto update(float deltaTime) -> void {
        timeSinceLastMove += deltaTime;
        // updating the direction of the snake head
        if (timeSinceLastMove >= speed_) {
            std::array<float, 2> direction = Snake::getDirection(currentDirection_);
            headPos_[0] += direction[0] * Cell::length_;
            headPos_[1] += direction[1] * Cell::length_;
            timeSinceLastMove = 0.0F;
            body_[0].update(headPos_[0], headPos_[1]);
        }
    }

private:
    static auto getDirection(Direction dir) -> std::array<float, 2> {
        switch (dir) {
        case Direction::NORTH : return {0.0F, 1.0F};
        case Direction::EAST : return {1.0F, 0.0F};
        case Direction::SOUTH : return {0.0F, -1.0F};
        case Direction::WEST : return {-1.0F, 0.0F};
        }
        return {1.0F, 0.0F};
    }

    std::vector<float> headPos_;
    std::vector<Cell> body_;
    float speed_ = 1.0F;
    float timeSinceLastMove;
    Direction currentDirection_;
};

class GameState {
public:
    virtual ~GameState() = default;
    virtual auto enter() -> void {}
    virtual auto exit() -> void {}
    virtual auto pause() -> void {}
    virtual auto resume() -> void {}
    virtual auto handleInput(GLFWwindow* window, float deltaTime) -> void = 0;
    virtual auto update(float deltaTime) -> void = 0;
    virtual auto render() -> void = 0;
};

class GameStateManager {
public:
    auto pushState(std::unique_ptr<GameState> state) -> void {
        if (!states_.empty()) {
            states_.top()->pause();
        }
        states_.push(std::move(state));
        states_.top()->enter();
    }

    auto popState() -> void {
        if (!states_.empty()) {
            states_.top()->exit();
            states_.pop();
            if (!states_.empty()) {
                states_.top()->resume();
            }
        }
    }

    auto changeState(std::unique_ptr<GameState> state) -> void {
        if (!states_.empty()) {
            states_.top()->exit();
            states_.pop();
        }
        states_.push(std::move(state));
        states_.top()->enter();
    }

    auto clear() -> void {
        while (!states_.empty()) {
            states_.top()->exit();
            states_.pop();
        }
    }

    auto handleInput(GLFWwindow* window, float deltaTime) -> void {
        if (!states_.empty()) {
            states_.top()->handleInput(window, deltaTime);
        }
    }

    auto update(float deltaTime) -> void {
        if (!states_.empty()) {
            states_.top()->update(deltaTime);
        }
    }

    auto render() -> void {
        if (!states_.empty()) {
            states_.top()->render();
        }
    }

    [[nodiscard]] auto isEmpty() const -> bool {
        return states_.empty();
    }

    [[nodiscard]] auto getCurrentState() const -> GameState* {
        return states_.empty() ? nullptr : states_.top().get();
    }

private:
    std::stack<std::unique_ptr<GameState>> states_;
};

class Playing : public GameState {
public:
    Playing() : food_(0.0F, 0.0F) {
    }

    auto handleInput(GLFWwindow* window, [[maybe_unused]] float deltaTime) -> void override {
        keystates[0] = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
        keystates[1] = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
        keystates[2] = (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);
        if (keystates[0] && !dAction_) {
            std::cout << "D pressed \n";
            snake_.changeCurrentDirection(Snake::turnRight(snake_.getCurrentDirection()));
        }
        dAction_ = keystates[0];
        if (keystates[1] && !aAction_) {
            std::cout << "A pressed \n";
            snake_.changeCurrentDirection(Snake::turnLeft(snake_.getCurrentDirection()));
        }
        aAction_ = keystates[1];
        if (keystates[2] && !escapeAction_) {
            std::cout << "Escape pressed \n";
        }
        escapeAction_ = keystates[2];
    }

    auto update([[maybe_unused]] float deltaTime) -> void override {
        snake_.update(deltaTime);
        if (foodEaten(snake_.getHeadPos(), food_.getCellPos())) {
            std::cout << "food eaten!\n";
        }
    }

    auto render() -> void override {
        glClear(GL_COLOR_BUFFER_BIT);
        snake_.draw();
        food_.draw();
    }

private:
    static auto foodEaten(std::vector<float> headPosition, std::vector<float> cellPosition) -> bool {
        return cellPosition[0] == headPosition[0] && cellPosition[1] == headPosition[1];
    }

    Snake snake_ = Snake(-1.0F, 1.0F);
    Cell food_;
    std::vector<bool> keystates = {false, false, false};
    bool dAction_ = false;
    bool aAction_ = false;
    bool escapeAction_ = false;
};

auto main() -> int {
    const int width = 800;
    const int height = 800;
    GameConfig::getInstance().setDimensions(width, height);
    Window win("Snake Game");

    if (win.init() != GLFW_TRUE) {
        return -1;
    }
    GameStateManager game;
    game.pushState(std::make_unique<Playing>());

    double lastTime = glfwGetTime();
    while (win.shouldClose() != GLFW_TRUE) {
        double currentTime = glfwGetTime();
        auto deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;
        game.handleInput(win.get(), deltaTime);
        game.update(deltaTime);
        game.render();
        glfwSwapBuffers(win.get());
        Window::pollEvents();
    }

    glfwTerminate();
    return 0;
}
