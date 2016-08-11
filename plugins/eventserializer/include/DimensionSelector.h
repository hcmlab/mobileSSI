// DimensionSelector.h
// author: Ionut Damian <damian@hcm-lab.de>
// created: 2013/03/19
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

#ifndef SSI_DIMENSION_SELECTOR_H
#define SSI_DIMENSION_SELECTOR_H

#include "base/IFilter.h"
#include "ioput/option/OptionList.h"

namespace ssi {

class DimensionSelector : public IFilter {

public:

class Options : public OptionList {

	public:

		Options () {

			dimensions[0] = '\0';
			addOption ("dimensions", dimensions, SSI_MAX_CHAR, SSI_CHAR, "indices of selected dimensions");
		};

		void setDimension (ssi_size_t index) {
			setDimensions (1, &index);
		}

		void setDimensions (ssi_size_t n_inds, ssi_size_t *inds) {
			dimensions[0] = '\0';
			if (n_inds > 0) {
				ssi_char_t s[SSI_MAX_CHAR];
				ssi_sprint (s, "%u", inds[0]);
				strcat (dimensions, s);
				for (ssi_size_t i = 1; i < n_inds; i++) {
					ssi_sprint (s, ",%u", inds[i]);
					strcat (dimensions, s);
				}
			}
		}

		ssi_char_t dimensions[SSI_MAX_CHAR];
	};

public:
	static const ssi_char_t *GetCreateName () { return "DimensionSelector"; };
	static IObject *Create (const ssi_char_t *file) { return new DimensionSelector (file); };
	~DimensionSelector ();
	DimensionSelector::Options *getOptions () { return &_options; };
	const ssi_char_t *getName () { return GetCreateName (); };
	const ssi_char_t *getInfo () { return "selects specific dimensions from a stream"; };

	void transform_enter (ssi_stream_t &stream_in,
		ssi_stream_t &stream_out,
		ssi_size_t xtra_stream_in_num = 0,
		ssi_stream_t xtra_stream_in[] = 0);
	void transform (ITransformer::info info,
		ssi_stream_t &stream_in,
		ssi_stream_t &stream_out,
		ssi_size_t xtra_stream_in_num = 0,
		ssi_stream_t xtra_stream_in[] = 0);
	void transform_flush (ssi_stream_t &stream_in,
		ssi_stream_t &stream_out,
		ssi_size_t xtra_stream_in_num = 0,
		ssi_stream_t xtra_stream_in[] = 0);

	ssi_size_t getSampleDimensionOut (ssi_size_t sample_dimension_in) { 
		
		_max_selected = sample_dimension_in;

		if (_options.dimensions[0] != '\0') {
			releaseSelection ();
			parseSelection ();
		}

		return _n_selected;
	};
	ssi_size_t getSampleBytesOut (ssi_size_t sample_bytes_in) { 
		
		_bytes = sample_bytes_in;
		return sample_bytes_in; 
	}
	ssi_type_t getSampleTypeOut (ssi_type_t sample_type_in) {

		_type = sample_type_in;
		return sample_type_in;
	}

protected:

	DimensionSelector (const ssi_char_t *file = 0);
	DimensionSelector::Options _options;
	ssi_char_t *_file;

	bool* _selector;
	ssi_size_t _n_selected;
	ssi_size_t _max_selected;
	ssi_type_t _type;
	ssi_size_t _bytes;

	void parseSelection ();
	void releaseSelection ();
	void select(ssi_size_t dimension);
};

}

#endif
