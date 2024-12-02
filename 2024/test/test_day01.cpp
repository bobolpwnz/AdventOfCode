#include <gtest/gtest.h>

#include "days.hpp"

namespace bblp::aoc::test {
TEST(Day1, test) {
    const auto result = day01();
    EXPECT_EQ("1941353", result.first);
    EXPECT_EQ("22539317", result.second);
}
};  // namespace bblp::aoc::test
