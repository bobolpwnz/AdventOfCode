#include <gtest/gtest.h>

#include "days/days.hpp"

namespace bblp::aoc::test {
TEST(Day14, test) {
    const auto result = day14();
    EXPECT_EQ("665", result.first);
    EXPECT_EQ("25434", result.second);
}
};  // namespace bblp::aoc::test
