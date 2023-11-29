#include <gtest/gtest.h>

#include "days/days.hpp"

namespace bblp::aoc::test {
TEST(Day6, test) {
    const auto result = day06();
    EXPECT_EQ("1282", result.first);
    EXPECT_EQ("3513", result.second);
}
};  // namespace bblp::aoc::test
