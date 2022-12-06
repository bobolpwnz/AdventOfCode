#include "days.hpp"

#include "common/file_utils.hpp"

#include <algorithm>
#include <cstdlib>
#include <numeric>
#include <vector>
#include <set>

namespace bblp::advent_of_code_2022 {
namespace {

auto parse(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    std::string input;
    input.reserve(BUFFER_SIZE);
    const auto lineCallback = [&input](const std::string& line) { input = line; };
    parseInput(filePath, lineCallback);
    return input;
}

uint32_t findEndOfMarkerIndex(const std::string& input, const uint32_t markerSize) {
    std::list<char> marker;
    uint32_t endIndex = 0;
    for (const auto character : input) {
        if (marker.size() == markerSize) {
            marker.pop_front();
        }

        marker.push_back(character);
        ++endIndex;

        if (marker.size() == markerSize) {
            const std::set<char> set{marker.begin(), marker.end()};
            if (marker.size() == set.size()) {
                break;
            }
        }
    }
    return endIndex;
}
}  // namespace

std::pair<std::string, std::string> day06() {
    static constexpr size_t START_OF_PACKET_MARKER_SIZE = 4;
    static constexpr size_t START_OF_MESSAGE_MARKER_SIZE = 14;

    const auto input = parse("resources/day06.txt");
    
    const auto endOfPacketIndex = findEndOfMarkerIndex(input, START_OF_PACKET_MARKER_SIZE);
    const auto endOfMessageIndex = findEndOfMarkerIndex(input, START_OF_MESSAGE_MARKER_SIZE);

    return {std::to_string(endOfPacketIndex), std::to_string(endOfMessageIndex)};
}
}  // namespace bblp::advent_of_code_2022
