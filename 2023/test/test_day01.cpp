#include <gtest/gtest.h>

#include "days.hpp"

namespace bblp::aoc::test {
TEST(Day1, test) {
    const auto result = day01();
    EXPECT_EQ("54697", result.first);
    EXPECT_EQ("54885", result.second);
}
};  // namespace bblp::aoc::test
