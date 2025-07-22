#include "config.h"
#include "window.h"
#include "states.h"
#include "playing.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <memory>

void drawSquare(float x, float y, float l) {
    float xOffset = l / GameConfig::getInstance().getAspectRatio();
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + xOffset, y);
    glVertex2f(x + xOffset, y - l);
    glVertex2f(x, y - l);
    glEnd();
}

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
