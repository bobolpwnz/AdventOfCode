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
enum class TileType : uint8_t {
    GROUND = 0,
    PIPE_VERTICAL,
    PIPE_HORIZONTAL,
    PIPE_CORNER_TOP_LEFT,
    PIPE_CORNER_TOP_RIGHT,
    PIPE_CORNER_BOTTOM_LEFT,
    PIPE_CORNER_BOTTOM_RIGHT,
    STARTING_POSITION
};

enum class LoopMembershipType : uint8_t { UNKNOWN, PART_OF_LOOP, INSIDE_OF_LOOP, OUTSIDE_OF_LOOP };

TileType charToTileType(const char c) {
    switch (c) {
        case '.':
            return TileType::GROUND;
        case '|':
            return TileType::PIPE_VERTICAL;
        case '-':
            return TileType::PIPE_HORIZONTAL;
        case 'L':
            return TileType::PIPE_CORNER_TOP_RIGHT;
        case 'J':
            return TileType::PIPE_CORNER_TOP_LEFT;
        case '7':
            return TileType::PIPE_CORNER_BOTTOM_LEFT;
        case 'F':
            return TileType::PIPE_CORNER_BOTTOM_RIGHT;
        case 'S':
            return TileType::STARTING_POSITION;
    }
    throw std::logic_error("Invalid tile type");
}

auto parse(const std::filesystem::path& filePath) {
    std::vector<std::vector<TileType>> input;
    const auto lineCallback = [&input](const std::string& line) {
        std::vector<TileType> row(line.size());
        std::transform(line.cbegin(), line.cend(), row.begin(), [](const char c) { return charToTileType(c); });
        input.push_back(row);
    };
    parseInput(filePath, lineCallback);
    return input;
}

Point findStartingPosition(const std::vector<std::vector<TileType>>& input) {
    for (auto y = 0U; y < input.size(); ++y) {
        for (auto x = 0U; x < input.at(y).size(); ++x) {
            if (input.at(y).at(x) == TileType::STARTING_POSITION) {
                return {x, y};
            }
        }
    }
    throw std::logic_error("Starting position not found");
}

std::pair<Point, Point> findAvailableMoves(const std::vector<std::vector<TileType>>& input, const Point& point) {
    const auto tileType = input.at(point.y).at(point.x);
    switch (tileType) {
        case TileType::PIPE_VERTICAL:
            return {{point.x, point.y - 1}, {point.x, point.y + 1}};
        case TileType::PIPE_HORIZONTAL:
            return {{point.x - 1, point.y}, {point.x + 1, point.y}};
        case TileType::PIPE_CORNER_BOTTOM_LEFT:
            return {{point.x, point.y + 1}, {point.x - 1, point.y}};
        case TileType::PIPE_CORNER_BOTTOM_RIGHT:
            return {{point.x, point.y + 1}, {point.x + 1, point.y}};
        case TileType::PIPE_CORNER_TOP_LEFT:
            return {{point.x, point.y - 1}, {point.x - 1, point.y}};
        case TileType::PIPE_CORNER_TOP_RIGHT:
            return {{point.x, point.y - 1}, {point.x + 1, point.y}};
        case TileType::STARTING_POSITION: {
            std::vector<Point> points;
            const auto left = input.at(point.y).at(point.x - 1);
            const auto right = input.at(point.y).at(point.x + 1);
            const auto top = input.at(point.y - 1).at(point.x);
            const auto bottom = input.at(point.y + 1).at(point.x);
            if (left == TileType::PIPE_HORIZONTAL || left == TileType::PIPE_CORNER_BOTTOM_RIGHT ||
                left == TileType::PIPE_CORNER_TOP_RIGHT) {
                points.push_back({point.x - 1, point.y});
            }
            if (right == TileType::PIPE_HORIZONTAL || right == TileType::PIPE_CORNER_BOTTOM_LEFT ||
                right == TileType::PIPE_CORNER_TOP_LEFT) {
                points.push_back({point.x + 1, point.y});
            }
            if (top == TileType::PIPE_VERTICAL || top == TileType::PIPE_CORNER_BOTTOM_LEFT ||
                top == TileType::PIPE_CORNER_BOTTOM_RIGHT) {
                points.push_back({point.x, point.y - 1});
            }
            if (bottom == TileType::PIPE_VERTICAL || bottom == TileType::PIPE_CORNER_TOP_LEFT ||
                bottom == TileType::PIPE_CORNER_TOP_RIGHT) {
                points.push_back({point.x, point.y + 1});
            }
            return {points.front(), points.back()};
        }
    }
    throw std::logic_error("?");
}

