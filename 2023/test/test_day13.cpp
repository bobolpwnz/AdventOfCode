#include <gtest/gtest.h>

#include "days.hpp"

namespace bblp::aoc::test {
TEST(Day13, test) {
    const auto result = day13();
    EXPECT_EQ("33735", result.first);
    EXPECT_EQ("38063", result.second);
}
};  // namespace bblp::aoc::test
