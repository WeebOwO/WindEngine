#pragma once

#include <vector>
#include <string_view>

namespace wind::io {
    std::vector<char> ReadFile(std::string_view filename);
}