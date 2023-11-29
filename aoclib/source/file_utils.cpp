#include "bblp/aoc/file_utils.hpp"

#include <fstream>

namespace bblp::aoc {
void parseInput(const std::filesystem::path& filePath, const std::function<void(const std::string&)>& lineCallback) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file");
    }

    std::string line;
    while (std::getline(file, line)) {
        lineCallback(line);
    }
}
}  // namespace bblp::aoc
