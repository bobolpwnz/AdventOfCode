#pragma once

#include "bblp/aoc/point.hpp"

#include <ostream>

namespace bblp::aoc {
std::ostream& operator<<(std::ostream& stream, const Point& point) {
    stream << "x=" << point.x << ";y=" << point.y;
    return stream;
}
}  // namespace bblp::aoc
