#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"
#include "bblp/aoc/string_utils.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <variant>
#include <vector>

namespace bblp::aoc {
namespace {

struct List;
std::ostream& operator<<(std::ostream& stream, const List& list);

struct List {
    std::weak_ordering operator<=>(const List& other) const {
        auto leftIter = content.cbegin();
        auto rightIter = other.content.cbegin();

        while (leftIter != content.cend() && rightIter != other.content.cend()) {
            const bool isLeftInt = std::holds_alternative<int32_t>(*leftIter);
            const bool isRightInt = std::holds_alternative<int32_t>(*rightIter);
            if (isLeftInt && isRightInt) {
                const auto leftInt = std::get<int32_t>(*leftIter);
                const auto rightInt = std::get<int32_t>(*rightIter);
                if (leftInt < rightInt) {
                    return std::weak_ordering::less;
                } else if (leftInt > rightInt) {
                    return std::weak_ordering::greater;
                }
            } else if (!isLeftInt && !isRightInt) {
                const std::weak_ordering cmp = std::get<List>(*leftIter) <=> std::get<List>(*rightIter);
                if (cmp < 0) {
                    return std::weak_ordering::less;
                } else if (cmp > 0) {
                    return std::weak_ordering::greater;
                }
            } else if (isLeftInt && !isRightInt) {
                List leftList;
                leftList.content.push_back(*leftIter);
                const std::weak_ordering cmp = leftList <=> std::get<List>(*rightIter);
                if (cmp < 0) {
                    return std::weak_ordering::less;
                } else if (cmp > 0) {
                    return std::weak_ordering::greater;
                }
            } else if (!isLeftInt && isRightInt) {
                List rightList;
                rightList.content.push_back(*rightIter);
                const std::weak_ordering cmp = std::get<List>(*leftIter) <=> rightList;
                if (cmp < 0) {
                    return std::weak_ordering::less;
                } else if (cmp > 0) {
                    return std::weak_ordering::greater;
                }
            } else {
                throw std::runtime_error("what?");
            }

            ++leftIter;
            ++rightIter;
        }

        if (leftIter == content.cend() && rightIter == other.content.cend()) {
            return std::weak_ordering::equivalent;
        }

        if (leftIter == content.end() && rightIter != other.content.end()) {
            return std::weak_ordering::less;
        }

        if (leftIter != content.end() && rightIter == other.content.end()) {
            return std::weak_ordering::greater;
        }

        return std::weak_ordering::equivalent;
    }

    bool operator==(const List& other) const { return (*this <=> other) == std::weak_ordering::equivalent; }

    std::vector<std::variant<int32_t, List>> content;
};

struct Packet {
    bool operator<(const Packet& other) const { return items < other.items; }
    bool operator<=(const Packet& other) const { return items <= other.items; }
    bool operator==(const Packet& other) const { return items == other.items; }

    List items;
};

std::ostream& operator<<(std::ostream& stream, const List& list) {
    std::cout << "[";
    for (auto iter = list.content.begin(); iter != list.content.cend(); ++iter) {
        if (iter != list.content.begin()) {
            std::cout << ',';
        }

        if (std::holds_alternative<int32_t>(*iter)) {
            std::cout << std::get<int32_t>(*iter);
        } else {
            std::cout << std::get<List>(*iter);
        }
    }
    std::cout << ']';
    return stream;
}

size_t findMatchingClosingBracketPos(const size_t start, const std::string& string) {
    size_t numberOfOpeningBrackets = 0;
    size_t numberOfClosingBrackets = 0;
    for (size_t i = start; i < string.size(); ++i) {
        const auto c = string.at(i);
        if (c == '[') {
            ++numberOfOpeningBrackets;
        } else if (c == ']') {
            ++numberOfClosingBrackets;
        }

        if (numberOfOpeningBrackets == numberOfClosingBrackets) {
            return i;
        }
    }
    return std::string::npos;
}

List parseList(const std::string& string) {
    List list;
    for (size_t i = 0U; i < string.size(); ++i) {
        const auto c = string.at(i);
        if (c == '[') {
            const auto closingBracketPos = findMatchingClosingBracketPos(i, string);
            const auto subLineString = string.substr(i + 1,  closingBracketPos - i - 1);
            const auto innerList = parseList(subLineString);
            list.content.emplace_back(innerList);
            i = closingBracketPos + 1;
        } else if (c >= '0' && c <= '9') {
            auto numberSize = 1U;
            while (i + numberSize < string.size() && string.at(i + numberSize) >= '0' &&
                    string.at(i + numberSize) <= '9') {
                ++numberSize;
            }

            const auto number = std::stol(string.substr(i, numberSize));
            i += numberSize - 1;
            list.content.emplace_back(number);
        }
    }

    return list;
}

Packet parsePacket(const std::string& line) {
    return Packet{parseList(line)};
}

auto parse(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    std::vector<Packet> packets;
    packets.reserve(BUFFER_SIZE);
    const auto lineCallback = [&packets](const std::string& line) {
        if (!line.empty()) {
            packets.push_back(parsePacket(line));
        }
    };
    parseInput(filePath, lineCallback);

    return packets;
}

bool arePacketsInCorrectOrder(const Packet& first, const Packet& second) {
    return first <= second;
}

int32_t calculateSumOfCorrectPackets(const std::vector<Packet>& packets, const bool printOutput) {
    auto sumOfCorrectPacketsIndices = 0;
    for (auto index = 0U; index < packets.size() - 1; index += 2) {
        const auto& first = packets.at(index);
        const auto& second = packets.at(index + 1);
        if (arePacketsInCorrectOrder(first, second)) {
            if (printOutput) {
                std::cout << "(Y) " << first.items << " vs " << second.items << '\n';
            }
            sumOfCorrectPacketsIndices += static_cast<int32_t>(index) / 2 + 1;
        } else {
            if (printOutput) {
                std::cout << "(N) " << first.items << " vs " << second.items << '\n';
            }
        }
    }
    return sumOfCorrectPacketsIndices;
}

int32_t calculateDecoderKey(const std::vector<Packet>& packets, const bool printOutput) {
    std::vector<Packet> extended = packets;
    const Packet packet1 = parsePacket("[[2]]");
    const Packet packet2 = parsePacket("[[6]]");
    extended.push_back(packet1);
    extended.push_back(packet2);

    std::sort(extended.begin(), extended.end());
    size_t index1 = 0;
    size_t index2 = 0;

    for (auto i = 0U; i < extended.size(); ++i) {
        if (extended.at(i) == packet1) {
            index1 = i + 1;
        }
        if (extended.at(i) == packet2) {
            index2 = i + 1;
        }

        if (printOutput) {
            std::cout << extended.at(i).items << '\n';
        }
    }
    return static_cast<int32_t>(index1 * index2);
}
}  // namespace

std::pair<std::string, std::string> day13(bool printOutput) {
    const auto input = parse("resources/day13.txt");
    const auto sumOfCorrectPackets = calculateSumOfCorrectPackets(input, printOutput);
    const auto decoderKey = calculateDecoderKey(input, printOutput);

    return {std::to_string(sumOfCorrectPackets), std::to_string(decoderKey)};
}
}  // namespace bblp::aoc
