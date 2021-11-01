/* Chart Display
 *
 * 2021 Alexander Nathan Hack <alex@anhack.xyz>
 *
 * Feb, 11th 2021
 * Version 0.0.1 (Alpha)
 * controller.cpp
 */

#include <SDL2/SDL.h>

#include "chartdisplay.hpp"
#include "utils.hpp"
#include "controller.hpp"
#include "logger.hpp"

#include "chart_downloader.hpp"


void mpe::Controller::init(void)
{
  ::reload_config();
  mp_log.push("<Controller> init");
  mp_log.push("<Controller> starting event loop");
  event_loop();
}

void mpe::Controller::event_loop(void)
{
  renderer.paint();
  SDL_Event event;
  while (running)
  {
    uint tries = 500;
    while (tries-- && SDL_PollEvent(&event))
    {
      events.handle_sdl_event(event);
    }
    events.process_events();
    events.check_mouse_hover();
    events.check_mouse_click();
    renderer.paint();
  }
}
