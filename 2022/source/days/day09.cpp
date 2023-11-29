#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <functional>
#include <numeric>
#include <vector>
#include <set>

namespace bblp::aoc {
namespace {

template <typename T>
int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

enum class Direction { RIGHT, LEFT, UP, DOWN };

struct Command {
    Command(const Direction direction, const int32_t distance) : direction(direction), distance(distance) {}

    Direction direction;
    int32_t distance;
};

struct Point {
    [[nodiscard]] bool operator==(const Point& other) const noexcept { return (x == other.x) && (y == other.y); };
    [[nodiscard]] bool operator<(const Point& other) const noexcept { 
        if (x != other.x) {
            return x < other.x;
        }

        return y < other.y;
    }

    [[nodiscard]] int32_t distanceTo(const Point& other) const noexcept {
        return std::max(std::abs(x - other.x), std::abs(y - other.y));
    }

    [[nodiscard]] bool isAdjacentTo(const Point& other) const noexcept {
        return (std::abs(x - other.x)) <= 1 && (std::abs(y - other.y) <= 1);
    }

    void follow(const Point& target) {
        if (isAdjacentTo(target)) {
            return;
        }

        const Point distance{target.x - x, target.y - y};
        x += sign(distance.x);
        y += sign(distance.y);
    }

    int32_t x{0};
    int32_t y{0};
};

Direction parseDirection(char character) {
    switch (character) {
        case 'R':
            return Direction::RIGHT;
        case 'L':
            return Direction::LEFT;
        case 'U':
            return Direction::UP;
        case 'D':
            return Direction::DOWN;
        default:
            throw std::runtime_error("Invalid direction");
    }
}

auto parse(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    std::vector<Command> input;
    input.reserve(BUFFER_SIZE);
    const auto lineCallback = [&input](const std::string& line) {
        if (!line.empty()) {
            const auto direction = parseDirection(line[0]);
            const auto distance = std::stol(line.substr(2, line.size() - 2));
            input.emplace_back(direction, distance);
        }
    };
    parseInput(filePath, lineCallback);
    return input;
}

void movePosition(Point& position, const Direction direction) {
    switch (direction) {
        case Direction::RIGHT:
            position.x += 1;
            break;
        case Direction::LEFT:
            position.x -= 1;
            break;
        case Direction::UP:
            position.y += 1;
            break;
        case Direction::DOWN:
            position.y -= 1;
            break;
        default:
            throw std::runtime_error("Invalid direction");
    }
}

size_t calculateVisitedPositionsPart1(const std::vector<Command>& input) {
    Point headPosition;
    Point tailPosition;
    std::set<Point> visitedPositions;
    for (const auto& command : input) {
        for (auto i = 0U; i < command.distance; ++i) {
            const Point currentHeadPosition = headPosition;
            movePosition(headPosition, command.direction);
            if ((tailPosition != currentHeadPosition) && (tailPosition.distanceTo(headPosition) > 1)) {
                tailPosition = currentHeadPosition;
            }

            visitedPositions.insert(tailPosition);
        }
    }
    return visitedPositions.size();
}

size_t calculateVisitedPositionsPart2(const std::vector<Command>& input) {
    static constexpr size_t ropeLength = 10;

    std::array<Point, ropeLength> snake;
    std::set<Point> visitedPositions;

    for (const auto& command : input) {
        for (auto i = 0; i < command.distance; ++i) {
            movePosition(snake.back(), command.direction);
            for (auto iter = snake.rbegin() + 1; iter != snake.rend(); ++iter) {
                iter->follow(*std::prev(iter));
            }

            visitedPositions.insert(snake.front());
        }
    }
    return visitedPositions.size();
}
}  // namespace

std::pair<std::string, std::string> day09() {
    const auto input = parse("resources/day09.txt");

    const auto visitedPositionsPart1 = calculateVisitedPositionsPart1(input);
    const auto visitedPositionsPart2 = calculateVisitedPositionsPart2(input);

    return {std::to_string(visitedPositionsPart1), std::to_string(visitedPositionsPart2)};
}
}  // namespace bblp::aoc
