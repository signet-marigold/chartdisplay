/* Chart Display
 *
 * 2021 Alexander Nathan Hack <alex@anhack.xyz>
 *
 * Feb, 11th 2021
 * Version 0.0.1 (Alpha)
 * z_score.hpp
 */

#ifndef Z_SCORE_HPP
#define Z_SCORE_HPP


#include <unordered_map>
#include <vector>
#include <string>

#include "chartdisplay.hpp"

class VectorStats
{
public:
  VectorStats(vec_iter_ld, vec_iter_ld);
  void compute(void);
  ld mean(void);
  ld standard_deviation(void);
private:
  vec_iter_ld start;
  vec_iter_ld end;
  ld m1;
  ld m2;
};

std::unordered_map<std::string, std::vector<ld>> z_score_thresholding(std::vector<ld>, int, ld, ld);


#endif  // Z_SCORE_HPP