Point moveToNextTile(const std::vector<std::vector<TileType>>& input,
                     const Point& previousPosition,
                     const Point& currentPosition) {
    const auto availableMoves = findAvailableMoves(input, currentPosition);
    if (previousPosition != availableMoves.first) {
        return availableMoves.first;
    } else if (previousPosition != availableMoves.second) {
        return availableMoves.second;
    } else {
        throw std::logic_error("I've got lost, pepeHands");
    }
}

uint64_t calculatePartOne(const std::vector<std::vector<TileType>>& input) {
    const auto startingPosition = findStartingPosition(input);
    const auto availableStartingMoves = findAvailableMoves(input, startingPosition);
    auto previousPosition = startingPosition;
    auto currentPosition = startingPosition;
    uint64_t result{0U};
    do {
        const auto nextPosition = moveToNextTile(input, previousPosition, currentPosition);
        previousPosition = currentPosition;
        currentPosition = nextPosition;

        ++result;
    } while (input.at(currentPosition.y).at(currentPosition.x) != TileType::STARTING_POSITION);

    return result / 2U;
}

void markRouteAsPartOfLoop(std::vector<std::vector<LoopMembershipType>>& loopMembership,
                           const Point& currentPosition,
                           const Point& previousPosition) {
    loopMembership.at(2 * currentPosition.y).at(2 * currentPosition.x) = LoopMembershipType::PART_OF_LOOP;

    const auto yDiff = currentPosition.y - previousPosition.y;
    if (yDiff > 0) {
        loopMembership.at(2 * currentPosition.y - 1).at(2 * currentPosition.x) = LoopMembershipType::PART_OF_LOOP;
    } else if (yDiff < 0) {
        loopMembership.at(2 * currentPosition.y + 1).at(2 * currentPosition.x) = LoopMembershipType::PART_OF_LOOP;
    }

    const auto xDiff = currentPosition.x - previousPosition.x;
    if (xDiff > 0) {
        loopMembership.at(2 * currentPosition.y).at(2 * currentPosition.x - 1) = LoopMembershipType::PART_OF_LOOP;
    } else if (xDiff < 0) {
        loopMembership.at(2 * currentPosition.y).at(2 * currentPosition.x + 1) = LoopMembershipType::PART_OF_LOOP;
    }
}

int64_t findNumberOfLoopPartsAbove(const std::vector<std::vector<LoopMembershipType>>& loopMembership,
                                   const Point& point) {
    int64_t result{0};
    Point currentPoint{point};
    while (currentPoint.y >= 0) {
        const auto type = loopMembership.at(currentPoint.y).at(currentPoint.x);
        if (type == LoopMembershipType::PART_OF_LOOP) {
            ++result;
        }
        --currentPoint.y;
    }
    return result;
}

int64_t findNumberOfLoopPartsBelow(const std::vector<std::vector<LoopMembershipType>>& loopMembership,
                                   const Point& point) {
    int64_t result{0};
    Point currentPoint{point};
    while (currentPoint.y < loopMembership.size()) {
        const auto type = loopMembership.at(currentPoint.y).at(currentPoint.x);
        if (type == LoopMembershipType::PART_OF_LOOP) {
            ++result;
        }
        ++currentPoint.y;
    }
    return result;
}

int64_t findNumberOfLoopPartsLeft(const std::vector<std::vector<LoopMembershipType>>& loopMembership,
                                  const Point& point) {
    int64_t result{0};
    Point currentPoint{point};
    while (currentPoint.x >= 0) {
        const auto type = loopMembership.at(currentPoint.y).at(currentPoint.x);
        if (type == LoopMembershipType::PART_OF_LOOP) {
            ++result;
        }
        --currentPoint.x;
    }
    return result;
}

int64_t findNumberOfLoopPartsRight(const std::vector<std::vector<LoopMembershipType>>& loopMembership,
                                   const Point& point) {
    int64_t result{0};
    Point currentPoint{point};
    while (currentPoint.x < loopMembership.at(currentPoint.y).size()) {
        const auto type = loopMembership.at(currentPoint.y).at(currentPoint.x);
        if (type == LoopMembershipType::PART_OF_LOOP) {
            ++result;
        }
        ++currentPoint.x;
    }
    return result;
}

