#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "bblp/aoc/point.hpp"

namespace bblp::aoc {

template <typename TileType, typename DimensionType = int64_t>
class Grid {
  public:
    Grid(const int64_t width, const DimensionType height, TileType val)
        : mWidth(width), mHeight(height), mTiles(width * height, val) {}

    Grid(const int64_t width, const DimensionType height, std::vector<TileType> tiles)
        : mWidth(width), mHeight(height), mTiles(std::move(tiles)) {}

    [[nodiscard]] inline DimensionType width() const { return mWidth; }
    [[nodiscard]] inline DimensionType height() const { return mHeight; }

    [[nodiscard]] inline TileType at(const Point& point) const { return at(point.x, point.y); }
    [[nodiscard]] inline TileType at(const DimensionType x, const DimensionType y) const {
        return mTiles.at(y * mWidth + x);
    }
    [[nodiscard]] inline TileType& at(const Point& point) { return at(point.x, point.y); }
    [[nodiscard]] inline TileType& at(const DimensionType x, const DimensionType y) {
        return mTiles.at(y * mWidth + x);
    }

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
    void set(const DimensionType x, const DimensionType y, TileType value) { mTiles[y * mWidth + x] = value; }

  private:
    DimensionType mWidth;
    DimensionType mHeight;
    std::vector<TileType> mTiles;
};
}  // namespace bblp::aoc
