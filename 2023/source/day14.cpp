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

template <>
struct std::hash<std::vector<std::string>> {
    std::size_t operator()(const std::vector<std::string>& k) const {
        const auto result = std::accumulate(k.begin(), k.cend(), std::string(""),
                                            [](const std::string& sum, const std::string& str) { return sum + str; });

        return std::hash<std::string>()(result);
    }
};

namespace bblp::aoc {
namespace {
constexpr char ROUND_ROCK{'O'};
constexpr char CUBE_ROCK{'#'};
constexpr char EMPTY_SPACE{'.'};

std::ostream& operator<<(std::ostream& stream, const std::vector<std::string>& out) {
    for (const auto& str : out) {
        stream << str << '\n';
    }
    return stream;
}

auto parse(const std::filesystem::path& filePath) {
    std::vector<std::string> input;

    const auto lineCallback = [&input](const std::string& line) { input.push_back(line); };
    parseInput(filePath, lineCallback);
    return input;
}

void tiltNorth(std::vector<std::string>& landscape) {
    for (int32_t y = 1U; y < landscape.size(); ++y) {
        for (int32_t x = 0U; x < landscape.front().size(); ++x) {
            if (landscape.at(y).at(x) == ROUND_ROCK) {
                uint32_t shift = y;
                while (shift > 0 && landscape.at(shift - 1).at(x) == EMPTY_SPACE) {
                    landscape.at(shift - 1).at(x) = ROUND_ROCK;
                    landscape.at(shift).at(x) = EMPTY_SPACE;
                    --shift;
                }
            }
        }
    }
}

void tiltSouth(std::vector<std::string>& landscape) {
    for (int32_t y = landscape.size() - 1; y >= 0; --y) {
        for (int32_t x = 0U; x < landscape.front().size(); ++x) {
            if (landscape.at(y).at(x) == ROUND_ROCK) {
                uint32_t shift = y;
                while (shift < landscape.size() - 1 && landscape.at(shift + 1).at(x) == EMPTY_SPACE) {
                    landscape.at(shift + 1).at(x) = ROUND_ROCK;
                    landscape.at(shift).at(x) = EMPTY_SPACE;
                    ++shift;
                }
            }
        }
    }
}

void tiltWest(std::vector<std::string>& landscape) {
    for (int32_t y = 0U; y < landscape.size(); ++y) {
        for (int32_t x = 1U; x < landscape.front().size(); ++x) {
            if (landscape.at(y).at(x) == ROUND_ROCK) {
                uint32_t shift = x;
                while (shift > 0 && landscape.at(y).at(shift - 1) == EMPTY_SPACE) {
                    landscape.at(y).at(shift - 1) = ROUND_ROCK;
                    landscape.at(y).at(shift) = EMPTY_SPACE;
                    --shift;
                }
            }
        }
    }
}

void tiltEast(std::vector<std::string>& landscape) {
    for (int32_t y = 0U; y < landscape.size(); ++y) {
        for (int32_t x = landscape.front().size() - 2; x >= 0; --x) {
            if (landscape.at(y).at(x) == ROUND_ROCK) {
                uint32_t shift = x;
                while (shift < landscape.front().size() - 1 && landscape.at(y).at(shift + 1) == EMPTY_SPACE) {
                    landscape.at(y).at(shift + 1) = ROUND_ROCK;
                    landscape.at(y).at(shift) = EMPTY_SPACE;
                    ++shift;
                }
            }
        }
    }
}

uint64_t calculateTotalLoad(const std::vector<std::string>& landscape) {
    uint64_t result{0U};
    for (auto y = 0U; y < landscape.size(); ++y) {
        for (auto x = 0U; x < landscape.front().size(); ++x) {
            if (landscape.at(y).at(x) == ROUND_ROCK) {
                result += landscape.size() - static_cast<uint64_t>(y);
            }
        }
    }

    return result;
}

uint64_t calculatePartOne(const std::vector<std::string>& input) {
    std::vector<std::string> landscape{input};
    tiltNorth(landscape);
    return calculateTotalLoad(landscape);
}

uint64_t calculatePartTwo(const std::vector<std::string>& input) {
    std::vector<std::string> landscape{input};
    std::unordered_map<std::vector<std::string>, int64_t> results;

    int64_t cycleSize{0U};
    int64_t cycleStart{0U};
    for (int64_t i = 0; i < 1000000000; ++i) {
        if (results.contains(landscape)) {
            cycleStart = i;
            cycleSize = i - results.at(landscape);
            break;
        } else {
            results.insert({landscape, i});
        }

        tiltNorth(landscape);
        tiltWest(landscape);
        tiltSouth(landscape);
        tiltEast(landscape);
    }

    const auto reminder{(1000000000 - cycleStart) % cycleSize};
    for (int64_t i = 0; i < reminder; ++i) {
        tiltNorth(landscape);
        tiltWest(landscape);
        tiltSouth(landscape);
        tiltEast(landscape);
    }

    return calculateTotalLoad(landscape);
}
}  // namespace

std::pair<std::string, std::string> day14() {
    const auto input = parse("resources/day14.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
