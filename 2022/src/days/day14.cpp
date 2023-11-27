#include "days.hpp"

#include "common/file_utils.hpp"
#include "common/string_utils.hpp"
#include "common/grid.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <numeric>
#include <vector>

namespace bblp::advent_of_code_2022 {
namespace {

struct Line {
    [[nodiscard]] bool isHorizontal() const noexcept { return start.y == end.y; }
    [[nodiscard]] bool isVertical() const noexcept { return start.x == end.x; }

    [[nodiscard]] int64_t horizontalLength() const noexcept { return end.x - start.x + 1; }
    [[nodiscard]] int64_t verticalLength() const noexcept { return end.y - start.y + 1; }

    Point start;
    Point end;
};

Line normalize(const Point& start, const Point& end) {
    const Line line{start, end};
    if (line.isHorizontal()) {
        if (start.x <= end.x) {
            return {start, end};
        } else {
            return {end, start};
        }
    } else if (line.isVertical()) {
        if (start.y <= end.y) {
            return {start, end};
        } else {
            return {end, start};
        }
    } else {
        throw std::runtime_error("Diagonals not supported");
    }
}

Point parsePoint(const std::string& string) {
    const auto coords = split(string, ",");
    return {std::stol(coords[0]), std::stol(coords[1])};
}

auto parse(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    std::vector<Line> input;
    input.reserve(BUFFER_SIZE);
    const auto lineCallback = [&input](const std::string& line) {
        if (line.empty()) {
            return;
        }

        std::vector<Line> lines;
        const auto points = split(line, "->");
        lines.reserve(points.size());
        for (auto iter = points.cbegin(); iter != points.cend() - 1; ++iter) {
            lines.emplace_back(normalize(parsePoint(*iter), parsePoint(*(iter + 1))));
        }
        input.insert(input.end(), lines.cbegin(), lines.cend());
    };
    parseInput(filePath, lineCallback);
    return input;
}

void printDelimiter(std::ostream& stream, const int32_t step) {
    stream << "========== " << std::setw(4) << step << " ==========" << '\n';
}

void printGrid(std::ostream& stream, const Grid<char>& grid, const Point& offset, const Point& size) {
    for (auto y = 0; y < size.y; ++y) {
        for (auto x = 0; x < size.x; ++x) {
            stream << grid.at(offset.x + x, offset.y + y);
        }
        stream << '\n';
    }
}

constexpr int32_t GRID_WIDTH = 1000;
constexpr int32_t GRID_HEIGHT = 1000;
constexpr Point SAND_START(500, 0);

constexpr char TILE_AIR = '.';
constexpr char TILE_ROCK = '#';
constexpr char TILE_SAND = 'o';
constexpr char TILE_SPAWN = '+';

void markInitial(Grid<char>& grid, const std::vector<Line>& lines) {
    for (const auto& line : lines) {
        if (line.isHorizontal()) {
            for (auto x = 0; x < line.horizontalLength(); ++x) {
                grid.at(line.start.x + x, line.start.y) = TILE_ROCK;
            }
        } else if (line.isVertical()) {
            for (auto y = 0; y < line.verticalLength(); ++y) {
                grid.at(line.start.x, line.start.y + y) = TILE_ROCK;
            }
        }
    }
    grid.at(SAND_START) = TILE_SPAWN;
}

int32_t simulate(Grid<char>& grid) {
    int32_t numberOfSandSpawned = 0;
    Point sand = SAND_START;
    while (sand.y < grid.height() - 1) {
        if (sand == SAND_START) {
            ++numberOfSandSpawned;
        }

        if (grid.at(sand.x, sand.y + 1) == TILE_AIR) {
            grid.at(sand) = TILE_AIR;
            sand.y++;
            grid.at(sand) = TILE_SAND;
        } else {
            if (grid.at(sand.x - 1, sand.y + 1) == TILE_AIR) {
                grid.at(sand) = TILE_AIR;
                sand.x--;
                sand.y++;
                grid.at(sand) = TILE_SAND;
            } else if (grid.at(sand.x + 1, sand.y + 1) == TILE_AIR) {
                grid.at(sand) = TILE_AIR;
                sand.x++;
                sand.y++;
                grid.at(sand) = TILE_SAND;
            } else {
                if (sand == SAND_START) {
                    grid.at(sand) = TILE_SAND;
                    break;
                } else  {
                    grid.at(sand) = TILE_SAND;
                    sand = SAND_START;
                }
            }
        }
    }
    return numberOfSandSpawned;
}

void saveToFile(const Grid<char>& grid, const std::filesystem::path& fileName) {
    std::ofstream output(fileName);
    printGrid(output, grid, Point(0, 0), Point(GRID_WIDTH, GRID_HEIGHT));
}

int32_t calculatePart1(const std::vector<Line>& lines) {

    Grid<char> grid(GRID_WIDTH, GRID_HEIGHT, TILE_AIR);
    markInitial(grid, lines);

    const auto numberOfSandSpawned = simulate(grid);

    saveToFile(grid, "resources/day14_1.txt");

    return numberOfSandSpawned - 1;
}

int32_t calculatePart2(const std::vector<Line>& lines) {
    Grid<char> grid(GRID_WIDTH, GRID_HEIGHT, TILE_AIR);
    markInitial(grid, lines);

    const auto maxIter = std::max_element(lines.cbegin(), lines.cend(), [](const Line& lhs, const Line& rhs) {
        return (lhs.end.y < rhs.end.y);
    });

    for (auto x = 0; x < grid.width(); ++x) {
        grid.at(x, maxIter->end.y + 2) = TILE_ROCK;
    }

    const auto numberOfSandSpawned = simulate(grid);

    saveToFile(grid, "resources/day14_2.txt");

    return numberOfSandSpawned;
}
}  // namespace

std::pair<std::string, std::string> day14() {
    const auto input = parse("resources/day14.txt");
    const auto spawnedSandPart1 = calculatePart1(input);
    const auto spawnedSandPart2 = calculatePart2(input);

    return {std::to_string(spawnedSandPart1), std::to_string(spawnedSandPart2)};
}
}  // namespace bblp::advent_of_code_2022
