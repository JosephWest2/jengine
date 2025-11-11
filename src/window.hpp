#pragma once

#include <utility>
#include "SDL3/SDL_video.h"
#include "renderer/base.hpp"

namespace jengine {

// returns {width, height}
std::pair<int, int> GetWindowSize(SDL_Window* window);
int GetWindowWidth(SDL_Window* window);
int GetWindowHeight(SDL_Window* window);

enum class WindowType {
    VULKAN,
};

class Window {
  public:
    Window(const char* title, int width, int height, WindowType window_type, bool constant_redraw);
    ~Window();

    bool ConstantRedraw() const;

    SDL_Window* SDL() const;

    Window(const Window&) = delete;
    Window(Window&&);
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&);

    void DrawFrame();

    void SetRenderer(renderer::Base* renderer);

  private:

    renderer::Base* renderer{nullptr};
    SDL_Window* window{nullptr};
    bool constant_redraw{true};
};

}  // namespace jengine
