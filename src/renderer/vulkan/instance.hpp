#pragma once

#include "VkBootstrap.h"

namespace jengine::renderer::vulkan {

class Instance {
  public:
      Instance(const char* app_name, bool use_validation_layers);
      ~Instance();
      vkb::Instance& VkbInstance() { return vkb_instance; }
      VkInstance GetInstance() { return vkb_instance.instance; }

      Instance(const Instance&) = delete;
      Instance& operator=(const Instance&) = delete;
      Instance(Instance&& other);
      Instance& operator=(Instance&& other);

      void Destroy();

    private:

      vkb::Instance vkb_instance;

      inline static bool vulkan_initialized{false};
};
}  // namespace jengine::renderer
