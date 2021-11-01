/* Chart Display
 *
 * 2021 Alexander Nathan Hack <alex@anhack.xyz>
 *
 * Mar, 14th 2021
 * Version 0.0.1 (Alpha)
 * logger.cpp
 */

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "chartdisplay.hpp"
#include "utils.hpp"
#include "logger.hpp"


void mpe::Log::write_to_file(std::string msg)
{
  if (log_file_path == "")
  {
    std::cerr << "Log file path unspecified" << std::endl;
    return;
  }
  std::string path = log_file_path;
  std::ofstream lfs;
  lfs.open(path, std::ofstream::app);
  if (lfs.is_open())
  {
    lfs << msg << "\n";
    lfs.close();
  }
  else
  {
    std::cerr << "Could not open logfile: " << path << std::endl;
  }
}
void mpe::Log::push(std::string msg)
{
  program_log.push_back(msg);
  if (Active_Screen == LOG)
 	{
 		repaint = true;
 	}
  write_to_file(msg);
}
std::string mpe::Log::pull(uint index)
{
  return program_log.at(index);
}
uint mpe::Log::get_size(void)
{
  return program_log.size();
}
mpe::Log::Log(void)
{
  log_file_path = get_resource_path("log") + "ChartDisplay.log";

  std::string path = log_file_path;
  std::ofstream lfs;
  lfs.open(path, std::ofstream::trunc);
  if (lfs.is_open())
  {
    lfs << "";
    lfs.close();
  }
  else
  {
    std::cout << "Could not open logfile: " << path << std::endl;
  }
}
