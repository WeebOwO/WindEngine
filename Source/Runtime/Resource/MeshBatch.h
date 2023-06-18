#pragma once

#include <memory>

#include "Runtime/Render/RHI/Buffer.h"
#include "Runtime/Resource/Material.h"

namespace wind {
struct MeshBatch {
    std::shared_ptr<Buffer>   vertexBuffer;
    std::shared_ptr<Buffer>   indexBuffer;
    std::shared_ptr<Material> material;
};

} // namespace wind