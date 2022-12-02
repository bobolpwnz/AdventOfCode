#include "day02.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace bblp::advent_of_code_2022 {
namespace {
enum class PickedType : uint32_t { ROCK = 1, PAPER = 2, SCIZORS = 3 };

enum class Result : uint32_t { LOSS, DRAW, WIN };

std::vector<std::pair<char, char>> parseInput(const std::filesystem::path& filePath) {
    std::ifstream file(filePath);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file");
    }

    std::string line;
    std::vector<std::pair<char, char>> rounds;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            std::istringstream iss(line);
            char their = '\0';
            char mine = '\0';
            iss >> their >> mine;
            rounds.emplace_back(their, mine);
        } else {
        }
    }
    return rounds;
}

bool isWin(PickedType theirPick, PickedType minePick) {
    return (theirPick == PickedType::PAPER && minePick == PickedType::SCIZORS) ||
           (theirPick == PickedType::ROCK && minePick == PickedType::PAPER) ||
           (theirPick == PickedType::SCIZORS && minePick == PickedType::ROCK);
}

PickedType losingPick(PickedType theirPick){
    switch (theirPick) {
        case PickedType::ROCK:
            return PickedType::SCIZORS;
        case PickedType::PAPER:
            return PickedType::ROCK;
        case PickedType::SCIZORS:
            return PickedType::PAPER;
        default:
            throw std::runtime_error("Invalid pick");
    }
};

PickedType winningPick(PickedType theirPick) {
    switch (theirPick) {
        case PickedType::ROCK:
            return PickedType::PAPER;
        case PickedType::PAPER:
            return PickedType::SCIZORS;
        case PickedType::SCIZORS:
            return PickedType::ROCK;
        default:
            throw std::runtime_error("Invalid pick");
    }
};
}  // namespace

void day02() {
    static const std::unordered_map<char, PickedType> THEIR_MAPPIGS = {
        {'A', PickedType::ROCK}, {'B', PickedType::PAPER}, {'C', PickedType::SCIZORS}};

    static const std::unordered_map<char, PickedType> MINE_PICK_MAPPIGS = {
        {'X', PickedType::ROCK}, {'Y', PickedType::PAPER}, {'Z', PickedType::SCIZORS}};

    static const std::unordered_map<char, Result> MINE_RESULT_MAPPIGS = {
        {'X', Result::LOSS}, {'Y', Result::DRAW}, {'Z', Result::WIN}};

    static constexpr uint32_t DRAW_BONUS = 3;
    static constexpr uint32_t WIN_BONUS = 6;

    const auto input = parseInput("resources/day02/input.txt");
    std::vector<std::pair<PickedType, PickedType>> part1{};
    part1.reserve(input.size());
    std::transform(input.cbegin(), input.cend(), std::back_inserter(part1), [](const auto pair) {
        return std::pair<PickedType, PickedType>(THEIR_MAPPIGS.at(pair.first), MINE_PICK_MAPPIGS.at(pair.second));
    });

    uint64_t totalScore = 0;
    for (const auto& round : part1) {
        if (round.first == round.second) {
            totalScore += DRAW_BONUS;
        } else if (isWin(round.first, round.second)) {
            totalScore += WIN_BONUS;
        }
        totalScore += static_cast<uint32_t>(round.second);
    }

    std::cout << "Total score (part 1) is " << totalScore << '\n';

    std::vector<std::pair<PickedType, Result>> part2{};
    part2.reserve(input.size());
    std::transform(input.cbegin(), input.cend(), std::back_inserter(part2), [](const auto pair) {
        return std::pair<PickedType, Result>(THEIR_MAPPIGS.at(pair.first), MINE_RESULT_MAPPIGS.at(pair.second));
    });

    uint32_t totalScorePart2 = 0;
    for (const auto& round : part2) {
        switch (round.second) {
            case Result::LOSS:
                totalScorePart2 += static_cast<uint32_t>(losingPick(round.first));
                break;
            case Result::DRAW:
                totalScorePart2 += static_cast<uint32_t>(round.first);
                totalScorePart2 += DRAW_BONUS;
                break;
            case Result::WIN:
                totalScorePart2 += static_cast<uint32_t>(winningPick(round.first));
                totalScorePart2 += WIN_BONUS;
                break;
        }
    }
    std::cout << "Total score (part 2) is " << totalScorePart2 << '\n';
}
}  // namespace bblp::advent_of_code_2022
