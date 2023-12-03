#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"
#include "bblp/aoc/point.hpp"
#include "bblp/aoc/string_utils.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace bblp::aoc {
namespace {

struct Number {
    Point position;
    std::string value;
};

struct Symbol {
    Point position;
    std::string value;
};


struct GearPart {
    Point position;
    std::vector<Number> parts;
};

auto parse(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    std::vector<std::string> input;
    input.reserve(BUFFER_SIZE);
    const auto lineCallback = [&input](const std::string& line) { input.push_back(line); };
    parseInput(filePath, lineCallback);
    return input;
}

bool isNumber(const char c) {
    return c >= '0' && c <= '9';
}

bool isSymbol(const char c) {
    return !isNumber(c) && c != '.';
}

std::vector<Number> findAllNumbers(const std::vector<std::string>& input) {
    std::vector<Number> numbers;
    for (std::size_t y = 0U; y < input.size(); ++y) {
        const auto& row = input.at(y);

        for (std::size_t x = 0U; x < row.size(); ++x) {
            if (isNumber(row.at(x))) {
                const auto numberStart = x;
                std::string number{""};
                while (x < row.size() && isNumber(row.at(x))) {
                    number += row.at(x);
                    ++x;
                }
                numbers.emplace_back(Point{numberStart, y}, number);
            }
        }
    }
    return numbers;
}

bool isAdjacentToSymbol(const std::vector<std::string>& input, const Number& number) {
    const auto& start = number.position;
    const auto length = number.value.length();

    if (start.y > 0) {
        const auto& row = input.at(start.y - 1);
        for (int64_t x = start.x - 1; x <= start.x + length; ++x) {
            if (x < 0 || x >= row.size()) {
                continue;
            }

            if (isSymbol(row.at(x))) {
                return true;
            }
        }
    }
    if (start.y < static_cast<int64_t>(input.size()) - 1) {
        const auto& row = input.at(start.y + 1);
        for (int64_t x = start.x - 1; x <= start.x + length; ++x) {
            if (x < 0 || x >= row.size()) {
                continue;
            }

            if (isSymbol(row.at(x))) {
                return true;
            }
        }
    }

    const auto& row = input.at(start.y);
    if (start.x > 0 && isSymbol(row.at(start.x - 1))) {
        return true;
    }
    if (start.x < row.size() - static_cast<int64_t>(length) - 1 && isSymbol(row.at(start.x + length))) {
        return true;
    }
    return false;
}

std::vector<Symbol> findAdjacentSymbols(const std::vector<std::string>& input, const Number& number) {
    std::vector<Symbol> adjacentSymbols;

    const auto& start = number.position;
    const auto length = number.value.length();

    if (start.y > 0) {
        const auto& row = input.at(start.y - 1);
        for (int64_t x = start.x - 1; x <= start.x + length; ++x) {
            if (x < 0 || x >= row.size()) {
                continue;
            }

            if (isSymbol(row.at(x))) {
                adjacentSymbols.emplace_back(Point{x, start.y - 1}, std::string{row.at(x)});
            }
        }
    }
    if (start.y < static_cast<int64_t>(input.size()) - 1) {
        const auto& row = input.at(start.y + 1);
        for (int64_t x = start.x - 1; x <= start.x + length; ++x) {
            if (x < 0 || x >= row.size()) {
                continue;
            }

            if (isSymbol(row.at(x))) {
                adjacentSymbols.emplace_back(Point{x, start.y + 1}, std::string{row.at(x)});
            }
        }
    }

    const auto& row = input.at(start.y);
    if (start.x > 0 && isSymbol(row.at(start.x - 1))) {
        adjacentSymbols.emplace_back(Point{start.x - 1, start.y}, std::string{row.at(start.x - 1)});
    }
    if (start.x < row.size() - static_cast<int64_t>(length) - 1 && isSymbol(row.at(start.x + length))) {
        adjacentSymbols.emplace_back(Point{start.x + length, start.y}, std::string{row.at(start.x + length)});
    }
    return adjacentSymbols;
}

uint32_t calculatePartOne(const std::vector<std::string>& input) {
    uint32_t result{0U};
    const auto numbers = findAllNumbers(input);
    for (const auto& number : numbers) {
        if (isAdjacentToSymbol(input, number)) {
            result += std::stoul(number.value);
        }
    }
    return result;
}

uint64_t calculatePartTwo(const std::vector<std::string>& input) {
    uint64_t result{0U};
    const auto numbers = findAllNumbers(input);
    std::vector<GearPart> allGears;
    for (const auto& number : numbers) {
        const auto adjacentSymbols = findAdjacentSymbols(input, number);
        for (const auto& symbol : adjacentSymbols) {
            if (symbol.value != "*") {
                continue;
            }

            auto iter = std::find_if(allGears.begin(), allGears.end(), [&symbol](const GearPart& gear) {
                return symbol.position == gear.position;
            });

            if (iter == allGears.end()) {
                allGears.push_back(GearPart{symbol.position, {number}});
            } else {
                iter->parts.push_back(number);
            }
        }
    }

    for (const auto& gear : allGears) {
        if (gear.parts.size() == 2U) {
            result += (static_cast<uint64_t>(std::stoi(gear.parts.at(0).value)) * std::stoi(gear.parts.at(1).value));
        }
    }

    return result;
}
}  // namespace

std::pair<std::string, std::string> day03() {
    const auto input = parse("resources/day03.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
