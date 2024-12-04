#include <gtest/gtest.h>

#include "days.hpp"

namespace bblp::aoc::test {
TEST(Day4, test) {
    const auto result = day04();
    EXPECT_EQ("2654", result.first);
    EXPECT_EQ("1990", result.second);
}
};  // namespace bblp::aoc::test
