#include <array>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <span>

#include "days/days.hpp"

static constexpr int CURRENT_DAY = 16;
static constexpr size_t MAX_DAY_COUNT = 25;

int main(const int argc, const char* argv[]) {
    namespace aoc = bblp::advent_of_code_2022;

    try {
        const std::array<std::function<std::pair<std::string, std::string>()>, MAX_DAY_COUNT> days{
            aoc::day01,
            aoc::day02,
            aoc::day03,
            aoc::day04,
            aoc::day05,
            aoc::day06,
            aoc::day07,
            aoc::day08,
            aoc::day09,
            []() { return aoc::day10(true); },
            aoc::day11,
            aoc::day12,
            []() { return aoc::day13(true); },
            aoc::day14,
            aoc::day15,
            aoc::day16};
        const std::span<const char*> args = {argv, static_cast<size_t>(argc)};

        int dayToRun = CURRENT_DAY;
        if (argc > 1) {
            dayToRun = std::stoi(args[1]);
        }

        if (dayToRun < 1 || dayToRun > CURRENT_DAY) {
            throw std::logic_error("Invalid day");
        }

        std::cout << "Running day " << dayToRun << '\n';
        const auto timepointBefore = std::chrono::system_clock::now();
        const auto result = days.at(dayToRun - 1)();
        const auto timepointAfter = std::chrono::system_clock::now();
        const auto elapsedTime = timepointAfter - timepointBefore;
        std::cout << "Part 1 result: " << result.first << '\n';
        std::cout << "Part 2 result: " << result.second << '\n';
        std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count()
                  << "ms" << '\n';
        return 0;
    } catch (const std::exception& ex) {
        std::cout << "Execution failed due to exception:  " << ex.what() << '\n';
        return -1;
    } catch (...) {
        std::cout << "Execution failed due to unknown failure" << '\n';
        return -2;
    }
}
