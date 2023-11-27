#include "days.hpp"

#include "common/file_utils.hpp"
#include "common/string_utils.hpp"

#include <algorithm>
#include <cstdlib>
#include <list>
#include <numeric>
#include <ranges>
#include <stack>
#include <string_view>
#include <vector>

namespace bblp::advent_of_code_2022 {
namespace {

struct File {
    File() = default;
    File(std::string name, const uint32_t size) : name(std::move(name)), size(size) {}

    std::string name;
    uint32_t size{0U};
};

struct Directory {
    Directory() = default;
    Directory(std::string name, Directory* parent) : name(std::move(name)), parent(parent) {}

    std::string name;
    std::list<Directory> directories;
    std::list<File> files;
    Directory* parent{nullptr};
    uint32_t totalSize{0U};
};

auto parse(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    std::vector<std::string> input;
    const auto lineCallback = [&input](const std::string& line) { input.push_back(line); };
    parseInput(filePath, lineCallback);
    return input;
}

Directory* parseEnterDirectoryLine(Directory& root,
                                   Directory* const currentDirectory,
                                   const std::string& directoryName) {
    static constexpr std::string_view directoryUpName{".."};

    if (directoryName == root.name) {
        return &root;
    } else if (directoryName == directoryUpName) {
        return currentDirectory->parent;
    } else {
        const auto iter =
            std::find_if(currentDirectory->directories.begin(), currentDirectory->directories.end(),
                         [&directoryName](const Directory& directory) { return directoryName == directory.name; });
        if (iter != currentDirectory->directories.end()) {
            return &*iter;
        } else {
            throw std::runtime_error("Unknown directory");
        }
    }
}

void parseListDirectoryLine(Directory* const currentDirectory, const std::string& line) {
    static constexpr std::string_view directoryEntry{"dir "};

    if (line.starts_with(directoryEntry)) {
        const std::string directoryName = line.substr(directoryEntry.size(), line.size() - directoryEntry.size());
        const auto iter =
            std::find_if(currentDirectory->directories.begin(), currentDirectory->directories.end(),
                         [&directoryName](const Directory& directory) { return directoryName == directory.name; });
        if (iter == currentDirectory->directories.end()) {
            currentDirectory->directories.emplace_back(directoryName, currentDirectory);
        }
    } else {
        const auto parts = split(line, " ");
        const auto iter = std::find_if(currentDirectory->files.begin(), currentDirectory->files.end(),
                                       [name = parts[1]](const File& file) { return name == file.name; });
        if (iter == currentDirectory->files.end()) {
            currentDirectory->files.emplace_back(parts[1], std::stoul(parts[0]));
        }
    }
}

void buildDirectoryTree(Directory& root, const std::vector<std::string>& input) {
    static constexpr std::string_view commandListDirectory{"$ ls"};
    static constexpr std::string_view commandEnterDirectory{"$ cd"};

    Directory* currentDirectory = nullptr;

    enum class Command { CD, LS };
    std::optional<Command> lastCommand;
    for (const auto& line : input) {
        if (line.empty()) {
            continue;
        }

        if (line.starts_with(commandListDirectory)) {
            lastCommand = Command::LS;
        } else if (line.starts_with(commandEnterDirectory)) {
            lastCommand = Command::CD;
            const auto headerSize = commandEnterDirectory.size() + 1;
            const auto directoryName = line.substr(headerSize, line.size() - headerSize);
            currentDirectory = parseEnterDirectoryLine(root, currentDirectory, directoryName);
        } else if (lastCommand.has_value()) {
            switch (*lastCommand) {
                case Command::LS:
                    parseListDirectoryLine(currentDirectory, line);
                    break;
                default:
                    throw std::runtime_error("Invalid last command");
            }
        } else {
            throw std::runtime_error("Invalid line");
        }
    }
}

void calculateDirectorySizes(Directory& directory) {
    for (const auto& file : directory.files) {
        directory.totalSize += file.size;
    }

    for (auto& childDirectory : directory.directories) {
        calculateDirectorySizes(childDirectory);
        directory.totalSize += childDirectory.totalSize;
    }
}

void collectDirectories(Directory& directory, std::vector<Directory*>& directoriesToCollect) {
    directoriesToCollect.push_back(&directory);

    for (auto& childDirectory : directory.directories) {
        collectDirectories(childDirectory, directoriesToCollect);
    }
}
}  // namespace

std::pair<std::string, std::string> day07() {
    const auto input = parse("resources/day07.txt");

    Directory root;
    root.name = "/";
    root.parent = &root;

    buildDirectoryTree(root, input);
    calculateDirectorySizes(root);

    std::vector<Directory*> allDirectories;
    static constexpr size_t expectedNumberOfDirectories = 1000U;
    allDirectories.reserve(expectedNumberOfDirectories);
    collectDirectories(root, allDirectories);
    std::sort(allDirectories.begin(), allDirectories.end(),
              [](const Directory* first, const Directory* second) { return first->totalSize < second->totalSize; });

    static constexpr uint32_t smallDirectorySizeThreshold = 100000U;

    const auto sumOfDirectoriesSmallSizes = std::accumulate(
        allDirectories.cbegin(), allDirectories.cend(), 0U, [](const uint32_t sum, const Directory* directory) {
            if (directory->totalSize <= smallDirectorySizeThreshold) {
                return sum + directory->totalSize;
            } else {
                return sum;
            }
        });

    static constexpr uint32_t filesystemSize = 70000000U;
    static constexpr uint32_t sizeNeededForUpdate = 30000000U;

    const uint32_t spaceFree = filesystemSize - root.totalSize;
    const uint32_t spaceNeeded = sizeNeededForUpdate - spaceFree;

    const auto iter = std::find_if(allDirectories.cbegin(), allDirectories.cend(),
                                   [spaceNeeded](const Directory* dir) { return dir->totalSize >= spaceNeeded; });

    return {std::to_string(sumOfDirectoriesSmallSizes), std::to_string((*iter)->totalSize)};
}
}  // namespace bblp::advent_of_code_2022
