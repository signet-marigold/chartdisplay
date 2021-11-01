/* Chart Display
 *
 * 2021 Alexander Nathan Hack <alex@anhack.xyz>
 *
 * Feb, 11th 2021
 * Version 0.0.1 (Alpha)
 * renderer.cpp
 */

#include <iostream>
#include <math.h>
#include <iomanip>
#include <sstream>
#include <limits>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "chartdisplay.hpp"
#include "utils.hpp"
#include "z_score.hpp"
#include "renderer.hpp"
#include "logger.hpp"


int update_btn_rect(uint id, SDL_Rect rect)
{
  int index = -1;
  for (uint i = 0; i < btn_callback_list.size(); i++)
  {
    if (btn_callback_list.at(i).id == id)
    {
      index = i;
      break;
    }
  }
  if (index == -1)
  {
    std::cerr << "Button callback rect update failed. ID " << id << " does not exist." << std::endl;
    return -1;
  }

  btn_callback_list.at(index).rect = rect;
  return 0;
}

Box mpe::Renderer::render_text
(
	const int x,
	const int y,
	const char *text,
	const uint font_size,
	const SDL_Color *color,
	const std::string font_family,
	const std::string font_name,
	Justify justify
){
	TTF_Font *font;
	font = TTF_OpenFont(get_font_path(font_family, font_name).c_str(), font_size);
	if (!font)
	{
		std::cerr << "TTF_OpenFont: " << TTF_GetError() << std::endl;
		return {-1, -1};
	}

	SDL_Surface *surface_message = TTF_RenderText_Blended(font, text, *color);
	SDL_Texture *message = SDL_CreateTextureFromSurface(sdl.renderer, surface_message);
	SDL_FreeSurface(surface_message);
	int text_w, text_h;
	TTF_SizeText(font, text, &text_w, &text_h);

	int draw_x;
	int draw_y;

	switch (justify)
	{
		case TOP_RIGHT:
			draw_x = x;
			draw_y = y-text_h;
			break;
		case TOP_LEFT:
			draw_x = x-text_w;
			draw_y = y-text_h;
			break;
		case BOTTOM_RIGHT:
			draw_x = x;
			draw_y = y;
			break;
		case BOTTOM_LEFT:
			draw_x = x-text_w;
			draw_y = y;
			break;
		case CENTER:
			draw_x = x-(text_w/2);
			draw_y = y-(text_h/2);
			break;
    case CENTER_TOP:
      draw_x = x-(text_w/2);
      draw_y = y-text_h;
      break;
    case CENTER_BOTTOM:
      draw_x = x-(text_w/2);
      draw_y = y;
      break;
    case CENTER_LEFT:
      draw_x = x-text_w;
      draw_y = y-(text_h/2);
      break;
    case CENTER_RIGHT:
      draw_x = x;
      draw_y = y-(text_h/2);
      break;
	}

	SDL_Rect message_rect = {draw_x, draw_y, text_w, text_h};

	SDL_RenderCopy(sdl.renderer, message, NULL, &message_rect);

	SDL_DestroyTexture(message);
	TTF_CloseFont(font);
	font = NULL;

	return {text_w, text_h};
}

Box mpe::Renderer::get_text_size
(
	const char *text,
	const uint font_size,
	const std::string font_family,
	const std::string font_name
){
	TTF_Font *font;
	font = TTF_OpenFont(get_font_path(font_family, font_name).c_str(), font_size);
	if (!font)
	{
		std::cerr << "TTF_OpenFont: " << TTF_GetError() << std::endl;
		return {-1, -1};
	}

	int text_w, text_h;
	TTF_SizeText(font, text, &text_w, &text_h);

	TTF_CloseFont(font);
	font = NULL;

	return {text_w, text_h};
}

