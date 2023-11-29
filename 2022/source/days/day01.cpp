#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"

#include <algorithm>
#include <sstream>
#include <vector>

namespace bblp::aoc {
namespace {
std::vector<int> parseEflsCalories(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    std::vector<int> elfsCalories;
    elfsCalories.reserve(BUFFER_SIZE);

    int totalCalories = 0;
    const auto lineCallback = [&elfsCalories, &totalCalories](const std::string& line) {
        if (!line.empty()) {
            std::istringstream iss(line);
            int currentCalories = 0;
            iss >> currentCalories;
            totalCalories += currentCalories;
        } else {
            elfsCalories.push_back(totalCalories);
            totalCalories = 0;
        }
    };
    parseInput(filePath, lineCallback);
    return elfsCalories;
}

int calculateTopThreeSum(const std::vector<int>& input) {
    int topThreeSum = 0;
    for (auto i = 0; i < 3; ++i) {
        topThreeSum += input[i];
    }
    return topThreeSum;
}
}  // namespace

std::pair<std::string, std::string> day01() {
    auto elfsCalories = parseEflsCalories("resources/day01.txt");

    const auto iter = std::max_element(elfsCalories.begin(), elfsCalories.end());
    const auto max = *iter;

    std::sort(elfsCalories.begin(), elfsCalories.end(), std::greater{});
    const auto topThreeSum = calculateTopThreeSum(elfsCalories);

    return {std::to_string(max), std::to_string(topThreeSum)};
}
}  // namespace bblp::aoc
