#include "playing.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>

auto Playing::enter() -> void {
    // initialize game
}

auto Playing::exit() -> void {
    // clean up
}

auto Playing::handleInput(GLFWwindow* window, [[maybe_unused]]float deltaTime) -> void {
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

auto Playing::update([[maybe_unused]] float deltaTime) -> void {
    // update snake position
    // check collisions
    // spawn food
}

auto Playing::render() -> void {
    glClear(GL_COLOR_BUFFER_BIT);

    // draw snake
}

