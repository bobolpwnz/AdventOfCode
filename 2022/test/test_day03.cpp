#include <gtest/gtest.h>

#include "days/days.hpp"

namespace bblp::aoc::test {
TEST(Day3, test) {
    const auto result = day03();
    EXPECT_EQ("8202", result.first);
    EXPECT_EQ("2864", result.second);
}
};  // namespace bblp::aoc::test
