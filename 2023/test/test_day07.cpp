#include <gtest/gtest.h>

#include "days.hpp"

namespace bblp::aoc::test {
TEST(Day7, test) {
    const auto result = day07();
    EXPECT_EQ("253313241", result.first);
    EXPECT_EQ("253362743", result.second);
}
};  // namespace bblp::aoc::test
