#include <gtest/gtest.h>

#include "days.hpp"

namespace bblp::aoc::test {
TEST(Day10, test) {
    const auto result = day10();
    EXPECT_EQ("6886", result.first);
    EXPECT_EQ("371", result.second);
}
};  // namespace bblp::aoc::test
