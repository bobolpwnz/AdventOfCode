#include <gtest/gtest.h>

#include "days/days.hpp"

namespace bblp::advent_of_code_2022::test {
TEST(Day5, test) {
    const auto result = day05();
    EXPECT_EQ("BZLVHBWQF", result.first);
    EXPECT_EQ("TDGJQTZSL", result.second);
}
};  // namespace bblp::advent_of_code_2022::test
