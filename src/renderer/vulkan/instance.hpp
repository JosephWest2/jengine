#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace jengine::renderer::vulkan {

vk::raii::Instance CreateInstance(vk::raii::Context& context, const char* app_name, bool use_validation_layers = true);

class Instance {
  public:
    Instance(const char* app_name, bool use_validation_layers = true);

    vk::raii::Instance& GetInstance() { return instance; }
    vk::raii::Context& GetContext() { return context; }
    const vk::Instance& GetInstanceHandle() const { return *instance; }

    Instance(const Instance&) = delete;
    Instance& operator=(const Instance&) = delete;
    Instance(Instance&&) = delete;
    Instance& operator=(Instance&&) = delete;

  private:
    vk::raii::Context context;
    vk::raii::Instance instance;

};
}  // namespace jengine::renderer::vulkan
