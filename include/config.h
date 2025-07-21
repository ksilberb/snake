#pragma once
class GameConfig {
public:
    static GameConfig& getInstance();
    int getWidth() const;
    int getHeight() const;
    float getAspectRatio() const;
    void setDimensions(int width, int height);

private:
    GameConfig() = default;
    int width_ = 800;
    int height_ = 800;
};
