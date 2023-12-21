#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"
#include "bblp/aoc/point.hpp"
#include "bblp/aoc/point_io.hpp"
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
#include <queue>
#include <set>
#include <stdexcept>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

namespace bblp::aoc {
namespace {

class TileMap {
  public:
    explicit TileMap(const std::vector<std::string>& data);

    const char at(const Point& point) const { return mTiles.at(point.y).at(point.x); }
    char& at(const Point& point) { return mTiles.at(point.y).at(point.x); }

    std::size_t width() const noexcept { return mTiles.front().size(); }
    std::size_t height() const noexcept { return mTiles.size(); }

  private:
    std::vector<std::string> mTiles;
};

TileMap::TileMap(const std::vector<std::string>& data) : mTiles(data) {}

enum class GardenTile : uint8_t { EMPTY = '.', ROCK = '#' };

auto parse(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE{1000U};

    std::vector<std::string> input;
    input.reserve(BUFFER_SIZE);

    const auto lineCallback = [&input](const std::string& line) {
        if (!line.empty()) {
            input.push_back(line);
        }
    };
    parseInput(filePath, lineCallback);
    return TileMap(input);
}

Point findStartingPosition(const TileMap& input) {
    for (auto y = 0U; y < input.height(); ++y) {
        for (auto x = 0U; x < input.width(); ++x) {
            if (input.at({x, y}) == 'S') {
                return {x, y};
            }
        }
    }
    throw std::logic_error("Starting position not found");
}

void depthFirstSearch(const TileMap& input,
                      const Point& from,
                      const Point& to,
                      const uint32_t maxDistance,
                      const int32_t currentDistance,
                      std::unordered_map<Point, uint32_t, PointHash>& visited) {
    auto iter = visited.find(from);
    if (iter != visited.end() && currentDistance <= iter->second) {
        return;
    }

    if (from.x < 0 || from.x >= input.width() || from.y < 0 || from.y >= input.height()) {
        return;
    }

    if (currentDistance > maxDistance) {
        return;
    }

    if (input.at(from) == '#') {
        return;
    }

    if ((from.x + from.y) % 2 == 0) {
        visited[from] = currentDistance;
    }

    const auto up = Point(from.x, from.y - 1);
    const auto down = Point(from.x, from.y + 1);
    const auto left = Point(from.x - 1, from.y);
    const auto right = Point(from.x + 1, from.y);
    depthFirstSearch(input, up, to, maxDistance, currentDistance - 1, visited);
    depthFirstSearch(input, down, to, maxDistance, currentDistance - 1, visited);
    depthFirstSearch(input, left, to, maxDistance, currentDistance - 1, visited);
    depthFirstSearch(input, right, to, maxDistance, currentDistance - 1, visited);
}

uint64_t calculatePartOne(const TileMap& input) {
    static constexpr uint32_t numberOfSteps{64U};

    const auto startingPos = findStartingPosition(input);
    std::unordered_map<Point, uint32_t, PointHash> visited;
    depthFirstSearch(input, startingPos, startingPos, numberOfSteps, numberOfSteps, visited);

    return visited.size();
}

uint64_t calculatePartTwo(const TileMap& input) {
    return 0U;
}
}  // namespace

std::pair<std::string, std::string> day21() {
    const auto input = parse("resources/day21.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
