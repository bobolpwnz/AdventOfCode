#include <gtest/gtest.h>

#include "days.hpp"

namespace bblp::aoc::test {
TEST(Day3, test) {
    const auto result = day03();
    EXPECT_EQ("529618", result.first);
    EXPECT_EQ("77509019", result.second);
}
};  // namespace bblp::aoc::test
