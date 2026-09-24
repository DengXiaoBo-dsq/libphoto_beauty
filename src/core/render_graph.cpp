#include "core/render_graph.h"
namespace pbe {
void RenderGraph::clear() { passes_.clear(); }
void RenderGraph::add(const char* name, Pass pass) {
    passes_.push_back({name ? name : "pass", std::move(pass)});
}
void RenderGraph::execute(RenderContext& ctx) const {
    for (const auto& p : passes_) if (p.fn) p.fn(ctx);
}
}
