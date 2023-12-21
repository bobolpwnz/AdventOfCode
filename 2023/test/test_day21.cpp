#include <gtest/gtest.h>

#include "days.hpp"

namespace bblp::aoc::test {
TEST(Day21, test) {
    const auto result = day21();
    EXPECT_EQ("3682", result.first);
    EXPECT_EQ("0", result.second);
}
};  // namespace bblp::aoc::test
