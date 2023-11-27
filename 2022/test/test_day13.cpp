#include <gtest/gtest.h>

#include "days/days.hpp"

namespace bblp::advent_of_code_2022::test {
TEST(Day13, test) {
    const auto result = day13(false);
    EXPECT_EQ("5605", result.first);
    EXPECT_EQ("24969", result.second);
}
};  // namespace bblp::advent_of_code_2022::test
