#include "days.hpp"

#include "common/file_utils.hpp"

#include <algorithm>
#include <sstream>
#include <vector>

namespace bblp::advent_of_code_2022 {
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

std::tuple<int64_t, int64_t> day01() {
    auto elfsCalories = parseEflsCalories("resources/day01/input.txt");

    const auto iter = std::max_element(elfsCalories.begin(), elfsCalories.end());
    const auto max = *iter;

    std::sort(elfsCalories.begin(), elfsCalories.end(), std::greater{});
    const auto topThreeSum = calculateTopThreeSum(elfsCalories);

    return {max, topThreeSum};
}
}  // namespace bblp::advent_of_code_2022
