#include <gtest/gtest.h>

#include "days/days.hpp"

namespace bblp::aoc::test {
TEST(Day8, test) {
    const auto result = day08();
    EXPECT_EQ("1705", result.first);
    EXPECT_EQ("371200", result.second);
}
};  // namespace bblp::aoc::test
