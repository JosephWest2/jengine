#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>
namespace jengine::renderer::vulkan::descriptors {
class DescriptorLayoutBuilder {
  public:
    void AddBinding(uint32_t binding, VkDescriptorType type);
    void Clear();
    VkDescriptorSetLayout Build(VkDevice device,
                                VkShaderStageFlags shader_stage_flags,
                                void* pnext = nullptr,
                                VkDescriptorSetLayoutCreateFlags flags = 0);

  private:
    std::vector<VkDescriptorSetLayoutBinding> bindings{};
};

}
