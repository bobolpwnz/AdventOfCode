#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"
#include "bblp/aoc/string_utils.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <numeric>
#include <ranges>
#include <vector>

namespace bblp::aoc {
namespace {

using ContainerId = char;
using StackLayout = std::vector<std::vector<ContainerId>>;

struct MoveProcedure {
    uint32_t quantity;
    uint32_t source;
    uint32_t destination;
};

using MoveProcedures = std::vector<MoveProcedure>;

StackLayout parseStartingLayout(const std::vector<std::string>& stackLayoutLines) {
    static constexpr size_t BUFFER_SIZE = 10;

    const auto width = stackLayoutLines.front().size();

    StackLayout layout{};
    layout.reserve(BUFFER_SIZE);
    for (auto i = 0U; i < width; ++i) {
        const bool isStackColumn = (i - 1) % 4 == 0;
        if (isStackColumn) {
            std::vector<ContainerId> stack{};
            stack.reserve(BUFFER_SIZE);

            for (const auto& line : std::ranges::reverse_view{stackLayoutLines}) {
                const auto symbol = line.at(i);
                if (line.at(i) >= 'A' && line.at(i) <= 'Z') {
                    stack.push_back(symbol);
                }
            }

            layout.push_back(stack);
        }
    }
    return layout;
}

MoveProcedure parseMoveProcedure(const std::string& line) {
    static constexpr size_t QUANTITY_INDEX = 1;
    static constexpr size_t SOURCE_INDEX = 3;
    static constexpr size_t DESTINATION_INDEX = 5;

    const auto elements = split(line, " ");
    return MoveProcedure{std::stoul(elements[QUANTITY_INDEX]), std::stoul(elements[SOURCE_INDEX]),
                         std::stoul(elements[DESTINATION_INDEX])};
}

std::pair<StackLayout, MoveProcedures> parseStacks(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    std::vector<std::string> stackLayoutLines;
    std::pair<StackLayout, MoveProcedures> input;
    bool parsedStartingLayout = false;
    const auto lineCallback = [&input, &stackLayoutLines, &parsedStartingLayout](const std::string& line) {
        if (!line.empty()) {
            if (!parsedStartingLayout) {
                stackLayoutLines.push_back(line);
            } else {
                input.second.push_back(parseMoveProcedure(line));
            }
        } else {
            if (!parsedStartingLayout) {
                input.first = parseStartingLayout(stackLayoutLines);
            }
            parsedStartingLayout = true;
        }
    };
    parseInput(filePath, lineCallback);
    return input;
}

using MoveAlgorithm = std::function<void(const MoveProcedure&, std::vector<ContainerId>&, std::vector<ContainerId>&)>;

std::string reorganize(const std::pair<StackLayout, MoveProcedures>& input, const MoveAlgorithm& algorithm) {
    StackLayout stacks = input.first;
    for (const auto& moveProdecure : input.second) {
        auto& sourceStack = stacks[moveProdecure.source - 1];
        auto& destinationStack = stacks[moveProdecure.destination - 1];
        algorithm(moveProdecure, sourceStack, destinationStack);
    }

    return std::accumulate(
        stacks.cbegin(), stacks.cend(), std::string(),
        [](const std::string& result, const std::vector<ContainerId>& stack) { return result + stack.back(); });
}
}  // namespace

std::pair<std::string, std::string> day05() {
    const auto input = parseStacks("resources/day05.txt");

    const auto algorithmPart1 = [](const MoveProcedure& moveProcedure, std::vector<ContainerId>& source,
                                   std::vector<ContainerId>& destination) {
        for (auto i = 0; i < moveProcedure.quantity; ++i) {
            if (source.empty()) {
                break;
            }

            const auto containerToMove = source.back();
            source.pop_back();
            destination.push_back(containerToMove);
        }
    };
    const auto summaryPart1 = reorganize(input, algorithmPart1);

    const auto algorithmPart2 = [](const MoveProcedure& moveProcedure, std::vector<ContainerId>& source,
                                   std::vector<ContainerId>& destination) {
        std::vector<ContainerId> toMove{source.end() - moveProcedure.quantity, source.end()};

        for (auto i = 0; i < moveProcedure.quantity; ++i) {
            source.pop_back();
        }
        destination.insert(destination.end(), toMove.begin(), toMove.end());
    };
    const auto summaryPart2 = reorganize(input, algorithmPart2);

    return {summaryPart1, summaryPart2};
}
}  // namespace bblp::aoc
