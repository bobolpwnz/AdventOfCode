#include "string_utils.hpp"

namespace bblp::advent_of_code_2022 {
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
}  // namespace bblp::advent_of_code_2022
