#include "load_shader_module.hpp"

#include <vulkan/vulkan_core.h>

#include <format>
#include <fstream>
#include <vector>

namespace jengine::renderer::vulkan::pipelines {

vk::raii::ShaderModule LoadShaderModule(const char* file_path, const vk::raii::Device& device) {
    std::ifstream file(file_path, std::ios::ate | std::ios::binary);

    if (!file) {
        throw std::runtime_error(std::format("Failed to open file: {}", file_path));
    }

    size_t file_size = static_cast<size_t>(file.tellg());
    std::vector<uint32_t> buffer(file_size / sizeof(uint32_t));

    file.seekg(0);

    file.read(reinterpret_cast<char*>(buffer.data()), file_size);

    file.close();

    return device.createShaderModule({.codeSize = file_size, .pCode = buffer.data()});
}

}  // namespace jengine::renderer::vulkan::pipelines
