#include "days.hpp"

#include "common/file_utils.hpp"

#include <algorithm>
#include <cstdlib>
#include <numeric>
#include <vector>

namespace bblp::advent_of_code_2022 {
namespace {
using SectionRange = std::pair<uint32_t, uint32_t>;

SectionRange parseSectionRange(const std::string& string) {
    static constexpr char delimiter = '-';

    const auto delimiterPosition = string.find(delimiter);
    const auto start = string.substr(0, delimiterPosition);
    const auto end = string.substr(delimiterPosition + 1, string.size() - delimiterPosition);

    return {std::stoul(start), std::stoul(end)};
}

std::vector<std::pair<SectionRange, SectionRange>> parseSections(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    std::vector<std::pair<SectionRange, SectionRange>> sections;
    sections.reserve(BUFFER_SIZE);
    const auto lineCallback = [&sections](const std::string& line) {
        if (!line.empty()) {
            const auto delimiterPosition = line.find(',');
            const auto firstElf = line.substr(0, delimiterPosition);
            const auto secondElf = line.substr(delimiterPosition + 1, line.size() - delimiterPosition);

            sections.emplace_back(parseSectionRange(firstElf), parseSectionRange(secondElf));
        }
    };
    parseInput(filePath, lineCallback);
    return sections;
}

bool isSectionContainedWithin(const SectionRange& first, const SectionRange& second) {
    return (first.first >= second.first) && (first.second <= second.second);
}

bool isSectionBorderWithin(const uint32_t target, const SectionRange& section) {
    return (section.first <= target) && (target <= section.second);
}
}  // namespace

std::pair<std::string, std::string> day04() {
    const auto input = parseSections("resources/day04.txt");

    const auto areSectionsOverlappingFully = [](const std::pair<SectionRange, SectionRange>& sections) {
        return (isSectionContainedWithin(sections.first, sections.second) ||
                isSectionContainedWithin(sections.second, sections.first));
    };
    const auto numberOfSectionsFullyOverlapping =
        std::count_if(input.begin(), input.end(), areSectionsOverlappingFully);

    const auto areSectionsOverlappingPartially = [](const std::pair<SectionRange, SectionRange>& sections) {
        return (isSectionBorderWithin(sections.first.first, sections.second) ||
                isSectionBorderWithin(sections.first.second, sections.second) ||
                isSectionBorderWithin(sections.second.first, sections.first) ||
                isSectionBorderWithin(sections.second.second, sections.first));
    };
    const auto numberOfSectionsPartiallyOverlapping =
        std::count_if(input.begin(), input.end(), areSectionsOverlappingPartially);

    return {std::to_string(numberOfSectionsFullyOverlapping), std::to_string(numberOfSectionsPartiallyOverlapping)};
}
}  // namespace bblp::advent_of_code_2022
