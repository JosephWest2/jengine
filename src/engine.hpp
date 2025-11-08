#pragma once

#include <vector>

#include "SDL3/SDL_events.h"
#include "window.hpp"

namespace jengine {

class Engine {
  public:

    Engine(bool headless = false);

    ~Engine();
    void Run();

    // takes ownership of the window
    void AddWindow(Window&& window);

  private:
    bool quit{false};
    std::vector<Window> windows;

    // ensure sdl is initialized and cleaned up once
    inline static bool sdl_initialized{false};

    void HandleEvent(SDL_Event& event);
};


}  // namespace jengine
