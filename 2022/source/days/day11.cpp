#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"
#include "bblp/aoc/string_utils.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <string_view>
#include <vector>

namespace bblp::aoc {
namespace {

using MonkeyId = int32_t;

struct Monkey {
    using WorryLevel = uint64_t;

    std::list<WorryLevel> items;
    std::function<WorryLevel(WorryLevel)> operation;
    std::function<bool(WorryLevel)> test;
    MonkeyId monkeyToThrowToIfTrue{0};
    MonkeyId monkeyToThrowToIfFalse{0};
    uint64_t numberOfInspectedItems{0};
    int32_t reducer{0};
};

auto parse(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10;
    static constexpr std::string_view MONKEY_STRING = "Monkey";
    static constexpr std::string_view ITEMS_STRING = "Starting items: ";
    static constexpr std::string_view OPERATION_STRING = "Operation: ";
    static constexpr std::string_view TEST_STRING = "Test: divisible by ";
    static constexpr std::string_view IF_TRUE_STRING = "If true: throw to monkey ";
    static constexpr std::string_view IF_FALSE_STRING = "If false: throw to monkey ";

    std::vector<Monkey> input;
    input.reserve(BUFFER_SIZE);
    Monkey* lastMonkey = nullptr;
    const auto lineCallback = [&input, &lastMonkey](const std::string& line) {
        if (line.empty()) {
            return;
        }

        if (line.find(MONKEY_STRING) != std::string::npos) {
            input.emplace_back();
            lastMonkey = &input.back();
        } else if (const auto itemsPos = line.find(ITEMS_STRING); itemsPos != std::string::npos) {
            const auto items = split(line.substr(itemsPos + ITEMS_STRING.size()), ",");
            for (const auto& item : items) {
                lastMonkey->items.emplace_back(std::stol(item));
            }
        } else if (const auto operationPos = line.find(OPERATION_STRING); operationPos != std::string::npos) {
            if (const auto multiPos = line.find('*'); multiPos != std::string::npos) {
                if (const auto oldPos = line.find("old", multiPos); oldPos != std::string::npos) {
                    lastMonkey->operation = [](const Monkey::WorryLevel level) -> Monkey::WorryLevel {
                        return level * level;
                    };
                } else {
                    const auto val = std::stol(line.substr(multiPos + 1));
                    lastMonkey->operation = [val](const Monkey::WorryLevel level) -> Monkey::WorryLevel {
                        return level * val;
                    };
                }
            } else if (const auto addPos = line.find('+'); addPos != std::string::npos) {
                const auto val = std::stol(line.substr(addPos + 1));
                lastMonkey->operation = [val](const Monkey::WorryLevel level) -> Monkey::WorryLevel {
                    return level + val;
                };
            }
        } else if (const auto testPos = line.find(TEST_STRING); testPos != std::string::npos) {
            const auto val = std::stol(line.substr(testPos + TEST_STRING.size()));
            lastMonkey->test = [val](const Monkey::WorryLevel level) -> bool { return level % val == 0; };
            lastMonkey->reducer = val;
        } else if (const auto ifTruePos = line.find(IF_TRUE_STRING); ifTruePos != std::string::npos) {
            const auto val = std::stol(line.substr(ifTruePos + IF_TRUE_STRING.size()));
            lastMonkey->monkeyToThrowToIfTrue = val;
        } else if (const auto ifFalsePos = line.find(IF_FALSE_STRING); ifFalsePos != std::string::npos) {
            const auto val = std::stol(line.substr(ifFalsePos + IF_FALSE_STRING.size()));
            lastMonkey->monkeyToThrowToIfFalse = val;
        }
    };
    parseInput(filePath, lineCallback);
    return input;
}

int32_t calculateReducer(const std::vector<Monkey>& monkeys) {
    return std::accumulate(monkeys.cbegin(), monkeys.cend(), 1,
                           [](const int32_t mul, const Monkey& monkey) -> int32_t { return mul * monkey.reducer; });
}

void transferItems(std::list<Monkey::WorryLevel>& destination,
                   std::list<Monkey::WorryLevel>::const_iterator begin,
                   std::list<Monkey::WorryLevel>::const_iterator end) {
    destination.insert(destination.end(), std::move(begin), std::move(end));
}

uint64_t calculateMonkeyBusiness(const std::vector<Monkey>& input,
                                 const int32_t numberOfRounds,
                                 const int32_t divisor,
                                 const int32_t reducer) {
    static constexpr std::array<int32_t, 12> roundsToPrint{1,    20,   1000, 2000, 3000, 4000,
                                                           5000, 6000, 7000, 8000, 9000, 10000};

    auto monkeys = std::vector<Monkey>(input);
    for (int i = 1; i <= numberOfRounds; ++i) {
        for (auto& monkey : monkeys) {
            monkey.numberOfInspectedItems += monkey.items.size();

            auto itemsToInspect = std::list<Monkey::WorryLevel>(monkey.items);
            auto itemsToThrowIfTrue = std::list<Monkey::WorryLevel>();
            auto itemsToThrowIfFalse = std::list<Monkey::WorryLevel>();
            for (const uint64_t item : itemsToInspect) {
                const uint64_t newItem = (monkey.operation(item) / divisor);
                if (monkey.test(newItem)) {
                    itemsToThrowIfTrue.emplace_back(newItem % reducer);
                } else {
                    itemsToThrowIfFalse.emplace_back(newItem % reducer);
                }
            }
            transferItems(monkeys[monkey.monkeyToThrowToIfTrue].items, itemsToThrowIfTrue.begin(),
                          itemsToThrowIfTrue.end());
            transferItems(monkeys[monkey.monkeyToThrowToIfFalse].items, itemsToThrowIfFalse.begin(),
                          itemsToThrowIfFalse.end());
            monkey.items.clear();
        }
    }

    std::sort(monkeys.begin(), monkeys.end(), [](const Monkey& lhs, const Monkey& rhs) {
        return lhs.numberOfInspectedItems > rhs.numberOfInspectedItems;
    });

    return monkeys.front().numberOfInspectedItems * (monkeys.begin() + 1)->numberOfInspectedItems;
}
}  // namespace

std::pair<std::string, std::string> day11() {
    const auto input = parse("resources/day11.txt");
    static constexpr int32_t roundCountPart1 = 20;
    static constexpr int32_t roundCountPart2 = 10000;
    static constexpr int32_t divisorPart1 = 3;
    static constexpr int32_t divisorPart2 = 1;

    const auto reducer = calculateReducer(input);
    const uint64_t monkeyBusinessPart1 = calculateMonkeyBusiness(input, roundCountPart1, divisorPart1, reducer);
    const uint64_t monkeyBusinessPart2 = calculateMonkeyBusiness(input, roundCountPart2, divisorPart2, reducer);
    return {std::to_string(monkeyBusinessPart1), std::to_string(monkeyBusinessPart2)};
}
}  // namespace bblp::aoc
