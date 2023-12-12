#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"
#include "bblp/aoc/point.hpp"
#include "bblp/aoc/string_utils.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <fstream>
#include <future>
#include <iostream>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>
#include <set>

namespace bblp::aoc {
namespace {
static constexpr char UNKNOWN = '?';
static constexpr char DAMAGED = '#';
static constexpr char WORKING = '.';

struct CondictionRecord {
    std::string individual;
    std::vector<uint32_t> groups;
    uint32_t currentNumberOfDamaged;
    uint32_t expectedNumberOfDamaged;
    uint32_t minNumberOfUnknownsAndDamaged;
    std::vector<uint32_t> positionsOfUnknowns;
    bool part2;
};

struct Section {
    uint64_t start;
    uint64_t length;
};

std::vector<std::size_t> findAllUnknowns(const std::string& str) {
    std::vector<std::size_t> positions;
    positions.reserve(32);
    for (std::size_t i = 0U; i < str.size(); ++i) {
        if (str.at(i) == UNKNOWN) {
            positions.push_back(i);
        }
    }
    return positions;
}

uint32_t findNumberOfDamaged(const std::string& str) {
    return std::count(str.cbegin(), str.cend(), DAMAGED);
}

auto parse(const std::filesystem::path& filePath) {
    std::vector<CondictionRecord> input;

    const auto lineCallback = [&input](const std::string& line) {
        const auto parts = split(line, " ");
        const auto groupStrings = split(parts.back(), ",");
        std::vector<uint32_t> groups(groupStrings.size());
        std::transform(groupStrings.cbegin(), groupStrings.cend(), groups.begin(),
                       [](const std::string& str) { return std::stoul(str); });

        input.push_back(CondictionRecord{parts.front(), groups});
    };
    parseInput(filePath, lineCallback);
    return input;
}

std::vector<Section> findAllSections(const std::string& record) {
    std::vector<Section> sections;
    for (uint64_t i = 0U; i < record.size(); ++i) {
        if (record.at(i) == DAMAGED || record.at(i) == UNKNOWN) {
            uint64_t sectionStart = i;
            uint64_t sectionSize = 0U;
            while (i < record.size() && (record.at(i) == DAMAGED || record.at(i) == UNKNOWN)) {
                ++i;
                ++sectionSize;
            }
            sections.emplace_back(sectionStart, sectionSize);
        }
    }
    return sections;
}

bool isSatisfyingGroupRequirement(const CondictionRecord& record) {
    std::vector<uint32_t> existingGroups;
    existingGroups.push_back(0);
    auto* current = &existingGroups.front();
    for (const auto c : record.individual) {
        if (c == DAMAGED) {
            ++(*current);
        } else if (c == WORKING) {
            if (*current != 0) {
                existingGroups.push_back(0);
                current = &existingGroups.back();
            }
        }
    }

    if (existingGroups.back() == 0) {
        existingGroups.pop_back();
    }

    return existingGroups.size() == record.groups.size() && existingGroups == record.groups;
}

bool canSatisfyGroupRequirement(const CondictionRecord& record) {
    const auto sections = findAllSections(record.individual);
    for (auto groupIndex = 0U; groupIndex < record.groups.size(); ++groupIndex) {
        const auto any = std::any_of(
            sections.cbegin(), sections.cend(),
            [group = record.groups.at(groupIndex)](const Section& section) { return group <= section.length; });
        if (!any) {
            return false;
        }
    }

    return true;
}

uint64_t calculateNumberOfArrangements(const CondictionRecord& record,
                                       uint32_t currentIndividualIndex,
                                       uint32_t currentGroupIndex, std::set<std::string>& results) {
    const auto numberOfUnknowns = record.positionsOfUnknowns.size();
    if (numberOfUnknowns == 0U) {
        if (isSatisfyingGroupRequirement(record)) {
            std::string result{record.individual};
            std::replace(result.begin(), result.end(), '?', '.');

            if (results.contains(result)) {
                std::cout << result << '\n';
            }

            results.insert(result);
            return 1U;
        } else {
            return 0;
        }
    }

    if (record.currentNumberOfDamaged == record.expectedNumberOfDamaged) {
        if (isSatisfyingGroupRequirement(record) && currentGroupIndex >= record.groups.size()) {
            std::string result{record.individual};
            std::replace(result.begin(), result.end(), '?', '.');

            if (results.contains(result)) {
                std::cout << result << '\n';
            }

            results.insert(result);
            return 1U;
        }
    }

    if (record.currentNumberOfDamaged + numberOfUnknowns < record.expectedNumberOfDamaged) {
        return 0U;
    }
    if (currentGroupIndex >= record.groups.size()) {
        return 0U;
    }

    uint64_t result{0U};
    const auto firstNonWorking = record.individual.find_first_of("#?", currentIndividualIndex);
    uint32_t count = 0;
    while (firstNonWorking + count < record.individual.size() && record.individual.at(firstNonWorking + count) != '.' &&
           count < record.groups.at(currentGroupIndex)) {
        ++count;
    }

    if (count == record.groups.at(currentGroupIndex)) {
        std::string withDamaged{record.individual};
        uint32_t unknownsReplaced = 0;
        uint32_t damagedAdded = 0;
        for (auto i = 0; i < count; ++i) {
            if (withDamaged.at(firstNonWorking + i) == UNKNOWN) {
                withDamaged.at(firstNonWorking + i) = DAMAGED;
                ++unknownsReplaced;
                ++damagedAdded;
            }
        }

        if (firstNonWorking + count < withDamaged.size() && withDamaged.at(firstNonWorking + count) == UNKNOWN) {
            withDamaged.at(firstNonWorking + count) = WORKING;
            ++unknownsReplaced;
            ++count;
        }
        if (firstNonWorking + count >= withDamaged.size() ||
            withDamaged.at(firstNonWorking + count - (unknownsReplaced - damagedAdded)) != DAMAGED) {
            const std::vector<uint32_t> newUnknowns{record.positionsOfUnknowns.cbegin() + unknownsReplaced,
                                                    record.positionsOfUnknowns.cend()};
            const CondictionRecord recordWithDamaged{withDamaged,
                                                     record.groups,
                                                     record.currentNumberOfDamaged + damagedAdded,
                                                     record.expectedNumberOfDamaged,
                                                     record.minNumberOfUnknownsAndDamaged,
                                                     newUnknowns};
            if (canSatisfyGroupRequirement(recordWithDamaged)) {
                result +=
                    calculateNumberOfArrangements(recordWithDamaged, firstNonWorking + count, currentGroupIndex + 1, results);
            }
        }
    }

    const auto firstUnknown = record.individual.find_first_of("?", currentIndividualIndex);
    if (firstUnknown <= firstNonWorking) {
        std::string withWorking{record.individual};
        withWorking.at(firstUnknown) = WORKING;
        const CondictionRecord recordWithWorking{
            withWorking,
            record.groups,
            record.currentNumberOfDamaged,
            record.expectedNumberOfDamaged,
            record.minNumberOfUnknownsAndDamaged,
            {record.positionsOfUnknowns.cbegin() + 1, record.positionsOfUnknowns.cend()}};
        if (canSatisfyGroupRequirement(recordWithWorking)) {
            result += calculateNumberOfArrangements(recordWithWorking, currentIndividualIndex + 1, currentGroupIndex,
                                                    results);
        }
    }

    return result;
}

uint64_t calculatePartOne(const std::vector<CondictionRecord>& input) {
    std::ofstream output("output.txt");
    std::vector<CondictionRecord> records{input.size()};
    std::transform(input.cbegin(), input.cend(), records.begin(), [](const CondictionRecord& record) {
        const uint32_t expectedNumberOfDamaged = std::accumulate(record.groups.cbegin(), record.groups.cend(), 0U,
                                                                 [](uint32_t sum, uint32_t val) { return sum + val; });
        return CondictionRecord{record.individual,
                                record.groups,
                                findNumberOfDamaged(record.individual),
                                expectedNumberOfDamaged,
                                0,
                                findAllUnknowns(record.individual),
                                false};
    });

    return std::accumulate(
        records.cbegin(), records.cend(), 0ULL, [&output](uint64_t sum, const CondictionRecord& record) {
            std::set<std::string> results;

            const auto result = calculateNumberOfArrangements(record, 0, 0, results);
            const auto groups = std::accumulate(
                record.groups.cbegin(), record.groups.cend(), std::string(""),
                [](std::string str, const uint32_t& group) { return str + ", " + std::to_string(group); });

            output << record.individual << " -> " << groups << " = " << result << '\n';
            for (const auto& result : results) {
                output << result << '\n';
            }
            output << '\n';

            return sum + results.size();
        });
}

uint64_t calculatePartTwo(const std::vector<CondictionRecord>& input) {
    return 0;
    std::vector<CondictionRecord> records{input.size()};
    std::transform(input.cbegin(), input.cend(), records.begin(), [](const CondictionRecord& record) {
        const auto individual = record.individual + "?" + record.individual + "?" + record.individual + "?" +
                                record.individual + "?" + record.individual;
        std::vector<uint32_t> groups;
        groups.insert(groups.end(), record.groups.begin(), record.groups.end());
        groups.insert(groups.end(), record.groups.begin(), record.groups.end());
        groups.insert(groups.end(), record.groups.begin(), record.groups.end());
        groups.insert(groups.end(), record.groups.begin(), record.groups.end());
        groups.insert(groups.end(), record.groups.begin(), record.groups.end());

        const auto expectedNumberOfDamaged =
            std::accumulate(groups.cbegin(), groups.cend(), 0U, [](uint32_t sum, uint32_t val) { return sum + val; });

        return CondictionRecord{individual,
                                groups,
                                findNumberOfDamaged(individual),
                                expectedNumberOfDamaged,
                                0,
                                findAllUnknowns(individual),
                                true};
    });

    std::vector<uint64_t> results(input.size());
    for (int i = 0; i < records.size(); i += 8) {
        std::vector<std::thread> threads;
        threads.reserve(8);
        for (int j = 0; j < 8; ++j) {
            threads.emplace_back([record = records.at(8 * i + j), i, j, &results]() {
                std::set<std::string> setResults;
                std::cout << "starting " << 8 * i + j << '\n';
                results.at(8 * i + j) = calculateNumberOfArrangements(record, 0, 0, setResults);
                std::cout << "ending " << 8 * i + j << '\n';
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }
    }

    return std::accumulate(results.cbegin(), results.cend(), 0ULL,
                           [](uint64_t sum, uint64_t result) { return sum + result; });

    /*
    return std::accumulate(records.cbegin(), records.cend(), 0ULL, [](uint64_t sum, const CondictionRecord& record) {
        std::cout << "Calculating for " << record.individual << '\n';
        return sum + calculateNumberOfArrangements(record);
    });*/
}
}  // namespace

std::pair<std::string, std::string> day12() {
    const auto input = parse("resources/day12.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
