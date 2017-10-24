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

#include "SearchFeaturesPupil.h"

namespace ssi {

	char SearchFeaturesPupil::ssi_log_name[] = "SearchFeaturesPupil";

	SearchFeaturesPupil::SearchFeaturesPupil(const ssi_char_t *file) : _file(0) {

		if (file) {
			if (!OptionList::LoadXML(file, _options)) {
				OptionList::SaveXML(file, _options);
			}
			_file = ssi_strcpy(file);
		}
	}

	SearchFeaturesPupil::~SearchFeaturesPupil() {

		if (_file) {
			OptionList::SaveXML(_file, _options);
			delete[] _file;
		}
	}

	void SearchFeaturesPupil::transform_enter(ssi_stream_t &stream_in,
		ssi_stream_t &stream_out,
		ssi_size_t xtra_stream_in_num,
		ssi_stream_t xtra_stream_in[]) {

	}

	void SearchFeaturesPupil::transform(ITransformer::info info,
		ssi_stream_t &stream_in,
		ssi_stream_t &stream_out,
		ssi_size_t xtra_stream_in_num,
		ssi_stream_t xtra_stream_in[]) {

		ssi_size_t sample_dimension = stream_in.dim;
		ssi_size_t sample_number = info.frame_num + info.delta_num; // stream_in.num

		ssi_real_t *ptr_in = ssi_pcast(ssi_real_t, stream_in.ptr);
		ssi_real_t *ptr_out = ssi_pcast(ssi_real_t, stream_out.ptr);

		std::vector<float> statusValues = getValues(ptr_in, sample_number, sample_dimension, SEARCHFEATURES_PUPIL_DIM_STATUS);
		std::vector<float> pupilWidthValues = getValues(ptr_in, sample_number, sample_dimension, SEARCHFEATURES_PUPIL_DIM_PUPIL_WIDTH);
		std::vector<float> pupilHeightValues = getValues(ptr_in, sample_number, sample_dimension, SEARCHFEATURES_PUPIL_DIM_PUPIL_HEIGHT);
		std::vector<float> pupilSizes = getPupilSizes(pupilWidthValues, pupilHeightValues);
		std::vector<float> pupilSizeChanges = getPupilSizeChanges(pupilSizes);

		std::vector<float> blinkDurations = getBlinkDurations(statusValues, stream_in.sr);

		if (_options.blinkCount)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getBlinkCount(blinkDurations));
		}

		if (_options.blinkDurationMean)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getBlinkDurationMean(blinkDurations));
		}

		if (_options.blinkDurationMin)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getBlinkDurationMin(blinkDurations));
		}

		if (_options.blinkDurationMax)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getBlinkDurationMax(blinkDurations));
		}

		if (_options.blinkDurationMedian)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getBlinkDurationMedian(blinkDurations));
		}

		if (_options.blinkDurationSkew)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getBlinkDurationSkew(blinkDurations));
		}

		if (_options.blinkDurationKurtosis)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getBlinkDurationKurtosis(blinkDurations));
		}

		if (_options.blinkDurationRange)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getBlinkDurationRange(blinkDurations));
		}

		if (_options.blinkDurationTotal)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getBlinkDurationTotal(blinkDurations));
		}

		if (_options.blinkDurationVar)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getBlinkDurationVar(blinkDurations));
		}

		if (_options.pupilSizeChangeMean)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getPupilSizeChangeMean(pupilSizeChanges));
		}

		if (_options.pupilSizeChangeMin)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getPupilSizeChangeMin(pupilSizeChanges));
		}

		if (_options.pupilSizeChangeMax)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getPupilSizeChangeMax(pupilSizeChanges));
		}

		if (_options.pupilSizeChangeMedian)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getPupilSizeChangeMedian(pupilSizeChanges));
		}

		if (_options.pupilSizeChangeSkew)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getPupilSizeChangeSkew(pupilSizeChanges));
		}

		if (_options.pupilSizeChangeKurtosis)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getPupilSizeChangeKurtosis(pupilSizeChanges));
		}

		if (_options.pupilSizeChangeRange)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getPupilSizeChangeRange(pupilSizeChanges));
		}

		if (_options.pupilSizeChangeVar)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getPupilSizeChangeVar(pupilSizeChanges));
		}

		if (_options.pupilSizeChangeTotal)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getPupilSizeChangeTotal(pupilSizeChanges));
		}

		statusValues.clear();
		pupilWidthValues.clear();
		pupilHeightValues.clear();
		pupilSizes.clear();
		pupilSizeChanges.clear();
		blinkDurations.clear();
	}

	void SearchFeaturesPupil::transform_flush(ssi_stream_t &stream_in,
		ssi_stream_t &stream_out,
		ssi_size_t xtra_stream_in_num,
		ssi_stream_t xtra_stream_in[]) {

	}

	/*
	 * Blink count
	 *
	 * based on
	 * Bulling, A. et al. - Eye movement analysis for activity recognition using electrooculography
	 */
	int SearchFeaturesPupil::getBlinkCount(std::vector<float> blinkDurations)
	{
		return static_cast<int>(blinkDurations.size());
	}

	/*
	* Mean of the blink durations
	*
	* based on
	* Bulling, A. et al. - Eye movement analysis for activity recognition using electrooculography
	*/
	float SearchFeaturesPupil::getBlinkDurationMean(std::vector<float> blinkDurations)
	{
		return MathTools::GetMean(blinkDurations);
	}

	/*
	* Minimum blink durations
	*/
	float SearchFeaturesPupil::getBlinkDurationMin(std::vector<float> blinkDurations)
	{
		return MathTools::GetMin(blinkDurations);
	}

	/*
	* Maximum blink durations
	*/
	float SearchFeaturesPupil::getBlinkDurationMax(std::vector<float> blinkDurations)
	{
		return MathTools::GetMax(blinkDurations);
	}

	/*
	* Median blink durations
	*/
	float SearchFeaturesPupil::getBlinkDurationMedian(std::vector<float> blinkDurations)
	{
		return MathTools::GetMedian(blinkDurations);
	}

	/*
	* Skew of blink durations
	*/
	float SearchFeaturesPupil::getBlinkDurationSkew(std::vector<float> blinkDurations)
	{
		return MathTools::GetSkew(blinkDurations);
	}

	/*
	* Kurtosis of blink durations
	*/
	float SearchFeaturesPupil::getBlinkDurationKurtosis(std::vector<float> blinkDurations)
	{
		return MathTools::GetKurtosis(blinkDurations);
	}

	/*
	* Range of blink durations
	*/
	float SearchFeaturesPupil::getBlinkDurationRange(std::vector<float> blinkDurations)
	{
		return MathTools::GetSum(blinkDurations);
	}

	/*
	* Total blink durations
	*/
	float SearchFeaturesPupil::getBlinkDurationTotal(std::vector<float> blinkDurations)
	{
		return MathTools::GetSum(blinkDurations);
	}

	/*
	* Variance of the blink durations
	*
	* based on
	* Bulling, A. et al. - Eye movement analysis for activity recognition using electrooculography
	*/
	float SearchFeaturesPupil::getBlinkDurationVar(std::vector<float> blinkDurations)
	{
		return MathTools::GetVariance(blinkDurations);
	}

	/*
	* Mean of pupil size changes
	*
	* based on
	* Partala, Timo et al. - Pupil size variation as an indication of affective processing
	*/
	float SearchFeaturesPupil::getPupilSizeChangeMean(std::vector<float> pupilSizeChanges)
	{
		return MathTools::GetMean(pupilSizeChanges);
	}

	/*
	* Minimum pupil size changes
	*/
	float SearchFeaturesPupil::getPupilSizeChangeMin(std::vector<float> pupilSizeChanges)
	{
		return MathTools::GetMin(pupilSizeChanges);
	}

	/*
	* Maximum pupil size changes
	*/
	float SearchFeaturesPupil::getPupilSizeChangeMax(std::vector<float> pupilSizeChanges)
	{
		return MathTools::GetMax(pupilSizeChanges);
	}

	/*
	* Median pupil size changes
	*/
	float SearchFeaturesPupil::getPupilSizeChangeMedian(std::vector<float> pupilSizeChanges)
	{
		return MathTools::GetMedian(pupilSizeChanges);
	}

	/*
	* Skew of pupil size changes
	*/
	float SearchFeaturesPupil::getPupilSizeChangeSkew(std::vector<float> pupilSizeChanges)
	{
		return MathTools::GetSkew(pupilSizeChanges);
	}

	/*
	* Kurtosis of pupil size changes
	*/
	float SearchFeaturesPupil::getPupilSizeChangeKurtosis(std::vector<float> pupilSizeChanges)
	{
		return MathTools::GetKurtosis(pupilSizeChanges);
	}

	/*
	* Range of pupil size changes
	*/
	float SearchFeaturesPupil::getPupilSizeChangeRange(std::vector<float> pupilSizeChanges)
	{
		return MathTools::GetRange(pupilSizeChanges);
	}

	/*
	* Variance of pupil size changes
	*
	* based on
	* Partala, Timo et al. - Pupil size variation as an indication of affective processing
	*/
	float SearchFeaturesPupil::getPupilSizeChangeVar(std::vector<float> pupilSizeChanges)
	{
		return MathTools::GetVariance(pupilSizeChanges);
	}

	/*
	* Total pupil size changes
	*
	* based on
	* Partala, Timo et al. - Pupil size variation as an indication of affective processing
	*/
	float SearchFeaturesPupil::getPupilSizeChangeTotal(std::vector<float> pupilSizeChanges)
	{
		return MathTools::GetSum(pupilSizeChanges);
	}


	/***********************************************************************************************\
	|* Helper functions
	\***********************************************************************************************/


	/*
	* Helper function to get pupil size
	*
	* a mathematically correct perspective transform of an ellipse back to a circle is not possible without more parameters,
	* so we approximate the actual pupil size by taking the maximum of width and height
	* http://stackoverflow.com/questions/26191714/how-to-rectify-a-detected-ellipse
	*/
	std::vector<float> SearchFeaturesPupil::getPupilSizes(std::vector<float> pupilWidthValues, std::vector<float> pupilHeightValues)
	{
		std::vector<float> pupilSizes;

		for (size_t i = 0; i < pupilWidthValues.size(); i++)
		{
			pupilSizes.push_back(max(pupilWidthValues[i], pupilHeightValues[i]));
		}

		return pupilSizes;
	}

	/*
	* Helper function to get pupil size change
	*/
	std::vector<float> SearchFeaturesPupil::getPupilSizeChanges(std::vector<float> pupilSizes)
	{
		std::vector<float> pupilChanges;

		for (size_t i = 0; i < pupilSizes.size(); i++)
		{
			if (i + 1 < pupilSizes.size())
			{
				pupilChanges.push_back(abs(pupilSizes[i] - pupilSizes[i + 1]));
			}
		}

		return pupilChanges;
	}

	/*
	* Helper function to get blink durations
	*/
	std::vector<float> SearchFeaturesPupil::getBlinkDurations(std::vector<float> statusValues, ssi_time_t sr)
	{
		int blinkSamples = 0;

		std::vector<float> blinkDurations;

		bool prevEyeClosed = false;
		bool eyeClosed = false;

		for (size_t i = 0; i < statusValues.size(); i++)
		{
			eyeClosed = (statusValues[i] == -10);

			// Blink ongoing
			if (eyeClosed)
			{
				blinkSamples++;
			}

			// Blink finished
			if (eyeClosed == false && prevEyeClosed == true)
			{
				// Only add blinks which are longer than a certain duration
				if (blinkSamples >= SEARCHFEATURES_PUPIL_BLINK_THRESHOLD_MIN && blinkSamples <= SEARCHFEATURES_PUPIL_BLINK_THRESHOLD_MAX)
				{
					blinkDurations.push_back(static_cast<float>((static_cast<float>(blinkSamples) / sr)));
				}

				blinkSamples = 0;
			}

			prevEyeClosed = eyeClosed;
		}

		return blinkDurations;
	}

	/*
	 * Helper function to get the values as a list
	 */
	std::vector<float> SearchFeaturesPupil::getValues(ssi_real_t *ptr_in, ssi_size_t sample_number, ssi_size_t sample_dimension, int dimension)
	{
		std::vector<float> values;

		for (ssi_size_t i = 0; i < sample_number; i++)
		{
			values.push_back(ptr_in[i * sample_dimension + dimension]);
		}

		return values;
	}
}
