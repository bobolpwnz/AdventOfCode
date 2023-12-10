#include <gtest/gtest.h>

#include "days.hpp"

namespace bblp::aoc::test {
TEST(Day9, test) {
    const auto result = day09();
    EXPECT_EQ("1743490457", result.first);
    EXPECT_EQ("1053", result.second);
}
};  // namespace bblp::aoc::test
