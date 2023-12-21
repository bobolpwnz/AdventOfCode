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
    explicit TileMap(const std::vector<std::string>& data) : mTiles(data) {}
    virtual ~TileMap() = default;

    virtual const char at(const Point& point) const = 0;
    virtual bool isValidCoords(const Point& point) const = 0;

    virtual std::size_t width() const noexcept = 0;
    virtual std::size_t height() const noexcept = 0;

  protected:
    std::vector<std::string>& tiles() noexcept { return mTiles; }
    const std::vector<std::string>& tiles() const noexcept { return mTiles; }

  private:
    std::vector<std::string> mTiles;
};

class FiniteTileMap : public TileMap {
  public:
    explicit FiniteTileMap(const std::vector<std::string>& data) : TileMap(data) {}

    const char at(const Point& point) const { return tiles().at(point.y).at(point.x); }
    bool isValidCoords(const Point& point) const {
        if (point.x < 0 || point.x >= width() || point.y < 0 || point.y >= height()) {
            return false;
        }
        return true;
    }

    std::size_t width() const noexcept { return tiles().front().size(); }
    std::size_t height() const noexcept { return tiles().size(); }
};

class InfiniteTileMap : public TileMap {
  public:
    explicit InfiniteTileMap(const std::vector<std::string>& data) : TileMap(data) {}

    const char at(const Point& point) const override {
        const auto y = point.y % foldingHeight();
        const auto x = point.x % foldingWidth();

        return tiles().at(y < 0 ? y + foldingHeight() : y).at(x < 0 ? x + foldingWidth() : x);
    }

    bool isValidCoords(const Point&) const { return true; }

    std::size_t width() const noexcept override { return ULLONG_MAX; }
    std::size_t height() const noexcept override { return ULLONG_MAX; }

  private:
    inline std::size_t foldingWidth() const noexcept { return tiles().front().size(); }
    inline std::size_t foldingHeight() const noexcept { return tiles().size(); }
};

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
    return input;
}

Point findStartingPosition(const std::vector<std::string>& input) {
    for (auto y = 0U; y < input.size(); ++y) {
        for (auto x = 0U; x < input.front().size(); ++x) {
            if (input.at(y).at(x) == 'S') {
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

struct StackElement {
    Point position;
    int32_t distance;
};

bool canMoveTo(const TileMap& map,
               const Point& to,
               const uint32_t maxDistance,
               const int32_t currentDistance,
               std::unordered_map<Point, uint32_t, PointHash>& visited) {
    auto iter = visited.find(to);
    if (iter != visited.end() && currentDistance <= iter->second) {
        return false;
    }

    if (!map.isValidCoords(to)) {
        return false;
    }

    if (currentDistance > maxDistance) {
        return false;
    }

    if (map.at(to) == '#') {
        return false;
    }

    visited[to] = currentDistance;

    return true;
}

void progressStack(const TileMap& map,
                   const uint32_t maxDistance,
                   std::unordered_map<Point, uint32_t, PointHash>& visited,
                   std::vector<StackElement>& stack) {
    const StackElement last = stack.back();
    // std::cout << last.position << " at " << last.distance << '\n';
    const auto up = Point(last.position.x, last.position.y - 1);
    const auto down = Point(last.position.x, last.position.y + 1);
    const auto left = Point(last.position.x - 1, last.position.y);
    const auto right = Point(last.position.x + 1, last.position.y);
    if (canMoveTo(map, up, maxDistance, last.distance - 1, visited)) {
        stack.push_back(StackElement{up, last.distance - 1});
    } else if (canMoveTo(map, right, maxDistance, last.distance - 1, visited)) {
        stack.push_back(StackElement{right, last.distance - 1});
    } else if (canMoveTo(map, left, maxDistance, last.distance - 1, visited)) {
        stack.push_back(StackElement{left, last.distance - 1});
    } else if (canMoveTo(map, down, maxDistance, last.distance - 1, visited)) {
        stack.push_back(StackElement{down, last.distance - 1});
    } else {
        stack.pop_back();
    }
}

void depthFirstSearch(const TileMap& input,
                      const Point& startingPos,
                      const uint32_t maxDistance,
                      std::unordered_map<Point, uint32_t, PointHash>& visited) {
    std::vector<StackElement> stack;
    stack.reserve(maxDistance);
    stack.push_back(StackElement{startingPos, static_cast<int32_t>(maxDistance)});

    while (!stack.empty()) {
        progressStack(input, maxDistance, visited, stack);
    }
}

uint64_t calculateValid(const std::unordered_map<Point, uint32_t, PointHash>& visited) {
    const auto valid = [](const Point& p) -> bool { return ((p.x + p.y) % 2 == 0); };

    return std::accumulate(visited.cbegin(), visited.cend(), 0ULL,
                           [&valid](uint64_t sum, const std::pair<Point, uint32_t>& value) {
                               return valid(value.first) ? sum + 1U : sum;
                           });
}

uint64_t calculatePartOne(const TileMap& input, const Point& startingPos) {
    static constexpr uint32_t numberOfSteps{64U};

    std::unordered_map<Point, uint32_t, PointHash> visited;
    depthFirstSearch(input, startingPos, numberOfSteps, visited);
    // depthFirstSearch(input, startingPos, startingPos, numberOfSteps, numberOfSteps, visited);

    return calculateValid(visited);
}


uint64_t calculatePartTwo(const TileMap& input, const Point& startingPos) {
    static constexpr uint32_t numberOfSteps{10U};

    std::unordered_map<Point, uint32_t, PointHash> visited;
    visited.reserve(numberOfSteps);
    depthFirstSearch(input, startingPos, numberOfSteps, visited);

    return calculateValid(visited);
}
}  // namespace

std::pair<std::string, std::string> day21() {
    const auto input = parse("resources/day21.txt");
    const auto startingPos = findStartingPosition(input);

    return {std::to_string(calculatePartOne(FiniteTileMap(input), startingPos)),
            std::to_string(calculatePartTwo(InfiniteTileMap(input), startingPos))};
}
}  // namespace bblp::aoc
