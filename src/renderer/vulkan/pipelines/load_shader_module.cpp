#include "load_shader_module.hpp"

#include <vulkan/vulkan_core.h>

#include <fstream>
#include <vector>

namespace jengine::renderer::vulkan::pipelines {

VkShaderModule LoadShaderModule(const char* file_path, VkDevice device) {
    std::ifstream file(file_path, std::ios::ate | std::ios::binary);

    if (!file) {
        return nullptr;
    }

    size_t file_size = static_cast<size_t>(file.tellg());
    std::vector<uint32_t> buffer(file_size / sizeof(uint32_t));

    file.seekg(0);

    file.read(reinterpret_cast<char*>(buffer.data()), file_size);

    file.close();

    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.codeSize = file_size;
    create_info.pCode = buffer.data();

    VkShaderModule result{};
    if (vkCreateShaderModule(device, &create_info, nullptr, &result) != VK_SUCCESS) {
        return nullptr;
    }
    return result;
}

}  // namespace jengine::renderer::vulkan
