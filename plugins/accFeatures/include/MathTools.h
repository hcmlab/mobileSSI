// MathTools.h
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
#pragma once

#ifndef SSI_MATHTOOLS_H
#define SSI_MATHTOOLS_H

#include <vector>
#include <algorithm>

namespace ssi {

class MathTools {

public:
	static float GetSum(const std::vector<float> values);
	static float GetMean(const std::vector<float> values);
	static float GetMin(const std::vector<float> values);
	static float GetMax(const std::vector<float> values);
	static float GetMedian(const std::vector<float> values);
	static float GetStdDeviation(const std::vector<float> values);
	static float GetSkew(const std::vector<float> values);
	static float GetKurtosis(const std::vector<float> values);
	static float GetRange(const std::vector<float> values);
	
};

}
#endif
