#include <gtest/gtest.h>

#include "days.hpp"

namespace bblp::aoc::test {
TEST(Day8, test) {
    const auto result = day08();
    EXPECT_EQ("14681", result.first);
    EXPECT_EQ("14321394058031", result.second);
}
};  // namespace bblp::aoc::test
