#include <array>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <span>

#include "day01/day01.hpp"
#include "day02/day02.hpp"

static constexpr int CURRENT_DAY = 1;
static constexpr size_t MAX_DAY_COUNT = 25;
static constexpr int ARGV_INTEGER_BASE = 10;

int main(const int argc, const char* argv[]) {
    namespace aoc = bblp::advent_of_code_2022;

    try {
        const std::array<std::function<std::tuple<int64_t, int64_t>()>, MAX_DAY_COUNT> days{aoc::day01, aoc::day02};
        const std::span<const char*> args = {argv, static_cast<size_t>(argc)};

        int dayToRun = CURRENT_DAY;
        if (argc > 1) {
            dayToRun = std::strtol(args[1], nullptr, ARGV_INTEGER_BASE);
        }

        if (dayToRun < 1 || dayToRun > CURRENT_DAY) {
            throw std::logic_error("Invalid day");
        }

        std::cout << "Running day " << dayToRun << '\n';
        const auto timepointBefore = std::chrono::system_clock::now();
        const auto result = days.at(dayToRun - 1)();
        const auto timepointAfter = std::chrono::system_clock::now();
        const auto elapsedTime = timepointAfter - timepointBefore;
        std::cout << "Part 1 result: " << std::get<0>(result) << '\n';
        std::cout << "Part 2 result: " << std::get<1>(result) << '\n';
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
