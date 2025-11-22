#pragma once

#include <vulkan/vulkan_core.h>

#include <vulkan/vulkan.hpp>

namespace jengine::renderer::vulkan::descriptors {
class DescriptorLayoutBuilder {
  public:
    void AddBinding(uint32_t binding, vk::DescriptorType type);
    void Clear();
    vk::DescriptorSetLayout Build(vk::Device device,
                                  vk::ShaderStageFlags shader_stage_flags,
                                  void* pnext = nullptr,
                                  vk::DescriptorSetLayoutCreateFlags flags = vk::DescriptorSetLayoutCreateFlags());

  private:
    std::vector<vk::DescriptorSetLayoutBinding> bindings{};
};

}  // namespace jengine::renderer::vulkan::descriptors
