/* Chart Display
 *
 * 2021 Alexander Nathan Hack <alex@anhack.xyz>
 *
 * Feb, 11th 2021
 * Version 0.0.1 (Alpha)
 * utils.cpp
 */

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <utility>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "chartdisplay.hpp"
#include "csv.hpp"
#include "logger.hpp"


std::string get_resource_path(const std::string &sub_dir = "")
{
	static std::string base_res;
	if (base_res.empty())
  {
		char *base_path = SDL_GetBasePath();
		if (base_path)
    {
			base_res = base_path;
			SDL_free(base_path);
		}
		else
		{
			std::cerr << "Error getting resource path: " << SDL_GetError() << std::endl;
			return "";
		}
		size_t pos = base_res.rfind("bin");
		base_res = base_res.substr(0, pos) + "res" + PATH_SEP;
	}
	return sub_dir.empty() ? base_res : base_res + sub_dir + PATH_SEP;
}

std::string get_font_path(const std::string &font_family = "", const std::string &font_name = "")
{
	std::string font_folder = "fonts";
	return get_resource_path(PROJ_DIR) + font_folder + PATH_SEP + font_family + PATH_SEP + font_name;
}

std::vector<std::pair<std::string, std::string>> load_config(std::string filename)
{
	std::vector<std::pair<std::string, std::string>> config_list;
	std::string config_path = get_resource_path("config") + filename;
	std::ifstream istrm;
	istrm.open(config_path);
	if (istrm.is_open())
	{
		std::string line;
		while (getline(istrm, line))
		{
			line.erase(
				std::remove_if(
					line.begin(), line.end(), isspace
				), line.end()
			);
			if (line[0] == '#' || line.empty())
			continue;
			uint delimiter_pos = line.find("=");
			std::string key = line.substr(0, delimiter_pos);
			std::string value = line.substr(delimiter_pos + 1);
			std::pair<std::string, std::string> key_value_pair(key, value);
			config_list.push_back(key_value_pair);
		}
	}
	else
	{
		std::cerr << "Couldn't open config file " << config_path << std::endl;
	}
	return config_list;
}

int reload_config(void)
{
	mp_log.push("<utils> reloading config");
  std::vector<std::pair<std::string, std::string>> config_list = load_config("settings.cfg");
  for (uint i = 0; i < config_list.size(); i++)
  {
    std::pair key_value_pair = config_list.at(i);
    std::string key = key_value_pair.first;
    std::string value = key_value_pair.second;

    if (key == "active_chart")
    {
      Chart_Data chart;
      std::string chart_loc_str = "csv/" + value;
      const char *chart_loc = chart_loc_str.c_str();

      if (read_csv(chart_loc, chart) != 0)
      {
        return -1;
      }
      calc_mov_avg(chart, {15,40,120});

      Active_Chart = chart;

      //reset chart display modifiers
      v_pan = 0;
      h_pan = 0;
      s_pan = 20;
      total_bar_width = 8;

			if (Active_Screen == GRAPH)
			{
				repaint = true;
			}
    }
  }
  return 0;
}
