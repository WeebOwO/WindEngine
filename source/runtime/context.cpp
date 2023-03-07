#include "context.h"

namespace wind {
    void RenderContext::Init() {
        vk::InstanceCreateInfo createInfo;
        
    }

    void RenderContext::Quit() {
        
    }

    RenderContext& RenderContext::GetInstance() {
        static RenderContext context;
        return context;
    }
}