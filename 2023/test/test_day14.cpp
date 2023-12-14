#include <gtest/gtest.h>

#include "days.hpp"

namespace bblp::aoc::test {
TEST(Day14, test) {
    const auto result = day14();
    EXPECT_EQ("108955", result.first);
    EXPECT_EQ("106689", result.second);
}
};  // namespace bblp::aoc::test
