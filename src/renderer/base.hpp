#pragma once

namespace jengine::renderer {
class Base {
  public:
    virtual ~Base() = default;

    virtual void DrawFrame() = 0;
};
}  // namespace jengine::renderer
