#include <gtest/gtest.h>

#include "days/days.hpp"

namespace bblp::aoc::test {
TEST(Day2, test) {
    const auto result = day02();
    EXPECT_EQ("15523", result.first);
    EXPECT_EQ("15702", result.second);
}
};  // namespace bblp::aoc::test
