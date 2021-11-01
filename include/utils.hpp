/* Chart Display
 *
 * 2021 Alexander Nathan Hack <alex@anhack.xyz>
 *
 * Feb, 11th 2021
 * Version 0.0.1 (Alpha)
 * utils.hpp
 */

#ifndef UTILS_HPP
#define UTILS_HPP


#include <string>
#include <utility>//std::pair
#include <vector>

#include <SDL2/SDL.h>

#include "chartdisplay.hpp"

std::string get_resource_path(const std::string &);
std::string get_font_path(const std::string &, const std::string &);
std::vector<std::pair<std::string, std::string>> load_config(std::string);
int reload_config(void);


#endif  // UTILS_HPP
