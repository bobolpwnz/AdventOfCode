#include <gtest/gtest.h>

#include "days/days.hpp"

namespace bblp::aoc::test {
TEST(Day1, test) {
    const auto result = day01();
    EXPECT_EQ("71300", result.first);
    EXPECT_EQ("209691", result.second);
}
};  // namespace bblp::aoc::test
