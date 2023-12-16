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

namespace bblp::aoc {
namespace {
constexpr char TILE_EMPTY{'.'};
constexpr char TILE_SPLITTER_HORIZIONTAL{'-'};
constexpr char TILE_SPLITTER_VERTICAL{'|'};
constexpr char TILE_REFLECTOR_BACKSLASH{'\\'};
constexpr char TILE_REFLECTOR_FORWARDSLASH{'/'};

struct Move {
    Point currentPosition;
    Point previousPosition;

    bool operator<(const Move& other) const noexcept {
        return (currentPosition < other.currentPosition) ||
               ((!(other.currentPosition < currentPosition)) && (previousPosition < other.previousPosition));
    }
};

auto parse(const std::filesystem::path& filePath) {
    std::vector<std::string> input;

    const auto lineCallback = [&input](const std::string& line) { input.push_back(line); };
    parseInput(filePath, lineCallback);
    return input;
}

void moveForward(const std::vector<std::string>& input, const Point& currentPosition, Move& beam) {
    const auto xDiff = currentPosition.x - beam.previousPosition.x;
    const auto yDiff = currentPosition.y - beam.previousPosition.y;
    beam.currentPosition.x = currentPosition.x + xDiff;
    beam.currentPosition.y = currentPosition.y + yDiff;
    beam.previousPosition = currentPosition;
}

void reflectRight(const std::vector<std::string>& input, const Point& currentPosition, Move& beam) {
    const auto xDiff = currentPosition.x - beam.previousPosition.x;
    const auto yDiff = currentPosition.y - beam.previousPosition.y;
    if (std::abs(xDiff) > 0) {
        beam.currentPosition.x = currentPosition.x;
        beam.currentPosition.y = currentPosition.y + xDiff;
        beam.previousPosition = currentPosition;
    } else {
        beam.currentPosition.x = currentPosition.x + yDiff;
        beam.currentPosition.y = currentPosition.y;
        beam.previousPosition = currentPosition;
    }
}

void reflectLeft(const std::vector<std::string>& input, const Point& currentPosition, Move& beam) {
    const auto xDiff = currentPosition.x - beam.previousPosition.x;
    const auto yDiff = currentPosition.y - beam.previousPosition.y;
    if (std::abs(xDiff) > 0) {
        beam.currentPosition.x = currentPosition.x;
        beam.currentPosition.y = currentPosition.y - xDiff;
        beam.previousPosition = currentPosition;
    } else {
        beam.currentPosition.x = currentPosition.x - yDiff;
        beam.currentPosition.y = currentPosition.y;
        beam.previousPosition = currentPosition;
    }
}

void collectMoves(const std::vector<std::string>& input, const Move& startingMove, std::set<Move>& moves) {
    std::vector<Move> beams{{startingMove}};
    while (!beams.empty()) {
        auto& currentBeam = beams.front();
        const auto currentPosition{currentBeam.currentPosition};
        if (currentPosition.x < 0 || currentPosition.x >= input.front().size() || currentPosition.y < 0 ||
            currentPosition.y >= input.size()) {
            beams.erase(beams.begin());
            continue;
        }

        if (moves.contains(currentBeam)) {
            // we have already been here
            beams.erase(beams.begin());
            continue;
        } else {
            moves.insert(currentBeam);
        }

        const auto& currentTile = input.at(currentPosition.y).at(currentPosition.x);
        if (currentTile == TILE_EMPTY) {
            moveForward(input, currentPosition, currentBeam);
        } else if (currentTile == TILE_REFLECTOR_BACKSLASH) {
            reflectRight(input, currentPosition, currentBeam);
        } else if (currentTile == TILE_REFLECTOR_FORWARDSLASH) {
            reflectLeft(input, currentPosition, currentBeam);
        } else if (currentTile == TILE_SPLITTER_HORIZIONTAL) {
            const auto xDiff = currentPosition.x - currentBeam.previousPosition.x;
            const auto yDiff = currentPosition.y - currentBeam.previousPosition.y;
            if (std::abs(xDiff) > 0) {
                moveForward(input, currentPosition, currentBeam);
            } else {
                Move newBeam{currentBeam};
                reflectLeft(input, currentPosition, currentBeam);
                reflectRight(input, currentPosition, newBeam);
                beams.push_back(newBeam);
            }
        } else if (currentTile == TILE_SPLITTER_VERTICAL) {
            const auto xDiff = currentPosition.x - currentBeam.previousPosition.x;
            const auto yDiff = currentPosition.y - currentBeam.previousPosition.y;
            if (std::abs(xDiff) > 0) {
                Move newBeam{currentBeam};
                reflectLeft(input, currentPosition, currentBeam);
                reflectRight(input, currentPosition, newBeam);
                beams.push_back(newBeam);
            } else {
                moveForward(input, currentPosition, currentBeam);
            }
        }
    }
}

uint64_t calculateEnergized(const std::vector<std::string>& input, const std::set<Move>& moves) {
    std::vector<std::string> printout(input.size(), std::string(input.front().size(), '.'));
    for (const auto& move : moves) {
        printout.at(move.currentPosition.y).at(move.currentPosition.x) = '#';
    }

    uint64_t result{0U};
    for (const auto& line : printout) {
        result += std::count(line.cbegin(), line.cend(), '#');
        //std::cout << line << '\n';
    }
    return result;
}

uint64_t calculatePartOne(const std::vector<std::string>& input) {
    std::set<Move> moves;
    collectMoves(input, {{0, 0}, {-1, 0}}, moves);
    return calculateEnergized(input, moves);
}

uint64_t calculatePartTwo(const std::vector<std::string>& input) {
    std::vector<std::set<Move>> solutions;
    for (auto i = 0; i < input.front().size(); ++i)
    {
        std::set<Move> moves;
        collectMoves(input, {{0, i}, {-1, i}}, moves);
        solutions.push_back(moves);
    }

    for (auto i = 0; i < input.front().size(); ++i) {
        std::set<Move> moves;
        collectMoves(input, {{input.size() - 1, i}, {input.size(), i}}, moves);
        solutions.push_back(moves);
    }

    for (auto i = 0; i < input.size(); ++i) {
        std::set<Move> moves;
        collectMoves(input, {{i, 0}, {i, -1}}, moves);
        solutions.push_back(moves);
    }

    for (auto i = 0; i < input.size(); ++i) {
        std::set<Move> moves;
        collectMoves(input, {{i, input.front().size() - 1}, {i, input.front().size()}}, moves);
        solutions.push_back(moves);
    }

    uint64_t result{0U};
    for (const auto& moves : solutions) {
        const auto val = calculateEnergized(input, moves);
        if (result < val) {
            result = val;
        }
    }

    return result;
}
}  // namespace

std::pair<std::string, std::string> day16() {
    const auto input = parse("resources/day16.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
