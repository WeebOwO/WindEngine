#include <iostream>

#include <vulkan/vulkan.hpp>

#include "macro.h"

namespace wind {
    
class RenderContext {
public:
    PERMIT_COPY(RenderContext);

    static void Init();
    static void Quit();

    static RenderContext& GetInstance();
    ~RenderContext();

private:
    RenderContext();    
};  
}; // namespace wind