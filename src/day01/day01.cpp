#include "day01.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <functional>

namespace bblp::advent_of_code_2022 {
namespace {
std::vector<int> readEflsCalories(const std::filesystem::path& filePath) {
    std::ifstream file(filePath);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file");
    }

    std::vector<int> elfsCalories;
    std::string line;
    int totalCalories = 0;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            std::istringstream iss(line);
            int currentCalories = 0;
            iss >> currentCalories;
            totalCalories += currentCalories;
        } else {
            elfsCalories.push_back(totalCalories);
            totalCalories = 0;
        }
    }
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

void day01() {
    auto elfsCalories = readEflsCalories("resources/day01/input.txt");

    const auto max = std::max_element(elfsCalories.begin(), elfsCalories.end());
    std::cout << "max is " << *max << '\n';

    std::sort(elfsCalories.begin(), elfsCalories.end(), std::greater{});
    const auto topThreeSum = calculateTopThreeSum(elfsCalories);

    std::cout << "top 3 sum " << topThreeSum << '\n';
}
}  // namespace bblp::advent_of_code_2022
