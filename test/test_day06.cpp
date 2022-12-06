#include <gtest/gtest.h>

#include "days/days.hpp"

namespace bblp::advent_of_code_2022::test {
TEST(Day6, test) {
    const auto result = day06();
    EXPECT_EQ("1282", result.first);
    EXPECT_EQ("3513", result.second);
}
};  // namespace bblp::advent_of_code_2022::test
