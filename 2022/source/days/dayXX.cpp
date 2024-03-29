#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"

#include <algorithm>
#include <cstdlib>
#include <numeric>
#include <vector>

namespace bblp::aoc {
namespace {

auto parse(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    auto input;
    const auto lineCallback = [&input](const std::string& line) {};
    parseInput(filePath, lineCallback);
    return input;
}
}  // namespace

std::pair<std::string, std::string> dayXX() {
    const auto input = parse("resources/dayXX.txt");
    return {"", ""};
}
}  // namespace bblp::aoc
