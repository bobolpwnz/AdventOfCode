#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"
#include "bblp/aoc/grid.hpp"
#include "bblp/aoc/point.hpp"

#include <algorithm>
#include <array>
#include <numeric>
#include <queue>

namespace bblp::aoc {
namespace {

struct Step {
    int32_t distance;
    Point position;
};

auto parse(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    std::string tiles;
    tiles.reserve(BUFFER_SIZE);
    int32_t width = 0;
    int32_t height = 0;

    const auto lineCallback = [&tiles, &width, &height](const std::string& line) {
        if (!line.empty()) {
            width = static_cast<int32_t>(line.size());
            ++height;
            tiles.append(line);
        }
    };
    parseInput(filePath, lineCallback);
    return Grid<char>(width, height, {tiles.begin(), tiles.end()});
}

bool isMovePossible(const Point& target, const Grid<char>& grid) {
    return target.x >= 0 && target.x < grid.width() && target.y >= 0 && target.y < grid.height();
}

bool isMovePossible(const Point& target, const Grid<char>& valueGrid, const Grid<bool>& visitedGrid, const char currentValue) {
    if (target.x < 0 || target.x >= valueGrid.width() || target.y < 0 || target.y >= valueGrid.height()) {
        return false;
    }

    if (visitedGrid.at(target)) {
        return false;
    }

    auto targetValue = valueGrid.at(target);
    if (targetValue == 'S') {
        targetValue = 'a';
    }

    return targetValue >= currentValue - 1;
}

int32_t findShortestPath(const Grid<char>& grid, const char destinationValue) {
    const std::array<Point, 4> possibleMoves{Point(1, 0), Point(-1, 0), Point(0, 1), Point(0, -1)};
    const auto end = grid.find('E');
    if (!end.has_value()) {
        throw std::runtime_error("End not found");
    }

    Grid<bool> visited(grid.width(), grid.height(), false);
    visited.set(end->x, end->y, true);

    std::queue<Step> route;
    route.push({0, *end});

    while (!route.empty()) {
        const auto& currentStep = route.front();
        const auto currentPosition = currentStep.position;
        const auto currentValue = grid.at(currentPosition);

        if (currentValue == destinationValue) {
            return currentStep.distance;
        }

        for (const auto& possibleMove : possibleMoves) {
            const Point target(currentPosition.x + possibleMove.x, currentPosition.y + possibleMove.y);

            if (isMovePossible(target, grid, visited, currentValue)) {
                visited.set(target, true);
                route.push({currentStep.distance + 1, target});
            }
        }

        route.pop();
    }

    return -1;
}
}  // namespace

std::pair<std::string, std::string> day12() {
    const auto input = parse("resources/day12.txt");

    const auto shortestPathPart1 = findShortestPath(input, 'S');
    const auto shortestPathPart2 = findShortestPath(input, 'a');

    return {std::to_string(shortestPathPart1), std::to_string(shortestPathPart2)};
}
}  // namespace bblp::aoc
