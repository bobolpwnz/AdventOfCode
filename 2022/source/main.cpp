#include <array>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <span>

#include "days/days.hpp"
#include "bblp/aoc/application.hpp"

static constexpr int CURRENT_DAY = 16;
static constexpr size_t MAX_DAY_COUNT = 25;

int main(const int argc, const char* argv[]) {
    try {
        const std::array<std::function<std::pair<std::string, std::string>()>, MAX_DAY_COUNT> days{
            bblp::aoc::day01,
            bblp::aoc::day02,
            bblp::aoc::day03,
            bblp::aoc::day04,
            bblp::aoc::day05,
            bblp::aoc::day06,
            bblp::aoc::day07,
            bblp::aoc::day08,
            bblp::aoc::day09,
            []() { return bblp::aoc::day10(true); },
            bblp::aoc::day11,
            bblp::aoc::day12,
            []() { return bblp::aoc::day13(true); },
            bblp::aoc::day14,
            bblp::aoc::day15,
            bblp::aoc::day16};
        bblp::aoc::Application app{argc, argv, days};
        app.run();
        return 0;
    } catch (const std::exception& ex) {
        std::cout << "Execution failed due to exception:  " << ex.what() << '\n';
        return -1;
    } catch (...) {
        std::cout << "Execution failed due to unknown failure" << '\n';
        return -2;
    }
}
