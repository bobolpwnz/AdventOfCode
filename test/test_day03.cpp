#include <gtest/gtest.h>

#include "days/day03.hpp"

namespace bblp::advent_of_code_2022::test {
TEST(Day3, test) {
    const auto result = day03();
    EXPECT_EQ(8202, std::get<0>(result));
    EXPECT_EQ(2864, std::get<1>(result));
}
};  // namespace bblp::advent_of_code_2022::test
