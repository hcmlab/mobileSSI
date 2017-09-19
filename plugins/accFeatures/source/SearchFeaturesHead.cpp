// Searchfeatures.cpp
// author: Michael Dietz <michael.dietz@informatik.uni-augsburg.de>
// created: 20/6/2016
// Copyright (C) University of Augsburg, Lab for Human Centered Multimedia
//
// *************************************************************************************************
//
// This file is part of Social Signal Interpretation (SSI) developed at the 
// Lab for Human Centered Multimedia of the University of Augsburg
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

#include "SearchFeaturesHead.h"

namespace ssi {

	char SearchFeaturesHead::ssi_log_name[] = "SearchFeaturesHead";

	SearchFeaturesHead::SearchFeaturesHead(const ssi_char_t *file) : _file(0) {

		if (file) {
                        if (!OptionList::LoadXML(file, &_options)) {
                                OptionList::SaveXML(file, &_options);
			}
			_file = ssi_strcpy(file);
		}
	}

	SearchFeaturesHead::~SearchFeaturesHead() {

		if (_file) {
                        OptionList::SaveXML(_file, &_options);
			delete[] _file;
		}
	}

	void SearchFeaturesHead::transform_enter(ssi_stream_t &stream_in,
		ssi_stream_t &stream_out,
		ssi_size_t xtra_stream_in_num,
		ssi_stream_t xtra_stream_in[]) {

	}

