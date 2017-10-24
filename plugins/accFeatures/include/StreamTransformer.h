// Gate.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2013/06/06
// Copyright (C) University of Augsburg, Lab for Human Centered Multimedia
//
// *************************************************************************************************
//
// This file is part of Social Signal Interpretation (SSI) developed at the 
// Lab for Human Centered Multimedia of the University of Augsburg
//
// This library is free software; you can redistribute itand/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or any laterversion.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FORA PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along withthis library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
//*************************************************************************************************

#pragma once

#ifndef SSI_SIGNAL_STREAMTRANSFORMER_H
#define SSI_SIGNAL_STREAMTRANSFORMER_H

#include "base/IFilter.h"
#include "signal/SignalCons.h"
#include "ioput/option/OptionList.h"

namespace ssi {

	class StreamTransformer : public IFilter {

	public:

		class Options : public OptionList {

		public:

			Options()
				: upperThreshX(1000), lowerThreshX(-1000), upperThreshY(1000), lowerThreshY(-1000), maxDiff(600), absolute(true) {

				addOption("upperThreshX", &upperThreshX, 1, SSI_REAL, "upper threshold", false);
				addOption("lowerThreshX", &lowerThreshX, 1, SSI_REAL, "lower threshold", false);
				addOption("upperThreshY", &upperThreshY, 1, SSI_REAL, "upper threshold", false);
				addOption("lowerThreshY", &lowerThreshY, 1, SSI_REAL, "lower threshold", false);
				addOption("maxDiff", &maxDiff, 1, SSI_REAL, "maximum difference between two values", false);
				addOption("absolute", &absolute, 1, SSI_BOOL, "use absolute threshold", false);
			};

			ssi_real_t upperThreshX;
			ssi_real_t lowerThreshX;
			ssi_real_t upperThreshY;
			ssi_real_t lowerThreshY;
			ssi_real_t maxDiff;
			bool absolute;
		};

	public:

		static const ssi_char_t *GetCreateName() { return "StreamTransformer"; };
		static IObject *Create(const ssi_char_t *file) { return new StreamTransformer(file); };
		~StreamTransformer();

		Options *getOptions() { return &_options; };
		const ssi_char_t *getName() { return GetCreateName(); };
		const ssi_char_t *getInfo() { return "Replaces values below threshold."; };

		void transform_enter(ssi_stream_t &stream_in,
			ssi_stream_t &stream_out,
			ssi_size_t xtra_stream_in_num = 0,
			ssi_stream_t xtra_stream_in[] = 0);
		void transform(ITransformer::info info,
			ssi_stream_t &stream_in,
			ssi_stream_t &stream_out,
			ssi_size_t xtra_stream_in_num = 0,
			ssi_stream_t xtra_stream_in[] = 0);
		void transform_flush(ssi_stream_t &stream_in,
			ssi_stream_t &stream_out,
			ssi_size_t xtra_stream_in_num = 0,
			ssi_stream_t xtra_stream_in[] = 0);

		ssi_size_t getSampleDimensionOut(ssi_size_t sample_dimension_in) {
			return sample_dimension_in;
		}
		ssi_size_t getSampleBytesOut(ssi_size_t sample_bytes_in) {
			return sample_bytes_in;
		}
		ssi_type_t getSampleTypeOut(ssi_type_t sample_type_in) {
			if (sample_type_in != SSI_REAL) {
				ssi_err("type %s not supported", SSI_TYPE_NAMES[sample_type_in]);
			}
			return SSI_REAL;
		}

	protected:

		StreamTransformer(const ssi_char_t *file = 0);
		StreamTransformer::Options _options;
		ssi_char_t *_file;

		ssi_real_t _lastValueX;
		ssi_real_t _lastValueY;

		void transformThreshold(ssi_size_t dim, ssi_size_t num, ssi_real_t *srcptr, ssi_real_t *dstptr);
	};

}

#endif
