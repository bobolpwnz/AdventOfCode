#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"
#include "bblp/aoc/point.hpp"
#include "bblp/aoc/string_utils.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <regex>
#include <vector>

namespace bblp::aoc {
namespace {
struct Rule {
    int32_t preceding;
    int32_t following;
};

struct Update {
    std::vector<int32_t> pages;
};

struct Manual {
    std::vector<Rule> rules;
    std::vector<Update> updates;
};

auto parse(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    Manual input;
    input.rules.reserve(BUFFER_SIZE);
    input.updates.reserve(BUFFER_SIZE);
    const auto lineCallback = [&input](const std::string& line) {
        if (line.find('|') != std::string::npos) {
            const auto parts = split(line, "|");
            input.rules.emplace_back(std::stoul(parts.front()), std::stoul(parts.back()));
        } else if (line.find(',') != std::string::npos) {
            const auto parts = split(line, ",");
            Update newUpdate;
            newUpdate.pages.reserve(32);
            for (const auto& part : parts) {
                newUpdate.pages.push_back(std::stoul(part));
            }
            input.updates.push_back(newUpdate);
        } else {
        }
    };
    parseInput(filePath, lineCallback);
    return input;
}

bool areUpdatePagesInCorrectOrder(const Update& update, const std::vector<Rule>& rules) {
    for (auto iter = update.pages.cbegin(); iter != update.pages.cend(); ++iter) {
        for (auto iter2 = std::next(iter); iter2 != update.pages.cend(); ++iter2) {
            for (const auto& rule : rules) {
                if (rule.preceding != *iter2) {
                    continue;
                }

                if (rule.following == *iter) {
                    return false;
                }
            }
        }
    }

    return true;
}

Update reorderUpdate(const Update& update, const std::vector<Rule>& rules) {
    Update newUpdate{update.pages};

    std::sort(newUpdate.pages.begin(), newUpdate.pages.end(), [&rules](int32_t left, int32_t right) {
        for (const auto& rule : rules) {
            if (rule.preceding == left && rule.following == right) {
                return true;
            }

            if (rule.preceding == right && rule.following == left) {
                return false;
            }
        }
        return false;
    });

    return newUpdate;
}

int calculatePartOne(const Manual& input) {
    int result = 0;
    for (const auto& update : input.updates) {
        if (areUpdatePagesInCorrectOrder(update, input.rules)) {
            result += update.pages.at(update.pages.size() / 2);
        }
    }

    return result;
}

int calculatePartTwo(const Manual& input) {
    int result = 0;
    for (const auto& update : input.updates) {
        if (!areUpdatePagesInCorrectOrder(update, input.rules)) {
            const Update reorderedUpdate = reorderUpdate(update, input.rules);
            result += reorderedUpdate.pages.at(reorderedUpdate.pages.size() / 2);
        }
    }

    return result;
}
}  // namespace

std::pair<std::string, std::string> day05() {
    const auto input = parse("resources/day05.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
