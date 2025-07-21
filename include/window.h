#pragma once
#include <string>
struct GLFWwindow;
class Window {
public:
    Window(int width, int height, std::string title);
    ~Window();
    auto init() -> int;
    [[nodiscard]] auto get() const ->GLFWwindow*;
    [[nodiscard]] auto shouldClose() const -> int;
    static auto pollEvents() -> void;

private:
    auto setupCallbacks() -> void;
    GLFWwindow* window_ = nullptr;
    std::string title_;
};
