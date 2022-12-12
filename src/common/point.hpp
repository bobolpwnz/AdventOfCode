#pragma once

#include <cstdint>

namespace bblp::advent_of_code_2022 {
class Point {
  public:
    Point() : x(0), y(0) {}
    Point(const int32_t x, const int32_t y) : x(x), y(y) {}

    [[nodiscard]] bool operator==(const Point& other) const noexcept { return (x == other.x) && (y == other.y); }

    int32_t x;
    int32_t y;
};
}  // namespace bblp::advent_of_code_2022
