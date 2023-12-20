#include <gtest/gtest.h>

#include "days.hpp"

namespace bblp::aoc::test {
TEST(Day19, test) {
    const auto result = day19();
    EXPECT_EQ("263678", result.first);
    EXPECT_EQ("125455345557345", result.second);
}
};  // namespace bblp::aoc::test