void markOutsideTilesAsNotPartOfLoop(std::vector<std::vector<LoopMembershipType>>& loopMembership) {
    for (auto y = 0U; y < loopMembership.size(); ++y) {
        for (auto x = 0U; x < loopMembership.at(y).size(); ++x) {
            const auto part = loopMembership.at(y).at(x);
            if (part == LoopMembershipType::PART_OF_LOOP) {
                continue;
            }

            const auto loopPartsAbove = findNumberOfLoopPartsAbove(loopMembership, {x, y});
            const auto loopPartsBelow = findNumberOfLoopPartsBelow(loopMembership, {x, y});
            const auto loopPartsLeft = findNumberOfLoopPartsLeft(loopMembership, {x, y});
            const auto loopPartsRight = findNumberOfLoopPartsRight(loopMembership, {x, y});

            if (loopPartsAbove == 0 || loopPartsBelow == 0 || loopPartsLeft == 0 || loopPartsRight == 0) {
                loopMembership.at(y).at(x) = LoopMembershipType::OUTSIDE_OF_LOOP;
            }
        }
    }
}

void fillFromOutside(std::vector<std::vector<LoopMembershipType>>& loopMembership) {
    int64_t numberChanged{0};
    do {
        numberChanged = 0;
        for (auto y = 0U; y < loopMembership.size(); ++y) {
            for (auto x = 0U; x < loopMembership.at(y).size(); ++x) {
                if (loopMembership.at(y).at(x) != LoopMembershipType::OUTSIDE_OF_LOOP) {
                    continue;
                }

                if (x > 0) {
                    const Point next{x - 1, y};
                    if (loopMembership.at(next.y).at(next.x) == LoopMembershipType::UNKNOWN) {
                        loopMembership.at(next.y).at(next.x) = LoopMembershipType::OUTSIDE_OF_LOOP;
                        ++numberChanged;
                    }
                }
                if (x < loopMembership.at(y).size() - 1) {
                    const Point next{x + 1, y};
                    if (loopMembership.at(next.y).at(next.x) == LoopMembershipType::UNKNOWN) {
                        loopMembership.at(next.y).at(next.x) = LoopMembershipType::OUTSIDE_OF_LOOP;
                        ++numberChanged;
                    }
                }
                if (y > 0) {
                    const Point next{x, y - 1};
                    if (loopMembership.at(next.y).at(next.x) == LoopMembershipType::UNKNOWN) {
                        loopMembership.at(next.y).at(next.x) = LoopMembershipType::OUTSIDE_OF_LOOP;
                        ++numberChanged;
                    }
                }
                if (y < loopMembership.size() - 1) {
                    const Point next{x, y + 1};
                    if (loopMembership.at(next.y).at(next.x) == LoopMembershipType::UNKNOWN) {
                        loopMembership.at(next.y).at(next.x) = LoopMembershipType::OUTSIDE_OF_LOOP;
                        ++numberChanged;
                    }
                }
            }
        }
    } while (numberChanged != 0);
}

void markTilesInsideOfLoop(std::vector<std::vector<LoopMembershipType>>& loopMembership) {
    for (auto y = 0U; y < loopMembership.size() / 2; ++y) {
        for (auto x = 0U; x < loopMembership.at(y).size() / 2; ++x) {
            if (loopMembership.at(2 * y).at(2 * x) == LoopMembershipType::UNKNOWN) {
                loopMembership.at(2 * y).at(2 * x) = LoopMembershipType::INSIDE_OF_LOOP;
            }
        }
    }
}

int64_t calculatePartTwo(const std::vector<std::vector<TileType>>& input) {
    std::vector<std::vector<LoopMembershipType>> loopMembership(
        input.size() * 2, std::vector<LoopMembershipType>(input.front().size() * 2));

    const auto startingPosition = findStartingPosition(input);
    const auto availableStartingMoves = findAvailableMoves(input, startingPosition);
    auto previousPosition = startingPosition;
    auto currentPosition = startingPosition;

    do {
        markRouteAsPartOfLoop(loopMembership, currentPosition, previousPosition);
        const auto nextPosition = moveToNextTile(input, previousPosition, currentPosition);
        previousPosition = currentPosition;
        currentPosition = nextPosition;
    } while (input.at(currentPosition.y).at(currentPosition.x) != TileType::STARTING_POSITION);

    markRouteAsPartOfLoop(loopMembership, currentPosition, previousPosition);
    markOutsideTilesAsNotPartOfLoop(loopMembership);
    fillFromOutside(loopMembership);
    markTilesInsideOfLoop(loopMembership);

    return std::accumulate(
        loopMembership.cbegin(), loopMembership.cend(), 0LL,
        [](int64_t sum, const std::vector<LoopMembershipType>& row) {
            return sum +
                   std::accumulate(row.cbegin(), row.cend(), 0LL, [](int64_t rowSum, const LoopMembershipType type) {
                       return rowSum + (type == LoopMembershipType::INSIDE_OF_LOOP ? 1 : 0);
                   });
        });
}
}  // namespace

std::pair<std::string, std::string> day10() {
    const auto input = parse("resources/day10.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
