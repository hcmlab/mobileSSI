// ISAlignStrms.h
// author: Florian Lingenfelser <florian.lingenfelser@informatik.uni-augsburg.de>
// created: 2010/05/19
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

#ifndef SSI_MODEL_ISALIGNSTRMS_H
#define SSI_MODEL_ISALIGNSTRMS_H

#include "base/ISamples.h"

namespace ssi {

class ISAlignStrms : public ISamples {

public:

	explicit ISAlignStrms (ISamples *samples);
	~ISAlignStrms ();

	void reset () { _samples.reset (); };//
	ssi_sample_t *get (ssi_size_t index);//
	ssi_sample_t *next (); //

	ssi_size_t getSize () { return _samples.getSize (); };//
	ssi_size_t getSize (ssi_size_t class_id) { return _samples.getSize (class_id); };//

	ssi_size_t getClassSize () { return _samples.getClassSize (); };//
	const ssi_char_t *getClassName (ssi_size_t class_id) { return _samples.getClassName (class_id); };//
	
	ssi_size_t getUserSize () { return _samples.getUserSize (); };//
	const ssi_char_t *getUserName (ssi_size_t user_id) { return _samples.getUserName (user_id); };//

	ssi_size_t getStreamSize () { return 1; };//
	ssi_stream_t getStream (ssi_size_t index) { 		
		return _stream_ref; 
	};

	bool hasMissingData () { return _samples.hasMissingData (); };
	bool supportsShallowCopy () { return false; };

protected:

	ISAlignStrms (ISAlignStrms const &samples);
	void align ();

	ISamples &_samples;
	ssi_sample_t _sample_in, _sample_out;
	ssi_stream_t _stream;
	ssi_stream_t _stream_ref;
	ssi_size_t _n_streams;
	ssi_size_t _dim;
};

}

#endif
