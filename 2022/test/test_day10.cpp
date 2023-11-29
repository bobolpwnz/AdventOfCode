#include <gtest/gtest.h>

#include "days/days.hpp"

namespace bblp::aoc::test {
TEST(Day10, test) {
    static constexpr bool printOutput = false;

    const auto result = day10(printOutput);
    EXPECT_EQ("13720", result.first);
    EXPECT_EQ(
        "####.###..#..#.###..#..#.####..##..#..#.#....#..#.#..#.#..#.#..#....#.#..#.#..#.###..###..#..#.#..#.####...#.."
        "#....####.#....#..#.#..#.###..#..#..#...#....#..#.#....#..#.#..#.#.#..#..#.#....#..#.#..#.#....###...##..#..#."
        "#..#.####..##..#..#.",
        result.second);
}
};  // namespace bblp::aoc::test
