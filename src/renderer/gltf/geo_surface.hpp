#pragma once

#include <cstdint>
namespace jengine::renderer::gltf {
    struct GeoSurface {
        uint32_t start_index;
        uint32_t index_count;
    };
}
