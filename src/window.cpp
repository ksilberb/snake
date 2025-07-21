#include "window.h"
#include "config.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

Window::Window(int width, int height, std::string title)
    : title_(std::move(title)) {
    GameConfig::getInstance().setDimensions(width, height);
}

Window::~Window() {
    if (window_ != nullptr) {
        glfwDestroyWindow(window_);
    }
}

auto Window::init() -> int{
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

[[nodiscard]] auto Window::get() const -> GLFWwindow* {
    return window_;
}

[[nodiscard]] auto Window::shouldClose() const -> int {
    return glfwWindowShouldClose(window_);
}

auto Window::pollEvents() -> void {
    glfwPollEvents();
}

auto Window::setupCallbacks() -> void {
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
