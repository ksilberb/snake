#pragma once
#include "states.h"

class Playing : public GameState {
public:
    auto enter() -> void override;
    auto exit() -> void override;
    auto handleInput(GLFWwindow* window, [[maybe_unused]] float deltaTime) -> void override;
    auto update([[maybe_unused]] float deltaTime) -> void override;
    auto render() -> void override;
};