void mpe::Renderer::draw_log(void)
{
  const uint log_size = mp_log.get_size();
  const int font_size = 14;
  const int line_padding = 4;

  const int line_height = font_size + line_padding;

  uint extra_lines = 0;

  //std::string font_family = "Source_Code_Pro";
  //std::string font_name = "SourceCodePro-Regular.ttf";

  std::string font_family = "NotoMono-hinted";//sooooo much better
  std::string font_name = "NotoMono-Regular.ttf";

  for (uint i = 1; i <= log_size; i++)
  {
    std::string text_str = std::to_string(log_size - i + 1) + ": " + mp_log.pull(log_size - i);
		const char *text = text_str.c_str();

    Box full_text_size = get_text_size(text, font_size, font_family, font_name);

    if (full_text_size.w > window_w)
    {
      std::vector<std::string> broken_lines;
      std::string remaining_text_str = text_str;

      bool process_string = true;
      while (process_string)
      {
        std::string trunc_text_str = remaining_text_str;
        const char *trunc_text = trunc_text_str.c_str();

        Box trunc_text_size = get_text_size(trunc_text, font_size, font_family, font_name);
        float divisor = (float)trunc_text_size.w / window_w;

        trunc_text_str = trunc_text_str.substr(0, (trunc_text_str.size() / divisor));

        broken_lines.push_back(trunc_text_str);

        remaining_text_str = remaining_text_str.substr(trunc_text_str.size(), remaining_text_str.size() - trunc_text_str.size());

        const char *remaining_text = remaining_text_str.c_str();
        Box remaining_text_size = get_text_size(remaining_text, font_size, font_family, font_name);

        if (remaining_text_size.w < window_w)
        {
          process_string = false;

          broken_lines.push_back(remaining_text_str);
        }
      }

      for (uint j = broken_lines.size(); j > 0; j--)
      {
        std::string broken_text_str = broken_lines.at(j - 1);
    		const char *broken_text = broken_text_str.c_str();
        int text_y = window_h - (line_height * (i - 1 + extra_lines));
        if (text_y <= 0)
        {
          return;
        }
        else
        {
          render_text(0, text_y, broken_text, font_size, &white, font_family, font_name, TOP_RIGHT);
          extra_lines++;
        }
      }
      extra_lines--;
    }
    else
    {
      int text_y = window_h - (line_height * (i - 1 + extra_lines));
      if (text_y <= 0)
      {
        return;
      }
      else
      {
        render_text(0, text_y, text, font_size, &white, font_family, font_name, TOP_RIGHT);
      }
    }
  }
}

void mpe::Renderer::find_bar_size(ushort bar_width, ushort &space, ushort &width)
{
	ushort padding = static_cast<ushort>((bar_width - 4) / 4);

	space = padding;
	width = bar_width - padding;
}

