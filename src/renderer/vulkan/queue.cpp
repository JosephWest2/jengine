#include "queue.hpp"

namespace jengine::renderer::vulkan {
Queue::Queue(uint32_t queue_family_index, const vk::raii::Device& device) : queue_family_index(queue_family_index) {
    queue = device.getQueue(queue_family_index, 0);
};
}  // namespace jengine::renderer::vulkan
