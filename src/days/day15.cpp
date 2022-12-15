#include "days.hpp"

#include "common/file_utils.hpp"
#include "common/point.hpp"
#include "common/string_utils.hpp"

#include <algorithm>
#include <cstdlib>
#include <numeric>
#include <vector>

namespace bblp::advent_of_code_2022 {
namespace {
constexpr int64_t MIN = 0;
constexpr int64_t MAX = 4000000;
constexpr int64_t FREQUENCY_MUL = 4000000;
constexpr int64_t INVALID_REQUENCY = -1;

bool operator<(const Point& lhs, const Point& rhs) {
    return lhs.x < rhs.x;
}

std::ostream& operator<<(std::ostream& stream, const Point& point) {
    stream << "x=" << point.x << ";y=" << point.y;
    return stream;
}

Point parsePoint(const std::string& string) {
    const auto strings = split(string, ",");
    const auto stringX = split(strings[0], "=");
    const auto stringY = split(strings[1], "=");
    return {std::stol(stringX[1]), std::stol(stringY[1])};
}

auto parse(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10;

    std::vector<std::pair<Point, Point>> input;
    input.reserve(BUFFER_SIZE);
    const auto lineCallback = [&input](const std::string& line) {
        if (line.empty()) {
            return;
        }

        const auto items = split(line, ":");
        input.emplace_back(parsePoint(items[0]), parsePoint(items[1]));
    };
    parseInput(filePath, lineCallback);
    return input;
}

int64_t calculateDistance(const Point& from, const Point& to) {
    return std::abs(from.x - to.x) + std::abs(from.y - to.y);
}

bool isInRangeOfSensor(const Point& target, const Point& sensor, const Point& beacon) {
    return (calculateDistance(target, sensor) <= calculateDistance(beacon, sensor));
}

int64_t minLeft(const Point& sensor, const Point& beacon) {
    return sensor.x - std::abs(beacon.x);
}

int64_t maxRight(const Point& sensor, const Point& beacon) {
    return sensor.x + std::abs(beacon.x);
}

int64_t calculateNumberOfInvalidPositions(const std::vector<std::pair<Point, Point>>& input) {
    const auto minIter = std::min_element(input.cbegin(), input.cend(), [](const auto& lhs, const auto& rhs) {
        return minLeft(lhs.first, lhs.second) < minLeft(rhs.first, rhs.second);
    });

    const auto maxIter = std::max_element(input.cbegin(), input.cend(), [](const auto& lhs, const auto& rhs) {
        return maxRight(lhs.first, lhs.second) < maxRight(rhs.first, rhs.second);
    });

    const auto minX = minLeft(minIter->first, minIter->second);
    const auto maxX = maxRight(maxIter->first, maxIter->second);

    const int64_t targetRow = 2000000;
    int32_t numberOfInvalidPositions = 0;
    for (int64_t x = minX; x <= maxX; ++x) {
        const Point target(x, targetRow);
        for (const auto& pair : input) {
            if (target == pair.first || target == pair.second) {
                continue;
            }
            if (isInRangeOfSensor(target, pair.first, pair.second)) {
                ++numberOfInvalidPositions;
                break;
            }
        }
    }
    return numberOfInvalidPositions;
}

bool isInRangeOfOtherSensors(const Point& target,
                             const std::vector<std::pair<Point, Point>>& input) {

    return std::any_of(input.cbegin(), input.cend(), [&target](const std::pair<Point, Point>& pair) {
        return isInRangeOfSensor(target, pair.first, pair.second);
    });
}

bool isPointValid(const Point& point) {
    return point.x >= MIN && point.x <= MAX && point.y >= MIN && point.y <= MAX;
}

bool checkPoint(const Point& point, const std::vector<std::pair<Point, Point>>& input) {
    if (isPointValid(point)) {
        if (!isInRangeOfOtherSensors(point, input)) {
            return false;
        }
    }
    return true;
}

int64_t checkTopLeftDiagonal(const Point& sensor,
                             const int64_t distance,
                             const std::vector<std::pair<Point, Point>>& input) {
    int64_t x = sensor.x - distance;
    int64_t y = sensor.y;
    while (x <= sensor.x && y >= sensor.y - distance) {
        if (!checkPoint(Point(x, y), input)) {
            return x * FREQUENCY_MUL + y;
        }
        ++x;
        --y;
    }
    return INVALID_REQUENCY;
}

int64_t checkTopRightDiagonal(const Point& sensor,
                              const int64_t distance,
                              const std::vector<std::pair<Point, Point>>& input) {
    int64_t x = sensor.x + distance;
    int64_t y = sensor.y;
    while (x >= sensor.x && y <= sensor.y + distance) {
        if (!checkPoint(Point(x, y), input)) {
            return x * FREQUENCY_MUL + y;
        }

        --x;
        ++y;
    }
    return INVALID_REQUENCY;
}

int64_t checkBottomLeftDiagonal(const Point& sensor,
                                const int64_t distance,
                                const std::vector<std::pair<Point, Point>>& input) {
    int64_t x = sensor.x - distance;
    int64_t y = sensor.y;
    while (x <= sensor.x && y <= sensor.y + distance) {
        if (!checkPoint(Point(x, y), input)) {
            return x * FREQUENCY_MUL + y;
        }
        ++x;
        ++y;
    }
    return INVALID_REQUENCY;
}

int64_t checkBottomRightDiagonal(const Point& sensor,
                                 const int64_t distance,
                                 const std::vector<std::pair<Point, Point>>& input) {
    int64_t x = sensor.x + distance;
    int64_t y = sensor.y;
    while (x >= sensor.x && y >= sensor.y - distance) {
        if (!checkPoint(Point(x, y), input)) {
            return x * FREQUENCY_MUL + y;
        }
        --x;
        --y;
    }
    return INVALID_REQUENCY;
}

int64_t calculateTuningFrequency(const std::vector<std::pair<Point, Point>>& input) {
    for (const auto& pair : input) {
        const auto& sensor = pair.first;
        const auto& beacon = pair.second;
        const auto distance = calculateDistance(sensor, beacon);

        if (const auto result = checkTopLeftDiagonal(sensor, distance + 1, input); result != INVALID_REQUENCY) {
            return result;
        }

        if (const auto result = checkTopLeftDiagonal(sensor, distance + 2, input); result != INVALID_REQUENCY) {
            return result;
        }

        if (const auto result = checkTopRightDiagonal(sensor, distance + 1, input); result != INVALID_REQUENCY) {
            return result;
        }

        if (const auto result = checkTopRightDiagonal(sensor, distance + 2, input); result != INVALID_REQUENCY) {
            return result;
        }

        if (const auto result = checkBottomLeftDiagonal(sensor, distance + 1, input); result != INVALID_REQUENCY) {
            return result;
        }

        if (const auto result = checkBottomLeftDiagonal(sensor, distance + 2, input); result != INVALID_REQUENCY) {
            return result;
        }

        if (const auto result = checkBottomRightDiagonal(sensor, distance + 1, input); result != INVALID_REQUENCY) {
            return result;
        }

        if (const auto result = checkBottomRightDiagonal(sensor, distance + 2, input); result != INVALID_REQUENCY) {
            return result;
        }
    }
    return INVALID_REQUENCY;
}
}  // namespace

std::pair<std::string, std::string> day15() {
    const auto input = parse("resources/day15.txt");

    const auto numberOfInvalidPositions = calculateNumberOfInvalidPositions(input);
    const auto tuningFrequency = calculateTuningFrequency(input);

    return {std::to_string(numberOfInvalidPositions), std::to_string(tuningFrequency)};
}
}  // namespace bblp::advent_of_code_2022
