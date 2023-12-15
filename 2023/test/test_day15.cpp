#include <gtest/gtest.h>

#include "days.hpp"

namespace bblp::aoc::test {
TEST(Day15, test) {
    const auto result = day15();
    EXPECT_EQ("505379", result.first);
    EXPECT_EQ("263211", result.second);
}
};  // namespace bblp::aoc::test
