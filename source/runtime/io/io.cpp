#include "runtime/io/io.h"

#include <fstream>
#include <stdexcept>

namespace wind::io {
std::vector<char> ReadFile(std::string_view filename) {
    std::ifstream file(filename.data(), std::ios::ate | std::ios::binary);
    if (!file.is_open()) { throw std::runtime_error("failed to open file!"); }

    size_t            fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}
} // namespace wind::io