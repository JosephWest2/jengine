#include "deletion_queue.hpp"
#include <vulkan/vulkan_core.h>
#include <format>
#include <stdexcept>
#include <typeindex>

namespace jengine::renderer::vulkan {

void DeletionQueue::Flush() {
    for (auto& [object, type_index] : queue) {
        if (type_index == typeid(VkCommandBuffer)) {

        } else {
            throw std::runtime_error(std::format("type flush not implemented for: {}", type_index.name()));
        }


    }
}
}  // namespace jengine::renderer::vulkan
