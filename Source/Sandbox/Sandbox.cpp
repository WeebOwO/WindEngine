#include "Runtime/Engine.h"

auto main() -> int {
    wind::Engine engine;
    engine.SetShowCase(ShowCase::Pbr);
    engine.Run();
    return 0;
}