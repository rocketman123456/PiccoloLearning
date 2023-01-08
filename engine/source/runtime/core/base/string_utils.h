#pragma once

#include <string>
#include <vector>

namespace Piccolo
{
    void split_multi_char(const std::string& in, const std::string& token, std::vector<std::string>& out);
    void split_single_char(const std::string& in, const char token, std::vector<std::string>& out);

    std::string replace_all(const std::string& target, const std::string& from, const std::string& to);

    float string_match(const std::string& left, const std::string& right);
} // namespace Piccolo