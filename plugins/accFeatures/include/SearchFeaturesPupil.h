// Searchfeatures.h
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

#pragma once

#ifndef SSI_SEARCHFEATURES_PUPIL_H
#define SSI_SEARCHFEATURES_PUPIL_H

#include "base/IFeature.h"
#include "ioput/option/OptionList.h"
#include "MathTools.h"

namespace ssi {

#define SEARCHFEATURES_PUPIL_BLINK_THRESHOLD_MIN	3 // Number of samples before eye closed counts as blink
#define SEARCHFEATURES_PUPIL_BLINK_THRESHOLD_MAX	150 // Number of samples before eye closed does not count as blink anymore

#define SEARCHFEATURES_PUPIL_DIM_STATUS				 0
#define SEARCHFEATURES_PUPIL_DIM_PUPIL_X			 1
#define SEARCHFEATURES_PUPIL_DIM_PUPIL_Y			 2
#define SEARCHFEATURES_PUPIL_DIM_PUPIL_WIDTH		 3
#define SEARCHFEATURES_PUPIL_DIM_PUPIL_HEIGHT		 4
#define SEARCHFEATURES_PUPIL_DIM_PUPIL_ELLIPSE_ANGLE 5
#define SEARCHFEATURES_PUPIL_DIM_PUPIL_AVERAGERADIUS 6
#define SEARCHFEATURES_PUPIL_DIM_PUPIL_CONFIDENCE	 7
#define SEARCHFEATURES_PUPIL_DIM_COUNT			     8

class SearchFeaturesPupil : public IFeature {

	public:

		class Options : public OptionList {

		public:

			Options() {
				addOption("blinkCount", &blinkCount, 1, SSI_BOOL, "Counts blinks");
				addOption("blinkDurationMean", &blinkDurationMean, 1, SSI_BOOL, "Mean of the blink duration");
				addOption("blinkDurationMin", &blinkDurationMin, 1, SSI_BOOL, "Minimum blink duration");
				addOption("blinkDurationMax", &blinkDurationMax, 1, SSI_BOOL, "Maximum blink duration");
				addOption("blinkDurationMedian", &blinkDurationMedian, 1, SSI_BOOL, "Median blink duration");
				addOption("blinkDurationSkew", &blinkDurationSkew, 1, SSI_BOOL, "Skewness of blink duration");
				addOption("blinkDurationKurtosis", &blinkDurationKurtosis, 1, SSI_BOOL, "Kurtosis of blink duration");
				addOption("blinkDurationRange", &blinkDurationRange, 1, SSI_BOOL, "Range of blink duration");
				addOption("blinkDurationTotal", &blinkDurationTotal, 1, SSI_BOOL, "Total blink duration (in seconds)");
				addOption("blinkDurationVar", &blinkDurationVar, 1, SSI_BOOL, "Variance of the blink duration");
				addOption("pupilSizeChangeMean", &pupilSizeChangeMean, 1, SSI_BOOL, "Mean of the pupil size change");
				addOption("pupilSizeChangeMin", &pupilSizeChangeMin, 1, SSI_BOOL, "Minimum pupil size change");
				addOption("pupilSizeChangeMax", &pupilSizeChangeMax, 1, SSI_BOOL, "Maximum pupil size change");
				addOption("pupilSizeChangeMedian", &pupilSizeChangeMedian, 1, SSI_BOOL, "Median pupil size change");
				addOption("pupilSizeChangeSkew", &pupilSizeChangeSkew, 1, SSI_BOOL, "Skewness of pupil size change");
				addOption("pupilSizeChangeKurtosis", &pupilSizeChangeKurtosis, 1, SSI_BOOL, "Kurtosis of pupil size change");
				addOption("pupilSizeChangeRange", &pupilSizeChangeRange, 1, SSI_BOOL, "Range of pupil size change");
				addOption("pupilSizeChangeVar", &pupilSizeChangeVar, 1, SSI_BOOL, "Variance of the pupil size change");
				addOption("pupilSizeChangeTotal", &pupilSizeChangeTotal, 1, SSI_BOOL, "Total pupil size change");
			}

			bool blinkCount				= true, // true
				blinkDurationMean		= true, // true
				blinkDurationMin		= false,
				blinkDurationMax		= false,
				blinkDurationMedian		= false,
				blinkDurationSkew		= false,
				blinkDurationKurtosis	= false,
				blinkDurationRange		= false,
				blinkDurationTotal		= true, // true
				blinkDurationVar		= true, // true
				pupilSizeChangeMean		= true, // true
				pupilSizeChangeMin		= false,
				pupilSizeChangeMax		= false,
				pupilSizeChangeMedian	= false,
				pupilSizeChangeSkew		= false,
				pupilSizeChangeKurtosis	= false,
				pupilSizeChangeRange	= false,
				pupilSizeChangeVar		= true, // true
				pupilSizeChangeTotal	= true; // true
		};

	public:

