#pragma once
class GameConfig {
public:
    static auto getInstance() -> GameConfig&;
    [[nodiscard]] auto getWidth() const -> int;
    [[nodiscard]] auto getHeight() const -> int;
    [[nodiscard]] auto getAspectRatio() const -> float;
    auto setDimensions(int width, int height) -> void;

private:
    GameConfig() = default;
    int width_ = 800;
    int height_ = 800;
};

