#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"
#include "bblp/aoc/string_utils.hpp"
#include "bblp/aoc/point.hpp"

#include <algorithm>
#include <cstdlib>
#include <numeric>
#include <regex>
#include <vector>
#include <iostream>

namespace bblp::aoc {
namespace {

auto parse(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    std::vector<std::string> input;
    input.reserve(BUFFER_SIZE);
    const auto lineCallback = [&input](const std::string& line) { input.push_back(line); };
    parseInput(filePath, lineCallback);
    return input;
}

bool checkInDirection(const std::vector<std::string>& input, const Point center, int xDiff, int yDiff) {
    if (center.x + 3 * xDiff < 0 || center.x + 3 * xDiff >= input.at(center.y).size()) {
        return false;
    }

    if (center.y + 3 * yDiff < 0 || center.y + 3 * yDiff >= input.size()) {
        return false;
    }

    return (input.at(center.y + yDiff).at(center.x + xDiff) == 'M' &&
            input.at(center.y + 2 * yDiff).at(center.x + 2 * xDiff) == 'A' &&
            input.at(center.y + 3 * yDiff).at(center.x + 3 * xDiff) == 'S');
}

int calculatePartOne(const std::vector<std::string>& input) {
    uint32_t count = 0U;
    for (int y = 0; y < input.size(); ++y) {
        for (int x = 0; x < input.at(y).size(); ++x) {
            if (input.at(y).at(x) != 'X') {
                continue;
            }


            if (checkInDirection(input, {x, y}, -1, -1)) {
                count++;
            }

            if (checkInDirection(input, {x, y}, 0, -1)) {
                count++;
            }

            if (checkInDirection(input, {x, y}, 1, -1)) {
                count++;
            }

            if (checkInDirection(input, {x, y}, 1, 0)) {
                count++;
            }

            if (checkInDirection(input, {x, y}, 1, 1)) {
                count++;
            }

            if (checkInDirection(input, {x, y}, 0, 1)) {
                count++;
            }

            if (checkInDirection(input, {x, y}, -1, 1)) {
                count++;
            }

            if (checkInDirection(input, {x, y}, -1, 0)) {
                count++;
            }
        }
    }
    return count;
}

int calculatePartTwo(const std::vector<std::string>& input) {
    uint32_t count = 0U;
    for (int y = 1; y < input.size() - 1; ++y) {
        for (int x = 1; x < input.at(y).size() - 1; ++x) {
            if (input.at(y).at(x) != 'A') {
                continue;
            }

            bool const crossForward = (input.at(y - 1).at(x - 1) == 'M' && input.at(y + 1).at(x + 1) == 'S') ||
                                      (input.at(y - 1).at(x - 1) == 'S' && input.at(y + 1).at(x + 1) == 'M');
            bool const crossBackward = (input.at(y - 1).at(x + 1) == 'M' && input.at(y + 1).at(x - 1) == 'S') ||
                                      (input.at(y - 1).at(x + 1) == 'S' && input.at(y + 1).at(x - 1) == 'M');

            if (crossForward && crossBackward) {
                count++;
            }
        }
    }
    return count;
}
}  // namespace

std::pair<std::string, std::string> day04() {
    const auto input = parse("resources/day04.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
