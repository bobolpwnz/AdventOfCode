#include <gtest/gtest.h>

#include "days.hpp"

namespace bblp::aoc::test {
TEST(Day20, test) {
    const auto result = day20();
    EXPECT_EQ("841763884", result.first);
    EXPECT_EQ("246006621493687", result.second);
}
};  // namespace bblp::aoc::test
