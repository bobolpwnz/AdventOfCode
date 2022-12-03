#include "day02.hpp"
#include "common/file_utils.hpp"

#include <algorithm>
#include <numeric>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace bblp::advent_of_code_2022 {
namespace {
enum class Pick : uint32_t { ROCK = 1, PAPER = 2, SCIZORS = 3 };
enum class Result : uint32_t { LOSS, DRAW, WIN };

constexpr uint32_t DRAW_BONUS = 3;
constexpr uint32_t WIN_BONUS = 6;

std::vector<std::pair<char, char>> parseRounds(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    std::vector<std::pair<char, char>> rounds;
    rounds.reserve(BUFFER_SIZE);
    const auto lineCallback = [&rounds](const std::string& line) {
        std::istringstream iss(line);
        char their = '\0';
        char mine = '\0';
        iss >> their >> mine;
        rounds.emplace_back(their, mine);
    };
    parseInput(filePath, lineCallback);
    return rounds;
}

template <typename TheirType, typename MineType>
std::vector<std::pair<TheirType, MineType>> transformInput(const std::vector<std::pair<char, char>>& input,
                                                           const std::unordered_map<char, TheirType>& theirMappings,
                                                           const std::unordered_map<char, MineType>& mineMappings) {
    std::vector<std::pair<TheirType, MineType>> transformed{};
    transformed.reserve(input.size());
    const auto transformFunc = [&theirMappings, &mineMappings](const auto pair) {
        return std::pair<TheirType, MineType>(theirMappings.at(pair.first), mineMappings.at(pair.second));
    };
    std::transform(input.cbegin(), input.cend(), std::back_inserter(transformed), transformFunc);
    return transformed;
}

bool isWinning(const Pick theirPick, const Pick minePick) {
    return (theirPick == Pick::PAPER && minePick == Pick::SCIZORS) ||
           (theirPick == Pick::ROCK && minePick == Pick::PAPER) ||
           (theirPick == Pick::SCIZORS && minePick == Pick::ROCK);
}

Pick losingPick(const Pick theirPick) {
    switch (theirPick) {
        case Pick::ROCK:
            return Pick::SCIZORS;
        case Pick::PAPER:
            return Pick::ROCK;
        case Pick::SCIZORS:
            return Pick::PAPER;
        default:
            throw std::runtime_error("Invalid pick");
    }
};

Pick winningPick(const Pick theirPick) {
    switch (theirPick) {
        case Pick::ROCK:
            return Pick::PAPER;
        case Pick::PAPER:
            return Pick::SCIZORS;
        case Pick::SCIZORS:
            return Pick::ROCK;
        default:
            throw std::runtime_error("Invalid pick");
    }
};

uint32_t calculateTotalScorePart1(const std::vector<std::pair<Pick, Pick>>& rounds) {
    const auto calculateRoundScore = [](const uint32_t sum, const std::pair<Pick, Pick>& round) {
        if (round.first == round.second) {
            return sum + static_cast<uint32_t>(round.second) + DRAW_BONUS;
        } else if (isWinning(round.first, round.second)) {
            return sum + static_cast<uint32_t>(round.second) + WIN_BONUS;
        } else {
            return sum + static_cast<uint32_t>(round.second);
        }
    };

    return std::accumulate(rounds.cbegin(), rounds.cend(), 0U, calculateRoundScore);
}

uint32_t calculateTotalScorePart2(const std::vector<std::pair<Pick, Result>>& rounds) {
    const auto calculateRoundScore = [](const uint32_t sum, const std::pair<Pick, Result>& round) {
        switch (round.second) {
            case Result::LOSS:
                return sum + static_cast<uint32_t>(losingPick(round.first));
            case Result::DRAW:
                return sum + static_cast<uint32_t>(round.first) + DRAW_BONUS;
            case Result::WIN:
                return sum + static_cast<uint32_t>(winningPick(round.first)) + WIN_BONUS;
            default:
                throw std::runtime_error("Invalid round result");
        }
    };
    
    return std::accumulate(rounds.cbegin(), rounds.cend(), 0U, calculateRoundScore);
}
}  // namespace

std::tuple<int64_t, int64_t> day02() {
    const std::unordered_map<char, Pick> theirPickMappings = {
        {'A', Pick::ROCK}, {'B', Pick::PAPER}, {'C', Pick::SCIZORS}};
    const std::unordered_map<char, Pick> minePickMappings = {
        {'X', Pick::ROCK}, {'Y', Pick::PAPER}, {'Z', Pick::SCIZORS}};
    const std::unordered_map<char, Result> mineResultMappings = {
        {'X', Result::LOSS}, {'Y', Result::DRAW}, {'Z', Result::WIN}};

    const auto input = parseRounds("resources/day02/input.txt");

    const auto inputPart1 = transformInput(input, theirPickMappings, minePickMappings);
    const auto totalScorePart1 = calculateTotalScorePart1(inputPart1);

    const auto inputPart2 = transformInput(input, theirPickMappings, mineResultMappings);
    const auto totalScorePart2 = calculateTotalScorePart2(inputPart2);

    return {totalScorePart1, totalScorePart2};
}
}  // namespace bblp::advent_of_code_2022
