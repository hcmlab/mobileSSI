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
#include <math.h>
#include <float.h>

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
		float median;

		std::sort(values.begin(), values.end());
		int n = static_cast<int>(values.size());

		if (n > 0)
		{
			if (n % 2 == 0)
			{
				// Even
				median = (values[n / 2 - 1] + values[n / 2]) / 2.0f;
			}
			else
			{
				// odd
				median = values[(n - 1) / 2];
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
}
