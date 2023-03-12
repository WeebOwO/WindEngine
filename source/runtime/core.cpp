#include "core.h"

#include "context.h"

namespace wind {
    void Init() {
        RenderContext::Init();
    }
    void Quit() {
        RenderContext::Quit();
    }
}