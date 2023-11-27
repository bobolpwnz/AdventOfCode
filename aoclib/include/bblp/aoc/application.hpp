#pragma once

#include <array>
#include <functional>
#include <string>
#include <optional>

namespace bblp {
namespace aoc {
class Application {
  public:
    using DayFunction = std::function<std::pair<std::string, std::string>(void)>;
    static constexpr size_t MAX_DAY_COUNT = 25;

    Application(const int argc, const char* argv[], const std::array<DayFunction, MAX_DAY_COUNT>& dayFunctions);

    void run();

  private:
    uint32_t getLastDayToRun() const;

    std::array<DayFunction, MAX_DAY_COUNT> mDayFunctions;
    std::optional<uint32_t> mDayToRun;
};
}  // namespace aoc
}  // namespace bblp
