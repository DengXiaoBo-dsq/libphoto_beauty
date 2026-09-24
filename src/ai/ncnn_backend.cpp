#ifdef PBE_ENABLE_NCNN
#include "ai/ai_backend.h"
#include <ncnn/net.h>
namespace pbe {
class NcnnBackend final : public AIBackend {
public:
    explicit NcnnBackend(const char*) {}
    bool available() const override { return true; }
    bool analyze_face(const uint8_t*,int,int,PBEFaceData&) override {
        // Model-specific implementation is intentionally separate from the core ABI.
        return false;
    }
};
}
#endif
