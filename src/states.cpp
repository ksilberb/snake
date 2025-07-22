#include "states.h"

auto GameStateManager::pushState(std::unique_ptr<GameState> state) -> void {
    if (!states_.empty()) {
        states_.top()->pause();
    }
    states_.push(std::move(state));
    states_.top()->enter();
}

auto GameStateManager::popState() -> void {
    if (!states_.empty()) {
        states_.top()->exit();
        states_.pop();
        if (!states_.empty()) {
            states_.top()->resume();
        }
    }
}

auto GameStateManager::changeState(std::unique_ptr<GameState> state) -> void {
    if (!states_.empty()) {
        states_.top()->exit();
        states_.pop();
    }
    states_.push(std::move(state));
    states_.top()->enter();
}

auto GameStateManager::clear() -> void{
    while (!states_.empty()) {
        states_.top()->exit();
        states_.pop();
    }
}

auto GameStateManager::handleInput(GLFWwindow* window, float deltaTime) -> void {
    if (!states_.empty()) {
        states_.top()->handleInput(window, deltaTime);
    }
}

auto GameStateManager::update(float deltaTime) -> void {
    if (!states_.empty()) {
        states_.top()->update(deltaTime);
    }
}

auto GameStateManager::render() -> void {
    if (!states_.empty()) {
        states_.top()->render();
    }
}

auto GameStateManager::isEmpty() const -> bool {
    return states_.empty();
}

auto GameStateManager::getCurrentState() const -> GameState* {
    return states_.empty() ? nullptr : states_.top().get();
}
