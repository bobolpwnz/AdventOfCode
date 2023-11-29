#include <gtest/gtest.h>

#include "days/days.hpp"

namespace bblp::aoc::test {
TEST(Day7, test) {
    const auto result = day07();
    EXPECT_EQ("1642503", result.first);
    EXPECT_EQ("6999588", result.second);
}
};  // namespace bblp::aoc::test
