/* Chart Display
 *
 * 2021 Alexander Nathan Hack <alex@anhack.xyz>
 *
 * Feb, 11th 2021
 * Version 0.0.1 (Alpha)
 * sdl_system.cpp
 */

#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "chartdisplay.hpp"
#include "sdl_system.hpp"


mpe::SDL_System::SDL_System(void)
{
  int init = SDL_Init(SDL_INIT_VIDEO);
  if (init != 0)
  {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    errors++;
    SDL_Quit();
  }

  //builds window name from macros
  char window_title[50];
  sprintf(window_title, "%s %d.%d.%d %s", APP_NAME, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_BUILD);

  window = SDL_CreateWindow
  (
    window_title,
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    WINDOW_WIDTH, WINDOW_HEIGHT,
    SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
  );
  if (window == nullptr)
  {
    std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
    errors++;
    SDL_Quit();
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == nullptr)
  {
    SDL_DestroyWindow(window);
    std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
    errors++;
    SDL_Quit();
  }

  SDL_SetWindowMinimumSize(window, WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT);

  //ttf lib
  if (!TTF_WasInit())
  {
    if (TTF_Init() == -1)
    {
      std::cerr << "SDL_TTF_Init Error: " << TTF_GetError() << std::endl;
      errors++;
      TTF_Quit();
    }
  }

  //img lib
  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags))
  {
    std::cerr << "SDL_IMG_Init Error: " << IMG_GetError() << std::endl;
    errors++;
    IMG_Quit();
  }
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
}

mpe::SDL_System::~SDL_System(void)
{
  if (renderer)
  {
    SDL_DestroyRenderer(renderer);
  }
  if (window)
  {
    SDL_DestroyWindow(window);
  }
  SDL_Quit();

  //ttf lib
  if (TTF_WasInit())
  {
    TTF_Quit();
  }
}
