#include <gtest/gtest.h>

#include "days.hpp"

namespace bblp::aoc::test {
TEST(Day2, test) {
    const auto result = day02();
    EXPECT_EQ("299", result.first);
    EXPECT_EQ("364", result.second);
}
};  // namespace bblp::aoc::test
