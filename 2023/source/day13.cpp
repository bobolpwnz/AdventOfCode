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
#include <numeric>
#include <optional>
#include <set>
#include <stdexcept>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

namespace bblp::aoc {
namespace {
struct Landscape {
    std::vector<std::string> pattern;
};

auto parse(const std::filesystem::path& filePath) {
    std::vector<Landscape> input;

    input.push_back(Landscape{});
    const auto lineCallback = [&input](const std::string& line) {
        if (line.empty()) {
            input.push_back(Landscape{});
        } else {
            input.back().pattern.push_back(line);
        }
    };
    parseInput(filePath, lineCallback);
    return input;
}

uint32_t isVerticalPattern(const Landscape& landscape, Point coords) {
    uint32_t diffs{0U};
    for (auto k = 0U; k < landscape.pattern.size(); ++k) {
        if (landscape.pattern.at(k).at(coords.x) != landscape.pattern.at(k).at(coords.y)) {
            ++diffs;
        }
    }
    return diffs;
}

bool isVerticalPattern(const Landscape& landscape, uint32_t i, uint32_t allowedDiffs) {
    uint32_t totalDiffs{0U};
    for (int32_t j = 0; j <= i; j++) {
        const auto leftIndex = i - j;
        const auto rightIndex = i + 1 + j;
        if (leftIndex < 0 || rightIndex >= landscape.pattern.front().size()) {
            break;
        }

        totalDiffs += isVerticalPattern(landscape, Point{leftIndex, rightIndex});
        if (totalDiffs > allowedDiffs) {
            return false;
        }
    }
    return totalDiffs == allowedDiffs;
}

uint32_t isHorizontalPattern(const Landscape& landscape, Point coords) {
    uint32_t diffs{0U};
    for (auto k = 0U; k < landscape.pattern.front().size(); ++k) {
        if (landscape.pattern.at(coords.x).at(k) != landscape.pattern.at(coords.y).at(k)) {
            ++diffs;
        }
    }
    return diffs;
}

bool isHorizontalPattern(const Landscape& landscape, uint32_t i, uint32_t allowedDiffs) {
    uint32_t totalDiffs{0U};
    for (int32_t j = 0; j <= i; j++) {
        const auto topIndex = i - j;
        const auto bottomIndex = i + 1 + j;
        if (topIndex < 0 || bottomIndex >= landscape.pattern.size()) {
            break;
        }

        totalDiffs += isHorizontalPattern(landscape, Point{topIndex, bottomIndex});
        if (totalDiffs > allowedDiffs) {
            return false;
        }
    }
    return totalDiffs == allowedDiffs;
}

std::optional<uint64_t> findVerticalPattern(const Landscape& landscape, uint32_t allowedDiffs) {
    for (auto i = 0U; i < landscape.pattern.front().size() - 1; ++i) {
        if (isVerticalPattern(landscape, i, allowedDiffs)) {
            return i + 1;
        }
    }
    return {};
}

std::optional<uint64_t> findHorizontalPattern(const Landscape& landscape, uint32_t allowedDiffs) {
    for (auto i = 0U; i < landscape.pattern.size() - 1; ++i) {
        if (isHorizontalPattern(landscape, i, allowedDiffs)) {
            return i + 1;
        }
    }
    return {};
}

uint64_t calculateReflectionPattern(const Landscape& landscape, uint32_t allowedDiffs) {
    const auto verticalPattern = findVerticalPattern(landscape, allowedDiffs);
    if (verticalPattern) {
        return *verticalPattern;
    } else {
        const auto horizontalPattern = findHorizontalPattern(landscape, allowedDiffs);
        if (horizontalPattern) {
            return *horizontalPattern * 100ULL;
        } else {
            throw std::logic_error("Pattern not found");
        }
    }
}

uint64_t calculatePartOne(const std::vector<Landscape>& input) {
    return std::accumulate(input.cbegin(), input.cend(), 0ULL, [](uint64_t sum, const Landscape& landscape) {
        return sum + calculateReflectionPattern(landscape, 0U);
    });
}

uint64_t calculatePartTwo(const std::vector<Landscape>& input) {
    return std::accumulate(input.cbegin(), input.cend(), 0ULL, [](uint64_t sum, const Landscape& landscape) {
        return sum + calculateReflectionPattern(landscape, 1U);
    });
}
}  // namespace

std::pair<std::string, std::string> day13() {
    const auto input = parse("resources/day13.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
