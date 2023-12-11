#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"
#include "bblp/aoc/point.hpp"
#include "bblp/aoc/string_utils.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace bblp::aoc {
namespace {
constexpr char GALAXY{'#'};

auto parse(const std::filesystem::path& filePath) {
    std::vector<std::string> input;
    const auto lineCallback = [&input](const std::string& line) { input.push_back(line); };
    parseInput(filePath, lineCallback);
    return input;
}

bool isRowWithoutGalaxy(const std::vector<std::string>& input, std::size_t row) {
    return input.at(row).find(GALAXY) == std::string::npos;
}

bool isColWithoutGalaxy(const std::vector<std::string>& input, std::size_t col) {
    for (std::size_t y = 0; y < input.size(); ++y) {
        if (input.at(y).at(col) == GALAXY) {
            return false;
        }
    }
    return true;
}

std::vector<uint64_t> findRowsWithoutGalaxies(const std::vector<std::string>& input) {
    std::vector<uint64_t> rowsWithoutGalaxies;
    for (std::size_t i = 0; i < input.size(); ++i) {
        if (isRowWithoutGalaxy(input, i)) {
            rowsWithoutGalaxies.push_back(i);
        }
    }
    return rowsWithoutGalaxies;
}

std::vector<uint64_t> findColsWithoutGalaxies(const std::vector<std::string>& input) {
    std::vector<uint64_t> colsWithoutGalaxies;
    for (std::size_t i = 0; i < input.front().size(); ++i) {
        if (isColWithoutGalaxy(input, i)) {
            colsWithoutGalaxies.push_back(i);
        }
    }
    return colsWithoutGalaxies;
}

std::vector<Point> findAllGalaxies(const std::vector<std::string>& input) {
    std::vector<Point> galaxies;
    for (std::size_t y = 0U; y < input.size(); ++y) {
        for (std::size_t x = 0U; x < input.at(y).size(); ++x) {
            if (input.at(y).at(x) == GALAXY) {
                galaxies.emplace_back(x, y);
            }
        }
    }
    return galaxies;
}

uint64_t findNumberOfRowsOrColsWithoutGalaxies(const std::vector<uint64_t>& input, uint64_t first, uint64_t second) {
    return std::accumulate(input.cbegin(), input.cend(), static_cast<uint64_t>(0U),
                           [first, second](uint64_t sum, uint64_t val) {
                               const auto isValBetween = [first, second](uint64_t val) -> bool {
                                   if (first < second) {
                                       return val > first && val < second;
                                   } else {
                                       return val > second && val < first;
                                   }
                               };

                               return sum + (isValBetween(val) ? 1U : 0U);
                           });
}

uint64_t calculatePartOne(const std::vector<std::string>& input) {
    const auto rowsWithoutGalaxies = findRowsWithoutGalaxies(input);
    const auto colsWithoutGalaxies = findColsWithoutGalaxies(input);
    const auto galaxies = findAllGalaxies(input);

    std::vector<std::vector<uint64_t>> distances{galaxies.size(), std::vector<uint64_t>(galaxies.size())};
    for (std::size_t sourceGalaxyIndex = 0U; sourceGalaxyIndex < galaxies.size(); ++sourceGalaxyIndex) {
        for (std::size_t destGalaxyIndex = sourceGalaxyIndex + 1; destGalaxyIndex < galaxies.size();
             ++destGalaxyIndex) {
            const auto& source = galaxies.at(sourceGalaxyIndex);
            const auto& dest = galaxies.at(destGalaxyIndex);

            const auto numberOfRowsWithoutGalaxiesBetween =
                findNumberOfRowsOrColsWithoutGalaxies(rowsWithoutGalaxies, source.y, dest.y);
            const auto numberOfColsWithoutGalaxiesBetween =
                findNumberOfRowsOrColsWithoutGalaxies(colsWithoutGalaxies, source.x, dest.x);

            const auto xDiff = std::abs(source.x - dest.x);
            const auto yDiff = std::abs(source.y - dest.y);
            const auto dist = xDiff + yDiff + numberOfRowsWithoutGalaxiesBetween + numberOfColsWithoutGalaxiesBetween;

            distances.at(sourceGalaxyIndex).at(destGalaxyIndex) = dist;
        }
    }

    return std::accumulate(distances.cbegin(), distances.cend(), static_cast<uint64_t>(0U),
                           [](uint64_t sum, const std::vector<uint64_t>& row) {
                               return sum + std::accumulate(row.cbegin(), row.cend(), static_cast<uint64_t>(0U),
                                                            [](uint64_t sum, uint64_t val) { return sum + val; });
                           });
}

int64_t calculatePartTwo(const std::vector<std::string>& input) {
    const auto rowsWithoutGalaxies = findRowsWithoutGalaxies(input);
    const auto colsWithoutGalaxies = findColsWithoutGalaxies(input);
    const auto galaxies = findAllGalaxies(input);

    std::vector<std::vector<uint64_t>> distances{galaxies.size(), std::vector<uint64_t>(galaxies.size())};
    for (std::size_t sourceGalaxyIndex = 0U; sourceGalaxyIndex < galaxies.size(); ++sourceGalaxyIndex) {
        for (std::size_t destGalaxyIndex = sourceGalaxyIndex + 1; destGalaxyIndex < galaxies.size();
             ++destGalaxyIndex) {
            const auto& source = galaxies.at(sourceGalaxyIndex);
            const auto& dest = galaxies.at(destGalaxyIndex);

            const auto numberOfRowsWithoutGalaxiesBetween =
                findNumberOfRowsOrColsWithoutGalaxies(rowsWithoutGalaxies, source.y, dest.y);
            const auto numberOfColsWithoutGalaxiesBetween =
                findNumberOfRowsOrColsWithoutGalaxies(colsWithoutGalaxies, source.x, dest.x);

            const uint64_t xDiff = std::abs(source.x - dest.x);
            const uint64_t yDiff = std::abs(source.y - dest.y);
            const uint64_t dist = xDiff + yDiff +
                                  (numberOfRowsWithoutGalaxiesBetween + numberOfColsWithoutGalaxiesBetween) *
                                      static_cast<uint64_t>(999999U);

            distances.at(sourceGalaxyIndex).at(destGalaxyIndex) = dist;
        }
    }

    return std::accumulate(distances.cbegin(), distances.cend(), static_cast<uint64_t>(0U),
                           [](uint64_t sum, const std::vector<uint64_t>& row) {
                               return sum + std::accumulate(row.cbegin(), row.cend(), static_cast<uint64_t>(0U),
                                                            [](uint64_t sum, uint64_t val) { return sum + val; });
                           });
}
}  // namespace

std::pair<std::string, std::string> day11() {
    const auto input = parse("resources/day11.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
