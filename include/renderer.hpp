/* Chart Display
 *
 * 2021 Alexander Nathan Hack <alex@anhack.xyz>
 *
 * Feb, 11th 2021
 * Version 0.0.1 (Alpha)
 * renderer.hpp
 */

#ifndef RENDERER_HPP
#define RENDERER_HPP


#include <SDL2/SDL.h>

#include "chartdisplay.hpp"
#include "sdl_system.hpp"


namespace mpe
{
  class Renderer
  {
  private:
    SDL_System sdl;

    bool repaint = true;
    int paint_count = 0;
    Box render_text
    (
    	const int,
    	const int,
    	const char *,
    	const uint,
    	const SDL_Color *,
    	const std::string,
    	const std::string,
    	Justify justify
    );
    Box get_text_size
    (
    	const char *,
    	const uint,
    	const std::string,
    	const std::string
    );
    void find_bar_size(ushort, ushort &, ushort &);
    void draw_log(void);
    void draw_chart(void);
    void draw_mouse_hover(void);
    void draw_pitchfork(std::vector<std::vector<uint>> &);
    void draw_tracked_symbols(void);
  public:
    void paint(void);
  };
}


#endif  // RENDERER_HPP
