/* Chart Display
 *
 * 2021 Alexander Nathan Hack <alex@anhack.xyz>
 *
 * Feb, 11th 2021
 * Version 0.0.1 (Alpha)
 * chartdisplay.hpp
 */

#ifndef CHARTDISPLAY_HPP
#define CHARTDISPLAY_HPP


#include <vector>
#include <string>

#include <SDL2/SDL.h>

#ifdef _WIN32
	const char PATH_SEP = '\\';
#else
	const char PATH_SEP = '/';
#endif


#define PROJ_DIR "chartdisplay"
#define APP_NAME "Chart Display"
#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 1
#define VERSION_BUILD "Alpha"

#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 480

#define WINDOW_MIN_WIDTH 320
#define WINDOW_MIN_HEIGHT 240


//shortcodes for c types (cuz i hate how the full names look)
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef long long int llint;
typedef unsigned long long int ullint;
typedef long double ld;
typedef std::vector<ld>::iterator vec_iter_ld;

//color codes, hopefully so that the gui looks consistent
//right now, just used for text
const SDL_Color black   = {0, 0, 0, 255};
const SDL_Color gray    = {127, 127, 127, 255};
const SDL_Color white   = {255, 255, 255, 255};
const SDL_Color red     = {255, 71, 38, 255};
const SDL_Color orange  = {255, 136, 38, 255};
const SDL_Color yellow  = {255, 255, 38, 255};
const SDL_Color green   = {38, 255, 49, 255};
const SDL_Color cyan    = {38, 244, 255, 255};
const SDL_Color blue    = {38, 139, 255, 255};
const SDL_Color purple  = {150, 38, 255, 255};
const SDL_Color magenta = {215, 38, 255, 255};
const SDL_Color pink    = {255, 38, 161, 255};

enum Justify : uchar
{
  TOP_RIGHT,
	TOP_LEFT,
	BOTTOM_RIGHT,
	BOTTOM_LEFT,
	CENTER,
	CENTER_TOP,
	CENTER_BOTTOM,
	CENTER_LEFT,
	CENTER_RIGHT
};

enum Screen : uchar
{
	LOG,
  GRAPH,
	NORMALIZED_GRAPH,
	SYMBOLS,
	TRADES,
	LOGIC,
	SETTINGS
};
inline Screen Active_Screen = GRAPH;

struct Chart_Data
{
	std::vector<std::string> dv;
	std::vector<double> ov, hv, lv, cv;
	std::vector<uint> vv;
	uint rows = 0;
	double top_price;
	uint top_volume;
  std::vector<std::vector<double>> ma;
};

struct Point
{
	int x, y;
};

struct Box
{
	int w, h;
};

//struct Rect : Point, Box {}; //need to change the name. also, this is really just a redef of SDL_Rect

inline int  v_pan = 0;//chart vertical pan
inline uint h_pan = 0;//chart horizontal pan
inline int  s_pan = 20;//chart scale pan
inline ushort total_bar_width = 8;//chart bar total width

inline bool running = true;//can stop event loop and exit program
inline bool repaint = true;//set to true every frame you want to draw to the screen, auto flips false

inline int hovering_mouse_x;
inline int hovering_mouse_y;
inline bool mouse_hovering = false;

inline bool repaint_hovering = false;

inline bool draw_moving_avg = true;

inline int window_w = 0;
inline int window_h = 0;


inline Chart_Data Active_Chart;

struct Btn_Callback
{
  uint id;
  SDL_Rect rect;
  void (*ptr)();
};

inline std::vector<Btn_Callback> btn_callback_list;


#endif  // CHARTDISPLAY_HPP
