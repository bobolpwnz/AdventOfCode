#pragma once

#include <filesystem>
#include <functional>
#include <string>

namespace bblp::aoc {
void parseInput(const std::filesystem::path& filePath, const std::function<void(const std::string&)>& lineCallback);
}