void mpe::Renderer::draw_chart(void)
{
	const ushort bar_width = total_bar_width;
	ushort space, width;
	find_bar_size(bar_width, space, width);
	//settings
	const int scale = s_pan;//multiplier
	const int offset = window_h + v_pan;
	const int grid_width = 15 * (width + space);
	const int grid_height = 5 * scale;

	const int volume_max_height = 270;//in pixels


	double price_spacer = grid_height;

	//update price spacing based off of vertical scale
	if (scale == 1) price_spacer *= 2;
	if (scale < 4) price_spacer *= 2;
	if (scale < 8) price_spacer *= 2;
	if (scale < 13) price_spacer *= 2;
	if (scale > 22) price_spacer /= 2;
	if (scale > 53) price_spacer /= 2;
	if (scale > 91) price_spacer /= 2;

	int date_spacer = 15;

	//update date spacing based off of horizontal bar size
	if (bar_width < 6) date_spacer *= 2;
	if (bar_width < 3) date_spacer *= 2;
	if (bar_width == 1) date_spacer *= 2;

	const Chart_Data chart = Active_Chart;//protect original chart data from edits

	//for main drawing loop; draws just inside the screen bounds
	uint loops = window_w / bar_width;
	if (loops > chart.rows) loops = chart.rows;

	std::vector<ld> input(chart.vv.begin(), chart.vv.end());
	int lag = 20;
	ld threshold = 5.0;
	ld influence = 0.0;
	std::unordered_map<std::string, std::vector<ld>> output = z_score_thresholding(input, lag, threshold, influence);

	int volume_block = 0;
	ullint last_volume = 0;

	//cap upper horizontal scroll bounds
	if (h_pan > chart.rows - loops) h_pan = chart.rows - loops;

	//draw grid lines
	//Draws horizontal lines
	for (int i = 0; i * grid_height < offset; i++)
	{
		int pos = offset - i * grid_height;
		const int extra_lines = 2;

		if (i % 3)
		{
			SDL_SetRenderDrawColor(sdl.renderer, 50, 50, 0, 0);
		}
		else
		{
			SDL_SetRenderDrawColor(sdl.renderer, 100, 100, 0, 0);
		}

		SDL_RenderDrawLine(sdl.renderer,
			0,
			pos,
			window_w,
			pos);

		SDL_SetRenderDrawColor(sdl.renderer, 20, 20, 0, 0);
		for (int j = 0; j < extra_lines-1; j++)
		{
			int spacer = (grid_height / (extra_lines)) * (j+1);
			SDL_RenderDrawLine(sdl.renderer,
				0,
				pos - spacer,
				window_w,
				pos - spacer);
		}
	}
	//Draws vertical lines
	int vert_lines = window_w / grid_width + 26;
	for (int i = 0; i < vert_lines; i++)
	{
		int adj = static_cast<int>(h_pan) % (grid_width * 3);
		int pos = i * grid_width + -adj * bar_width;
		const int extra_lines = 3;

		if (i % 3)
		{
			SDL_SetRenderDrawColor(sdl.renderer, 50, 50, 0, 0);
		}
		else
		{
			SDL_SetRenderDrawColor(sdl.renderer, 100, 100, 0, 0);
		}

		SDL_RenderDrawLine(sdl.renderer,
			pos,
			0,
			pos,
			offset);

		SDL_SetRenderDrawColor(sdl.renderer, 20, 20, 0, 0);
		for (int j = 0; j < extra_lines-1; j++)
		{
			int spacer = (grid_width / (extra_lines)) * (j+1);
			SDL_RenderDrawLine(sdl.renderer,
				pos + spacer,
				0,
				pos + spacer,
				offset);
		}
	}



  double highest_visable_price = 0.0;
  double lowest_visable_price = std::numeric_limits<double>::max();

	//Line loop
	for (uint i = 0; i < loops; i++)
	{
		uint adj_i = i + h_pan;

		//for readability, don't know how it affects performance
		double open  = chart.ov.at(adj_i);
		double high  = chart.hv.at(adj_i);
		double low   = chart.lv.at(adj_i);
		double close = chart.cv.at(adj_i);
		uint volume  = chart.vv.at(adj_i);

    //record lowest and highest bars visable
    if (highest_visable_price < high) highest_visable_price = high;
    if (lowest_visable_price > low) lowest_visable_price = low;

		//get volume cords, drawn on the bottom
		const SDL_Rect vol = {
			static_cast<int>(i * bar_width),
			window_h - static_cast<int>(volume / (chart.top_volume / volume_max_height)),
			width,
			window_h
		};

		//cords for high and low, represented by a line
		const SDL_Rect line = {
			static_cast<int>(i * bar_width + width / 2),
			static_cast<int>(-(high * scale) + offset),
			1,
			static_cast<int>((high - low) * scale)
		};

		//cords for open and close, represented by a bar
		float top = (open > close) ? open : close;
		const SDL_Rect bar = {
			static_cast<int>(i * bar_width),
			static_cast<int>(-(top * scale)) + offset,
			width,
			static_cast<int>(std::abs(open - close) * scale)
		};

		//pick color; if this point's open is higher than
		//last point's open, set to green. otherwise, set to red
		int color = 0;
		if (i == 0) color = 1;
		else if (chart.ov.at(adj_i) > chart.ov.at(adj_i - 1)) color = 2;
		else color = 1;

		//set color
		if (color == 1) SDL_SetRenderDrawColor(sdl.renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
		if (color == 2) SDL_SetRenderDrawColor(sdl.renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);

		//draw price line
		SDL_RenderDrawRect(sdl.renderer, &line);

		//mask out price bar
		SDL_SetRenderDrawColor(sdl.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderFillRect(sdl.renderer, &bar);

		//set color again
		if (color == 1) SDL_SetRenderDrawColor(sdl.renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
		if (color == 2) SDL_SetRenderDrawColor(sdl.renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);

		//draw price bar outline
		SDL_RenderDrawRect(sdl.renderer, &bar);

		//draw volume bars
		SDL_SetRenderDrawColor(sdl.renderer, 0, 70, 70, 0);
		SDL_RenderFillRect(sdl.renderer, &vol);
		SDL_SetRenderDrawColor(sdl.renderer, 0, 255, 255, 0);
		SDL_RenderDrawRect(sdl.renderer, &vol);
	}


  //draw arrow if chart is off screen
  //highest point lower than screen
  int win_dif = std::abs(window_w - window_h);
  int win_avg;
  if (window_w > window_h) win_avg = window_h + win_dif;
  if (window_w < window_h) win_avg = window_w + win_dif;
  else win_avg = window_h;

  int arrow_size = win_avg / 24;
  if (arrow_size < 40) arrow_size = 40;
  float arrow_pos_scaling = float(win_avg) / 2000;//last number, max screen avg
  if (arrow_pos_scaling > 1.0f) arrow_pos_scaling = 1.0;
  int top_arrow_pos    = window_h/2 - arrow_pos_scaling * (window_h/3);
  int bottom_arrow_pos = window_h/2 + arrow_pos_scaling * (window_h/3);


  if (static_cast<int>(-(highest_visable_price * scale) + offset) > window_h)
  {
    std::string img_path = get_resource_path(PROJ_DIR) + "images" + PATH_SEP + "down_arrow_trans_full.png";
    SDL_Texture *img = IMG_LoadTexture(sdl.renderer, img_path.c_str());
  	SDL_Rect texr = {
      window_w/2 - arrow_size/2,
      bottom_arrow_pos - arrow_size/2,
      arrow_size, arrow_size
    };
    SDL_RenderCopy(sdl.renderer, img, NULL, &texr);
    SDL_DestroyTexture(img);
  }
  //lowest point higher than screen
  if (static_cast<int>(-(lowest_visable_price * scale) + offset) < 0)
  {
    std::string img_path = get_resource_path(PROJ_DIR) + "images" + PATH_SEP + "up_arrow_trans_full.png";
    SDL_Texture *img = IMG_LoadTexture(sdl.renderer, img_path.c_str());
  	SDL_Rect texr = {
      window_w/2 - arrow_size/2,
      top_arrow_pos - arrow_size/2,
      arrow_size, arrow_size
    };
    SDL_RenderCopy(sdl.renderer, img, NULL, &texr);
    SDL_DestroyTexture(img);
  }




	//Text loop
	for (uint i = 0; i < loops; i++)
	{
		uint adj_i = i + h_pan;

		//draw volume values
		if (volume_block > 0) volume_block--;
		if (output["signals"].at(adj_i))
		{
			//check a band of positive values for most significant
			ushort band_off = 0;
			ushort top_off = 0;
			ullint top_val = 0;
			while (output["signals"].at(adj_i+band_off))
			{
				if (chart.vv.at(adj_i+band_off) > top_val)
				{
					top_val = chart.vv.at(adj_i+band_off);
					top_off = band_off;
				}
				band_off++;
			}
			//check next few for most significant
			int new_off = 0;
			for (int i = 0; i < 20; i++)
			{
				if (chart.vv.at(adj_i+top_off+i) > top_val)
				{
					top_val = chart.vv.at(adj_i+top_off+i);
					new_off = i;
				}
			}
			top_off += new_off;
			if (volume_block == 0 || std::labs(last_volume - top_val) > (chart.top_volume / 4))
			{
				last_volume = top_val;
				//volume_block adds a little bit of padding after
				//it draws, so the volume text doesn't overlap each other
				volume_block = 10;
				uint font_size = 16;
				const int bottom_padding = -4;
				std::string text_str = std::to_string(top_val);
				//quick and dirty way of adding commas
				int n = text_str.length() - 3;
				while (n > 0)
				{
					text_str.insert(n, ",");
					n -= 3;
				}
				const char *text = text_str.c_str();
				render_text((i + top_off) * bar_width, window_h - (top_val / (chart.top_volume / volume_max_height)) - bottom_padding, text, font_size, &white, "open_sans", "OpenSans-Regular.ttf", TOP_RIGHT);
			}
		}

		//draw dates
		if (adj_i % date_spacer == 0)
		{
			uint font_size = 16;
			const int bottom_padding = -4;
			std::string text_str = chart.dv.at(adj_i);
			const char *text = text_str.c_str();
			render_text(i * bar_width, 40 - bottom_padding, text, font_size, &white, "open_sans", "OpenSans-Regular.ttf", TOP_RIGHT);
			SDL_SetRenderDrawColor(sdl.renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
			SDL_RenderDrawLine(sdl.renderer, i * bar_width, 100 - bottom_padding, i * bar_width, 40 - bottom_padding);
		}
	}



	//Moving average loop
	if (draw_moving_avg)
	{
		for (uint i = chart.ma.size(); i-- > 0; )//reverse for loop fixes colors on first part of moving avg
		{
			SDL_Point ma_points[loops];
			for (uint j = 0; j < loops; j++)
			{
				uint adj_j = j + h_pan;

				ma_points[j].x = j * bar_width + static_cast<int>(width / 2);
				ma_points[j].y = static_cast<int>(-(chart.ma[i].at(adj_j) * scale - offset));
			}
			//pick drawing color
			switch (i)
			{
				case 0:  SDL_SetRenderDrawColor(sdl.renderer, 255, 0,   246, SDL_ALPHA_OPAQUE); break;
				case 1:  SDL_SetRenderDrawColor(sdl.renderer, 255, 108, 0,   SDL_ALPHA_OPAQUE); break;
				case 2:  SDL_SetRenderDrawColor(sdl.renderer, 0,   174, 255, SDL_ALPHA_OPAQUE); break;
				case 3:  SDL_SetRenderDrawColor(sdl.renderer, 79,  255, 116, SDL_ALPHA_OPAQUE); break;
				case 4:  SDL_SetRenderDrawColor(sdl.renderer, 112, 62,  230, SDL_ALPHA_OPAQUE); break;
				case 5:  SDL_SetRenderDrawColor(sdl.renderer, 255, 192, 104, SDL_ALPHA_OPAQUE); break;
				case 6:  SDL_SetRenderDrawColor(sdl.renderer, 110, 146, 215, SDL_ALPHA_OPAQUE); break;
				case 7:  SDL_SetRenderDrawColor(sdl.renderer, 255, 107, 107, SDL_ALPHA_OPAQUE); break;
				case 8:  SDL_SetRenderDrawColor(sdl.renderer, 90,  250, 255, SDL_ALPHA_OPAQUE); break;
				default: SDL_SetRenderDrawColor(sdl.renderer, 255, 255, 255, SDL_ALPHA_OPAQUE); break;
			};
			//draw moving average
			SDL_RenderDrawLines(sdl.renderer, ma_points, loops);
		}
	}



	//draw price text
	uint font_size = 16;
	const int bottom_padding = -4;
	const int side_margin = 20;
	for (int i = 0; i * price_spacer < offset; i++)
	{
		double price = (i * price_spacer) / scale;
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2) << price;
		std::string s = ss.str();
		std::string text_str = "$" + ss.str();
		const char *text = text_str.c_str();
		//render right side price values
		render_text(
      window_w - side_margin,
      offset - (i * price_spacer) - bottom_padding,
      text, font_size, &white, "open_sans", "OpenSans-Regular.ttf", TOP_LEFT);
		//render left side price values
		render_text(
      side_margin,
      offset - (i * price_spacer) - bottom_padding,
      text, font_size, &white, "open_sans", "OpenSans-Regular.ttf", TOP_RIGHT);
	}
}

void mpe::Renderer::draw_mouse_hover(void)
{
	const int mouse_x = hovering_mouse_x;
	//const int mouse_y = hovering_mouse_y;

	const Chart_Data chart = Active_Chart;

	const ushort bar_width = total_bar_width;

  const uint adj_h_pan = (mouse_x / bar_width) + h_pan;
	const uint adj_mouse_x = (mouse_x / bar_width) * bar_width;


  //if (adj_h_pan >= chart.rows) return;


	ushort space, width;
	find_bar_size(bar_width, space, width);
	const int scale = s_pan;
	const int offset = window_h + v_pan;


	uint y_pos_top = (-(chart.hv.at(adj_h_pan) * scale) + offset);
	//uint y_pos_bottom = (-(chart.lv.at(adj_h_pan) * scale) + offset);


	std::string font_family = "PT_Mono";
	std::string font_face = "PTMono-Regular.ttf";

	const SDL_Color text_color = white;

	uint font_size = 11;
	uint spacing = 11;
	const int bottom_padding = -46;
	const int text_box_margin = 10;


	std::string date_text_str = "  Date: " + chart.dv.at(adj_h_pan);
	const char *date_text = date_text_str.c_str();

	std::string high_text_str = "  High: " + std::to_string(chart.hv.at(adj_h_pan));
	const char *high_text = high_text_str.c_str();

	std::string low_text_str = "   Low: " + std::to_string(chart.lv.at(adj_h_pan));
	const char *low_text = low_text_str.c_str();

	std::string open_text_str = "  Open: " + std::to_string(chart.ov.at(adj_h_pan));
	const char *open_text = open_text_str.c_str();

	std::string close_text_str = " Close: " + std::to_string(chart.cv.at(adj_h_pan));
	const char *close_text = close_text_str.c_str();

	std::string volume_text_str = std::to_string(chart.vv.at(adj_h_pan));
	//add commas
	int vn = volume_text_str.length() - 3;
	while (vn > 0)
	{
		volume_text_str.insert(vn, ",");
		vn -= 3;
	}
	volume_text_str = "Volume: " + volume_text_str;
	const char *volume_text = volume_text_str.c_str();

	std::string point_text_str = std::to_string(adj_h_pan);
	//add commas
	int pn = point_text_str.length() - 3;
	while (pn > 0)
	{
		point_text_str.insert(pn, ",");
		pn -= 3;
	}
	point_text_str = " Point: OC " + point_text_str;
	const char *point_text = point_text_str.c_str();


	//set text box to same width as the longest text
	Box date_size   = get_text_size(date_text,   font_size, font_family, font_face);
	Box high_size   = get_text_size(high_text,   font_size, font_family, font_face);
	Box low_size    = get_text_size(low_text,    font_size, font_family, font_face);
	Box open_size   = get_text_size(open_text,   font_size, font_family, font_face);
	Box close_size  = get_text_size(close_text,  font_size, font_family, font_face);
	Box volume_size = get_text_size(volume_text, font_size, font_family, font_face);
	Box point_size  = get_text_size(point_text,  font_size, font_family, font_face);

	int text_box_width = 0;

	if (text_box_width < date_size.w)   text_box_width = date_size.w;
	if (text_box_width < high_size.w)   text_box_width = high_size.w;
	if (text_box_width < low_size.w)    text_box_width = low_size.w;
	if (text_box_width < open_size.w)   text_box_width = open_size.w;
	if (text_box_width < close_size.w)  text_box_width = close_size.w;
	if (text_box_width < volume_size.w) text_box_width = volume_size.w;
	if (text_box_width < point_size.w)  text_box_width = point_size.w;


	uint adj_line_pos = adj_mouse_x + (width / 2);
	//center line pos on even bar widths
	//if (bar_width % 2 == 0) adj_line_pos--;

	const SDL_Rect backdrop = {
		static_cast<int>(adj_line_pos) - text_box_margin,
		static_cast<int>(y_pos_top) + bottom_padding - static_cast<int>(spacing * 7) - text_box_margin,
		text_box_width + (text_box_margin * 2),
		static_cast<int>(spacing * 7) + (text_box_margin * 2)
	};


	SDL_SetRenderDrawBlendMode(sdl.renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(sdl.renderer, 0, 0, 0, 180);
	SDL_RenderFillRect(sdl.renderer, &backdrop);
	SDL_SetRenderDrawBlendMode(sdl.renderer, SDL_BLENDMODE_NONE);



	render_text(adj_line_pos, y_pos_top + bottom_padding - (spacing * 6), date_text,   font_size, &text_color, font_family, font_face, TOP_RIGHT);
	render_text(adj_line_pos, y_pos_top + bottom_padding - (spacing * 5), high_text,   font_size, &text_color, font_family, font_face, TOP_RIGHT);
	render_text(adj_line_pos, y_pos_top + bottom_padding - (spacing * 4), open_text,   font_size, &text_color, font_family, font_face, TOP_RIGHT);
	render_text(adj_line_pos, y_pos_top + bottom_padding - (spacing * 3), close_text,  font_size, &text_color, font_family, font_face, TOP_RIGHT);
	render_text(adj_line_pos, y_pos_top + bottom_padding - (spacing * 2), low_text,    font_size, &text_color, font_family, font_face, TOP_RIGHT);
	render_text(adj_line_pos, y_pos_top + bottom_padding - (spacing * 1), volume_text, font_size, &text_color, font_family, font_face, TOP_RIGHT);
	render_text(adj_line_pos, y_pos_top + bottom_padding - (spacing * 0), point_text,  font_size, &text_color, font_family, font_face, TOP_RIGHT);


	SDL_SetRenderDrawColor(sdl.renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	//vertical line
	SDL_RenderDrawLine(sdl.renderer, adj_line_pos, y_pos_top + bottom_padding, adj_line_pos, y_pos_top + bottom_padding + 38);
	//horizontal line
	SDL_RenderDrawLine(sdl.renderer, adj_line_pos, y_pos_top + bottom_padding, adj_line_pos + text_box_width, y_pos_top + bottom_padding);







	double open  = chart.ov.at(adj_h_pan);
	double high  = chart.hv.at(adj_h_pan);
	double low   = chart.lv.at(adj_h_pan);
	double close = chart.cv.at(adj_h_pan);
	//uint volume  = chart.vv.at(adj_h_pan);




	//cords for high and low, represented by a line
	const SDL_Rect line = {
		static_cast<int>(adj_line_pos),
		static_cast<int>(-(high * scale) + offset),
		1,
		static_cast<int>((high - low) * scale)
	};

	//cords for open and close, represented by a bar
	float top = (open > close) ? open : close;
	const SDL_Rect bar = {
		static_cast<int>(adj_mouse_x),
		static_cast<int>(-(top * scale)) + offset,
		width,
		static_cast<int>(std::abs(open - close) * scale)
	};

  //pick color; if this point's open is higher than
  //last point's open, set to green. otherwise, set to red
  int color = 0;
  if (mouse_x / bar_width == 0) color = 1;
  else if (chart.ov.at(adj_h_pan) > chart.ov.at(adj_h_pan - 1)) color = 2;
  else color = 1;

	//set color
	if (color == 1) SDL_SetRenderDrawColor(sdl.renderer, 255, 110, 110, SDL_ALPHA_OPAQUE);
	if (color == 2) SDL_SetRenderDrawColor(sdl.renderer, 160, 255, 160, SDL_ALPHA_OPAQUE);

	//draw price line
	SDL_RenderDrawRect(sdl.renderer, &line);

	//mask out price bar
	SDL_SetRenderDrawColor(sdl.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(sdl.renderer, &bar);

	//set color again
	if (color == 1) SDL_SetRenderDrawColor(sdl.renderer, 255, 110, 110, SDL_ALPHA_OPAQUE);
	if (color == 2) SDL_SetRenderDrawColor(sdl.renderer, 160, 255, 160, SDL_ALPHA_OPAQUE);

	//draw price bar outline
	SDL_RenderDrawRect(sdl.renderer, &bar);
}

void mpe::Renderer::draw_pitchfork(std::vector<std::vector<uint>> &points)
{
	const Chart_Data chart = Active_Chart;
	const int scale = s_pan;
	const int offset = window_h + v_pan;

	const ushort bar_width = total_bar_width;
	ushort space, width;
	find_bar_size(bar_width, space, width);

	auto adj_x = [&](double in)
	{
		int out = static_cast<int>((in - h_pan) * bar_width + static_cast<int>(width / 2));
		return out;
	};
	auto adj_y = [&](double in)
	{
		int out = static_cast<int>(-(in * scale - offset));
		return out;
	};

	double length = 80.0;

	for (uint i = 0; i < points.size(); i++)
	{
		uint   brace_start_x, brace_end_x, handle_start_x;
		double brace_start_y, brace_end_y, handle_start_y;
		for (int j = 0; j < 3; j++)
		{
			llint point = points[i].at(j);
			switch (j)
			{
				case 0:
					handle_start_x = point;
					handle_start_y = chart.hv.at(point);
					break;
				case 1:
					brace_start_x = point;
					brace_start_y = chart.hv.at(point);
					break;
				case 2:
					brace_end_x = point;
					brace_end_y = chart.hv.at(point);
					break;
			}
		}

		double handle_end_x = (brace_start_x + brace_end_x) / 2;
		double handle_end_y = (brace_start_y + brace_end_y) / 2;

		double alpha = atan2(handle_end_y - handle_start_y, handle_end_x - handle_start_x);

		double tine_middle_x = handle_end_x  + length * cos(alpha);
		double tine_middle_y = handle_end_y  + length * sin(alpha);
		double tine_top_x    = brace_start_x + length * cos(alpha);
		double tine_top_y    = brace_start_y + length * sin(alpha);
		double tine_bottom_x = brace_end_x   + length * cos(alpha);
		double tine_bottom_y = brace_end_y   + length * sin(alpha);

		//double full_tang_x = handle_start_x  + (length * 2) * cos(alpha);//extra length doesn't match rest of
		//double full_tang_y = handle_start_y  + (length * 2) * sin(alpha);//the tines, not using until fixed


		SDL_SetRenderDrawColor(sdl.renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

		//handle
		SDL_Point pfh_points[2] = {adj_x(handle_start_x), adj_y(handle_start_y), adj_x(handle_end_x), adj_y(handle_end_y)};
		SDL_RenderDrawLines(sdl.renderer, pfh_points, 2);

		//brace
		SDL_Point pfb_points[2] = {adj_x(brace_start_x), adj_y(brace_start_y), adj_x(brace_end_x), adj_y(brace_end_y)};
		SDL_RenderDrawLines(sdl.renderer, pfb_points, 2);

		//middle tine
		SDL_Point pfmt_points[2] = {adj_x(handle_end_x), adj_y(handle_end_y), adj_x(tine_middle_x), adj_y(tine_middle_y)};
		SDL_RenderDrawLines(sdl.renderer, pfmt_points, 2);

		//top tine
		SDL_Point pftt_points[2] = {adj_x(brace_start_x), adj_y(brace_start_y), adj_x(tine_top_x), adj_y(tine_top_y)};
		SDL_RenderDrawLines(sdl.renderer, pftt_points, 2);

		//bottom tine
		SDL_Point pfbt_points[2] = {adj_x(brace_end_x), adj_y(brace_end_y), adj_x(tine_bottom_x), adj_y(tine_bottom_y)};
		SDL_RenderDrawLines(sdl.renderer, pfbt_points, 2);

		//full tang
		//SDL_Point pfft_points[2] = {adj_x(handle_start_x), adj_y(handle_start_y), adj_x(full_tang_x), adj_y(full_tang_y)};
		//SDL_RenderDrawLines(sdl.renderer, pfft_points, 2);
	}
}

void mpe::Renderer::draw_tracked_symbols(void)
{
  short text_padding = -2;
  uint font_size = 17;

  std::vector<std::string> tracked_symbols = {"AMD", "APPL", "TSLA", "MSFT", "GOOG", "IBM"};
  int size = tracked_symbols.size();

  for (int i = 0; i < size; i++)
  {
    std::string screen_text_str = tracked_symbols.at(i);
    const char *screen_text = screen_text_str.c_str();

    std::string font_family = "open_sans";
    std::string font_face = "OpenSans-Italic.ttf";

    Box text_size = get_text_size(screen_text, font_size, font_family, font_face);
    render_text(20, text_padding + i * (text_size.h + (text_padding * 2)), screen_text, font_size, &white, font_family, font_face, BOTTOM_RIGHT);

    SDL_Point line_points[2] = {0, (i + 1) * (text_size.h + (text_padding * 2)), window_w, (i + 1) * (text_size.h + (text_padding * 2))};
    SDL_SetRenderDrawColor(sdl.renderer, 60, 60, 60, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLines(sdl.renderer, line_points, 2);
  }


  std::string screen_text_str = "[ add new symbol... ]";
  const char *screen_text = screen_text_str.c_str();

  std::string font_family = "open_sans";
  std::string font_face = "OpenSans-Regular.ttf";
  Justify justify = CENTER_BOTTOM;
  Point text_pos = {window_w / 2, 240};
  Box text_box = render_text(text_pos.x, text_pos.y, screen_text, 14, &white, font_family, font_face, justify);

  Box adj;

  switch (justify)
	{
		case TOP_RIGHT:
			adj.w = 0;
			adj.h = text_box.h;
			break;
		case TOP_LEFT:
			adj.w = text_box.w;
			adj.h = text_box.h;
			break;
		case BOTTOM_RIGHT:
			adj.w = 0;
			adj.h = 0;
			break;
		case BOTTOM_LEFT:
			adj.w = text_box.w;
			adj.h = 0;
			break;
		case CENTER:
			adj.w = text_box.w/2;
			adj.h = text_box.h/2;
			break;
    case CENTER_TOP:
      adj.w = text_box.w/2;
      adj.h = text_box.h;
      break;
    case CENTER_BOTTOM:
      adj.w = text_box.w/2;
      adj.h = 0;
      break;
    case CENTER_LEFT:
      adj.w = text_box.w;
      adj.h = text_box.h/2;
      break;
    case CENTER_RIGHT:
      adj.w = 0;
      adj.h = text_box.h/2;
      break;
	}

  //Rect text_rect = {text_pos.x - adj.w, text_pos.y - adj.h, text_box.w, text_box.h};
  const SDL_Rect back = {text_pos.x - adj.w, text_pos.y - adj.h, text_box.w, text_box.h};

  SDL_SetRenderDrawBlendMode(sdl.renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(sdl.renderer, 0, 0, 255, 180);
  SDL_RenderFillRect(sdl.renderer, &back);
  SDL_SetRenderDrawBlendMode(sdl.renderer, SDL_BLENDMODE_NONE);

	SDL_Rect btn = back;

	update_btn_rect(0, btn);
}

void mpe::Renderer::paint(void)
{
  if (!::repaint)
  {
    return;
  }
  ::repaint = false;

  std::cout << "\rRepaint " << paint_count++ << std::flush;

  int w, h;

  SDL_GetWindowSize(sdl.window, &w, &h);

  window_w = w;
  window_h = h;

  SDL_SetRenderDrawColor(sdl.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(sdl.renderer);

  std::string screen_text_str = "";

  switch(Active_Screen)
  {
    case LOG:
      screen_text_str = "Log";
      draw_log();
      break;
    case GRAPH:
      screen_text_str = "Graph";
      if (Active_Chart.rows != 0)
      {
        draw_chart();

        if (repaint_hovering)
        {
          repaint_hovering = false;
          draw_mouse_hover();
        }
      }
      break;
    case NORMALIZED_GRAPH:
      screen_text_str = "Normalized Graph";
      break;
    case SYMBOLS:
      screen_text_str = "Symbols";
      draw_tracked_symbols();
      break;
    case TRADES:
      screen_text_str = "Trades";
      break;
    case LOGIC:
      screen_text_str = "Logic";
      break;
    case SETTINGS:
      screen_text_str = "Settings";
      break;
  }

  std::string font_family = "Bebas_Neue";
  std::string font_face = "BebasNeue-Regular.ttf";

  uint font_size = 28;
  uint text_box_margin = 4;

  const char *screen_text = screen_text_str.c_str();
  Box text_size = get_text_size(screen_text, font_size, font_family, font_face);

  Point text_box_loc = {w - text_size.w - 20, h - 12};

  const SDL_Rect backdrop = {
    static_cast<int>(text_box_loc.x - text_box_margin) - 5,
    static_cast<int>(text_box_loc.y - text_size.h - text_box_margin),
    static_cast<int>(text_size.w + (text_box_margin * 2)) + 10,
    static_cast<int>(text_size.h + (text_box_margin * 2)) - 3
  };

  SDL_SetRenderDrawBlendMode(sdl.renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(sdl.renderer, 0, 0, 0, 180);
  SDL_RenderFillRect(sdl.renderer, &backdrop);
  SDL_SetRenderDrawBlendMode(sdl.renderer, SDL_BLENDMODE_NONE);

  render_text(text_box_loc.x, text_box_loc.y, screen_text, font_size, &white, font_family, font_face, TOP_RIGHT);

  SDL_RenderPresent(sdl.renderer);
}


/*pitchfork_points = {
  {19,46,51},
  {137,154,169}
};
draw_pitchfork(pitchfork_points, h);*/
