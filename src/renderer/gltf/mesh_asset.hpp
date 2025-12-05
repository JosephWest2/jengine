#pragma once

#include <string>
#include <vector>

#include "renderer/gltf/geo_surface.hpp"
#include "renderer/vulkan/buffers/mesh_buffers.hpp"

namespace jengine::renderer::gltf {
struct MeshAsset {
    std::string name;
    std::vector<GeoSurface> surfaces;
    std::vector<uint32_t> indices;
    std::vector<vulkan::buffers::Vertex> vertices;
};
}  // namespace jengine::renderer::gltf
