#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"
#include "bblp/aoc/point.hpp"
#include "bblp/aoc/string_utils.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string_view>
#include <vector>
#include <optional>

namespace bblp::aoc {
namespace {
struct ConversionMap {
    uint64_t destinationStart;
    uint64_t sourceStart;
    uint64_t rangeLength;
};

struct SeedRange
{
    uint64_t start;
    uint64_t length;
};

struct Almanac {
    std::vector<uint64_t> seeds;
    std::vector<SeedRange> seedRanges;
    std::vector<ConversionMap> seedToSoilMap;
    std::vector<ConversionMap> soilToFertilizerMap;
    std::vector<ConversionMap> fertilizerToWaterMap;
    std::vector<ConversionMap> waterToLightMap;
    std::vector<ConversionMap> lightToTemperatureMap;
    std::vector<ConversionMap> temperatureToHumidityMap;
    std::vector<ConversionMap> humidityToLocationMap;
};

std::vector<uint64_t> parseSeeds(const std::string& input) {
    const auto seedsStrings = split(trim(split(input, ":").back()), " ");
    std::vector<uint64_t> seeds(seedsStrings.size());
    std::transform(seedsStrings.cbegin(), seedsStrings.cend(), seeds.begin(),
                   [](const std::string& str) { return std::stoull(str); });
    return seeds;
}

std::vector<SeedRange> parseSeedRanges(const std::string& input) {
    const auto numbers = split(trim(split(input, ":").back()), " ");
    std::vector<SeedRange> seedRanges;
    for (std::size_t i = 0U; i < numbers.size(); i += 2) {
        seedRanges.emplace_back(std::stoull(numbers.at(i)), std::stoull(numbers.at(i + 1)));
    }
    return seedRanges;
}

auto parse(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    Almanac input{};
    std::vector<ConversionMap>* currentMap = &input.seedToSoilMap;
    const auto lineCallback = [&input, &currentMap](const std::string& line) {
        if (line.starts_with("seeds")) {
            input.seeds = parseSeeds(line);
            input.seedRanges = parseSeedRanges(line);
        } else if (line.starts_with("seed-to-soil map")) {
            currentMap = &input.seedToSoilMap;
        } else if (line.starts_with("soil-to-fertilizer map")) {
            currentMap = &input.soilToFertilizerMap;
        } else if (line.starts_with("fertilizer-to-water map")) {
            currentMap = &input.fertilizerToWaterMap;
        } else if (line.starts_with("water-to-light map")) {
            currentMap = &input.waterToLightMap;
        } else if (line.starts_with("light-to-temperature map")) {
            currentMap = &input.lightToTemperatureMap;
        } else if (line.starts_with("temperature-to-humidity map")) {
            currentMap = &input.temperatureToHumidityMap;
        } else if (line.starts_with("humidity-to-location map")) {
            currentMap = &input.humidityToLocationMap;
        } else if (line.length() > 0) {
            const auto numbers = split(line, " ");
            currentMap->emplace_back(std::stoull(numbers.at(0)), std::stoull(numbers.at(1)), std::stoull(numbers.at(2)));
        }
    };
    parseInput(filePath, lineCallback);
    return input;
}

uint64_t findCorrespondingDestination(const uint64_t sourceValue, const std::vector<ConversionMap>& conversionMaps)
{
    for (const auto& map : conversionMaps) {
        if (sourceValue >= map.sourceStart && sourceValue < map.sourceStart + map.rangeLength) {
            return map.destinationStart + (sourceValue - map.sourceStart);
        }
    }

    return sourceValue;
}

std::optional<uint64_t> findCorrespondingSource(const uint64_t destinationValue,
    const std::vector<ConversionMap>& conversionMaps)
{
    for (const auto& map : conversionMaps) {
        if (destinationValue >= map.destinationStart && destinationValue < map.destinationStart + map.rangeLength) {
            return map.sourceStart + (destinationValue - map.destinationStart);
        }
    }

    return {};
}

uint64_t calculatePartOne(const Almanac& input) {
    uint64_t result{ULLONG_MAX};
    for (const auto seed : input.seeds) {
        const uint64_t soil = findCorrespondingDestination(seed, input.seedToSoilMap);
        const uint64_t fertilizer = findCorrespondingDestination(soil, input.soilToFertilizerMap);
        const uint64_t water = findCorrespondingDestination(fertilizer, input.fertilizerToWaterMap);
        const uint64_t light = findCorrespondingDestination(water, input.waterToLightMap);
        const uint64_t temperature = findCorrespondingDestination(light, input.lightToTemperatureMap);
        const uint64_t humidity = findCorrespondingDestination(temperature, input.temperatureToHumidityMap);
        const uint64_t location = findCorrespondingDestination(humidity, input.humidityToLocationMap);
        if (location < result) {
            result = location;
        }
    }

    return result;
}

uint64_t calculatePartTwo(const Almanac& input) {
    std::optional<uint64_t> result;

    std::uint64_t location{0U};
    while (!result.has_value()) {
        auto humidity = findCorrespondingSource(location, input.humidityToLocationMap);
        if (!humidity.has_value()) {
            humidity = location;
        }

        auto temperature = findCorrespondingSource(*humidity, input.temperatureToHumidityMap);
        if (!temperature.has_value()) {
            temperature = humidity;
        }

        auto light = findCorrespondingSource(*temperature, input.lightToTemperatureMap);
        if (!light.has_value()) {
            light = temperature;
        }
    
        auto water = findCorrespondingSource(*light, input.waterToLightMap);
        if (!water.has_value()) {
            water = light;
        }

        auto fertilizer = findCorrespondingSource(*water, input.fertilizerToWaterMap);
        if (!fertilizer.has_value()) {
            fertilizer = water;
        }

        auto soil = findCorrespondingSource(*fertilizer, input.soilToFertilizerMap);
        if (!soil.has_value()) {
            soil = fertilizer;
        }

        auto seed = findCorrespondingSource(*soil, input.seedToSoilMap);
        if (!seed.has_value()) {
            seed = soil;
        }


        for (const auto& seedRange : input.seedRanges) {
            if (seed >= seedRange.start && seed < seedRange.start + seedRange.length) {
                result = location;
            }
        }

        ++location;
    }
    return *result;
}
}  // namespace

std::pair<std::string, std::string> day05() {
    const auto input = parse("resources/day05.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
