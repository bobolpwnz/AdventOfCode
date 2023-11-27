#include "bblp/aoc/application.hpp"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <span>

namespace bblp {
namespace aoc {

Application::Application(const int argc,
                         const char* argv[],
                         const std::array<DayFunction, MAX_DAY_COUNT>& dayFunctions) {
    std::copy(dayFunctions.begin(), dayFunctions.end(), mDayFunctions.begin());

    const std::span<const char*> args{argv, static_cast<size_t>(argc)};
    if (argc > 1) {
        mDayToRun = std::stoul(args[1]);
    }
}

void Application::run() {
    const auto dayToRun = (mDayToRun.has_value() ? *mDayToRun : getLastDayToRun());

    std::cout << "Running day " << dayToRun << '\n';
    if (dayToRun < 1 || dayToRun > MAX_DAY_COUNT) {
        throw std::logic_error("Invalid day");
    }

    const auto& dayFunction = mDayFunctions.at(dayToRun - 1);
    if (dayFunction == nullptr) {
        throw std::logic_error("No solution for requested day");
    }

    const auto timepointBefore = std::chrono::system_clock::now();
    const auto result = dayFunction();
    const auto timepointAfter = std::chrono::system_clock::now();
    const auto elapsedTime = timepointAfter - timepointBefore;
    std::cout << "Part 1 result: " << result.first << '\n';
    std::cout << "Part 2 result: " << result.second << '\n';
    std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count() << "ms"
              << '\n';
}

uint32_t Application::getLastDayToRun() const {
    const auto iter = std::find_if(mDayFunctions.rbegin(), mDayFunctions.rend(),
                                   [](const auto& dayFunction) { return dayFunction != nullptr; });
    if (iter != mDayFunctions.rend()) {
        return mDayFunctions.rend() - iter;
    } else {
        throw std::logic_error("No solutions found");
    }
}
}  // namespace aoc
}  // namespace bblp
