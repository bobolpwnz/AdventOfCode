#include <gtest/gtest.h>

#include "days/days.hpp"

namespace bblp::aoc::test {
TEST(Day4, test) {
    const auto result = day04();
    EXPECT_EQ("511", result.first);
    EXPECT_EQ("821", result.second);
}
};  // namespace bblp::aoc::test
