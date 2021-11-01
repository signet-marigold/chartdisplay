/* Chart Display
 *
 * 2021 Alexander Nathan Hack <alex@anhack.xyz>
 *
 * Feb, 11th 2021
 * Version 0.0.1 (Alpha)
 * events.hpp
 */

#ifndef EVENTS_HPP
#define EVENTS_HPP


#include <SDL2/SDL.h>


//int add_btn(ushort id, SDL_Rect *rect, void (*ptr)());
//int remove_btn(ushort id);
//void replace_btn(ushort id, SDL_Rect *rect, void (*ptr)());
//int update_btn_rect(ushort id, SDL_Rect *rect);
//void remove_all_btns(void);
//void check_btn_callback(void);

namespace mpe
{
  class Events
  {
  public:
    void check_mouse_hover(void);
    void check_mouse_click(void);
    void handle_sdl_event(SDL_Event &);
    void process_events(void);
  };
}


#endif  // EVENTS_HPP
