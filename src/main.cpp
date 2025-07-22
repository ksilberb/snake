#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <stack>
#include <memory>

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
    int width_ = 800;
    int height_ = 800;
    int numGridCells_ = 30;
    std::vector<AspectRatioCallback> aspectRatioCallbacks_;
};

class Window {
public:
    Window(int width, int height, std::string title) : title_(std::move(title)) {
        GameConfig::getInstance().setDimensions(width, height);
    }

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
    Cell(float xPos, float yPos) : x_(xPos), y_(yPos) {
        Cell::length_ = 2.0F / GameConfig::getInstance().getNumGridCells();
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

private:
    static auto updateOffset(float aspectRatio) -> void {
        offset_ = length_ / aspectRatio;
    }

    float x_;
    float y_;
    static inline float length_ = 0.0F;
    static inline float offset_ = 0.0F;
};

class Snake {
public:
    Snake(float xInit, float yInit) : headPos_{xInit, yInit} {
        body_.emplace_back(headPos_[0], headPos_[1]);
    }

    auto draw() -> void {
        for (const Cell& cell : body_) {
            cell.draw();
        }
    }

    auto update(float deltaTime) -> void {
        // need to define an accumulator that will add up the delta time up to a threshold.
        // move the snake head 1 block over in cardinal directions every 1.0 seconds.
    }

private:
    std::vector<float> headPos_;
    std::vector<Cell> body_;
    float speed_ = 0.0F;
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
    auto enter() -> void override {
        // initialize game
    }

    auto exit() -> void override {
        // clean up
    }

    auto handleInput(GLFWwindow* window, [[maybe_unused]] float deltaTime) -> void override {
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            // MOVE RIGHT
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            // MOVE LEFT
        }
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            // EXIT GAME
        }
    }

    auto update([[maybe_unused]] float deltaTime) -> void override {
        snake.update(deltaTime);
        // check collisions
        // spawn food
    }

    auto render() -> void override {
        glClear(GL_COLOR_BUFFER_BIT);
        snake.draw();
    }

private:
    Snake snake = Snake(-1.0F, 1.0F);
};

auto main() -> int {
    const int WIDTH = 600;
    const int HEIGHT = 600;
    Window win(WIDTH, HEIGHT, "Snake Game");

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
