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
#include <list>
#include <numeric>
#include <optional>
#include <queue>
#include <set>
#include <stdexcept>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

namespace bblp::aoc {
namespace {
struct Rule {
    enum class Type { LESS, GREATER, FORWARD };

    Type type;
    char category;
    uint32_t value;
    std::string destination;
};

struct Workflow {
    std::string name;
    std::vector<Rule> rules;
};

struct Part {
    std::array<uint32_t, 4> categories;
};

struct Sorter {
    std::unordered_map<std::string, Workflow> workflows;
    std::vector<Part> parts;
};

struct Range {
    uint32_t start;
    uint32_t end;
};

std::ostream& operator<<(std::ostream& stream, const Part& part) {
    stream << "{";
    stream << "x=" << part.categories.at(0U) << ",";
    stream << "m=" << part.categories.at(1U) << ",";
    stream << "a=" << part.categories.at(2U) << ",";
    stream << "s=" << part.categories.at(3U);
    stream << "}";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const Rule& rule) {
    switch (rule.type) {
        case Rule::Type::LESS: {
            stream << rule.category << '<' << rule.value << ':' << rule.destination;
        } break;
        case Rule::Type::GREATER: {
            stream << rule.category << '>' << rule.value << ':' << rule.destination;
        } break;
        case Rule::Type::FORWARD: {
            stream << rule.destination;
        } break;
    }
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const std::array<Range, 4>& range) {
    stream << "x={" << range.at(0).start << ":" << range.at(0).end << "}"
           << ",";
    stream << "m={" << range.at(1).start << ":" << range.at(1).end << "}"
           << ",";
    stream << "a={" << range.at(2).start << ":" << range.at(2).end << "}"
           << ",";
    stream << "s={" << range.at(3).start << ":" << range.at(3).end << "}";
    return stream;
}

uint32_t categoryToIndex(char category) {
    switch (category) {
        case 'x':
            return 0U;
        case 'm':
            return 1U;
        case 'a':
            return 2U;
        case 's':
            return 3u;
    }
    throw std::logic_error("Invalid category");
}

std::array<uint32_t, 4> parseCategories(const std::vector<std::string>& parts) {
    std::array<uint32_t, 4> categories{};
    for (const auto& part : parts) {
        const auto categoryParts = split(part, "=");
        categories.at(categoryToIndex(categoryParts.front().front())) = std::stoul(categoryParts.back());
    }
    return categories;
}

Rule parseRule(const std::string& str) {
    const auto dstIndex = str.find(":");
    if (dstIndex == std::string::npos) {
        return Rule{Rule::Type::FORWARD, '\0', 0U, str};
    }

    const auto destination = str.substr(dstIndex + 1);
    const auto opIndex = str.find_first_of("<>");
    const auto type = (str.at(opIndex) == '<' ? Rule::Type::LESS : Rule::Type::GREATER);
    return Rule{type, str.front(), std::stoul(str.substr(opIndex + 1, dstIndex - opIndex - 1)), destination};
}

std::vector<Rule> parseRules(const std::string& str) {
    std::vector<Rule> rules;
    const auto parts = split(str, ",");
    for (const auto& part : parts) {
        rules.push_back(parseRule(part));
    }

    return rules;
}

Workflow parseWorkflow(const std::string& str) {
    const auto parts = split(str, "{");

    return Workflow{parts.front(), parseRules(parts.back().substr(0U, parts.back().length() - 1U))};
}

auto parse(const std::filesystem::path& filePath) {
    Sorter sorter;

    const auto lineCallback = [&sorter](const std::string& line) {
        if (line.empty()) {
            return;
        }

        if (line.starts_with('{')) {
            sorter.parts.emplace_back(parseCategories(split(line.substr(1U, line.length() - 2U), ",")));
        } else {
            const auto workflow = parseWorkflow(line);
            sorter.workflows.insert({workflow.name, workflow});
        }
    };
    parseInput(filePath, lineCallback);

    return sorter;
}

std::optional<std::string> applyRule(const Part& part, const Rule& rule) {
    switch (rule.type) {
        case Rule::Type::FORWARD:
            return rule.destination;
        case Rule::Type::LESS: {
            if (part.categories.at(categoryToIndex(rule.category)) < rule.value) {
                return rule.destination;
            }
        } break;
        case Rule::Type::GREATER: {
            if (part.categories.at(categoryToIndex(rule.category)) > rule.value) {
                return rule.destination;
            }
        } break;
    }
    return {};
}

std::string applyRule(const Part& part, const Workflow workflow) {
    for (const auto& rule : workflow.rules) {
        const auto destination = applyRule(part, rule);
        if (destination.has_value()) {
            return *destination;
        }
    }
    throw std::logic_error("Invalid logic");
}

bool isPartAccepted(const Part& part, const Sorter& sorter) {
    std::string destination{};
    const Workflow* currentWorkflow{&sorter.workflows.at("in")};
    // std::cout << currentWorkflow->name << " ";
    while (destination != "A" && destination != "R") {
        destination = applyRule(part, *currentWorkflow);
        // std::cout << " -> " << destination;
        if (sorter.workflows.contains(destination)) {
            currentWorkflow = &sorter.workflows.at(destination);
        }
    }
    // std::cout << '\n';
    if (destination == "A") {
        return true;
    } else if (destination == "R") {
        return false;
    }

    throw std::logic_error("Invalid destination");
}

uint64_t calculatePartOne(const Sorter& input) {
    std::vector<Part> acccepted{};
    for (const auto& part : input.parts) {
        if (isPartAccepted(part, input)) {
            acccepted.push_back(part);
        }
    }

    return std::accumulate(acccepted.cbegin(), acccepted.cend(), 0ULL, [](uint64_t sum, const Part& part) {
        return sum + std::accumulate(part.categories.cbegin(), part.categories.cend(), 0ULL,
                                     [](uint64_t sum, uint32_t rating) { return sum + rating; });
    });
}

std::optional<std::pair<std::string, std::pair<std::array<Range, 4>, std::array<Range, 4>>>> applyRule(
    const std::array<Range, 4>& currentRange,
    const Rule& rule) {
    switch (rule.type) {
        case Rule::Type::FORWARD:
            return {{rule.destination, {currentRange, currentRange}}};
        case Rule::Type::LESS: {
            const auto index = categoryToIndex(rule.category);
            if (rule.value < currentRange.at(index).start) {
                return {};
            }

            std::array<Range, 4> trueRange{currentRange};
            trueRange.at(index).end = rule.value - 1;
            std::array<Range, 4> falseRange{currentRange};
            falseRange.at(index).start = rule.value;
            return {{rule.destination, {trueRange, falseRange}}};
        } break;
        case Rule::Type::GREATER: {
            const auto index = categoryToIndex(rule.category);
            if (rule.value > currentRange.at(index).end) {
                return {};
            }

            std::array<Range, 4> trueRange{currentRange};
            trueRange.at(index).start = rule.value + 1;
            std::array<Range, 4> falseRange{currentRange};
            falseRange.at(index).end = rule.value;
            return {{rule.destination, {trueRange, falseRange}}};
        } break;
    }
    return {};
}

void findAcceptableRanges(const Sorter& sorter,
                          const Workflow& workflow,
                          const std::array<Range, 4>& currentRange,
                          std::vector<std::array<Range, 4>>& acceptedRanges) {
    std::array<Range, 4> nextRange{currentRange};
    for (const auto& rule : workflow.rules) {
        // std::cout << workflow.name;
        // std::cout << " - rule: " << rule << " for range " << nextRange << '\n';

        const auto ruleResult = applyRule(nextRange, rule);
        if (ruleResult.has_value()) {
            auto& result = *ruleResult;
            // std::cout << '\t' << "True : " << result.second.first << " Destination " << result.first << '\n';
            // std::cout << '\t' << "False: " << result.second.second << '\n';
            if (result.first == "A") {
                // std::cout << '\t' << '\t' << "Adding range to result " << result.second.first << '\n';
                acceptedRanges.push_back(result.second.first);
                nextRange = result.second.second;
            } else if (result.first == "R") {
                nextRange = result.second.second;
            } else {
                findAcceptableRanges(sorter, sorter.workflows.at(result.first), result.second.first, acceptedRanges);
                nextRange = result.second.second;
            }
    }
    else {
        // std::cout << '\t' << "No value" << '\n';
        break;
    }
}
}

Part toPart(uint32_t x, uint32_t m, uint32_t a, uint32_t s) {
    Part part{};
    part.categories.at(0) = x;
    part.categories.at(1) = m;
    part.categories.at(2) = a;
    part.categories.at(3) = s;
    return part;
}

uint64_t calculatePartTwo(const Sorter& sorter) {
    std::vector<std::array<Range, 4>> acceptedRanges;
    std::array<Range, 4> ratingsRanges{Range{1, 4000}, Range{1, 4000}, Range{1, 4000}, Range{1, 4000}};

    findAcceptableRanges(sorter, sorter.workflows.at("in"), ratingsRanges, acceptedRanges);

    /*
    for (const auto& range : acceptedRanges) {
        std::cout << range << '\n';
    }*/
    /*
    for (const auto& acceptedRange : acceptedRanges) {
        const auto part1 = toPart(acceptedRange.at(0).start, acceptedRange.at(1).start, acceptedRange.at(2).start,
                                  acceptedRange.at(3).start);
        const auto part2 =
            toPart(acceptedRange.at(0).end, acceptedRange.at(1).end, acceptedRange.at(2).end, acceptedRange.at(3).end);
        std::cout << part1 << " = " << isPartAccepted(part1, sorter) << '\n';
        std::cout << part2 << " = " << isPartAccepted(part2, sorter) << '\n';
    }*/

    return std::accumulate(
        acceptedRanges.cbegin(), acceptedRanges.cend(), 0ULL, [](uint64_t sum, const std::array<Range, 4>& range) {
            const auto rangeSum =
                std::accumulate(range.cbegin(), range.cend(), 1ULL,
                                [](uint64_t mul, const Range& range) { return mul * (range.end - range.start + 1); });
            return sum + rangeSum;
        });
}
}  // namespace

std::pair<std::string, std::string> day19() {
    const auto input = parse("resources/day19.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
