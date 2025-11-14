#pragma once

#include <vulkan/vulkan_core.h>
namespace jengine::renderer::vulkan::pipelines {

VkShaderModule LoadShaderModule(const char* file_path, VkDevice device);

}
