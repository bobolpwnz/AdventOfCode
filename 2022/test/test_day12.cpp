#include <gtest/gtest.h>

#include "days/days.hpp"

namespace bblp::aoc::test {
TEST(Day12, test) {
    const auto result = day12();
    EXPECT_EQ("462", result.first);
    EXPECT_EQ("451", result.second);
}
};  // namespace bblp::aoc::test
