#include <mbgl/style/style_bucket.hpp>

namespace mbgl {

StyleBucket::StyleBucket(StyleLayerType type) {
    switch (type) {
        case StyleLayerType::Fill: render.set<StyleBucketFill>(); break;
        case StyleLayerType::Line: render.set<StyleBucketLine>(); break;
        case StyleLayerType::Symbol: render.set<StyleBucketSymbol>(); break;
        case StyleLayerType::Raster: render.set<StyleBucketRaster>(); break;
        default: break;
    }
}

}