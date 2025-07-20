#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace snake {
    int WIDTH = 640;
    int HEIGHT = 480;
    float ASPECT_RATIO = (float)WIDTH / (float)HEIGHT;
}

void drawSquare(float x, float y, float l) {
    float xOffset = l / snake::ASPECT_RATIO;
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + xOffset, y);
    glVertex2f(x + xOffset, y - l);
    glVertex2f(x, y - l);
    glEnd();
}

// create a callback for when the window resizes

int main() {
    GLFWwindow* window;
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW\n" << std::endl;
        return -1;
    }

    window = glfwCreateWindow(snake::WIDTH, snake::HEIGHT, "Snake", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        drawSquare(0.0f, 0.0f, 0.1f);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
