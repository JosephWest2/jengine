#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace jengine::renderer::vulkan::pipelines {
class SharedComputePipelineLayout {
  public:
    struct ComputePushConstants {
        glm::vec4 data1{};
        glm::vec4 data2{};
        glm::vec4 data3{};
        glm::vec4 data4{};
    };
    SharedComputePipelineLayout(const vk::raii::Device& device,
                                const vk::DescriptorSetLayout* const descriptor_set_layout);

    const vk::PipelineLayout& GetPipelineLayout() const { return *layout; }
    ComputePushConstants& GetPushConstants() { return push_constants; }
    std::vector<glm::vec4> GetPushConstantsVec() {
        return {push_constants.data1, push_constants.data2, push_constants.data3, push_constants.data4};
    }

  private:
    vk::raii::PipelineLayout layout;
    ComputePushConstants push_constants;
};

}  // namespace jengine::renderer::vulkan::pipelines
