#include <gtest/gtest.h>

#include "days/days.hpp"

namespace bblp::advent_of_code_2022::test {
TEST(Day4, test) {
    const auto result = day04();
    EXPECT_EQ(511, std::get<0>(result));
    EXPECT_EQ(821, std::get<1>(result));
}
};  // namespace bblp::advent_of_code_2022::test
