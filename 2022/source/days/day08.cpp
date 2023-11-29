#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"
#include "bblp/aoc/grid.hpp"

#include <algorithm>
#include <cstdlib>
#include <numeric>
#include <vector>

namespace bblp::aoc {
namespace {

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

bool isTreeVisibleFromLeft(const int32_t x, const int32_t y, const Grid<char>& grid) {
    char largestTree = -1;
    for (auto index = 0; index < x; ++index) {
        if (grid.at(index, y) > largestTree) {
            largestTree = grid.at(index, y);
        }
    }
    return grid.at(x, y) > largestTree;
}

bool isTreeVisibleFromRight(const int32_t x, const int32_t y, const Grid<char>& grid) {
    char largestTree = -1;
    for (auto index = grid.width() - 1; index > x; --index) {
        if (grid.at(index, y) > largestTree) {
            largestTree = grid.at(index, y);
        }
    }
    return grid.at(x, y) > largestTree;
}

bool isTreeVisibleFromTop(const int32_t x, const int32_t y, const Grid<char>& grid) {
    char largestTree = -1;
    for (auto index = 0; index < y; ++index) {
        if (grid.at(x, index) > largestTree) {
            largestTree = grid.at(x, index);
        }
    }
    return grid.at(x, y) > largestTree;
}

bool isTreeVisibleFromBottom(const int32_t x, const int32_t y, const Grid<char>& grid) {
    char largestTree = -1;
    for (auto index = grid.height() - 1; index > y; --index) {
        if (grid.at(x, index) > largestTree) {
            largestTree = grid.at(x, index);
        }
    }
    return grid.at(x, y) > largestTree;
}

bool isTreeVisible(const int32_t x, int32_t y, const Grid<char>& grid) {
    if (x == 0 || y == 0 || x == grid.width() - 1 || y == grid.height() - 1) {
        return true;
    }

    return isTreeVisibleFromLeft(x, y, grid) || isTreeVisibleFromRight(x, y, grid) ||
           isTreeVisibleFromTop(x, y, grid) || isTreeVisibleFromBottom(x, y, grid);
}

int32_t countVisibleTrees(const Grid<char>& grid) {
    int32_t numberOfVisibleTrees = 0;
    for (auto x = 0; x < grid.width(); ++x) {
        for (auto y = 0; y < grid.height(); ++y) {
            if (isTreeVisible(x, y, grid)) {
                ++numberOfVisibleTrees;
            }
        }
    }
    return numberOfVisibleTrees;
}

int32_t findNumberOfTreesVisibleLeft(const int32_t x, const int32_t y, const Grid<char>& grid) {
    int32_t numberOfTreesVisible = 0;
    for (auto index = x - 1; index >= 0; --index) {
        ++numberOfTreesVisible;
        if (grid.at(index, y) >= grid.at(x, y)) {
            break;
        }
    }
    return numberOfTreesVisible;
}

int32_t findNumberOfTreesVisibleRight(const int32_t x, const int32_t y, const Grid<char>& grid) {
    int32_t numberOfTreesVisible = 0;
    for (auto index = x + 1; index < grid.width(); ++index) {
        ++numberOfTreesVisible;
        if (grid.at(index, y) >= grid.at(x, y)) {
            break;
        }
    }
    return numberOfTreesVisible;
}

int32_t findNumberOfTreesVisibleTop(const int32_t x, const int32_t y, const Grid<char>& grid) {
    int32_t numberOfTreesVisible = 0;
    for (auto index = y - 1; index >= 0; --index) {
        ++numberOfTreesVisible;
        if (grid.at(x, index) >= grid.at(x, y)) {
            break;
        }
    }
    return numberOfTreesVisible;
}

int32_t findNumberOfTreesVisibleBottom(const int32_t x, const int32_t y, const Grid<char>& grid) {
    int32_t numberOfTreesVisible = 0;
    for (auto index = y + 1; index < grid.height(); ++index) {
        ++numberOfTreesVisible;
        if (grid.at(x, index) >= grid.at(x, y)) {
            break;
        }
    }
    return numberOfTreesVisible;
}

int32_t calculateScenicScore(const int32_t x, const int32_t y, const Grid<char>& grid) {
    if (x == 0 || y == 0) {
        return 0;
    }

    return findNumberOfTreesVisibleLeft(x, y, grid) * findNumberOfTreesVisibleRight(x, y, grid) *
           findNumberOfTreesVisibleTop(x, y, grid) * findNumberOfTreesVisibleBottom(x, y, grid);
}

int32_t findHighestScenicScore(const Grid<char>& grid) {
    int32_t highestScenicScore = 0;
    for (auto x = 0; x < grid.width(); ++x) {
        for (auto y = 0; y < grid.height(); ++y) {
            const int32_t currentScore = calculateScenicScore(x, y, grid);
            if (currentScore > highestScenicScore) {
                highestScenicScore = currentScore;
            }
        }
    }
    return highestScenicScore;
}
}  // namespace

std::pair<std::string, std::string> day08() {
    const auto input = parse("resources/day08.txt");
    const auto treesVisible = countVisibleTrees(input);
    const auto highestScenicScore = findHighestScenicScore(input);

    return {std::to_string(treesVisible), std::to_string(highestScenicScore)};
}
}  // namespace bblp::aoc
