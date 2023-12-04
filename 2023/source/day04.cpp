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
struct Scratchcard {
    uint32_t id;
    uint32_t instances;
    std::vector<uint32_t> winningNumbers;
    std::vector<uint32_t> scratchedNumbers;
};

std::vector<uint32_t> parseNumbers(const std::string& str) {
    const auto parts = split(trim(str), " ");
    std::vector<uint32_t> numbers;
    for (auto iter = parts.cbegin(); iter != parts.cend(); ++iter) {
        if (iter->empty()) {
            continue;
        }
        numbers.emplace_back(std::stoul(*iter));
    }
    return numbers;
}

Scratchcard parseScratchcard(const std::string& line) {
    const auto parts = split(line, ":");
    const auto id = std::stoul(split(parts.at(0), " ").back());
    const auto numberParts = split(parts.at(1), "|");

    return Scratchcard{id, 1U, parseNumbers(numberParts.at(0)), parseNumbers(numberParts.at(1))};
}

auto parse(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    std::vector<Scratchcard> input;
    input.reserve(BUFFER_SIZE);
    const auto lineCallback = [&input](const std::string& line) { input.push_back(parseScratchcard(line)); };
    parseInput(filePath, lineCallback);
    return input;
}

uint32_t calculatePartOne(const std::vector<Scratchcard>& input) {
    uint32_t result{0U};
    for (const auto& card : input) {
        uint32_t cardScore{0U};
        for (const auto number : card.scratchedNumbers) {
            if (std::find(card.winningNumbers.cbegin(), card.winningNumbers.cend(), number) !=
                card.winningNumbers.cend()) {
                if (cardScore == 0U) {
                    cardScore = 1U;
                } else {
                    cardScore *= 2;
                }
            }
        }
        result += cardScore;
    }
    return result;
}

uint64_t calculatePartTwo(const std::vector<Scratchcard>& input) {
    std::vector<Scratchcard> copy{input.cbegin(), input.cend()};

    for (const auto& card : copy) {
        uint32_t cardScore{0U};
        for (const auto number : card.scratchedNumbers) {
            if (std::find(card.winningNumbers.cbegin(), card.winningNumbers.cend(), number) !=
                card.winningNumbers.cend()) {
                ++cardScore;
            }
        }

        for (auto i = 0U; i < cardScore; ++i) {
            const auto winningIndex = card.id + i;
            if (winningIndex < copy.size()) {
                copy.at(winningIndex).instances += card.instances;
            } else {
                std::cout << "sadge" << '\n';
            }
        }
    }

    return std::accumulate(copy.cbegin(), copy.cend(), 0U,
                           [](uint32_t sum, const Scratchcard& card) { return sum + card.instances; });
}
}  // namespace

std::pair<std::string, std::string> day04() {
    const auto input = parse("resources/day04.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
