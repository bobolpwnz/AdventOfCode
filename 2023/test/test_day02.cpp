#include <gtest/gtest.h>

#include "days.hpp"

namespace bblp::aoc::test {
TEST(Day2, test) {
    const auto result = day02();
    EXPECT_EQ("2164", result.first);
    EXPECT_EQ("69929", result.second);
}
};  // namespace bblp::aoc::test
