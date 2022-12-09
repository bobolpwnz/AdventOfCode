#include <gtest/gtest.h>

#include "days/days.hpp"

namespace bblp::advent_of_code_2022::test {
TEST(Day9, test) {
    const auto result = day09();
    EXPECT_EQ("6337", result.first);
    EXPECT_EQ("2455", result.second);
}
};  // namespace bblp::advent_of_code_2022::test
