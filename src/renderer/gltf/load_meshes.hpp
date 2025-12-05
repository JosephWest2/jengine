#pragma once

#include <filesystem>
#include <vector>
#include "renderer/gltf/mesh_asset.hpp"

namespace jengine::renderer::gltf {
std::vector<MeshAsset> LoadMeshes(const std::filesystem::path file_path);
}
