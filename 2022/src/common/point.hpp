#pragma once

#include <cstdint>

namespace bblp::advent_of_code_2022 {
class Point {
  public:
    constexpr Point() : x(0), y(0) {}
    constexpr Point(const int64_t x, const int64_t y) : x(x), y(y) {}

    [[nodiscard]] bool operator==(const Point& other) const noexcept { return (x == other.x) && (y == other.y); }

    int64_t x;
    int64_t y;
};
}  // namespace bblp::advent_of_code_2022
