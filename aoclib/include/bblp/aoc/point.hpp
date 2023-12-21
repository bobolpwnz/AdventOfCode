#pragma once

#include <cstdint>

namespace bblp::aoc {
class Point {
  public:
    constexpr Point() : x(0), y(0) {}
    constexpr Point(const int64_t x, const int64_t y) : x(x), y(y) {}

    [[nodiscard]] bool operator==(const Point& other) const noexcept { return (x == other.x) && (y == other.y); }
    [[nodiscard]] bool operator<(const Point& other) const noexcept {
        return (x < other.x) || ((!(other.x < x)) && (y < other.y));
    }

    int64_t x;
    int64_t y;
};

struct PointHash {
    size_t operator()(const Point& point) const {
        return point.x >= point.y ? point.x * point.x + point.x + point.y : point.x + point.y * point.y;
    }
};
}  // namespace bblp::aoc
