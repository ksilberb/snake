#include "config.h"

auto GameConfig::getInstance() -> GameConfig& {
    static GameConfig instance;
    return instance;
}

auto GameConfig::getWidth() const -> int {
    return width_;
}

auto GameConfig::getHeight() const -> int {
    return height_;
}

auto GameConfig::getAspectRatio() const -> float{
    return static_cast<float>(width_) / static_cast<float>(height_);
}

auto GameConfig::setDimensions(int width, int height) -> void {
    width_ = width;
    height_ = height;
}

