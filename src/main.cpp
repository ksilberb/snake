#include "window.h"
#include "config.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>

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
    Window game(WIDTH, HEIGHT, "Snake Game");
    if (game.init() != GLFW_TRUE) {
        return -1;
    }

    while (game.shouldClose() != GLFW_TRUE) {
        glClear(GL_COLOR_BUFFER_BIT);
        drawSquare(0.0f, 0.0f, 0.1f);
        glfwSwapBuffers(game.get());
        Window::pollEvents();
    }
    glfwTerminate();
    return 0;
}
