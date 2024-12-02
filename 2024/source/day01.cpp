#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"

#include <algorithm>
#include <cstdlib>
#include <numeric>
#include <regex>
#include <vector>

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

int calculatePartOne(const std::vector<std::string>& input) {
    return 0;
}

int calculatePartTwo(const std::vector<std::string>& input) {
    return 0;
}
}  // namespace

std::pair<std::string, std::string> day01() {
    const auto input = parse("resources/day01.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
