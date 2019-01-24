#include <string>
#include <thread>
#include <iostream>
#include "sdlxx/sdlxx.h"

using namespace std::chrono_literals;
using namespace std::string_literals;

class Vec2 {
 public:
  double x;
  double y;
};

int main()
{
  using namespace sdlxx;
  const auto sdl_sys = Sdl_system(SDL_INIT_VIDEO);

  constexpr auto windowHeight = 640;
  constexpr auto windowWidth = 480;

  // The Window is the operating system thing that contains our drawing space.
  // It's the thing with the X in the corner and the frame thing around it. You
  // may know it as "chrome".
  auto window =
      Sdl_window("hello, world", 100, 100, windowHeight, windowWidth, SDL_WINDOW_SHOWN);

  // The renderer lets us create the pixels in memory
  auto ren = Sdl_renderer(window, -1,
                          SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  auto image_path = "resources/"s + "ship.bmp"s;
  auto tex = Sdl_texture{image_path, ren};

  auto x = 0.0;
  auto y = 0.0;

  auto heading_deg = 0.0;

  Vec2 velocity{};

  auto rotate_left = false;
  auto rotate_right = false;
  auto accelerate = false;

  // Main Loop
  constexpr auto millis_per_update = 16ms;  // about 60FPS
  auto previous = std::chrono::high_resolution_clock::now();
  auto lag = previous - previous;

  bool quit{false};
  while (!quit) {
    auto current = std::chrono::high_resolution_clock::now();
    auto elapsed = current - previous;
    previous = current;
    lag += elapsed;

    // Process input
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      // If the window gets closed
      if (e.type == SDL_QUIT) { quit = true; }

      if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
          default:
            break;
          case SDLK_q:
            quit = true;
            break;

          case SDLK_w:
          case SDLK_UP:
            accelerate = true;
            break;
          case SDLK_a:
          case SDLK_LEFT:
            rotate_left = true;
            break;
          case SDLK_s:
          case SDLK_DOWN:
            // nothing
            break;
          case SDLK_d:
          case SDLK_RIGHT:
            rotate_right = true;
            break;
        }
      }
      if (e.type == SDL_KEYUP) {
        switch (e.key.keysym.sym) {
          case SDLK_w:
          case SDLK_UP:
            accelerate = false;
            break;
          case SDLK_a:
          case SDLK_LEFT:
            rotate_left = false;
            break;
          case SDLK_s:
          case SDLK_DOWN:
            break;
          case SDLK_d:
          case SDLK_RIGHT:
            rotate_right = false;
            break;
        }
      }

      // If the mouse is clicked
      if (e.type == SDL_MOUSEBUTTONDOWN) { quit = true; }
    }

    while (lag >= millis_per_update) {
      // Update

      if (rotate_left) { heading_deg -= 1.5; }
      if (rotate_right) { heading_deg += 1.5; }
      if (accelerate) {
        // We accelerate in the direction of the heading
        auto del_v = Vec2{};
        del_v.x = std::sin(heading_deg * 0.0174533) * .1;
        del_v.y = -std::cos(heading_deg * 0.0174533) * .1;

        velocity.x += del_v.x;
        velocity.y += del_v.y;
      }

      x += velocity.x;

      // Wrap
      if (x > (windowWidth + 62)) { x = -62; }
      if (x < -62) { x = windowWidth + 62; }

      y += velocity.y;
      // Wrap
      if (y > windowHeight) { y = -62; }
      if (y < -62) { y = windowHeight - 62; }

      lag -= millis_per_update;
    }

    std::printf("(% 3d, % 3d) [%f %f]\r", int(x), int(y), velocity.x, velocity.y);

    // Render
    ren.clear();
    ren.copy(tex, int(x), int(y), heading_deg);
    ren.present();
    std::fflush(stdout);
  }

  return 0;
}
