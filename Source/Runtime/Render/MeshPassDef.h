#pragma once

#include <cstdint>

#include "Runtime/Base/Macro.h"

namespace wind {
    // define our mesh pass
    enum MeshPassType : int {
        BasePass = BIT(0),
        TranslucentPass = BIT(1),
        ShadowPass = BIT(2)
    };
}