/* Chart Display
 *
 * 2021 Alexander Nathan Hack <alex@anhack.xyz>
 *
 * Feb, 11th 2021
 * Version 0.0.1 (Alpha)
 * csv.hpp
 */

#ifndef CSV_HPP
#define CSV_HPP


#include <vector>

#include "chartdisplay.hpp"

int read_csv(const char *, Chart_Data &);
void calc_mov_avg(Chart_Data &, std::vector<uint>);


#endif  // CSV_HPP
