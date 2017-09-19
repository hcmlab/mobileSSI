// FFTWfeat.cpp
// author: Tobias Watzka, Christian Dins
// created: 2017/04/04
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

#include "FFTWfeat.h"	
#include "fftw3.h"



namespace ssi {

ssi_char_t *FFTWfeat::ssi_log_name = "FFTWfeat___";

FFTWfeat::FFTWfeat (const ssi_char_t *file) 
	:_fft_out (0),
    _fft_in (0)
     {

	if (file) {
        if (!OptionList::LoadXML (file, &_options)) {
            OptionList::SaveXML (file, &_options);
		}
		_file = ssi_strcpy (file);
	}

	this->rfft = ((_options.nfft >> 1) + 1);
}

FFTWfeat::~FFTWfeat() {
}

SSI_INLINE void FFTWfeat::init_fft () {

	//_fft = new FFT (_fft_size, 1);
	_fft_out = new fftw_complex *[_fft_dim];
	_fft_in = new fftw_complex *[_fft_dim];
	for(ssi_size_t i = 0; i < _fft_dim; i++){
		_fft_in[i]= new fftw_complex[_fft_size];
		_fft_out[i] = new fftw_complex[_options.nfft];
	}
	this->plan = fftw_plan_dft_1d(_options.nfft, _fft_in[0], _fft_out[0], FFTW_FORWARD, FFTW_ESTIMATE);
}

SSI_INLINE void FFTWfeat::release_fft () {

	//delete _fft; _fft = 0;
	for(ssi_size_t i = 0; i < _fft_dim; i++){
		delete[] _fft_in[i];
		delete[] _fft_out[i];
	}	
	delete[] _fft_in; _fft_in = 0;
	delete[] _fft_out; _fft_out = 0;
}

void FFTWfeat::transform_enter (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]){

	_fft_dim = stream_in.dim;	
	_fft_size = _options.nfft;	
	init_fft();
    //_win_type = _options.wintype;
    //_win_size = _fft_size;

	/*
	if (_win_type != WINDOW_TYPE_RECTANGLE) {
		_window = FilterTools::Window (_win_size, _win_type, MATRIX_DIMENSION_COL);
	}
	*/
}

void FFTWfeat::transform (ITransformer::info info,
	ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]) {

	// double casten? ssi_time_t
	ssi_real_t *ptr_in;
	int16_t *ptr_in16;
	if(scaled){
	 ptr_in = ssi_pcast (ssi_real_t, stream_in.ptr);
	}
	else{
	 ptr_in16 = ssi_pcast (int16_t, stream_in.ptr);
	}

	ssi_real_t *ptr_out = ssi_pcast (ssi_real_t ,stream_out.ptr);



	for (ssi_size_t j = 0; j < _fft_size; j++) {
		for (ssi_size_t i = 0; i < _fft_dim; i++) {
			if (j < stream_in.num){
				if(scaled){
				_fft_in[i][j][0] = *ptr_in * 32768.0; //float to pcm
				}else{
				_fft_in[i][j][0] = *ptr_in16 * 1.0f; //pcm to float
				}
				_fft_in[i][j][1] = 0;

				if(scaled)ptr_in++;
				else ptr_in16++;
			} else {
				_fft_in[i][j][0] = 0;
				_fft_in[i][j][1] = 0;
			}			
		}
	}

	for (ssi_size_t i = 0; i < _fft_dim; i++) {
	
		for (ssi_size_t j = 0; j < _fft_size; j++) {		
			_fft_in[i][j][0] *= (0.5 * (1.0 - cos(2.0 * NUMPY_PI * (double) j / (double) (stream_in.num - 1))));;
		}
	
		fftw_execute(plan);
		//_fft->transform (_fft_size,_fft_in[i], _fft_out[i]);
	}


	for (ssi_size_t j = 0; j < rfft; j++){
		for (ssi_size_t i = 0; i < _fft_dim; i++){
			
			// normalize ortho
			_fft_out[i][j][0] *= 1.0 / sqrt(stream_in.num);

			*ptr_out = (float)_fft_out[i][j][0];
			ptr_out++;
		}
	}
	
}


void FFTWfeat::transform_flush (ssi_stream_t &stream_in,
	ssi_stream_t &stream_out,
	ssi_size_t xtra_stream_in_num,
	ssi_stream_t xtra_stream_in[]){

	//release_fft ();
    //delete _window; _window = 0;
    //_win_size = 0;
}

}
