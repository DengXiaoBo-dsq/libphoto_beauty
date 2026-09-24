#include "core/analyzer.h"
#include "effects/mask.h"
#include "core/color.h"
#include <algorithm>
#include <cmath>
namespace pbe {
PBESceneStats Analyzer::analyze(const ImageRGBA8& image) const {
    PBESceneStats s{};
    if (image.empty()) return s;
    const int step = std::max(1, std::max(image.width, image.height) / 384);
    double mean=0, mean2=0, hi=0, lo=0, warm=0, skin=0, n=0;
    for (int y=0;y<image.height;y+=step) {
        const uint8_t* row=image.row(y);
        for (int x=0;x<image.width;x+=step) {
            int i=x*4;
            RGB c{row[i]/255.0f,row[i+1]/255.0f,row[i+2]/255.0f};
            const double l=luminance(c);
            mean+=l; mean2+=l*l;
            hi += l>0.985; lo += l<0.03;
            warm += std::max(0.0f, c.r-c.b);
            skin += effects::fallback_skin_probability(c.r,c.g,c.b) ? 1.0 : 0.0;
            n+=1.0;
        }
    }
    mean/=n; mean2/=n;
    s.mean_luma=static_cast<float>(mean);
    s.contrast=static_cast<float>(std::sqrt(std::max(0.0,mean2-mean*mean)));
    s.highlight_clip=static_cast<float>(hi/n);
    s.shadow_clip=static_cast<float>(lo/n);
    s.warm_score=static_cast<float>(warm/n);
    s.skin_fraction=static_cast<float>(skin/n);
    return s;
}
}
