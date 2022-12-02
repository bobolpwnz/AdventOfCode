#include <gtest/gtest.h>

#include "days/day01.hpp"

namespace bblp::advent_of_code_2022::test {
TEST(Day1, test) {
    const auto result = day01();
    EXPECT_EQ(71300, std::get<0>(result));
    EXPECT_EQ(209691, std::get<1>(result));
}
};  // namespace bblp::advent_of_code_2022::test
