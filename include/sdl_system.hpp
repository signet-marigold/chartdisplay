/* Chart Display
 *
 * 2021 Alexander Nathan Hack <alex@anhack.xyz>
 *
 * Feb, 11th 2021
 * Version 0.0.1 (Alpha)
 * sdl_system.hpp
 */

#ifndef SDL_SYSTEM_HPP
#define SDL_SYSTEM_HPP


#include <SDL2/SDL.h>

#include "chartdisplay.hpp"


namespace mpe
{
  class SDL_System
  {
  public:
    uint errors = 0;
    SDL_Window* window;
    SDL_Renderer* renderer;

    SDL_System();
    ~SDL_System();
  };
}


#endif  // SDL_SYSTEM_HPP
