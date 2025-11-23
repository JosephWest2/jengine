#include "layout_builder.hpp"

namespace jengine::renderer::vulkan::descriptors {
void DescriptorLayoutBuilder::AddBinding(uint32_t binding, vk::DescriptorType type) {
    bindings.push_back(vk::DescriptorSetLayoutBinding{
        .binding = binding,
        .descriptorType = type,
        .descriptorCount = 1,
    });
}
void DescriptorLayoutBuilder::Clear() { bindings.clear(); }
vk::raii::DescriptorSetLayout DescriptorLayoutBuilder::Build(const vk::raii::Device& device,
                                                       vk::ShaderStageFlags shader_stage_flags,
                                                       void* pnext,
                                                       vk::DescriptorSetLayoutCreateFlags flags) {
    for (auto& b : bindings) {
        b.stageFlags |= shader_stage_flags;
    }

    return device.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo{
        .pNext = pnext,
        .flags = flags,
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindings = bindings.data(),
    });
}
}  // namespace jengine::renderer::vulkan::descriptors
