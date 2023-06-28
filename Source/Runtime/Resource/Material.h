#pragma once

#include <memory>
#include <unordered_map>

#include "Runtime/Render/RHI/Backend.h"
#include "Runtime/Render/RHI/CommandBuffer.h"
#include "Runtime/Render/RHI/Shader.h"

namespace wind {

struct DefaultLit {
    struct Parameter {

    };
};

template<typename ShadingModel> 
class ShaderParameter {
    ShadingModel shadingModel;
};

class Material {
    
};
} // namespace wind