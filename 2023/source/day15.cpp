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
auto parse(const std::filesystem::path& filePath) {
    std::vector<std::string> input;

    const auto lineCallback = [&input](const std::string& line) {
        const auto parts = split(line, ",");
        input.insert(input.begin(), parts.cbegin(), parts.cend());
    };
    parseInput(filePath, lineCallback);
    return input;
}

uint8_t calculateHash(const std::string& str) {
    uint8_t hash{0U};
    for (const auto c : str) {
        hash += c;
        hash *= 17;
    }
    return hash;
}


uint64_t calculatePartOne(const std::vector<std::string>& input) {
    return std::accumulate(input.cbegin(), input.cend(), 0ULL,
                           [](uint64_t sum, const std::string& str) { return sum + calculateHash(str); });
}

std::ostream& operator<<(std::ostream& stream, const std::array<std::vector<std::string>, 256>& boxes) {
    for (auto i = 0U; i < boxes.size(); ++i) {
        if (boxes.at(i).size() == 0U) {
            continue;
        }

        stream << "Box " << i << ":";
        for (const auto& str : boxes.at(i)) {
            stream << " [" << str << "]";
        }
        stream << '\n';
    }
    return stream;
}

uint64_t calculatePartTwo(const std::vector<std::string>& input) {
    std::array<std::vector<std::string>, 256> boxes{};
    for (const auto& str : input) {
        if (str.find('-') != std::string::npos) {
            const auto parts = split(str, "-");
            const auto& label = parts.front();
            const auto hash = calculateHash(label);
            auto& box = boxes.at(hash);
            if (hash == 214U) {
                int i = 0;
            }
            const auto iter = std::find_if(box.begin(), box.end(), [&label](const std::string& content) {
                return content.find(label) != std::string::npos;
            });

            if (iter != box.end()) {
                box.erase(iter);
            }        
        } else if (str.find('=') != std::string::npos) {
            const auto parts = split(str, "=");
            const auto& label = parts.front();
            const auto& lens = parts.back();
            const auto hash = calculateHash(label);
            auto& box = boxes.at(hash);
            if (hash == 214U) {
                int i = 0;
            }

            const auto iter = std::find_if(box.begin(), box.end(), [&label](const std::string& content) {
                return (label.size() == content.size() - 2U) && (content.find(label) == 0U);
            });

            if (iter != box.end()) {
                *iter = label + " " + lens;
            } else {
                box.push_back(label + " " + lens);
            }
        } else {
            throw std::logic_error("Invalid string");
        }
    }
    uint64_t result{0U}, result2{0U};
    for (auto i = 0U; i < boxes.size(); ++i) {
        const auto& box = boxes.at(i);
        for (auto j = 0U; j < box.size(); ++j) {
            const auto parts = split(box.at(j), " ");
            result += (i + 1ULL) * (j + 1ULL) * std::stoul(parts.back());
        }
    }

    return result;
}
}  // namespace

std::pair<std::string, std::string> day15() {
    const auto input = parse("resources/day15.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
