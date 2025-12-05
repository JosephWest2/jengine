#pragma once

#include "renderer/vulkan/buffers/mesh_buffers.hpp"
#include "renderer/gltf/geo_surface.hpp"

namespace jengine::renderer {
    struct GPULoadedMeshAsset {
        std::string name;
        std::vector<gltf::GeoSurface> surfaces;
        vulkan::buffers::MeshBuffers mesh_buffers;
        std::vector<glm::mat4> instances;
    };
}
