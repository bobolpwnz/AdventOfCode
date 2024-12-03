#include <gtest/gtest.h>

#include "days.hpp"

namespace bblp::aoc::test {
TEST(Day3, test) {
    const auto result = day03();
    EXPECT_EQ("166357705", result.first);
    EXPECT_EQ("88811886", result.second);
}
};  // namespace bblp::aoc::test
