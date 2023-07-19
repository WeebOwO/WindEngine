#include "Runtime/Engine.h"

auto main() -> int {
    wind::Engine engine;
    engine.SetShowCase(ShowCase::Sponza);
    engine.Run();
    return 0;
}