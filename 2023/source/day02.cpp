#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"
#include "bblp/aoc/string_utils.hpp"

#include <algorithm>
#include <cstdlib>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace bblp::aoc {
namespace {

struct Record {
    uint32_t red;
    uint32_t green;
    uint32_t blue;
};

struct Game {
    uint32_t id;
    std::vector<Record> records;
};

int parseGameId(const std::string& input) {
    const auto parts = split(input, " ");
    if (parts.size() != 2) {
        throw std::logic_error("Invalid line format");
    }

    return std::stoul(parts.back());
}

std::vector<Record> parseGameRecords(const std::string& input) {
    const auto recordStrings = split(input, ";");
    std::vector<Record> records(recordStrings.size());
    std::transform(recordStrings.cbegin(), recordStrings.cend(), records.begin(), [](const std::string& recordString) {
        Record record{0U, 0U, 0U};
        const auto revealStrings = split(recordString, ",");
        for (auto iter = revealStrings.cbegin(); iter != revealStrings.cend(); ++iter) {
            const auto str = trim(*iter);
            if (str.find("red") != std::string::npos) {
                record.red = std::stoul(split(str, " ").front());
            }
            if (str.find("green") != std::string::npos) {
                record.green = std::stoul(split(str, " ").front());
            }
            if (str.find("blue") != std::string::npos) {
                record.blue = std::stoul(split(str, " ").front());
            }
        }
        return record;
    });

    return records;
}

auto parse(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    std::vector<Game> input;
    input.reserve(BUFFER_SIZE);
    const auto lineCallback = [&input](const std::string& line) {
        const auto parts = split(line, ":");
        if (parts.size() != 2) {
            throw std::logic_error("Invalid line format");
        }
        input.emplace_back(parseGameId(parts.front()), parseGameRecords(parts.back()));
    };
    parseInput(filePath, lineCallback);
    return input;
}

bool isGamePossible(const Game& game) {
    for (auto iter = game.records.cbegin(); iter != game.records.cend(); ++iter) {
        if (iter->red > 12 || iter->green > 13 || iter->blue > 14) {
            return false;
        }
    }
    return true;
}

uint32_t findMinimalPossiblePower(const Game& game) {
    uint32_t minRed{0U};
    uint32_t minGreen{0U};
    uint32_t minBlue{0U};

    for (auto iter = game.records.cbegin(); iter != game.records.cend(); ++iter) {
        if (iter->red > minRed) {
            minRed = iter->red;
        }
        if (iter->green > minGreen) {
            minGreen = iter->green;
        }
        if (iter->blue > minBlue) {
            minBlue = iter->blue;
        }
    }
    return minRed * minGreen * minBlue;
}

uint32_t calculatePartOne(const std::vector<Game>& input) {
    uint32_t result{0U};
    for (auto iter = input.cbegin(); iter != input.cend(); ++iter) {
        if (isGamePossible(*iter)) {
            result += iter->id;
        }
    }
    return result;
}

uint32_t calculatePartTwo(const std::vector<Game>& input) {
    return std::accumulate(input.cbegin(), input.cend(), 0U,
                           [](uint32_t sum, const Game& game) { return sum + findMinimalPossiblePower(game); });
}
}  // namespace

std::pair<std::string, std::string> day02() {
    const auto input = parse("resources/day02.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
