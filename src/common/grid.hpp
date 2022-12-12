#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "common/point.hpp"

namespace bblp::advent_of_code_2022 {

template <typename TileType>
class Grid {
  public:
    Grid(const int32_t width, const int32_t height, TileType val)
        : mWidth(width), mHeight(height), mTiles(width * height, val) {}

    Grid(const int32_t width, const int32_t height, std::vector<TileType> tiles)
        : mWidth(width), mHeight(height), mTiles(std::move(tiles)) {}

    [[nodiscard]] inline int32_t width() const { return mWidth; }
    [[nodiscard]] inline int32_t height() const { return mHeight; }

    [[nodiscard]] inline TileType at(const Point& point) const { return at(point.x, point.y); }
    [[nodiscard]] inline TileType at(const int32_t x, const int32_t y) const { return mTiles.at(y * mWidth + x); }

    [[nodiscard]] inline std::optional<Point> find(const TileType value) const {
        for (auto x = 0; x < mWidth; ++x) {
            for (auto y = 0; y < mHeight; ++y) {
                if (at(x, y) == value) {
                    return Point(x, y);
                }
            }
        }
        return {};
    }

    void set(const Point& point, TileType value) { set(point.x, point.y, value); }
    void set(const int32_t x, const int32_t y, TileType value) { mTiles[y * mWidth + x] = value; }

  private:
    int32_t mWidth;
    int32_t mHeight;
    std::vector<TileType> mTiles;
};
}  // namespace bblp::advent_of_code_2022
