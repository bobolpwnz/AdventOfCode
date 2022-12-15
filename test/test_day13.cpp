#include <gtest/gtest.h>

#include "days/days.hpp"

namespace bblp::advent_of_code_2022::test {
TEST(Day13, test) {
    const auto result = day13();
    EXPECT_EQ("5605", result.first);
    EXPECT_EQ("", result.second);
}
};  // namespace bblp::advent_of_code_2022::test
