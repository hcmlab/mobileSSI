// CVLuminance.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/05/27
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

#ifndef SSI_IMAGE_CVLuminance_H
#define SSI_IMAGE_CVLuminance_H

#include "ICVFeature.h"
#include "ioput/option/OptionList.h"



namespace ssi {

class CVLuminance : public ICVFeature {

public:



public:

	class Options : public OptionList {

	public:

                Options () : code (1) {

                        //addOption ("code", &code, 1, SSI_INT, "color code (e.g. 2=BGR2RGB, 6=RGB2GRAY, 37=RGB2YCrCb, 41=RGB2HSV, 45=RGB2Lab, 51=RGB2Luv)");
		};

                int code;
	};

public:

        static const ssi_char_t *GetCreateName () { return "CVLuminance"; };
        static IObject *Create (const ssi_char_t *file) { return new CVLuminance (file); };
        ~CVLuminance ();

	Options *getOptions () { return &_options; };
	const ssi_char_t *getName () { return GetCreateName (); };
        const ssi_char_t *getInfo () { return "calculates the mean luminance of a frame"; };

	void setFormat (ssi_video_params_t format);
        void transform (ssi_time_t frame_rate,
                        IplImage *image_in,
                        ssi_stream_t &stream_out,
                        ssi_size_t xtra_stream_in_num,
                        ssi_stream_t xtra_stream_in[]);

        ssi_size_t getSampleDimensionOut(ssi_size_t sample_dimension_in) {
                return 1;
        }

        ssi_size_t getSampleBytesOut(ssi_size_t sample_bytes_in) {
                return sizeof(SSI_REAL);
        }

        ssi_type_t getSampleTypeOut(ssi_type_t sample_type_in) {
                return SSI_REAL;
        }

protected:

        CVLuminance (const ssi_char_t *file);
	Options _options;
	ssi_char_t *_file;
        IplImage* image_out;

	static ssi_char_t *ssi_log_name;
};

}

#endif
