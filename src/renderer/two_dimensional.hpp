#pragma once

#include "SDL3/SDL_render.h"
#include "renderer/base.hpp"

namespace jengine::renderer {

class TwoDimensional : public Base {
  public:
    TwoDimensional(SDL_Window* window);

  private:
    SDL_Renderer* renderer{nullptr};
};

}  // namespace jengine::renderer
