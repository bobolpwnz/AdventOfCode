#include <gtest/gtest.h>

#include "days.hpp"

namespace bblp::aoc::test {
TEST(Day6, test) {
    const auto result = day06();
    EXPECT_EQ("2374848", result.first);
    EXPECT_EQ("39132886", result.second);
}
};  // namespace bblp::aoc::test
