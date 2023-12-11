#include <gtest/gtest.h>

#include "days.hpp"

namespace bblp::aoc::test {
TEST(Day11, test) {
    const auto result = day11();
    EXPECT_EQ("9693756", result.first);
    EXPECT_EQ("717878258016", result.second);
}
};  // namespace bblp::aoc::test
