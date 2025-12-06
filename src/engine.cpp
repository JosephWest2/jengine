#include "engine.hpp"

#include <algorithm>
#include <cassert>
#include <format>
#include <iostream>
#include <print>
#include <stdexcept>

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"

namespace jengine {

void Engine::Run() {
    assert(sdl_initialized);
    while (!quit) {
        SDL_Event event;
        bool should_poll_events = std::ranges::any_of(windows, &Window::ConstantRedraw);
        if (!should_poll_events) {
            SDL_WaitEvent(&event);
            HandleEvent(event);
        }
        while (SDL_PollEvent(&event)) {
            HandleEvent(event);
        }
        for (auto& window : windows) {
            window.DrawFrame();
        }
    }
}

void Engine::HandleEvent(SDL_Event& event) {
    switch (event.type) {
        case SDL_EVENT_QUIT:
            quit = true;
            break;
        case SDL_EVENT_WINDOW_MINIMIZED:
            CallOnWindowByWindowID(event.window, &Window::HandleWindowMinimized);
            break;
        case SDL_EVENT_WINDOW_RESTORED:
            CallOnWindowByWindowID(event.window, &Window::HandleWindowRestored);
            break;
        case SDL_EVENT_WINDOW_RESIZED:
            CallOnWindowByWindowID(event.window, &Window::HandleWindowResized);
            break;

        default:
            break;
    }

}

Engine::Engine(bool headless) {
    if (sdl_initialized) {
        std::println("Warning: SDL already initialized, multiple instances of engine created");
    }
    auto init_flags = headless ? SDL_INIT_EVENTS : SDL_INIT_VIDEO | SDL_INIT_AUDIO;
    if (!SDL_Init(init_flags)) {
        throw std::runtime_error(std::format("SDL init failed: {}", SDL_GetError()));
    }
    sdl_initialized = true;
}

Engine::~Engine() {
    if (sdl_initialized) {
        SDL_Quit();
        sdl_initialized = false;
    }
}

void Engine::AddWindow(Window&& window) {
    windows.push_back(std::move(window));
}
}  // namespace jengine
