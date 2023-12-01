#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"

#include <algorithm>
#include <cstdlib>
#include <numeric>
#include <regex>
#include <vector>

namespace bblp::aoc {
namespace {

auto parse(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    std::vector<std::string> input;
    input.reserve(BUFFER_SIZE);
    const auto lineCallback = [&input](const std::string& line) { input.push_back(line); };
    parseInput(filePath, lineCallback);
    return input;
}

int toNumber(const std::string& val) {
    if (val.length() == 1) {
        return std::stoi(val);
    }

    if (val == "zero") {
        return 0;
    } else if (val == "one") {
        return 1;
    } else if (val == "two") {
        return 2;
    } else if (val == "three") {
        return 3;
    } else if (val == "four") {
        return 4;
    } else if (val == "five") {
        return 5;
    } else if (val == "six") {
        return 6;
    } else if (val == "seven") {
        return 7;
    } else if (val == "eight") {
        return 8;
    } else if (val == "nine") {
        return 9;
    } else {
        throw std::logic_error("Invalid toNumber() parameter: " + val);
    }
}

int calculateCalibration(const std::vector<std::pair<int, int>> values) {
    return std::accumulate(values.cbegin(), values.cend(), 0,
                           [](int sum, auto const& pair) { return sum + 10 * pair.first + pair.second; });
}

int calculatePartOne(const std::vector<std::string>& input) {
    std::vector<std::pair<int, int>> partOne(input.size());
    std::transform(input.cbegin(), input.cend(), partOne.begin(), [](const auto& line) -> std::pair<int, int> {
        const auto indexOfFirst = line.find_first_of("0123456789");
        const auto indexOfLast = line.find_last_of("0123456789");
        if (indexOfFirst == std::string::npos || indexOfLast == std::string::npos) {
            return {0, 0};
        }

        return {line.at(indexOfFirst) - '0', line.at(indexOfLast) - '0'};
    });
    return calculateCalibration(partOne);
}

int calculatePartTwo(const std::vector<std::string>& input) {
    const std::string pattern{"1|2|3|4|5|6|7|8|9|one|two|three|four|five|six|seven|eight|nine"};
    std::regex regex{pattern};
    std::smatch match;

    std::vector<std::pair<int, int>> partTwo(input.size());
    std::transform(input.cbegin(), input.cend(), partTwo.begin(),
                   [&match, &regex](const auto& line) -> std::pair<int, int> {
                       static constexpr size_t LINE_BUFFER_SIZE = 100U;

                       std::vector<std::string> results;
                       results.reserve(LINE_BUFFER_SIZE);

                       std::string search = line;
                       while (std::regex_search(search, match, regex)) {
                           const auto str = match[0].str();
                           results.push_back(str);
                           if (str.length() > 1) {
                               // case for sevenine etc.
                               search = match[0].str().back();
                               search += match.suffix();
                           } else {
                               search = match.suffix();
                           }
                       }

                       return {toNumber(results.front()), toNumber(results.back())};
                   });
    return calculateCalibration(partTwo);
}
}  // namespace

std::pair<std::string, std::string> day01() {
    const auto input = parse("resources/day01.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
