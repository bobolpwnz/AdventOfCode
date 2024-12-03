#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"
#include "bblp/aoc/string_utils.hpp"

#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <numeric>
#include <regex>
#include <vector>

namespace bblp::aoc {
namespace {


auto parse(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    std::string input;
    input.reserve(BUFFER_SIZE);
    const auto lineCallback = [&input](const std::string& line) { input.append(line); };
    parseInput(filePath, lineCallback);
    return input;
}

int calculatePartOne(const std::string& input) {
    const std::string pattern{R"(mul\([0-9]{1,3},[0-9]{1,3}\))"};
    std::regex regex{pattern};
    std::smatch match;

    auto searchStart{input.cbegin()};
    uint64_t result = 0U;
    while (std::regex_search(searchStart, input.cend(), match, regex)) {
        for (const auto& mul: match) {
            const auto parts = split(mul.str(), ",");
            std::string first{parts.front().cbegin() + 4, parts.front().cend()};
            std::string second{parts.back().cbegin(), parts.back().cend() - 1};
            result += std::stoul(first.data()) * std::stoul(second.data());
        }
        searchStart = match.suffix().first;
    }

    return result;
}

int calculatePartTwo(const std::string& input) {
    const std::string pattern{R"((do\(\))|(don't\(\))|(mul\([0-9]{1,3},[0-9]{1,3}\)))"};
    std::regex regex{pattern};
    std::smatch match;
    auto searchStart{input.cbegin()};
    uint64_t result = 0U;
    bool enabled = true;
    while (std::regex_search(searchStart, input.cend(), match, regex)) {
        if (match.empty()) {
            continue;
        }

        if (match[1].matched) {
            enabled = true;
        } else if (match[2].matched) {
            enabled = false;
        } else if (match[3].matched) {
            if (enabled) {
                const auto parts = split(match[3].str(), ",");
                std::string first{parts.front().cbegin() + 4, parts.front().cend()};
                std::string second{parts.back().cbegin(), parts.back().cend() - 1};
                result += std::stoul(first.data()) * std::stoul(second.data());
            }
        
        }

        searchStart = match.suffix().first;
    }

    return result;
}
}  // namespace

std::pair<std::string, std::string> day03() {
    const auto input = parse("resources/day03.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
