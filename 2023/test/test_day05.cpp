#include <gtest/gtest.h>

#include "days.hpp"

namespace bblp::aoc::test {
TEST(Day5, test) {
    const auto result = day05();
    EXPECT_EQ("175622908", result.first);
    EXPECT_EQ("5200543", result.second);
}
};  // namespace bblp::aoc::test
