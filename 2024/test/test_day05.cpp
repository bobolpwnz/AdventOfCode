#include <gtest/gtest.h>

#include "days.hpp"

namespace bblp::aoc::test {
TEST(Day5, test) {
    const auto result = day05();
    EXPECT_EQ("5091", result.first);
    EXPECT_EQ("4681", result.second);
}
};  // namespace bblp::aoc::test
