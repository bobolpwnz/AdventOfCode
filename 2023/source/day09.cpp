#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"
#include "bblp/aoc/point.hpp"
#include "bblp/aoc/string_utils.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace bblp::aoc {
namespace {

auto parse(const std::filesystem::path& filePath) {
    std::vector<std::vector<int64_t>> input;
    const auto lineCallback = [&input](const std::string& line) {
        const auto parts = split(line, " ");
        std::vector<int64_t> readings(parts.size());
        std::transform(parts.cbegin(), parts.cend(), readings.begin(),
                       [](const std::string& str) { return std::stoll(str); });
        input.push_back(readings);
    };
    parseInput(filePath, lineCallback);
    return input;
}

bool areAllReadingsZero(const std::vector<int64_t>& readings) {
    return std::all_of(readings.cbegin(), readings.cend(), [](int64_t val) { return val == 0; });
}

int64_t predictNextValue(const std::vector<int64_t>& readings) {
    std::vector<std::vector<int64_t>> process{{readings.cbegin(), readings.cend()}};

    while (!areAllReadingsZero(process.back())) {
        const auto& back = process.back();
        std::vector<int64_t> differences;
        differences.reserve(back.size() + 1U);
        for (std::size_t i = 0U; i < back.size() - 1U; ++i) {
            differences.emplace_back(back.at(i + 1) - back.at(i));
        }
        process.push_back(differences);
    }

    process.back().push_back(0);
    for (std::size_t i = 0U; i < process.size() - 1U; ++i) {
        auto& previous = process.at(process.size() - i - 1U);
        auto& current = process.at(process.size() - i - 2U);
        current.push_back(current.back() + previous.back());
    }
    return process.front().back();
}

int64_t predictPreviousValue(const std::vector<int64_t>& readings) {
    std::vector<std::vector<int64_t>> process{{readings.cbegin(), readings.cend()}};

    while (!areAllReadingsZero(process.back())) {
        const auto& back = process.back();
        std::vector<int64_t> differences;
        differences.reserve(back.size() + 1U);
        for (std::size_t i = 0U; i < back.size() - 1U; ++i) {
            differences.emplace_back(back.at(i + 1) - back.at(i));
        }
        process.push_back(differences);
    }

    process.back().insert(process.back().begin(), 0LL);
    for (std::size_t i = 0U; i < process.size() - 2U; ++i) {
        auto& previous = process.at(process.size() - i - 1U);
        auto& current = process.at(process.size() - i - 2U);
        const auto val = current.front() - previous.front();
        current.insert(current.begin(), val);
    }

    const auto val = process.front().front() - process.at(1U).front();
    return val;
}

int64_t calculatePartOne(const std::vector<std::vector<int64_t>>& input) {
    return std::accumulate(input.cbegin(), input.cend(), 0LL, [](int64_t sum, const std::vector<int64_t>& readings) {
        return sum + predictNextValue(readings);
    });
}

int64_t calculatePartTwo(const std::vector<std::vector<int64_t>>& input) {
    return std::accumulate(input.cbegin(), input.cend(), 0LL, [](int64_t sum, const std::vector<int64_t>& readings) {
        return sum + predictPreviousValue(readings);
    });
}
}  // namespace

std::pair<std::string, std::string> day09() {
    const auto input = parse("resources/day09.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
