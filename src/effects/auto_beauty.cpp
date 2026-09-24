#include "core/engine.h"

namespace pbe::effects {
// Auto-enhance is implemented on Engine because it needs image statistics.
// This file intentionally reserves the effects namespace for future learned
// parameter estimation that can be moved out of the engine without ABI changes.
}
