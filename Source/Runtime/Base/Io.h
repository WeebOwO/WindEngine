#pragma once

#include <string_view>
#include <vector>

#include "Runtime/Resource/Mesh.h"

namespace wind::io {
std::vector<char>     ReadFile(std::string_view filename);
std::vector<uint32_t> ReadSpirvBinaryFile(std::string_view filename);
Model::Builder        LoadModelFromFilePath(std::string_view filename);
} // namespace wind::io