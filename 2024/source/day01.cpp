#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"
#include "bblp/aoc/string_utils.hpp"

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
    std::vector<int32_t> leftList, rightList;
    leftList.reserve(input.size());
    rightList.reserve(input.size());

    for (auto const& line : input) {
        const auto parts = split(line, " ");
        leftList.emplace_back(std::stoul(parts.front()));
        rightList.emplace_back(std::stoul(parts.back()));
    }
    
    std::sort(leftList.begin(), leftList.end());
    std::sort(rightList.begin(), rightList.end());

    uint32_t distance = 0U;

    for (auto i = 0U; i < leftList.size(); ++i) {
        distance += std::abs(leftList.at(i) - rightList.at(i));
    }

    return distance;
}

int calculatePartTwo(const std::vector<std::string>& input) {
    std::vector<int32_t> leftList, rightList;
    leftList.reserve(input.size());
    rightList.reserve(input.size());

    for (auto const& line : input) {
        const auto parts = split(line, " ");
        leftList.emplace_back(std::stoul(parts.front()));
        rightList.emplace_back(std::stoul(parts.back()));
    }

    std::unordered_map<int32_t, uint32_t> scores;
    for (auto left : leftList) {
        auto const count = std::count(rightList.cbegin(), rightList.cend(), left);;
        scores[left] += left * count;
    }

    return std::accumulate(scores.cbegin(), scores.cend(), 0U, [](const uint32_t sum, const auto &item) { 
        return sum + item.second;
    });

}
}  // namespace

std::pair<std::string, std::string> day01() {
    const auto input = parse("resources/day01.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
