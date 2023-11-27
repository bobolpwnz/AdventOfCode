#include <gtest/gtest.h>

#include "days/days.hpp"

namespace bblp::advent_of_code_2022::test {
TEST(Day14, test) {
    const auto result = day14();
    EXPECT_EQ("665", result.first);
    EXPECT_EQ("25434", result.second);
}
};  // namespace bblp::advent_of_code_2022::test