	void SearchFeaturesHead::transform(ITransformer::info info,
		ssi_stream_t &stream_in,
		ssi_stream_t &stream_out,
		ssi_size_t xtra_stream_in_num,
		ssi_stream_t xtra_stream_in[]) {

		ssi_size_t sample_dimension = stream_in.dim;
		ssi_size_t sample_number = info.frame_num + info.delta_num; // stream_in.num

		ssi_real_t *ptr_in = ssi_pcast(ssi_real_t, stream_in.ptr);
		ssi_real_t *ptr_out = ssi_pcast(ssi_real_t, stream_out.ptr);

		std::vector<float> xValues = getValues(ptr_in, sample_number, sample_dimension, SEARCHFEATURES_HEAD_DIM_ACC_X);
		std::vector<float> yValues = getValues(ptr_in, sample_number, sample_dimension, SEARCHFEATURES_HEAD_DIM_ACC_Y);
		std::vector<float> zValues = getValues(ptr_in, sample_number, sample_dimension, SEARCHFEATURES_HEAD_DIM_ACC_Z);


		if (_options.meanX)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getMean(xValues));
		}
		
		if (_options.meanY)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getMean(yValues));
		}

		if (_options.meanZ)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getMean(zValues));
		}

		if (_options.stdDeviationX)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getStdDeviation(xValues));
		}

		if (_options.stdDeviationY)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getStdDeviation(yValues));
		}

		if (_options.stdDeviationZ)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getStdDeviation(zValues));
		}

		if (_options.energyX)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getEnergy(xValues));
		}

		if (_options.energyY)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getEnergy(yValues));
		}

		if (_options.energyZ)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getEnergy(zValues));
		}

		if (_options.correlationXY)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getCorrelation(xValues, yValues));
		}

		if (_options.correlationXZ)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getCorrelation(xValues, zValues));
		}

		if (_options.correlationYZ)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getCorrelation(yValues, zValues));
		}

		if (_options.displacementX)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getDisplacement(xValues, stream_in.sr));
		}

		if (_options.displacementY)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getDisplacement(yValues, stream_in.sr));
		}

		if (_options.displacementZ)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getDisplacement(zValues, stream_in.sr));
		}
	}

	void SearchFeaturesHead::transform_flush(ssi_stream_t &stream_in,
		ssi_stream_t &stream_out,
		ssi_size_t xtra_stream_in_num,
		ssi_stream_t xtra_stream_in[]) {

	}

	/*
	 * Mean acceleration value
	 *
	 * based on
	 * Bao, Ling et al. - Activity Recognition from User-Annotated Acceleration Data
	 * Ravi, N. et al. - Activity recognition from accelerometer data
	 */
	float SearchFeaturesHead::getMean(std::vector<float> values)
	{
		return MathTools::GetMean(values);
	}

	/*
	* Standard deviation for acceleration value
	*
	* based on
	* Bao, Ling et al. - Activity Recognition from User-Annotated Acceleration Data
	* Ravi, N. et al. - Activity recognition from accelerometer data
	*/
	float SearchFeaturesHead::getStdDeviation(std::vector<float> values)
	{
		return MathTools::GetStdDeviation(values);
	}

	/*
	* Energy for acceleration value
	*
	* based on
	* Bao, Ling et al. - Activity Recognition from User-Annotated Acceleration Data
	* Ravi, N. et al. - Activity recognition from accelerometer data
	*/
	float SearchFeaturesHead::getEnergy(std::vector<float> values)
	{
		float energy = 0;

		ssi_size_t sampleNumber = static_cast<ssi_size_t>(values.size());

		// Copy data for FFT
		ssi_real_t* dataCopy = new ssi_real_t[sampleNumber];
		for (ssi_size_t i = 0; i < sampleNumber; i++)
		{
			dataCopy[i] = values[i];
		}

		// FFT
		FFT fft(sampleNumber, 1);
		Matrix<ssi_real_t> fftInput(sampleNumber, 1, dataCopy);
		Matrix<ssi_real_t> fftOutput(1, fft.rfft);
		fft.transform(&fftInput, &fftOutput);

		// Calculate energy
		for (ssi_size_t i = 0; i < fft.rfft; i++)
		{
			energy += pow((double)fftOutput.data[i], (double)2);
		}

		energy = energy / static_cast<float>(fft.rfft);

		// delete dataCopy; // gets deleted in the matrix for some reason

		return energy;
	}

	/*
	* Correlation for acceleration value
	*
	* based on
	* Bao, Ling et al. - Activity Recognition from User-Annotated Acceleration Data
	* Ravi, N. et al. - Activity recognition from accelerometer data
	*/
	float SearchFeaturesHead::getCorrelation(std::vector<float> aValues, std::vector<float> bValues)
	{
		float correlation = 0;
		float covariance = 0;
		float meanA = getMean(aValues);
		float meanB = getMean(bValues);
		float stdDeviationA = getStdDeviation(aValues);
		float stdDeviationB = getStdDeviation(bValues);

		size_t sampleCount = aValues.size();
		for (int i = 0; i < sampleCount; i++)
		{
			covariance += (aValues[i] - meanA) * (bValues[i] - meanB);
		}

		covariance = covariance / static_cast<float>(sampleCount);

		if (stdDeviationA * stdDeviationB != 0)
		{
			correlation = covariance / (stdDeviationA * stdDeviationB);
		}

		return correlation;
	}

	/*
	 * Displacement for acceleration value in meter
	 */
	float SearchFeaturesHead::getDisplacement(std::vector<float> values, ssi_time_t sr)
	{
		float displacement = 0;
		float a = 0;
		float velNew = 0;
		float velOld = 0;

		ssi_size_t sampleNumber = static_cast<ssi_size_t>(values.size());

		// Time for which the object moves with that acceleration
		float t = static_cast<float>(1.0 / sr);

		// Sum up displacement steps
		for (ssi_size_t i = 0; i < sampleNumber; i++)
		{
			a = (abs(values[i]) < 0.1) ? 0 : values[i];

			// v1 = a * t + v0
			velNew = a * t + velOld;

			// d = v0 * t + 0.5 * a * t²  or  d = v0 * t + 0.5 * (v1 - v0) * t
			displacement += velOld * t + 0.5f * (velNew - velOld) * t;

			// Update v0
			velOld = velNew;
		}

		return displacement;
	}

	/*
	 * Helper function to get the values as a list
	 */
	std::vector<float> SearchFeaturesHead::getValues(ssi_real_t *ptr_in, ssi_size_t sample_number, ssi_size_t sample_dimension, int dimension)
	{
		std::vector<float> values;

		for (ssi_size_t i = 0; i < sample_number; i++)
		{
			values.push_back(ptr_in[i * sample_dimension + dimension]);
		}

		return values;
	}
}
