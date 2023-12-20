#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"
#include "bblp/aoc/point.hpp"
#include "bblp/aoc/string_utils.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <fstream>
#include <future>
#include <iostream>
#include <list>
#include <numeric>
#include <optional>
#include <queue>
#include <set>
#include <stdexcept>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

namespace bblp::aoc {
namespace {
auto parse(const std::filesystem::path& filePath) {
    std::vector<std::string> input;

    const auto lineCallback = [&input](const std::string& line) { input.push_back(line); };
    parseInput(filePath, lineCallback);
    return input;
}

uint64_t calculatePartOne(const std::vector<std::string>& input) {
    return 0U;
}

uint64_t calculatePartTwo(const std::vector<std::string>& input) {
    return 0U;
}
}  // namespace

std::pair<std::string, std::string> day18() {
    const auto input = parse("resources/day18.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
