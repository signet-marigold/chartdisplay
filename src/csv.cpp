/* Chart Display
 *
 * 2021 Alexander Nathan Hack <alex@anhack.xyz>
 *
 * Feb, 11th 2021
 * Version 0.0.1 (Alpha)
 * csv.cpp
 */

#include <iostream>
#include <algorithm>
#include <vector>

#include "chartdisplay.hpp"
#include "logger.hpp"

int read_csv(const char *filename, Chart_Data &chart)
{
	char d[11];
	double o, h, l, c, ac, v;

	double highest_price = 0;
	uint highest_volume = 0;

	if (FILE *fp = fopen(filename, "r"))
	{
		//skips the first line
		char ch;
		while (ch != '\n')
		{
		  ch = fgetc(fp);
		}

		uint rows = 0;
		//date,open,high,low,close,volume
    while (fscanf(fp, "%10[^,\n\t]%*c %lf%*c %lf%*c %lf%*c %lf%*c %lf%*c %lf%*c", d, &o, &h, &l, &c, &ac, &v) == 7)
		{
			rows++;
			std::string str_d(d);
			uint d_v = static_cast<uint>(v);
      chart.dv.push_back(str_d);
			chart.ov.push_back(o);
			chart.hv.push_back(h);
			chart.lv.push_back(l);
			chart.cv.push_back(c);
			chart.vv.push_back(d_v);

			if (h > highest_price) highest_price = h;
			if (d_v > highest_volume) highest_volume = d_v;
    }
		mp_log.push("<csv> Loading chart: " + std::string(filename) + "  Rows: " + std::to_string(rows));

    fclose(fp);

		chart.rows = rows;

		chart.top_price = highest_price;
		chart.top_volume = highest_volume;

		bool flip = false;
		if (flip)
		{
			std::reverse(chart.dv.begin(), chart.dv.end());
			std::reverse(chart.ov.begin(), chart.ov.end());
			std::reverse(chart.hv.begin(), chart.hv.end());
			std::reverse(chart.lv.begin(), chart.lv.end());
			std::reverse(chart.cv.begin(), chart.cv.end());
			std::reverse(chart.vv.begin(), chart.vv.end());
		}

		return 0;
	}
	return -1;
}

void calc_mov_avg(Chart_Data &chart, std::vector<uint> points)
{
	for (uint k = 0; k < points.size(); k++)
	{
		std::vector<double> ma;
		for (uint i = 0; i < chart.rows; i++)
		{
			uint loops;
			if (i < points[k])
			{
				loops = i + 1;
			}
			else
			{
				loops = points[k];
			}
			ld values = 0.0;
			for (uint j = 0; j < loops; j++)
			{
				uint index = i - j;
				double difference = chart.hv.at(index) - chart.lv.at(index);
				values += chart.lv.at(index) + (difference / 2);
			}
			double average = values / loops;
			ma.push_back(average);
		}
		chart.ma.push_back(ma);
	}
}
