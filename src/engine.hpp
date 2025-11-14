#pragma once

#include <concepts>
#include <functional>
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

    template <typename EventType, typename WindowFunc>
    void CallOnWindowByWindowID(EventType& event, WindowFunc&& window_func) {

        static_assert(requires(EventType& e) {
            { e.windowID } -> std::convertible_to<SDL_WindowID>;
        });
        static_assert(std::is_invocable_v<WindowFunc, Window&, EventType&>);
        static_assert(std::is_member_function_pointer_v<WindowFunc>);

        for (auto& window : windows) {
            if (window.ID() == event.windowID) {
                std::invoke(window_func, window, event);
            }
        }
    }
};

}  // namespace jengine
