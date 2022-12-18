#include <gtest/gtest.h>

#include "days/days.hpp"

namespace bblp::advent_of_code_2022::test {
TEST(Day16, test) {
    const auto result = day16();
    EXPECT_EQ("1559", result.first);
    EXPECT_EQ("", result.second);
}
};  // namespace bblp::advent_of_code_2022::test
