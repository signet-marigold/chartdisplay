/* Chart Display
 *
 * 2021 Alexander Nathan Hack <alex@anhack.xyz>
 *
 * Feb, 11th 2021
 * Version 0.0.1 (Alpha)
 * z_score.cpp
 */

#include <algorithm>
#include <cmath>
#include <numeric>
#include <iterator>
#include <unordered_map>

#include "chartdisplay.hpp"
#include "z_score.hpp"


VectorStats::VectorStats(vec_iter_ld start, vec_iter_ld end)
{
  this->start = start;
  this->end = end;
  this->compute();
}

//This method calculates the mean and standard deviation using STL function.
//This is the Two-Pass implementation of the Mean & Variance calculation.
void VectorStats::compute(void)
{
  ld sum = std::accumulate(start, end, 0.0);
  uint slice_size = std::distance(start, end);
  ld mean = sum / slice_size;
  std::vector<ld> diff(slice_size);
  std::transform(start, end, diff.begin(), [mean](ld x) { return x - mean; });
  ld sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
  ld std_dev = std::sqrt(sq_sum / slice_size);

  this->m1 = mean;
  this->m2 = std_dev;
}

ld VectorStats::mean(void)
{
  return m1;
}

ld VectorStats::standard_deviation(void)
{
  return m2;
}


//This is the implementation of the Smoothed Z-Score Algorithm.
//influence between 0 and 1
std::unordered_map<std::string, std::vector<ld>> z_score_thresholding(std::vector<ld> input, int lag, ld threshold, ld influence)
{
  std::unordered_map<std::string, std::vector<ld>> output;

  uint n = (uint) input.size();
  std::vector<ld> signals(input.size());
  std::vector<ld> filtered_input(input.begin(), input.end());
  std::vector<ld> filtered_mean(input.size());
  std::vector<ld> filtered_stddev(input.size());

  VectorStats lag_subvector_stats(input.begin(), input.begin() + lag);
  filtered_mean[lag - 1] = lag_subvector_stats.mean();
  filtered_stddev[lag - 1] = lag_subvector_stats.standard_deviation();

  for (uint i = lag; i < n; i++)
	{
    if (abs(input[i] - filtered_mean[i - 1]) > threshold * filtered_stddev[i - 1])
    {
      signals[i] = (input[i] > filtered_mean[i - 1]) ? 1.0 : -1.0;
      filtered_input[i] = influence * input[i] + (1 - influence) * filtered_input[i - 1];
    }
    else
    {
      signals[i] = 0.0;
      filtered_input[i] = input[i];
    }
    VectorStats lag_subvector_stats(filtered_input.begin() + (i - lag), filtered_input.begin() + i);
    filtered_mean[i] = lag_subvector_stats.mean();
    filtered_stddev[i] = lag_subvector_stats.standard_deviation();
  }

  output["signals"] = signals;
  output["filtered_mean"] = filtered_mean;
  output["filtered_stddev"] = filtered_stddev;

  return output;
};
