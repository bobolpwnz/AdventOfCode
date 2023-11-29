#include "bblp/aoc/string_utils.hpp"

namespace bblp::aoc {

static constexpr std::string_view WHITESPACE = " \n\r\t\f\v";

std::vector<std::string> split(const std::string& input, const std::string& delimiter) {
    size_t pos_start = 0;
    size_t pos_end = 0;
    const size_t delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = input.find(delimiter, pos_start)) != std::string::npos) {
        token = input.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(input.substr(pos_start));
    return res;
}

std::string ltrim(const std::string& s) {
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string& s) {
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string& s) {
    return rtrim(ltrim(s));
}
}  // namespace bblp::aoc
