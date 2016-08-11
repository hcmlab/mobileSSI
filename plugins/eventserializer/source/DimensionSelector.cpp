// DimensionSelector.cpp
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

#include "DimensionSelector.h"

namespace ssi {

DimensionSelector::DimensionSelector (const ssi_char_t *file) 
	: _file (0),
	_n_selected (0),
	_max_selected(0),
	_selector(0) {

	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}
}

DimensionSelector::~DimensionSelector () {

	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}
}

void DimensionSelector::transform_enter (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {		

	for (ssi_size_t i = 0; i < _max_selected; i++) {
		_selector[i] = false;
	}

	if (_options.dimensions[0] != '\0') {
		releaseSelection ();
		parseSelection ();
	}
}

void DimensionSelector::transform (ITransformer::info info,
	ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	ssi_byte_t *inptr = ssi_pcast (ssi_byte_t , stream_in.ptr);
	ssi_byte_t *outptr = ssi_pcast (ssi_byte_t, stream_out.ptr);

	for (ssi_size_t i = 0; i < stream_in.num; i++) {	

		ssi_size_t cur_selected = 0;
		for (ssi_size_t a = 0; a < _max_selected; a++) {
			if(_selector[a])
			{
				memcpy(outptr, inptr, sizeof(_bytes));
				outptr += _bytes;

				++cur_selected;

				//stop searching if we found all needed values
				if(cur_selected >= _n_selected)
					break;
			}

			//go to next input
			inptr += _bytes;
		}
	}
}

void DimensionSelector::transform_flush (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	releaseSelection ();
}

void DimensionSelector::parseSelection () {

	//initialize memory
	_selector = new bool[_max_selected];
	for(ssi_size_t i=0; i < _max_selected; ++i)
		_selector[i] = false;

	ssi_size_t n_dims = 0;
	int *dimensions = ssi_parse_indices (_options.dimensions, n_dims);
	for (ssi_size_t i = 0; i < n_dims; i++) {
		select(dimensions[i]);
	}
	delete[] dimensions;
}

void DimensionSelector::releaseSelection () 
{
	if(_selector != 0)
	{
		for (ssi_size_t a = 0; a < _max_selected; a++) {
			_selector[a] = false;
		}

		delete[] _selector;
		_selector = 0;
	}
	
	_n_selected = 0;
}

void DimensionSelector::select(ssi_size_t dimension)
{
	_selector[dimension] = true;
	++_n_selected;
};

}
