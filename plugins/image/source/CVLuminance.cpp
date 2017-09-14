// CVLuminance.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/07/29
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

#include "CVLuminance.h"
#include "ssiocv.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

namespace ssi {

ssi_char_t *CVLuminance::ssi_log_name = "ocvlumina_";

CVLuminance::CVLuminance (const ssi_char_t *file)
	: _file (0) {

	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}
}

CVLuminance::~CVLuminance () {

	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}
}

void CVLuminance::transform (ssi_time_t frame_rate,
                             IplImage *image_in,
                             ssi_stream_t &stream_out,
                             ssi_size_t xtra_stream_in_num,
                             ssi_stream_t xtra_stream_in[]) {


   double result=0;
   IplImage* image_out2=cvCreateImage(cvSize(image_in->width, image_in->height),image_in->depth, 1);
   CvMat image_inMat = cv::cvarrToMat(image_in);
   CvMat image_outMat= cv::cvarrToMat(image_out2);

   cvCvtColor((CvArr*)&image_inMat, (CvArr*)&image_outMat, CV_BGR2GRAY);
   image_out= cvCreateImage(cvSize(image_inMat.cols,image_inMat.rows),8,1);
    ssi_real_t *out = ssi_pcast (ssi_real_t, stream_out.ptr);

   for(int i=0; i< image_out->height*image_out->width; i++)
   {
       //result= result+ *(uint8_t*)(image_out->imageData+i);
       result= result+ *(image_outMat.data.ptr+i);
   }

   result= result /(image_out->height*image_out->width);


   *out=result;

   cvReleaseImage(&image_out);
   cvReleaseImage(&image_out2);

};

void CVLuminance::setFormat (ssi_video_params_t format) {

    ICVFeature::setFormatIn (format);
    image_out = cvCreateImageHeader (cvSize (format.widthInPixels, format.heightInPixels), format.depthInBitsPerChannel, format.numOfChannels);

}

}
