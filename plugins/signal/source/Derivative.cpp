// Derivative.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/01/03
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

#include "Derivative.h"
#include "signal/MatrixOps.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

const ssi_size_t IDerivative::FORMAT_SIZE = 5;
const ssi_char_t *IDerivative::FORMAT_NAMES[FORMAT_SIZE] = {"0th","1st","2nd","3rd","4th"};

ssi_size_t IDerivative::CountSetBits (ssi_bitmask_t format) {

	ssi_size_t count = 0;

    while (format) {
		count++ ;
        format &= (format - 1) ;
	}

	return count;
}

int IDerivative::FindHighestOrderBit (ssi_bitmask_t format) {

	int position = -1;

	while (format) {
		format = format >> 1;
		position++;
	}

	return position;
}

ssi_bitmask_t IDerivative::Names2Format (const ssi_char_t *names) {

	if (!names || names[0] == '\0') {
		return ALL;
	}

	ssi_char_t string[SSI_MAX_CHAR];
	ssi_bitmask_t format = NONE;

	char *pch;
	strcpy (string, names);
	pch = strtok (string, ", ");
	while (pch != NULL) {
		format = format | Name2Format (pch);
		pch = strtok (NULL, ", ");
	}

	return format;
}

ssi_bitmask_t IDerivative::Name2Format (const ssi_char_t *name) {

	ssi_bitmask_t format = NONE;

	for (ssi_size_t i = 0; i < FORMAT_SIZE; i++) {
		if (strcmp (name, FORMAT_NAMES[i]) == 0) {
#if __MINGW32__|__gnu_linux__
            format = ((uint64_t) 1) << i;
#else
			format = 1i64 << i;
#endif
			break;
		}
	}

	return format;
}

void Derivative::Options::set (ssi_bitmask_t format) {
	names[0] = '\0';
	for (ssi_bitmask_t i = 0; i < FORMAT_SIZE; i++) {
#if __MINGW32__|__gnu_linux__
        if (format & (((uint64_t) 1) << i)) {
 #else
  		if (format & (1i64 << i)) {
 #endif

			strcat (names, FORMAT_NAMES[i]);
			strcat (names, ",");
		}
	}
	if (names[strlen(names)-1] == ',') {
		names[strlen(names)-1] = '\0';
	}
}

Derivative::Derivative (const ssi_char_t *file)
	: _format (0),
	_depth (0),
	_history (0),
	_file (0) {

	if (file) {
		if (!OptionList::LoadXML(file, &_options)) {
			OptionList::SaveXML(file, &_options);
		}
		_file = ssi_strcpy (file);
	}
}

Derivative::~Derivative () {

	if (_file) {
		OptionList::SaveXML(_file, &_options);
		delete[] _file;
	}
}

void Derivative::transform_enter (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	_format = Names2Format (_options.names);
	_depth = FindHighestOrderBit (_format) + 1;

	_store_value = new char[_depth];
	unsigned int n = 1;
	for (unsigned int i = 0; i < _depth; i++) {
		_store_value[i] = _format & n ? 1 : 0;
		n = n << 1;
	}

	ssi_size_t sample_dimension = stream_in.dim;

	_history = new ssi_real_t[(_depth-1) * sample_dimension];
	_first_call = true;
}

void Derivative::transform (ITransformer::info info,
	ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	ssi_size_t sample_dimension = stream_in.dim;
	ssi_size_t sample_number = stream_in.num;

	ssi_real_t *srcptr = ssi_pcast (ssi_real_t, stream_in.ptr);
	ssi_real_t *dstptr = ssi_pcast (ssi_real_t, stream_out.ptr);
	ssi_real_t *histptr;
	ssi_real_t tmp, tmp2;

	// initialize _history during first call
	if (_first_call) {
		for (ssi_size_t i = 0; i < sample_dimension; ++i) {
			for (ssi_size_t j = 0; j < _depth-1; j++) {
				_history[i*(_depth-1) + j] = j == 0 ? srcptr[i] : 0;
			}
		}
		_first_call = false;
	}

	// calculate derivative
	for (ssi_size_t i = 0; i < sample_number; i++) {
		histptr = _history;
		for (ssi_size_t j = 0; j < sample_dimension; j++) {
			tmp = *srcptr++;
			if (_store_value[0]) {
				*dstptr++ = tmp;
			}
			for (ssi_size_t k = 1; k < _depth; k++) {
				tmp2 = tmp;
				tmp -= *histptr;
				*histptr++ = tmp2;
				if (_store_value[k]) {
					*dstptr++ = tmp;
				}
			}
		}
	}
}

void Derivative::transform_flush (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num ,
	ssi_stream_t xtra_stream_in[]) {

	delete[] _history; _history = 0;
	delete[] _store_value; _store_value = 0;
}

const ssi_char_t *Derivative::getName (ssi_size_t index) {

	ssi_bitmask_t format = Names2Format (_options.names);

	for (ssi_size_t i = 0; i < FORMAT_SIZE; i++) {

        #if __MINGW32__|__gnu_linux__
		if (format & (((uint64_t)1) << i)) {
		#else
        if (format & (1i64 << i)) {
		#endif
			if (index-- == 0) {
				return FORMAT_NAMES[i];
			}
		}
	}

	return 0;
}

}
