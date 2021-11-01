/* Chart Display
 *
 * 2021 Alexander Nathan Hack <alex@anhack.xyz>
 *
 * Mar, 14th 2021
 * Version 0.0.1 (Alpha)
 * logger.hpp
 */

#ifndef LOGGER_HPP
#define LOGGER_HPP


#include <vector>
#include <string>

#include "chartdisplay.hpp"

namespace mpe
{
  class Log
  {
  private:
    std::string log_file_path = "";
    std::vector<std::string> program_log;
    void write_to_file(std::string);//write one line to file
  public:
    void push(std::string);//pushback one line
    std::string pull(uint);//get one line
    uint get_size(void);
    Log();
  };
}

inline mpe::Log mp_log;


#endif  // LOGGER_HPP
