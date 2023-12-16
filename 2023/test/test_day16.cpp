#include <gtest/gtest.h>

#include "days.hpp"

namespace bblp::aoc::test {
TEST(Day16, test) {
    const auto result = day16();
    EXPECT_EQ("8323", result.first);
    EXPECT_EQ("8491", result.second);
}
};  // namespace bblp::aoc::test