		static const ssi_char_t *GetCreateName() { return "SearchFeaturesPupil"; };
		static IObject *Create(const ssi_char_t *file) { return new SearchFeaturesPupil(file); };
		~SearchFeaturesPupil();

		Options *getOptions() { return &_options; };
		const ssi_char_t *getName() { return GetCreateName(); };
		const ssi_char_t *getInfo() { return "This is just a sample object."; };

		void transform_enter(ssi_stream_t &stream_in,
			ssi_stream_t &stream_out,
			ssi_size_t xtra_stream_in_num = 0,
			ssi_stream_t xtra_stream_in[] = 0);
		void transform(ITransformer::info info,
			ssi_stream_t &stream_in,
			ssi_stream_t &stream_out,
			ssi_size_t xtra_stream_in_num,
			ssi_stream_t xtra_stream_in[]);
		void transform_flush(ssi_stream_t &stream_in,
			ssi_stream_t &stream_out,
			ssi_size_t xtra_stream_in_num = 0,
			ssi_stream_t xtra_stream_in[] = 0);

		ssi_size_t getSampleDimensionOut(ssi_size_t sample_dimension_in)
		{
			if (sample_dimension_in != SEARCHFEATURES_PUPIL_DIM_COUNT)
			{
				ssi_err("DIM is %i, expected %i\n", sample_dimension_in, SEARCHFEATURES_PUPIL_DIM_COUNT);
			}

			ssi_size_t dim = 0;

			if (_options.blinkCount) dim++;
			if (_options.blinkDurationMean) dim++;
			if (_options.blinkDurationMin) dim++;
			if (_options.blinkDurationMax) dim++;
			if (_options.blinkDurationMedian) dim++;
			if (_options.blinkDurationSkew) dim++;
			if (_options.blinkDurationKurtosis) dim++;
			if (_options.blinkDurationRange) dim++;
			if (_options.blinkDurationTotal) dim++;
			if (_options.blinkDurationVar) dim++;
			if (_options.pupilSizeChangeMean) dim++;
			if (_options.pupilSizeChangeMin) dim++;
			if (_options.pupilSizeChangeMax) dim++;
			if (_options.pupilSizeChangeMedian) dim++;
			if (_options.pupilSizeChangeSkew) dim++;
			if (_options.pupilSizeChangeKurtosis) dim++;
			if (_options.pupilSizeChangeRange) dim++;
			if (_options.pupilSizeChangeVar) dim++;
			if (_options.pupilSizeChangeTotal) dim++;

			return dim;
		}

		ssi_size_t getSampleBytesOut(ssi_size_t sample_bytes_in)
		{
			return sizeof(ssi_real_t);
		}

		ssi_type_t getSampleTypeOut(ssi_type_t sample_type_in)
		{
			if (sample_type_in != SSI_REAL)
			{
				ssi_err("type '%s' not supported", SSI_TYPE_NAMES[sample_type_in]);
			}
			return SSI_REAL;
		}

		int getBlinkCount(std::vector<float> blinkDurations);
		float getBlinkDurationMean(std::vector<float> blinkDurations);
		float getBlinkDurationMin(std::vector<float> blinkDurations);
		float getBlinkDurationMax(std::vector<float> blinkDurations);
		float getBlinkDurationMedian(std::vector<float> blinkDurations);
		float getBlinkDurationSkew(std::vector<float> blinkDurations);
		float getBlinkDurationKurtosis(std::vector<float> blinkDurations);
		float getBlinkDurationRange(std::vector<float> blinkDurations);
		float getBlinkDurationTotal(std::vector<float> blinkDurations);
		float getBlinkDurationVar(std::vector<float> blinkDurations);
		float getPupilSizeChangeMean(std::vector<float> pupilSizeChanges);
		float getPupilSizeChangeMin(std::vector<float> pupilSizeChanges);
		float getPupilSizeChangeMax(std::vector<float> pupilSizeChanges);
		float getPupilSizeChangeMedian(std::vector<float> pupilSizeChanges);
		float getPupilSizeChangeSkew(std::vector<float> pupilSizeChanges);
		float getPupilSizeChangeKurtosis(std::vector<float> pupilSizeChanges);
		float getPupilSizeChangeRange(std::vector<float> pupilSizeChanges);
		float getPupilSizeChangeVar(std::vector<float> pupilSizeChanges);
		float getPupilSizeChangeTotal(std::vector<float> pupilSizeChanges);

	protected:

		SearchFeaturesPupil(const ssi_char_t *file = 0);
		ssi_char_t *_file;
		Options _options;
		static char ssi_log_name[];

		std::vector<float> getValues(ssi_real_t *ptr_in, ssi_size_t sample_number, ssi_size_t sample_dimension, int dimension);
		std::vector<float> getBlinkDurations(std::vector<float> statusValues, ssi_time_t sr);
		std::vector<float> getPupilSizes(std::vector<float> pupilWidthValues, std::vector<float> pupilHeightValues);
		std::vector<float> getPupilSizeChanges(std::vector<float> pupilSizes);
	};

}

#endif
