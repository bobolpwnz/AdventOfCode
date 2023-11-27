#include <gtest/gtest.h>

#include "days/days.hpp"

namespace bblp::advent_of_code_2022::test {
TEST(Day15, test) {
    const auto result = day15();
    EXPECT_EQ("5112034", result.first);
    EXPECT_EQ("13172087230812", result.second);
}
};  // namespace bblp::advent_of_code_2022::test
