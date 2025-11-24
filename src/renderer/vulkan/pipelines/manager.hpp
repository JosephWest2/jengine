#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "renderer/vulkan/pipelines/gradient_pipeline.hpp"
#include "renderer/vulkan/pipelines/mesh_pipeline.hpp"
#include "renderer/vulkan/pipelines/shared_compute_pipeline_layout.hpp"
#include "renderer/vulkan/pipelines/sky_pipeline.hpp"
#include "renderer/vulkan/pipelines/triangle_pipeline.hpp"
namespace jengine::renderer::vulkan::pipelines {

class Manager {
  public:
    enum class SelectedSharedComputePipeline {
        GRADIENT,
        SKY,
    };
    Manager(const vk::raii::Device& device,
            vk::Format draw_image_format,
            const vk::DescriptorSetLayout* draw_image_descriptor_layout);

    SharedComputePipelineLayout& GetSharedComputePipelineLayout() { return shared_compute_pipeline_layout; }
    const vk::Pipeline& GetGradientPipeline() const { return gradient_pipeline.GetPipeline(); }
    const vk::Pipeline& GetSkyPipeline() const { return sky_pipeline.GetPipeline(); }
    const vk::Pipeline& GetTrianglePipeline() const { return triangle_pipeline.GetPipeline(); }
    const vk::Pipeline& GetSelectedSharedComputePipeline() const;
    const vk::Pipeline& GetMeshPipeline() const { return mesh_pipeline.GetPipeline(); }
    void SetSelectedSharedComputePipeline(SelectedSharedComputePipeline pipeline) {
        selected_shared_compute_pipeline = pipeline;
    }
    SelectedSharedComputePipeline GetSelectedSharedComputePipelineEnum() const {
        return selected_shared_compute_pipeline;
    }
    const std::string_view GetSelectedSharedComputePipelineName() const;

  private:
    SelectedSharedComputePipeline selected_shared_compute_pipeline{SelectedSharedComputePipeline::GRADIENT};
    SharedComputePipelineLayout shared_compute_pipeline_layout;
    GradientPipeline gradient_pipeline;
    SkyPipeline sky_pipeline;
    TrianglePipeline triangle_pipeline;
    MeshPipeline mesh_pipeline;
};
}  // namespace jengine::renderer::vulkan::pipelines
