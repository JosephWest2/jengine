#include "layout_builder.hpp"
#include <stdexcept>

namespace jengine::renderer::vulkan::descriptors {
void DescriptorLayoutBuilder::AddBinding(uint32_t binding, VkDescriptorType type) {
    VkDescriptorSetLayoutBinding layout_binding{};
    layout_binding.binding = binding;
    layout_binding.descriptorCount = 1;
    layout_binding.descriptorType = type;
    bindings.push_back(layout_binding);
}
void DescriptorLayoutBuilder::Clear() { bindings.clear(); }
VkDescriptorSetLayout DescriptorLayoutBuilder::Build(VkDevice device,
                                                     VkShaderStageFlags shader_stage_flags,
                                                     void* pnext,
                                                     VkDescriptorSetLayoutCreateFlags flags) {
    for (auto& b : bindings) {
        b.stageFlags |= shader_stage_flags;
    }

    VkDescriptorSetLayoutCreateInfo layout_create_info{};
    layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_create_info.bindingCount = static_cast<uint32_t>(bindings.size());
    layout_create_info.pBindings = bindings.data();
    layout_create_info.pNext = pnext;
    layout_create_info.flags = flags;

    VkDescriptorSetLayout layout;
    if (vkCreateDescriptorSetLayout(device, &layout_create_info, nullptr, &layout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set layout");
    }
    return layout;
}
}
