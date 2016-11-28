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

#ifndef SSI_SEARCHFEATURES_HEAD_H
#define SSI_SEARCHFEATURES_HEAD_H

#include "base/IFeature.h"
#include "ioput/option/OptionList.h"
#include "../libs/build/kissfft/include/FFT.h"
#include "MathTools.h"

namespace ssi {

#define SEARCHFEATURES_HEAD_DIM_ACC_X 0
#define SEARCHFEATURES_HEAD_DIM_ACC_Y 1
#define SEARCHFEATURES_HEAD_DIM_ACC_Z 2
#define SEARCHFEATURES_HEAD_DIM_COUNT 3

	class SearchFeaturesHead : public IFeature {

	public:

		class Options : public OptionList {

		public:

			Options() {
				addOption("meanX", &meanX, 1, SSI_BOOL, "Mean acceleration value for x-axis");
				addOption("meanY", &meanY, 1, SSI_BOOL, "Mean acceleration value for y-axis");
				addOption("meanZ", &meanZ, 1, SSI_BOOL, "Mean acceleration value for z-axis");
				addOption("stdDeviationX", &stdDeviationX, 1, SSI_BOOL, "Standard deviation for x-axis");
				addOption("stdDeviationY", &stdDeviationY, 1, SSI_BOOL, "Standard deviation for y-axis");
				addOption("stdDeviationZ", &stdDeviationZ, 1, SSI_BOOL, "Standard deviation for z-axis");
				addOption("energyX", &energyX, 1, SSI_BOOL, "Energy for x-axis");
				addOption("energyY", &energyY, 1, SSI_BOOL, "Energy for y-axis");
				addOption("energyZ", &energyZ, 1, SSI_BOOL, "Energy for z-axis");
				addOption("correlationXY", &correlationXY, 1, SSI_BOOL, "Correlation between x and y-axis");
				addOption("correlationXZ", &correlationXZ, 1, SSI_BOOL, "Correlation between x and z-axis");
				addOption("correlationYZ", &correlationYZ, 1, SSI_BOOL, "Correlation between y and z-axis");
				addOption("displacementX", &displacementX, 1, SSI_BOOL, "Displacement for x-axis");
				addOption("displacementY", &displacementY, 1, SSI_BOOL, "Displacement for y-axis");
				addOption("displacementZ", &displacementZ, 1, SSI_BOOL, "Displacement for z-axis");
			}

			bool meanX			= true,
				 meanY			= true,
				 meanZ			= true,
				 stdDeviationX	= true,
				 stdDeviationY	= true,
				 stdDeviationZ  = true,
				 energyX		= true,
				 energyY		= true,
				 energyZ		= true,
				 correlationXY	= true,
				 correlationXZ	= true,
				 correlationYZ	= true,
				 displacementX	= true,
				 displacementY	= true,
				 displacementZ	= true;
		};

	public:

		static const ssi_char_t *GetCreateName() { return "SearchFeaturesHead"; };
		static IObject *Create(const ssi_char_t *file) { return new SearchFeaturesHead(file); };
		~SearchFeaturesHead();

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
			if (sample_dimension_in != SEARCHFEATURES_HEAD_DIM_COUNT)
			{
				ssi_err("DIM is %i, expected %i\n", sample_dimension_in, SEARCHFEATURES_HEAD_DIM_COUNT);
			}

			ssi_size_t dim = 0;

			if (_options.meanX) dim++;
			if (_options.meanY) dim++;
			if (_options.meanZ) dim++;
			if (_options.stdDeviationX) dim++;
			if (_options.stdDeviationY) dim++;
			if (_options.stdDeviationZ) dim++;
			if (_options.energyX) dim++;
			if (_options.energyY) dim++;
			if (_options.energyZ) dim++;
			if (_options.correlationXY) dim++;
			if (_options.correlationXZ) dim++;
			if (_options.correlationYZ) dim++;
			if (_options.displacementX) dim++;
			if (_options.displacementY) dim++;
			if (_options.displacementZ) dim++;

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

		float getMean(std::vector<float> values);
		float getStdDeviation(std::vector<float> values);
		float getEnergy(std::vector<float> values);
		float getCorrelation(std::vector<float> aValues, std::vector<float> bValues);
		float getDisplacement(std::vector<float> values, ssi_time_t sr);

	protected:

		SearchFeaturesHead(const ssi_char_t *file = 0);
		ssi_char_t *_file;
		Options _options;
		static char ssi_log_name[];


		std::vector<float> getValues(ssi_real_t *ptr_in, ssi_size_t sample_number, ssi_size_t sample_dimension, int dimension);
	};

}

#endif
