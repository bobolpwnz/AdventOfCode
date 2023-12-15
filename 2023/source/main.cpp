#include <array>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <span>

#include "bblp/aoc/application.hpp"
#include "days.hpp"

static constexpr size_t MAX_DAY_COUNT = 25;

int main(const int argc, const char* argv[]) {
    try {
        const std::array<std::function<std::pair<std::string, std::string>()>, MAX_DAY_COUNT> days{
            bblp::aoc::day01, bblp::aoc::day02, bblp::aoc::day03, bblp::aoc::day04, bblp::aoc::day05,
            bblp::aoc::day06, bblp::aoc::day07, bblp::aoc::day08, bblp::aoc::day09, bblp::aoc::day10,
            bblp::aoc::day11, bblp::aoc::day12, bblp::aoc::day13, bblp::aoc::day14, bblp::aoc::day15};
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
