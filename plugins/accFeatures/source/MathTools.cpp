// MathTools.cpp
// author: Michael Dietz <michael.dietz@informatik.uni-augsburg.de>
// created: 20/6/2016
// Copyright (C) University of Augsburg, Lab for Human Centered Multimedia
//
// *************************************************************************************************
//
// This file is part of Smart Sensor Integration (SSI) developed at the 
// Lab for Multimedia Concepts and Applications of the University of Augsburg
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
//*************************************************************************************************

#include "MathTools.h"
#include <cmath>
#include <cfloat>

namespace ssi {
	
	/*
	* Calculates the sum of all values
	*/
	float MathTools::GetSum(std::vector<float> values)
	{
		float sum = 0;

		for (size_t i = 0; i < values.size(); i++)
		{
			sum += values[i];
		}

		return sum;
	}

	/*
	 * Calculates the mean of all values
	 */
	float MathTools::GetMean(std::vector<float> values)
	{
		float mean = 0;

		if (values.size() > 0)
		{
			mean = MathTools::GetSum(values) / static_cast<float>(values.size());
		}

		return mean;
	}

	/*
	* Calculates the minimum of all values
	*/
	float MathTools::GetMin(std::vector<float> values)
	{
		float min = FLT_MAX;

		for (size_t i = 0; i < values.size(); i++)
		{
			if (values[i] < min)
			{
				min = values[i];
			}
		}

		if (min == FLT_MAX)
		{
			min = 0;
		}

		return min;
	}

	/*
	* Calculates the maximum of all values
	*/
	float MathTools::GetMax(std::vector<float> values)
	{
		float max = -FLT_MAX;

		for (size_t i = 0; i < values.size(); i++)
		{
			if (values[i] > max)
			{
				max = values[i];
			}
		}

		if (max == -FLT_MAX)
		{
			max = 0;
		}

		return max;
	}

	/*
	* Calculates the median of all values
	*/
	float MathTools::GetMedian(std::vector<float> values)
	{
		float median = 0;
        std::vector<float> values2;
        for(float x: values)
            values2.push_back(x);

        std::sort(values2.begin(), values2.end());
        int n = static_cast<int>(values2.size());

		if (n > 0)
		{
			if (n % 2 == 0)
			{
				// Even
                median = (values2[n / 2 - 1] + values2[n / 2]) / 2.0f;
			}
			else
			{
				// Odd
                median = values2[(n - 1) / 2];
			}
		}

		return median;
	}

	/*
	* Calculates the standard deviation of all values
	*/
	float MathTools::GetStdDeviation(std::vector<float> values)
	{
		float stdDeviation = 0;
		float mean = MathTools::GetMean(values);

		if (values.size() > 0)
		{
			for (size_t i = 0; i < values.size(); i++)
			{
				stdDeviation += pow(values[i] - mean, 2);
			}

			stdDeviation = sqrt(stdDeviation / static_cast<float>(values.size()));
		}

		return stdDeviation;
	}

	/*
	* Calculates the skew of all values
	*/
	float MathTools::GetSkew(std::vector<float> values)
	{
		float skew = 0;
		float mean = MathTools::GetMean(values);
		float stdDeviation = MathTools::GetStdDeviation(values);

		if (values.size() > 0 && stdDeviation > 0)
		{
			for (size_t i = 0; i < values.size(); i++)
			{
				skew += pow((values[i] - mean) / stdDeviation, 3);
			}

			skew = skew / static_cast<float>(values.size());
		}

		return skew;
	}

	/*
	* Calculates the kurtosis of all values
	*/
	float MathTools::GetKurtosis(std::vector<float> values)
	{
		float kurtosis = 0;
		float mean = MathTools::GetMean(values);
		float stdDeviation = MathTools::GetStdDeviation(values);

		if (values.size() > 0 && stdDeviation > 0)
		{
			for (size_t i = 0; i < values.size(); i++)
			{
				kurtosis += pow((values[i] - mean) / stdDeviation, 4);
			}

			kurtosis = kurtosis / static_cast<float>(values.size());
		}

		return kurtosis;
	}

	/*
	* Calculates the range of all values
	*/
	float MathTools::GetRange(std::vector<float> values)
	{
		return MathTools::GetMax(values) - MathTools::GetMin(values);
	}

	/*
	* Calculates the variance of all values
	*/
	float MathTools::GetVariance(std::vector<float> values)
	{
		float variance = 0;
		float mean = MathTools::GetMean(values);

		if (values.size() > 0)
		{
			for (size_t i = 0; i < values.size(); i++)
			{
				variance += pow(values[i] - mean, 2);
			}

			variance = variance / static_cast<float>(values.size());
		}

		return variance;
	}

	/*
	* Calculates the root mean square
	*/
	float MathTools::GetRMS(std::vector<float> values)
	{
		float rms = 0;

		if (values.size() > 0)
		{
			for (size_t i = 0; i < values.size(); i++)
			{
				rms += pow(values[i], 2);
			}

			rms = sqrt(rms / static_cast<float>(values.size()));
		}

		return rms;
	}

	/*
	* Calculates the mean absolute deviation of all values
	*/
	float MathTools::GetMAD(std::vector<float> values)
	{
		float mad = 0;
		float mean = MathTools::GetMean(values);

		if (values.size() > 0)
		{
			for (size_t i = 0; i < values.size(); i++)
			{
#if __ANDROID_API__
                mad += fabs(values[i] - mean);
#else
				mad += abs(values[i] - mean);
#endif
			}

			mad = sqrt(mad / static_cast<float>(values.size()));
		}

		return mad;
	}

	/*
	* Calculates the interquartile range
	*/
	float MathTools::GetIQR(std::vector<float> values)
	{
		float iqr = 0;

		std::sort(values.begin(), values.end());
		int n = static_cast<int>(values.size());

		std::vector<float> lowerPercentile;
		std::vector<float> upperPercentile;

		if (n > 0)
		{
			if (n % 2 == 0)
			{
				// Even
				for (size_t i = 0; i < n; i++)
				{
					if (i < n / 2)
					{
						lowerPercentile.push_back(values[i]);
					}
					else
					{
						upperPercentile.push_back(values[i]);
					}
				}
			}
			else
			{
				// Odd
				for (size_t i = 0; i < n; i++)
				{
					if (i < (n - 1) / 2)
					{
						lowerPercentile.push_back(values[i]);
					}

					// Exclude median
					
					if (i > (n - 1) / 2)
					{
						upperPercentile.push_back(values[i]);
					}
				}
			}

			iqr = GetMedian(upperPercentile) - GetMedian(lowerPercentile);
		}

		return iqr;
	}

	/*
	* Calculates the crest factor
	*/
	float MathTools::GetCrest(std::vector<float> values)
	{
		float crest = 0;
		float peak = 0;
		float rms = MathTools::GetRMS(values);

		for (size_t i = 0; i < values.size(); i++)
		{
#if __ANDROID_API__
            if (fabs(values[i]) > peak)
            {
                peak = fabs(values[i]);
            }
#else
			if (abs(values[i]) > peak)
			{
				peak = abs(values[i]);
			}
#endif
		}

		if (rms > 0)
		{
			crest = peak / rms;
		}

		return crest;
	}
}
