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
#include <set>
#include <stdexcept>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

namespace bblp::aoc {
namespace {
static constexpr char UNKNOWN = '?';
static constexpr char DAMAGED = '#';
static constexpr char WORKING = '.';

struct ConditionRecord {
    std::string individual;
    std::vector<uint32_t> groups;

    bool operator==(const ConditionRecord& other) const {
        return individual == other.individual && groups == other.groups;
    }
};

struct CondictionRecordHash {
    size_t operator()(const ConditionRecord& record) const {
        std::string toHash{record.individual};
        for (const auto val : record.groups) {
            toHash += std::to_string(val);
        }

        return std::hash<std::string>()(toHash);
    }
};

auto parse(const std::filesystem::path& filePath) {
    std::vector<ConditionRecord> input;

    const auto lineCallback = [&input](const std::string& line) {
        const auto parts = split(line, " ");
        const auto groupStrings = split(parts.back(), ",");
        std::vector<uint32_t> groups(groupStrings.size());
        std::transform(groupStrings.cbegin(), groupStrings.cend(), groups.begin(),
                       [](const std::string& str) { return std::stoul(str); });

        input.push_back(ConditionRecord{parts.front(), groups});
    };
    parseInput(filePath, lineCallback);
    return input;
}

bool canFitNumberOfDamaged(const std::string& str, uint64_t number) {
    if (number > str.size()) {
        return false;
    }

    for (auto i = 0U; i < number; ++i) {
        if (str.at(i) == WORKING) {
            return false;
        }
    }
    return true;
}

uint64_t calculateNumberOfArrangements(const ConditionRecord& record,
                                       std::unordered_map<ConditionRecord, uint64_t, CondictionRecordHash>& cache) {
    if (cache.contains(record)) {
        return cache.at(record);
    }

    if (record.groups.empty()) {
        if (record.individual.find(DAMAGED) == std::string::npos) {
            return 1U;
        } else {
            return 0U;
        }
    }

    if (record.individual.size() == 0U) {
        return 0U;
    }

    const auto nextChar = record.individual.front();
    const auto nextGroup = record.groups.front();

    const auto damaged = [&record, &cache, nextGroup]() -> uint64_t {
        auto thisGroup = record.individual.substr(0U, nextGroup);
        if (!canFitNumberOfDamaged(thisGroup, nextGroup)) {
            return 0U;
        }

        if (record.individual.length() == nextGroup) {
            if (record.groups.size() == 1U) {
                return 1U;
            } else {
                return 0U;
            }
        }

        const char possibleSeparator = record.individual.at(nextGroup);
        if (possibleSeparator == WORKING || possibleSeparator == UNKNOWN) {
            return calculateNumberOfArrangements(
                {record.individual.substr(nextGroup + 1), {record.groups.cbegin() + 1, record.groups.cend()}}, cache);
        }

        return 0U;
    };

    const auto working = [&record, &cache]() -> uint64_t {
        return calculateNumberOfArrangements({record.individual.substr(1), record.groups}, cache);
    };

    uint64_t result{0U};
    if (nextChar == DAMAGED) {
        result = damaged();
    } else if (nextChar == WORKING) {
        result = working();
    } else if (nextChar == UNKNOWN) {
        result = damaged() + working();
    } else {
        throw std::logic_error("Invalid next char");
    }

    cache.insert({record, result});
    return result;
}

uint64_t calculatePartOne(const std::vector<ConditionRecord>& input) {
    return std::accumulate(input.cbegin(), input.cend(), 0ULL, [](uint64_t sum, const ConditionRecord& record) {
        std::unordered_map<ConditionRecord, uint64_t, CondictionRecordHash> cache;
        return sum + calculateNumberOfArrangements(record, cache);
    });
}

uint64_t calculatePartTwo(const std::vector<ConditionRecord>& input) {
    std::vector<ConditionRecord> records{input.size()};
    std::transform(input.cbegin(), input.cend(), records.begin(), [](const ConditionRecord& record) {
        const auto individual = record.individual + "?" + record.individual + "?" + record.individual + "?" +
                                record.individual + "?" + record.individual;
        std::vector<uint32_t> groups;
        groups.insert(groups.end(), record.groups.begin(), record.groups.end());
        groups.insert(groups.end(), record.groups.begin(), record.groups.end());
        groups.insert(groups.end(), record.groups.begin(), record.groups.end());
        groups.insert(groups.end(), record.groups.begin(), record.groups.end());
        groups.insert(groups.end(), record.groups.begin(), record.groups.end());

        return ConditionRecord{individual, groups};
    });

    return std::accumulate(records.cbegin(), records.cend(), 0ULL, [](uint64_t sum, const ConditionRecord& record) {
        std::unordered_map<ConditionRecord, uint64_t, CondictionRecordHash> cache;
        return sum + calculateNumberOfArrangements(record, cache);
    });
}
}  // namespace

std::pair<std::string, std::string> day12() {
    const auto input = parse("resources/day12.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
