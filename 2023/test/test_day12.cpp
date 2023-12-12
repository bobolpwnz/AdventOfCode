#include <gtest/gtest.h>

#include "days.hpp"

namespace bblp::aoc::test {
TEST(Day12, test) {
    const auto result = day12();
    EXPECT_EQ("7753", result.first);
    EXPECT_EQ("0", result.second);
}
};  // namespace bblp::aoc::test
