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
#include <vector>

namespace bblp::aoc {
namespace {
struct Race {
    uint64_t time;
    uint64_t distance;
};

auto parse(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    std::vector<Race> input;
    std::vector<uint64_t> times;
    std::vector<uint64_t> distances;
    std::vector<std::string> bigTimes;
    std::vector<std::string> bigDistances;
    Race bigRace{};

    const auto lineCallback = [&times, &distances, &bigTimes, &bigDistances](const std::string& line) {

        const auto parts = split(line, ":");
        const auto numbers = split(parts.back(), " ");
        for (const auto& number : numbers) {
            if (number.length() > 0) {
                if (line.starts_with("Time")) {
                    times.push_back(std::stoul(number));
                    bigTimes.push_back(number);
                } else if (line.starts_with("Distance")) {
                    distances.push_back(std::stoul(number));
                    bigDistances.push_back(number);
                } else {
                    throw std::logic_error("Invalid data");
                }
            }
        }
    };

    parseInput(filePath, lineCallback);
    for (auto i = 0U; i < times.size(); ++i) {
        input.emplace_back(times.at(i), distances.at(i));
    }
    std::ostringstream bigTimesStream;
    std::copy(bigTimes.cbegin(), bigTimes.cend(), std::ostream_iterator<std::string>(bigTimesStream, ""));
    std::string bigTime = bigTimesStream.str();

    std::ostringstream bigDistanceStream;
    std::copy(bigDistances.cbegin(), bigDistances.cend(), std::ostream_iterator<std::string>(bigDistanceStream, ""));
    std::string bigDistance = bigDistanceStream.str();

    return std::pair<std::vector<Race>, Race>{input, Race{std::stoull(bigTime), std::stoull(bigDistance)}};
}

uint64_t calculateDistanceTraveled(const uint64_t raceTime, const uint64_t buttonPressTime) {
    return buttonPressTime * (raceTime - buttonPressTime);
}

uint32_t calculatePartOne(const std::vector<Race>& input) {
    uint32_t result{1U};
    for (const auto& race : input) {
        uint32_t raceWinnings{0U};
        for (auto i = 0U; i < race.time; ++i) {
            if (calculateDistanceTraveled(race.time, i) > race.distance) {
                ++raceWinnings;
            }
        }
        result *= raceWinnings;
    }

    return result;
}

uint64_t calculatePartTwo(const Race& race) {
    uint64_t result{0U};
    for (auto i = 0U; i < race.time; ++i) {
        if (calculateDistanceTraveled(race.time, i) > race.distance) {
            ++result;
        }
    }

    return result;
}
}  // namespace

std::pair<std::string, std::string> day06() {
    const auto input = parse("resources/day06.txt");

    return {std::to_string(calculatePartOne(input.first)), std::to_string(calculatePartTwo(input.second))};
}
}  // namespace bblp::aoc
