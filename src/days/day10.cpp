#include "days.hpp"

#include "common/file_utils.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <numeric>
#include <vector>

namespace bblp::advent_of_code_2022 {
namespace {
constexpr int32_t CRT_WIDTH = 40;

enum class Instruction { NOOP = 1, ADDX = 2 };

auto parse(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;
    static constexpr std::string_view INSTRUCTION_NOOP = "noop";
    static constexpr std::string_view INSTRUCTION_ADDX = "addx";

    std::vector<std::pair<Instruction, int32_t>> input;
    input.reserve(BUFFER_SIZE);
    const auto lineCallback = [&input](const std::string& line) {
        if (!line.empty()) {
            if (line.starts_with(INSTRUCTION_NOOP)) {
                input.emplace_back(Instruction::NOOP, 0);
            } else if (line.starts_with(INSTRUCTION_ADDX)) {
                input.emplace_back(Instruction::ADDX, std::stol(line.substr(INSTRUCTION_ADDX.size() + 1)));
            }
        }
    };
    parseInput(filePath, lineCallback);
    return input;
}

int32_t calculateSignalStrength(const std::vector<std::pair<Instruction, int32_t>>& input) {
    static constexpr int32_t CYCLE_START = 20;
    static constexpr int32_t CYCLE_DIFF = 40;

    int32_t signalStrength = 0;
    int32_t registerValue = 1;
    int32_t cycleNumber = 0;
    for (const auto& command : input) {
        cycleNumber++;
        switch (command.first) {
            case Instruction::NOOP: {
                if ((cycleNumber + CYCLE_START) % CYCLE_DIFF == 0) {
                    const int32_t current = CYCLE_START * ((cycleNumber + 1) / CYCLE_START) * registerValue;
                    signalStrength += current;
                }
            } break;
            case Instruction::ADDX: {
                if (((cycleNumber + CYCLE_START) % CYCLE_DIFF == CYCLE_DIFF - 1) ||
                    ((cycleNumber + CYCLE_START) % CYCLE_DIFF == 0)) {
                    const int32_t current = CYCLE_START * ((cycleNumber + 2) / CYCLE_START) * registerValue;
                    signalStrength += current;
                }
                cycleNumber++;
                registerValue += command.second;
            } break;
        }
    }
    return signalStrength;
}

void printSpritePosition(std::ostream& stream, const int32_t registerValue) {
    stream << "Sprite position:  ";
    for (auto i = 0; i < CRT_WIDTH; ++i) {
        if (i == registerValue - 1 || i == registerValue || i == registerValue + 1) {
            stream << "#";
        } else {
            stream << ".";
        }
    }
    stream << "\n\n";
}

void printStartCycle(std::ostream& stream, const int32_t cycleNumber) {
    stream << "Start cycle  " << std::setw(3) << cycleNumber << ": ";
}

void printDuringCycle(std::ostream& stream, const int32_t cycleNumber) {
    stream << "During cycle " << std::setw(3) << cycleNumber << ": ";
}

void printEndOfCycle(std::ostream& stream, const int32_t cycleNumber) {
    stream << "End of cycle " << std::setw(3) << cycleNumber << ": ";
}

void printCrtRow(std::ostream& stream, const std::vector<char>& crt, const int32_t start) {
    stream << "Current CRT row : ";
    for (auto iter = crt.begin() + start; iter != crt.end(); ++iter) {
        stream << *iter;
    }
    stream << '\n';
}

void drawAtPosition(std::ostream& stream, std::vector<char>& crt, const int32_t registerValue, const int32_t crtPosition) {
    stream << "CRT draws pixel in position " << crtPosition << '\n';
    if (crtPosition == registerValue - 1 || crtPosition == registerValue || crtPosition == registerValue + 1) {
        crt.emplace_back('#');
    } else {
        crt.emplace_back('.');
    }
}

std::vector<char> calculateCrtOutput(const std::vector<std::pair<Instruction, int32_t>>& input, std::ostream& stream) {
    static constexpr size_t BUFFER_SIZE = 10000;

    int32_t registerValue = 1;
    int32_t cycleNumber = 0;
    std::vector<char> crt;
    crt.reserve(BUFFER_SIZE);

    printSpritePosition(stream, registerValue);
    for (const auto& command : input) {
        cycleNumber++;
        printStartCycle(stream, cycleNumber);
        switch (command.first) {
            case Instruction::NOOP:
                stream << "begin executing noop\n";

                drawAtPosition(stream, crt, registerValue, (cycleNumber - 1) % CRT_WIDTH);
                printCrtRow(stream, crt, CRT_WIDTH * (cycleNumber / CRT_WIDTH));
                stream << '\n';
                break;

            case Instruction::ADDX:
                stream << "begin executing addx " << command.second << '\n';
                printDuringCycle(stream, cycleNumber);
                drawAtPosition(stream, crt, registerValue, (cycleNumber - 1) % CRT_WIDTH);
                printCrtRow(stream, crt, CRT_WIDTH * (cycleNumber / CRT_WIDTH));

                cycleNumber++;
                stream << '\n';
                printDuringCycle(stream, cycleNumber);
                drawAtPosition(stream, crt, registerValue, (cycleNumber - 1) % CRT_WIDTH);
                printCrtRow(stream, crt, CRT_WIDTH * (cycleNumber / CRT_WIDTH));

                registerValue += command.second;
                printEndOfCycle(stream, cycleNumber);
                stream << "finish executing addx " << command.second << " (Register X is now " << registerValue
                          << ")\n";
                break;
        }

        printSpritePosition(stream, registerValue);
    }

    for (auto i = 0U; i < crt.size(); ++i) {
        if (i % CRT_WIDTH == 0) {
            stream << '\n';
        }
        stream << crt.at(i);
    }
    stream << '\n';
    return crt;
}
}  // namespace

std::pair<std::string, std::string> day10(const bool printOutput) {
    std::ostringstream stream;

    const auto input = parse("resources/day10.txt");
    const auto signalStrength = calculateSignalStrength(input);
    const auto crtOutput = calculateCrtOutput(input, stream);
    if (printOutput) {
        std::cout << stream.str() << '\n';
    }

    return {std::to_string(signalStrength), std::string{crtOutput.begin(), crtOutput.end()}};
}
}  // namespace bblp::advent_of_code_2022
