#pragma once

#include <cstdint>
namespace jengine::renderer {
class Base {
  public:
    virtual ~Base() = default;

    virtual void DrawFrame() = 0;
    virtual void HandleResize(uint32_t new_width __attribute__((unused)), uint32_t new_height __attribute__((unused))) {
    }
};
}  // namespace jengine::renderer
