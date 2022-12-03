#include "day03.hpp"

#include "common/file_utils.hpp"

#include <algorithm>
#include <numeric>
#include <vector>

namespace bblp::advent_of_code_2022 {
namespace {
std::vector<std::string> parseRucksacks(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    std::vector<std::string> rucksacks;
    rucksacks.reserve(BUFFER_SIZE);
    const auto lineCallback = [&rucksacks](const std::string& line) {
        if (!line.empty()) {
            rucksacks.emplace_back(line);
        }
    };
    parseInput(filePath, lineCallback);
    return rucksacks;
}

std::vector<std::tuple<std::string, std::string>> buildCompartments(const std::vector<std::string>& rucksacks) {
    std::vector<std::tuple<std::string, std::string>> compartments;
    compartments.reserve(rucksacks.size());

    std::transform(rucksacks.cbegin(), rucksacks.cend(), std::back_inserter(compartments), [](const auto& rucksack) {
        const auto firstCompartment = rucksack.substr(0, rucksack.length() / 2);
        const auto secondCompartment = rucksack.substr(rucksack.length() / 2);
        return std::tuple<std::string, std::string>{firstCompartment, secondCompartment};
    });
    return compartments;
}

char findMisplacedItem(const std::string& firstCompartment, const std::string& secondCompartment) {
    std::vector<char> misplacedItems;

    auto firstCompartmentCopy{firstCompartment};
    auto secondCompartmentCopy{secondCompartment};

    std::sort(firstCompartmentCopy.begin(), firstCompartmentCopy.end());
    std::sort(secondCompartmentCopy.begin(), secondCompartmentCopy.end());

    std::set_intersection(firstCompartmentCopy.cbegin(), firstCompartmentCopy.cend(), secondCompartmentCopy.cbegin(),
                          secondCompartmentCopy.cend(), std::back_inserter(misplacedItems));
    if (misplacedItems.empty()) {
        throw std::runtime_error("Common item not found");
    }
    return misplacedItems.front();
}

std::vector<char> findMisplacedItems(const std::vector<std::tuple<std::string, std::string>>& compartments) {
    std::vector<char> misplacedItems;
    misplacedItems.reserve(compartments.size());

    std::transform(
        compartments.begin(), compartments.end(), std::back_inserter(misplacedItems),
        [](const auto compartment) { return findMisplacedItem(std::get<0>(compartment), std::get<1>(compartment)); });
    return misplacedItems;
}

char findBadge(const std::string& first, const std::string& second, const std::string& third) {
    std::vector<char> firstTwoCommonItems;
    firstTwoCommonItems.reserve(first.size() + second.size());

    auto firstCopy{first};
    auto secondCopy{second};
    auto thirdCopy{third};

    std::sort(firstCopy.begin(), firstCopy.end());
    std::sort(secondCopy.begin(), secondCopy.end());
    std::sort(thirdCopy.begin(), thirdCopy.end());

    std::set_intersection(firstCopy.cbegin(), firstCopy.cend(), secondCopy.cbegin(), secondCopy.cend(),
                          std::back_inserter(firstTwoCommonItems));
    std::vector<char> commonItems;
    std::set_intersection(firstTwoCommonItems.cbegin(), firstTwoCommonItems.cend(), thirdCopy.cbegin(),
                          thirdCopy.cend(),
                          std::back_inserter(commonItems));

    if (commonItems.empty()) {
        throw std::runtime_error("Common item not found");
    }
    return commonItems.front();
}

std::vector<char> findLostBadges(const std::vector<std::string>& rucksacks) {
    static constexpr uint32_t GROUP_SIZE = 3;

    std::vector<char> badges;
    badges.reserve(rucksacks.size());
    for (auto i = 0; i < rucksacks.size(); i += GROUP_SIZE) {
        badges.push_back(findBadge(rucksacks[i], rucksacks[i + 1], rucksacks[i + 2]));
    }
    return badges;
}

uint32_t calculateTotalPriority(const std::vector<char>& misplacedItems) {
    static constexpr uint32_t LOWERCASE_PRIORITY_BASE = 1;
    static constexpr uint32_t UPPERCASE_PRIORITY_BASE = 27;

    const auto calculatePriority = [](const char item) {
        if (item >= 'a' && item <= 'z') {
            return item - 'a' + LOWERCASE_PRIORITY_BASE;
        } else if (item >= 'A' && item <= 'Z') {
            return item - 'A' + UPPERCASE_PRIORITY_BASE;
        } else {
            throw std::logic_error("Unknown item");
        }
    };

    return std::accumulate(
        misplacedItems.cbegin(), misplacedItems.cend(), 0U,
        [&calculatePriority](const auto sum, const char item) { return sum + calculatePriority(item); });
}
}  // namespace

std::tuple<int64_t, int64_t> day03() {
    const auto input = parseRucksacks("resources/day03/input.txt");

    const auto compartments = buildCompartments(input);
    const auto misplacedItems = findMisplacedItems(compartments);
    const auto totalPriority = calculateTotalPriority(misplacedItems);

    const auto lostBadges = findLostBadges(input);
    const auto badgesPriority = calculateTotalPriority(lostBadges);

    return {totalPriority, badgesPriority};
}
}  // namespace bblp::advent_of_code_2022
