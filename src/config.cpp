#include "config.h"

GameConfig& GameConfig::getInstance() {
    static GameConfig instance;
    return instance;
}

int GameConfig::getWidth() const {
    return width_;
}

int GameConfig::getHeight() const {
    return height_;
}

float GameConfig::getAspectRatio() const {
    return static_cast<float>(width_) / static_cast<float>(height_);
}

void GameConfig::setDimensions(int width, int height) {
    width_ = width;
    height_ = height;
}

