#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"
#include "bblp/aoc/string_utils.hpp"

#include <algorithm>
#include <cstdlib>
#include <numeric>
#include <regex>
#include <vector>

namespace bblp::aoc {
namespace {

struct Report {
    std::vector<int32_t> levels;
};

auto parse(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    std::vector<Report> input;
    input.reserve(BUFFER_SIZE);
    const auto lineCallback = [&input](const std::string& line) {
        const auto parts = split(line, " ");
        Report report;
        for (const auto& part : parts) {
            report.levels.push_back(std::stoul(part));
        }
        input.push_back(report);
    };
    parseInput(filePath, lineCallback);
    return input;
}

bool isReportSafe(const Report& report) {
    std::optional<bool> isIncreasing;
    for (size_t index = 0U; index < report.levels.size() - 1; ++index) {
        const auto diff = report.levels.at(index + 1) - report.levels.at(index);
        if (diff == 0) {
            return false;
        }

        if (isIncreasing.has_value()) {
            if (isIncreasing.value()) {
                if (diff < 0) {
                    return false;
                }
            } else {
                if (diff > 0) {
                    return false;
                }
            }
        } else {
            isIncreasing = diff > 0;
        }

        if (std::abs(diff) > 3) {
            return false;
        }
    }

    return true;
}

bool canReportBeSafe(const Report& input) {
    const auto isSafe = isReportSafe(input);
    if (isSafe) {
        return true;
    }

    for (auto i = 0U; i < input.levels.size(); ++i) {
        Report newReport{input};

        auto it = newReport.levels.begin();
        std::advance(it, i);
        newReport.levels.erase(it);

        if (isReportSafe(newReport)) {
            return true;
        }
    }
    return false;
}

int calculatePartOne(const std::vector<Report>& input) {
    return std::count_if(input.cbegin(), input.cend(), isReportSafe);
}

int calculatePartTwo(const std::vector<Report>& input) {
    return std::count_if(input.cbegin(), input.cend(), canReportBeSafe);
}
}  // namespace

std::pair<std::string, std::string> day02() {
    const auto input = parse("resources/day02.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
