#pragma once

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_raii.hpp>
namespace jengine::renderer::vulkan::pipelines {

    vk::raii::ShaderModule LoadShaderModule(const char* file_path, const vk::raii::Device& device);

}
