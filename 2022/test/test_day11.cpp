#include <gtest/gtest.h>

#include "days/days.hpp"

namespace bblp::aoc::test {
TEST(Day11, test) {
    const auto result = day11();
    EXPECT_EQ("95472", result.first);
    EXPECT_EQ("17926061332", result.second);
}
};  // namespace bblp::aoc::test
