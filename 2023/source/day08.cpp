#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"
#include "bblp/aoc/point.hpp"
#include "bblp/aoc/string_utils.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace bblp::aoc {
namespace {

struct Node {
    std::string left;
    std::string right;
};

struct Network {
    std::string instructions;

    std::unordered_map<std::string, Node> nodes;
};

auto parse(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    Network input;
    input.nodes.reserve(BUFFER_SIZE);
    const auto lineCallback = [&input](const std::string& line) {
        if (line.length() == 0U) {
            return;
        }

        if (line.find("=") == std::string::npos) {
            input.instructions = line;
        } else {
            const auto parts = split(line, "=");
            const auto nodeParts = split(trim(parts.back()), ",");
            input.nodes.insert(
                {trim(parts.front()), Node{trim(nodeParts.front().substr(1U)),
                                           trim(nodeParts.back().substr(0U, nodeParts.back().length() - 1U))}});
        }
    };
    parseInput(filePath, lineCallback);
    return input;
}

uint64_t calculatePartOne(const Network& input) {
    uint64_t result{0U};
    uint64_t currentStep{0U};
    std::string currentNodeName{"AAA"};
    const Node* currentNode{&input.nodes.at(currentNodeName)};

    while (currentNodeName != "ZZZ") {
        const auto direction = input.instructions.at(currentStep);
        if (direction == 'R') {
            currentNodeName = currentNode->right;
        } else if (direction == 'L') {
            currentNodeName = currentNode->left;
        } else {
            throw std::logic_error("Invalid direction");
        }
        currentNode = &input.nodes.at(currentNodeName);

        ++result;
        ++currentStep;
        if (currentStep >= input.instructions.size()) {
            currentStep = 0U;
        }
    }

    return result;
}

uint64_t calculatePartTwo(const Network& input) {
    std::vector<std::string> nodeNames;
    for (const auto& elem : input.nodes) {
        if (elem.first.back() == 'A') {
            nodeNames.push_back(elem.first);
        }
    }
    std::vector<uint64_t> results;
    for (const auto& nodeName : nodeNames) {
        uint64_t result{0U};
        uint64_t currentStep{0U};
        std::string currentNodeName{nodeName};
        const Node* currentNode{&input.nodes.at(currentNodeName)};

        while (currentNodeName.back() != 'Z') {
            const auto direction = input.instructions.at(currentStep);
            if (direction == 'R') {
                currentNodeName = currentNode->right;
            } else if (direction == 'L') {
                currentNodeName = currentNode->left;
            } else {
                throw std::logic_error("Invalid direction");
            }
            currentNode = &input.nodes.at(currentNodeName);

            ++result;
            ++currentStep;
            if (currentStep >= input.instructions.size()) {
                currentStep = 0U;
            }
        }
        results.push_back(result);
    }
    uint64_t result{0U};
    for (const auto r : results) {
        if (result == 0U) {
            result = r;
        } else {
            result = std::lcm(result, r);
        }
    }
    return result;
}
}  // namespace

std::pair<std::string, std::string> day08() {
    const auto input = parse("resources/day08.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
