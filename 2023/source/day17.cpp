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
#include <list>
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
struct PointHash {
    size_t operator()(const Point& point) const {
        return point.x >= point.y ? point.x * point.x + point.x + point.y : point.x + point.y * point.y;
    }
};

enum class Direction { UNKNOWN, UP, DOWN, RIGHT, LEFT };

struct Distance {
    uint64_t value;
    Direction direction;
};

auto parse(const std::filesystem::path& filePath) {
    std::vector<std::string> input;

    const auto lineCallback = [&input](const std::string& line) { input.push_back(line); };
    parseInput(filePath, lineCallback);
    return input;
}

Direction calculateDirection(const Point& current, const Point& next) {
    const auto xDiff = next.x - current.x;
    const auto yDiff = next.y - current.y;
    if (xDiff > 0) {
        return Direction::RIGHT;
    }
    if (xDiff < 0) {
        return Direction::LEFT;
    }
    if (yDiff > 0) {
        return Direction::DOWN;
    }
    if (yDiff < 0) {
        return Direction::UP;
    }
    throw std::logic_error("Current == Next");
}

bool canContinueViaDirection(const std::vector<std::vector<Distance>>& distances,
                             const Point& current,
                             const Direction nextDirection) {
    std::vector<Direction> directions;

    Point previous{current};
    for (int i = 0; i < 3; ++i) {
        if (previous.x < 0 || previous.x >= distances.front().size() || previous.y < 0 ||
            previous.y >= distances.size()) {
            break;
        }

        if (directions.size() > 0 && directions.back() == Direction::UNKNOWN) {
            break;
        }

        const auto& distanceAtCurrent = distances.at(previous.y).at(previous.x);
        Point next;
        switch (distanceAtCurrent.direction) {
            case Direction::UP:
                next = Point{previous.x, previous.y + 1};
                directions.emplace_back(Direction::UP);
                break;
            case Direction::DOWN:
                next = Point{previous.x, previous.y - 1};
                directions.emplace_back(Direction::DOWN);
                break;
            case Direction::RIGHT:
                next = Point{previous.x - 1, previous.y};
                directions.emplace_back(Direction::RIGHT);
                break;
            case Direction::LEFT:
                next = Point{previous.x + 1, previous.y};
                directions.emplace_back(Direction::LEFT);
                break;
            default:
                next = Point{0, 0};
                directions.emplace_back(Direction::UNKNOWN);
                break;
        }
        previous = next;
    }

    if (directions.size() != 3) {
        return true;
    }

    return !std::all_of(directions.cbegin(), directions.cend(),
                        [nextDirection](const Direction direction) { return direction == nextDirection; });
}

void calculateMinDistance(const std::vector<std::string>& input,
                          const Point& current,
                          const Point& next,
                          std::vector<std::vector<Distance>>& distances,
                          std::unordered_map<Point, Point, PointHash>& route) {
    if (next.x < 0 || next.x >= input.front().size() || next.y < 0 || next.y >= input.size()) {
        return;
    }

    const auto& distanceToCurrent = distances.at(current.y).at(current.x);
    const auto& distanceToNext = distances.at(next.y).at(next.x);
    const auto nextDistance = input.at(next.y).at(next.x) - '0';
    const auto nextDirection = calculateDirection(current, next);

    if (canContinueViaDirection(distances, current, nextDirection) &&
        distanceToNext.value > distanceToCurrent.value + nextDistance) {
        distances.at(next.y).at(next.x).value = distanceToCurrent.value + nextDistance;
        distances.at(next.y).at(next.x).direction = nextDirection;
        route[next] = current;
    }
}

Point findNextWithMinDistance(const std::set<Point>& graph, const std::vector<std::vector<Distance>>& distances) {
    Point closest{*graph.begin()};
    for (const auto& point : graph) {
        if (distances.at(point.y).at(point.x).value < distances.at(closest.y).at(closest.x).value) {
            closest = point;
        }
    }
    return closest;
}

std::vector<Point> findPath(const std::unordered_map<Point, Point, PointHash>& route,
                            const Point& start,
                            const Point& end) {
    std::vector<Point> path;
    Point current = end;
    while (current != start) {
        std::cout << current.x << ":" << current.y << '\n';
        path.push_back(current);
        current = route.at(current);
    }
    return path;
}

uint64_t calculateHeatLoss(const std::vector<std::string>& input, const std::vector<Point>& path) {
    uint64_t result{0U};
    for (const auto& point : path) {
        result += input.at(point.y).at(point.x) - '0';
    }
    return result;
}

uint64_t calculatePartOne(const std::vector<std::string>& input) {
    std::unordered_map<Point, Point, PointHash> route;
    std::vector<std::vector<Distance>> distances(
        input.size(), std::vector<Distance>(input.front().size(), Distance{ULLONG_MAX, Direction::UNKNOWN}));
    distances.at(0U).at(0U) = {0U, Direction::UNKNOWN};

    std::set<Point> visited;
    std::set<Point> graph;
    for (auto y = 0U; y < input.size(); ++y) {
        for (auto x = 0U; x < input.front().size(); ++x) {
            graph.insert(Point{x, y});
        }
    }

    Point current;
    while (!graph.empty()) {
        if (visited.empty()) {
            current = Point{0, 0};
        } else {
            current = findNextWithMinDistance(graph, distances);
        }

        graph.erase(current);
        visited.insert(current);
        calculateMinDistance(input, current, Point{current.x - 1, current.y}, distances, route);
        calculateMinDistance(input, current, Point{current.x + 1, current.y}, distances, route);
        calculateMinDistance(input, current, Point{current.x, current.y - 1}, distances, route);
        calculateMinDistance(input, current, Point{current.x, current.y + 1}, distances, route);
    }

    const auto path = findPath(route, {0, 0}, {input.front().size() - 1, input.size() - 1});
    const auto heatLoss = calculateHeatLoss(input, path);
    std::cout << "Path lenght is " << path.size() << '\n';
    std::cout << "Heat loss is " << heatLoss << '\n';

    std::vector<std::string> printout(input.size());
    for (auto i = 0; i < input.size(); ++i) {
        printout.at(i) = input.at(i);
    }
    for (const auto& point : path) {
        printout.at(point.y).at(point.x) = 'X';
    }

    for (const auto& line : printout) {
        std::cout << line << '\n';
    }

    return heatLoss;
}

uint64_t calculatePartTwo(const std::vector<std::string>& input) {
    return 0;
}
}  // namespace

std::pair<std::string, std::string> day17() {
    const auto input = parse("resources/day17.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
