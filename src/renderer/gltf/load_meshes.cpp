#include "load_meshes.hpp"

#include <print>
#include <vector>

#include "fastgltf/core.hpp"
#include "fastgltf/glm_element_traits.hpp"
#include "fastgltf/tools.hpp"
#include "fastgltf/types.hpp"
#include "renderer/gltf/geo_surface.hpp"
#include "renderer/gltf/mesh_asset.hpp"
#include "renderer/vulkan/buffers/mesh_buffers.hpp"

namespace jengine::renderer::gltf {

std::vector<MeshAsset> LoadMeshes(const std::filesystem::path file_path) {
    std::println("Loading meshes from {}", file_path.c_str());

    fastgltf::GltfFileStream file_stream(file_path);
    fastgltf::Parser parser{};
    fastgltf::Asset asset;
    auto result = parser.loadGltfBinary(file_stream, file_path.parent_path(), fastgltf::Options::LoadExternalBuffers);
    if (result) {
        asset = std::move(result.get());
    } else {
        throw std::runtime_error(std::format(
            "Failed to load gltf file {}, {}", file_path.c_str(), fastgltf::getErrorMessage(result.error())));
    }

    std::vector<MeshAsset> meshes;
    meshes.reserve(asset.meshes.size());

    std::vector<uint32_t> indices;
    std::vector<vulkan::buffers::Vertex> vertices;
    for (const auto& mesh : asset.meshes) {
        std::vector<GeoSurface> surfaces;
        indices.clear();
        vertices.clear();

        for (const auto& primitive : mesh.primitives) {
            GeoSurface geo_surface = {
                .start_index = static_cast<uint32_t>(indices.size()),
                .index_count = static_cast<uint32_t>(asset.accessors[primitive.indicesAccessor.value()].count),
            };

            size_t initial_vertex = vertices.size();
            {
                fastgltf::Accessor& index_accessor = asset.accessors[primitive.indicesAccessor.value()];
                indices.reserve(indices.size() + index_accessor.count);

                fastgltf::iterateAccessor<uint32_t>(
                    asset, index_accessor, [&](uint32_t index) { indices.push_back(index + initial_vertex); });
            }
            {
                fastgltf::Accessor& position_accessor =
                    asset.accessors[primitive.findAttribute("POSITION")->accessorIndex];
                vertices.resize(vertices.size() + position_accessor.count);
                fastgltf::iterateAccessorWithIndex<glm::vec3>(asset, position_accessor, [&](glm::vec3 v, size_t i) {
                    vertices[initial_vertex + i] = vulkan::buffers::Vertex{
                        .position = v,
                        .uv_x = 0,
                        .normal = glm::vec3(1, 0, 0),
                        .uv_y = 0,
                        .color = glm::vec4(1, 1, 1, 1),
                    };
                });
            }

            auto normals = primitive.findAttribute("NORMAL");
            if (normals) {
                fastgltf::Accessor& normal_accessor = asset.accessors[normals->accessorIndex];
                fastgltf::iterateAccessorWithIndex<glm::vec3>(
                    asset, normal_accessor, [&](glm::vec3 v, size_t i) { vertices[initial_vertex + i].normal = v; });
            }

            auto uvs = primitive.findAttribute("TEXCOORD_0");
            if (uvs) {
                fastgltf::Accessor& uv_accessor = asset.accessors[uvs->accessorIndex];
                fastgltf::iterateAccessorWithIndex<glm::vec2>(asset, uv_accessor, [&](glm::vec2 v, size_t i) {
                    vertices[initial_vertex + i].uv_x = v.x;
                    vertices[initial_vertex + i].uv_y = v.y;
                });
            }

            auto colors = primitive.findAttribute("COLOR_0");
            if (colors) {
                fastgltf::Accessor& color_accessor = asset.accessors[colors->accessorIndex];
                switch (color_accessor.type) {
                    case fastgltf::AccessorType::Vec4:
                        fastgltf::iterateAccessorWithIndex<glm::vec4>(
                            asset, color_accessor, [&](glm::vec4 v, size_t i) {
                                vertices[initial_vertex + i].color = v;
                            });
                        break;
                    case fastgltf::AccessorType::Vec3:
                        fastgltf::iterateAccessorWithIndex<glm::vec3>(
                            asset, color_accessor, [&](glm::vec3 v, size_t i) {
                                vertices[initial_vertex + i].color = glm::vec4(v, 1.0f);
                            });
                        break;
                    default:
                        throw std::runtime_error("Color accessor type not supported, only Vec4 and Vec3 are supported");
                }
            }

            surfaces.push_back(geo_surface);
        }

        constexpr bool OverrideColors = true;
        if (OverrideColors) {
            for (auto& vertex : vertices) {
                vertex.color = glm::vec4(vertex.normal, 1.f);
            }
        }
        meshes.push_back(MeshAsset{
            .name = std::string(mesh.name),
            .surfaces = surfaces,
            .indices = indices,
            .vertices = vertices,
        });
    }
    return meshes;
}

}  // namespace jengine::renderer::gltf
