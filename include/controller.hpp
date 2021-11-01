/* Chart Display
 *
 * 2021 Alexander Nathan Hack <alex@anhack.xyz>
 *
 * Feb, 11th 2021
 * Version 0.0.1 (Alpha)
 * controller.hpp
 */

#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP


#include "chartdisplay.hpp"
#include "events.hpp"
#include "renderer.hpp"
#include "controller.hpp"


namespace mpe
{
  class Controller
  {
  private:
    Renderer renderer;
    Events events;
    void event_loop(void);
  public:
    void init(void);
  };
}


#endif  // CONTROLLER_HPP
