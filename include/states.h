#pragma once
#include <memory>
#include <stack>
#include <GLFW/glfw3.h>

class GameState {
public:
    virtual ~GameState() = default;
    virtual auto enter() -> void {};
    virtual auto exit() -> void {};
    virtual auto pause() -> void {};
    virtual auto resume() -> void {};
    virtual auto handleInput(GLFWwindow* window, float deltaTime) -> void = 0;
    virtual auto update(float deltaTime) -> void = 0;
    virtual auto render() -> void = 0;
};

class GameStateManager {
public:
    auto pushState(std::unique_ptr<GameState> state) -> void;
    auto popState() -> void;
    auto changeState(std::unique_ptr<GameState> state) -> void;
    auto clear() -> void;
    auto handleInput(GLFWwindow* window, float deltaTime) -> void;
    auto update(float deltaTime) -> void;
    auto render() -> void;
    [[nodiscard]] auto isEmpty() const -> bool;
    [[nodiscard]] auto getCurrentState() const -> GameState*;
private:
    std::stack<std::unique_ptr<GameState>> states_;
};
