#include <gtest/gtest.h>

#include "days/day02.hpp"

namespace bblp::advent_of_code_2022::test {
TEST(Day2, test) {
    const auto result = day02();
    EXPECT_EQ(15523, std::get<0>(result));
    EXPECT_EQ(15702, std::get<1>(result));
}
};  // namespace bblp::advent_of_code_2022::test
