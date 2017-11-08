// Searchfeatures.cpp
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

#include "SearchFeaturesHead.h"

namespace ssi {

	char SearchFeaturesHead::ssi_log_name[] = "SearchFeaturesHead";

	SearchFeaturesHead::SearchFeaturesHead(const ssi_char_t *file) :
    _file(0),
    _initEnergyNorm(false),
    _normalizeEnergy(1.0),
    _initEntropyNorm(false),
    _normalizeEntropy(1.0)
    {

		if (file) {
            if (!OptionList::LoadXML(file, &_options)) {
                OptionList::SaveXML(file, &_options);
			}
			_file = ssi_strcpy(file);
		}
	}

	SearchFeaturesHead::~SearchFeaturesHead() {

		if (_file) {
            OptionList::SaveXML(_file, &_options);
			delete[] _file;
		}
	}

	void SearchFeaturesHead::transform_enter(ssi_stream_t &stream_in,
		ssi_stream_t &stream_out,
		ssi_size_t xtra_stream_in_num,
		ssi_stream_t xtra_stream_in[]) {

	}

	void SearchFeaturesHead::transform(ITransformer::info info,
		ssi_stream_t &stream_in,
		ssi_stream_t &stream_out,
		ssi_size_t xtra_stream_in_num,
		ssi_stream_t xtra_stream_in[]) {

        ssi_size_t sample_dimension = stream_in.dim;
        ssi_size_t sample_number = info.frame_num + info.delta_num; // stream_in.num

        ssi_real_t *ptr_in = ssi_pcast(ssi_real_t, stream_in.ptr);
        ssi_real_t *ptr_out = ssi_pcast(ssi_real_t, stream_out.ptr);

        std::vector<float> xValues = getValues(ptr_in, sample_number, sample_dimension,
                                               SEARCHFEATURES_HEAD_DIM_ACC_X);
        std::vector<float> yValues = getValues(ptr_in, sample_number, sample_dimension,
                                               SEARCHFEATURES_HEAD_DIM_ACC_Y);
        std::vector<float> zValues = getValues(ptr_in, sample_number, sample_dimension,
                                               SEARCHFEATURES_HEAD_DIM_ACC_Z);

        std::vector<float> xValuesNorm(xValues.size(), 1.0);
        std::vector<float> yValuesNorm(yValues.size(), 1.0);
        std::vector<float> zValuesNorm(zValues.size(), 1.0);

        if (_options.normalize) {
            for (int iter = 0; iter < sample_number; iter++) {
                xValuesNorm[iter] = xValues[iter] / _options.mean_dev_norm;
                yValuesNorm[iter] = yValues[iter] / _options.mean_dev_norm;
                zValuesNorm[iter] = zValues[iter] / _options.mean_dev_norm;
            }

            if(!_initEnergyNorm){
                std::vector<float> normTmp(sample_number, 1); //Auf uhr testen

                _normalizeEnergy = getEnergy(normTmp);
                _initEnergyNorm = true;
            }
        }

        if (_options.meanX) {
            if (_options.normalize) {
                *ptr_out = static_cast<ssi_real_t>(getMean(xValuesNorm));
                ptr_out++;
            } else {
                *ptr_out = static_cast<ssi_real_t>(getMean(xValues));
                ptr_out++;
            }
        }

        if (_options.meanY) {
            if (_options.normalize) {
                *ptr_out = static_cast<ssi_real_t>(getMean(yValuesNorm));
            } else {
                *ptr_out = static_cast<ssi_real_t>(getMean(yValues));
            }
             ptr_out++;
        }

        if (_options.meanZ) {
            if (_options.normalize) {
                *ptr_out = static_cast<ssi_real_t>(getMean(zValuesNorm));
            } else {
                *ptr_out = static_cast<ssi_real_t>(getMean(zValues));
            }
             ptr_out++;
        }

        if (_options.stdDeviationX) {
            if (_options.normalize) {
                *ptr_out = static_cast<ssi_real_t>(getStdDeviation(xValuesNorm));
            } else {
                *ptr_out = static_cast<ssi_real_t>(getStdDeviation(xValues));
            }
             ptr_out++;
        }

		if (_options.stdDeviationY)
		{
            if (_options.normalize) {
                *ptr_out = static_cast<ssi_real_t>(getStdDeviation(yValuesNorm));
            } else {
                *ptr_out = static_cast<ssi_real_t>(getStdDeviation(yValues));
            }
             ptr_out++;
		}

		if (_options.stdDeviationZ)
		{
            if (_options.normalize) {
                *ptr_out = static_cast<ssi_real_t>(getStdDeviation(zValuesNorm));
            } else {
                *ptr_out = static_cast<ssi_real_t>(getStdDeviation(zValues));
            }
             ptr_out++;
		}

		if (_options.energyX)
		{
            if(_options.normalize) {
                *ptr_out = static_cast<ssi_real_t>(getEnergy(xValuesNorm)/_normalizeEnergy) ;
            } else {
                *ptr_out = static_cast<ssi_real_t>(getEnergy(xValues));
            }
             ptr_out++;
		}

		if (_options.energyY)
		{
            if(_options.normalize) {
                *ptr_out = static_cast<ssi_real_t>(getEnergy(yValuesNorm)/_normalizeEnergy) ;
            } else {
                *ptr_out = static_cast<ssi_real_t>(getEnergy(yValues));
            }
             ptr_out++;
		}

		if (_options.energyZ)
		{
            if(_options.normalize) {
                *ptr_out = static_cast<ssi_real_t>(getEnergy(zValuesNorm)/_normalizeEnergy) ;
            } else {
                *ptr_out = static_cast<ssi_real_t>(getEnergy(zValues));
            }
             ptr_out++;
		}

		if (_options.correlationXY){

            if(_options.normalize) {
                *ptr_out = static_cast<ssi_real_t>(getCorrelation(xValuesNorm, yValuesNorm));
            } else {
                *ptr_out = static_cast<ssi_real_t>(getCorrelation(xValues, yValues));
            }
             ptr_out++;
		}

		if (_options.correlationXZ)
		{
            if(_options.normalize) {
                *ptr_out = static_cast<ssi_real_t>(getCorrelation(xValuesNorm, zValuesNorm));
            } else {
                *ptr_out = static_cast<ssi_real_t>(getCorrelation(xValues, zValues));
            }
             ptr_out++;
		}

		if (_options.correlationYZ) {
            if (_options.normalize) {
                *ptr_out = static_cast<ssi_real_t>(getCorrelation(yValuesNorm, zValuesNorm));
            } else {
                *ptr_out = static_cast<ssi_real_t>(getCorrelation(yValues, zValues));
            }
             ptr_out++;
        }

		if (_options.displacementX)
		{
            *ptr_out = static_cast<ssi_real_t>(getDisplacement(xValues, stream_in.sr));
         ptr_out++;
        }

		if (_options.displacementY)
		{
            *ptr_out = static_cast<ssi_real_t>(getDisplacement(yValues, stream_in.sr));
         ptr_out++;
        }

		if (_options.displacementZ)
		{
            *ptr_out = static_cast<ssi_real_t>(getDisplacement(zValues, stream_in.sr));
           ptr_out++;
        }

		if (_options.entropyX)
		{
            *ptr_out = static_cast<ssi_real_t>(getFrequencyDomainEntropy(xValues));
             ptr_out++;
        }

		if (_options.entropyY)
		{
            *ptr_out = static_cast<ssi_real_t>(getFrequencyDomainEntropy(yValues));
             ptr_out++;
        }

		if (_options.entropyZ)
		{
            *ptr_out = static_cast<ssi_real_t>(getFrequencyDomainEntropy(zValues));
             ptr_out++;
        }

		if (_options.skewX)
		{
            *ptr_out = static_cast<ssi_real_t>(getSkew(xValues));
             ptr_out++;
        }

		if (_options.skewY)
		{
            *ptr_out = static_cast<ssi_real_t>(getSkew(yValues));
             ptr_out++;
        }

		if (_options.skewZ)
		{
            *ptr_out = static_cast<ssi_real_t>(getSkew(zValues));
             ptr_out++;
        }

		if (_options.kurtosisX)
		{
            *ptr_out = static_cast<ssi_real_t>(getKurtosis(xValues));
             ptr_out++;
        }

		if (_options.kurtosisY)
		{
            *ptr_out = static_cast<ssi_real_t>(getKurtosis(yValues));
             ptr_out++;
        }

		if (_options.kurtosisZ)
		{
            *ptr_out = static_cast<ssi_real_t>(getKurtosis(zValues));
             ptr_out++;
        }

		if (_options.iqrX)
		{
            if(_options.normalize){
                *ptr_out = static_cast<ssi_real_t>(getIQR(xValuesNorm));
            } else {
                *ptr_out = static_cast<ssi_real_t>(getIQR(xValues));
            }
             ptr_out++;
        }

		if (_options.iqrY)
		{
            if(_options.normalize){
                *ptr_out = static_cast<ssi_real_t>(getIQR(yValuesNorm));
            } else {
                *ptr_out = static_cast<ssi_real_t>(getIQR(yValues));
            }
             ptr_out++;
		}

		if (_options.iqrZ)
		{
            if(_options.normalize){
                *ptr_out = static_cast<ssi_real_t>(getIQR(zValuesNorm));
            } else {
                *ptr_out = static_cast<ssi_real_t>(getIQR(zValues));
            }
             ptr_out++;
		}

		if (_options.madX)
		{
            if(_options.normalize){
                *ptr_out = static_cast<ssi_real_t>(getMAD(xValuesNorm));
            } else {
                *ptr_out = static_cast<ssi_real_t>(getMAD(xValues));
            }
             ptr_out++;
        }

		if (_options.madY)
		{
            if(_options.normalize){
                *ptr_out = static_cast<ssi_real_t>(getMAD(yValuesNorm));
            } else {
                *ptr_out = static_cast<ssi_real_t>(getMAD(yValues));
            }
             ptr_out++;
		}

		if (_options.madZ)
		{
            if(_options.normalize){
                *ptr_out = static_cast<ssi_real_t>(getMAD(zValuesNorm));
            } else {
                *ptr_out = static_cast<ssi_real_t>(getMAD(zValues));
            }
             ptr_out++;
		}

		if (_options.rmsX)
		{

            *ptr_out = static_cast<ssi_real_t>(getRMS(xValues));
             ptr_out++;
        }


		if (_options.rmsY)
		{
            *ptr_out = static_cast<ssi_real_t>(getRMS(yValues));
             ptr_out++;
        }

		if (_options.rmsZ)
		{
            *ptr_out = static_cast<ssi_real_t>(getRMS(zValues));
             ptr_out++;
        }

		if (_options.varianceX)
		{
            *ptr_out = static_cast<ssi_real_t>(getVariance(xValues));
             ptr_out++;
        }

		if (_options.varianceY)
		{
            *ptr_out = static_cast<ssi_real_t>(getVariance(yValues));
             ptr_out++;
        }

		if (_options.varianceZ)
		{
            *ptr_out = static_cast<ssi_real_t>(getVariance(zValues));
             ptr_out++;
        }

		if (_options.signalMagnitudeArea)
		{
            *ptr_out = static_cast<ssi_real_t>(getSignalMagnitudeArea(xValues, yValues, zValues));
             ptr_out++;
        }

		if (_options.haarFilterX)
		{
            *ptr_out= static_cast<ssi_real_t>(getHaarFilter(xValues));
             ptr_out++;
        }

		if (_options.haarFilterY)
		{
            *ptr_out = static_cast<ssi_real_t>(getHaarFilter(yValues));
             ptr_out++;
        }

		if (_options.haarFilterZ)
		{
            *ptr_out = static_cast<ssi_real_t>(getHaarFilter(zValues));
             ptr_out++;
        }

		if (_options.haarFilterBiaxialXY)
		{
            *ptr_out = static_cast<ssi_real_t>(getHaarFilterBiaxial(xValues, yValues));
             ptr_out++;
        }

		if (_options.haarFilterBiaxialYZ)
		{
            *ptr_out = static_cast<ssi_real_t>(getHaarFilterBiaxial(yValues, zValues));
             ptr_out++;
		}

		if (_options.haarFilterBiaxialZX)
		{
            *ptr_out = static_cast<ssi_real_t>(getHaarFilterBiaxial(zValues, xValues));
             ptr_out++;
        }

		if (_options.crestX)
		{
            *ptr_out = static_cast<ssi_real_t>(getCrest(xValues));
             ptr_out++;
        }

		if (_options.crestY)
		{
            *ptr_out = static_cast<ssi_real_t>(getCrest(yValues));
             ptr_out++;
        }

		if (_options.crestZ)
		{
            *ptr_out = static_cast<ssi_real_t>(getCrest(zValues));
             ptr_out++;
        }

		if (_options.spectralFluxX)
		{
            *ptr_out = static_cast<ssi_real_t>(getSpectralFlux(xValues));
             ptr_out++;
        }

		if (_options.spectralFluxY)
		{
            *ptr_out = static_cast<ssi_real_t>(getSpectralFlux(yValues));
             ptr_out++;
        }

		if (_options.spectralFluxZ)
		{
            *ptr_out = static_cast<ssi_real_t>(getSpectralFlux(zValues));
             ptr_out++;
        }

		if (_options.spectralCentroidX)
		{
            *ptr_out = static_cast<ssi_real_t>(getSpectralCentroid(xValues));
             ptr_out++;
        }

		if (_options.spectralCentroidY)
		{
            *ptr_out = static_cast<ssi_real_t>(getSpectralCentroid(yValues));
             ptr_out++;
        }

		if (_options.spectralCentroidZ)
		{
            *ptr_out = static_cast<ssi_real_t>(getSpectralCentroid(zValues));
             ptr_out++;
        }

		if (_options.spectralRolloffX)
		{
            *ptr_out = static_cast<ssi_real_t>(getSpectralRolloff(xValues));
             ptr_out++;
        }

		if (_options.spectralRolloffY)
		{
            *ptr_out = static_cast<ssi_real_t>(getSpectralRolloff(yValues));
             ptr_out++;
        }

		if (_options.spectralRolloffZ)
		{
            *ptr_out = static_cast<ssi_real_t>(getSpectralRolloff(zValues));
             ptr_out++;
        }

		if (_options.dctX)
		{
			for (int k = 0; k < SEARCHFEATURES_HEAD_DCT_DOEFFICIENTS; k++)
			{
                *ptr_out = static_cast<ssi_real_t>(getDiscreteCosineTransform(xValues, k));
                 ptr_out++;
			}
		}

		if (_options.dctY)
		{
			for (int k = 0; k < SEARCHFEATURES_HEAD_DCT_DOEFFICIENTS; k++)
			{
                *ptr_out = static_cast<ssi_real_t>(getDiscreteCosineTransform(yValues, k));
                 ptr_out++;
            }
		}

		if (_options.dctZ)
		{
			for (int k = 0; k < SEARCHFEATURES_HEAD_DCT_DOEFFICIENTS; k++)
			{
                *ptr_out = static_cast<ssi_real_t>(getDiscreteCosineTransform(zValues, k));
                 ptr_out++;
            }
		}
        if(_options.maxX)
        {
             if(_options.normalize){
                 *ptr_out = static_cast<ssi_real_t>(getMax(xValuesNorm));
             }else
             {
                *ptr_out = static_cast<ssi_real_t>(getMax(xValues));
             }
              ptr_out++;
        }
        if(_options.minX)
        {
            if(_options.normalize){
            *ptr_out = static_cast<ssi_real_t>(getMin(xValuesNorm));
           }
            else
           {
            *ptr_out = static_cast<ssi_real_t>(getMin(xValues));
           }
             ptr_out++;
        }
        if(_options.medX)
        {
            if(_options.normalize){
                float median=getMedian(xValuesNorm);
                 *ptr_out = static_cast<ssi_real_t>(median);
            }
             else
            {
                 *ptr_out = static_cast<ssi_real_t>(getMedian(xValues));
            }
             ptr_out++;
        }
        if(_options.fftSumX)
        {
           *ptr_out++ = static_cast<ssi_real_t>(getFFTSum(xValues));
        }
        if(_options.maxY)
        {
            if(_options.normalize){
            *ptr_out = static_cast<ssi_real_t>(getMax(yValuesNorm));
            }
             else
            {
                *ptr_out = static_cast<ssi_real_t>(getMax(yValues));
            }
             ptr_out++;
        }
        if(_options.minY)
        {
            if(_options.normalize){
            *ptr_out = static_cast<ssi_real_t>(getMin(yValuesNorm));
            }
             else
            {

                *ptr_out = static_cast<ssi_real_t>(getMin(yValues));
            }
             ptr_out++;
        }
        if(_options.medY)
        {
            if(_options.normalize){
            *ptr_out = static_cast<ssi_real_t>(getMedian(yValuesNorm));
            }
             else
            {

                *ptr_out = static_cast<ssi_real_t>(getMedian(yValues));
            }
             ptr_out++;
        }
        if(_options.fftSumY)
        {

            *ptr_out = static_cast<ssi_real_t>(getFFTSum(yValues));
         ptr_out++;
        }
        if(_options.maxZ)
        {
            if(_options.normalize){
            *ptr_out = static_cast<ssi_real_t>(getMax(zValuesNorm));
            }
             else
            {

                *ptr_out = static_cast<ssi_real_t>(getMax(zValues));
            }
             ptr_out++;
        }
        if(_options.minZ)
        {
            if(_options.normalize){
            *ptr_out = static_cast<ssi_real_t>(getMin(zValuesNorm));
            }
             else
            {

                *ptr_out = static_cast<ssi_real_t>(getMax(zValues));
            }
             ptr_out++;
        }
        if(_options.medZ)
        {
            if(_options.normalize){
            *ptr_out = static_cast<ssi_real_t>(getMedian(zValuesNorm));
            }
             else
            {

                *ptr_out = static_cast<ssi_real_t>(getMax(zValues));
            }
             ptr_out++;
        }
        if(_options.fftSumZ)
        {
            if(_options.normalize){
            *ptr_out = static_cast<ssi_real_t>(getFFTSum(zValuesNorm));
            }
             else
            {

                *ptr_out = static_cast<ssi_real_t>(getMax(zValues));
            }
             ptr_out++;
        }
        if(_options.magMean)
        {
            if(_options.normalize){

                float maxMag=sqrt(3*_options.mean_dev_norm*_options.mean_dev_norm);
                *ptr_out = static_cast<ssi_real_t>(getMean(getMagnitudes(xValues, yValues, zValues)))/maxMag;
            }
             else
            {

                *ptr_out = static_cast<ssi_real_t>(getMean(getMagnitudes(xValues, yValues, zValues)));
            }
             ptr_out++;
        }
        if(_options.magStd)
        {
            if(_options.normalize){

                *ptr_out = static_cast<ssi_real_t>(getStdDeviation(getMagnitudes(xValuesNorm, yValuesNorm, zValuesNorm)));
            }
             else
            {

                *ptr_out = static_cast<ssi_real_t>(getStdDeviation(getMagnitudes(xValues, yValues, zValues)));
            }
             ptr_out++;
        }
        if(_options.magMax)
        {
            if(_options.normalize){

                *ptr_out = static_cast<ssi_real_t>(getMax(getMagnitudes(xValuesNorm, yValuesNorm, zValuesNorm)));
            }
             else
            {

                *ptr_out = static_cast<ssi_real_t>(getStdDeviation(getMagnitudes(xValues, yValues, zValues)));
            }
             ptr_out++;
        }
        if(_options.magMin)
        {
            if(_options.normalize){


            *ptr_out = static_cast<ssi_real_t>(getMin(getMagnitudes(xValuesNorm, yValuesNorm, zValuesNorm)));
            }
             else
            {
                *ptr_out = static_cast<ssi_real_t>(getMedian(getMagnitudes(xValues, yValues, zValues)));
            }
             ptr_out++;
        }
        if(_options.magSemiQuartile)
        {
            if(_options.normalize){

            *ptr_out = static_cast<ssi_real_t>(getIQR(getMagnitudes(xValuesNorm, yValuesNorm, zValuesNorm)));
            }
             else
            {
                *ptr_out = static_cast<ssi_real_t>(getMedian(getMagnitudes(xValues, yValues, zValues)));
            }
             ptr_out++;
        }
        if(_options.magMed)
        {
            if(_options.normalize){


            *ptr_out = static_cast<ssi_real_t>(getMedian(getMagnitudes(xValuesNorm, yValuesNorm, zValuesNorm)));
            }
             else
            {
                *ptr_out = static_cast<ssi_real_t>(getMedian(getMagnitudes(xValues, yValues, zValues)));
            }
             ptr_out++;
        }
        if(_options.magFftSum )
        {
            *ptr_out = static_cast<ssi_real_t>(getFFTSum(getMagnitudes(xValues, yValues, zValues)));
         ptr_out++;
        }


		xValues.clear();
		yValues.clear();
		zValues.clear();
	}

	void SearchFeaturesHead::transform_flush(ssi_stream_t &stream_in,
		ssi_stream_t &stream_out,
		ssi_size_t xtra_stream_in_num,
		ssi_stream_t xtra_stream_in[]) {

	}

	/*
	 * Mean acceleration value
	 *
	 * based on
	 * Bao, Ling et al. - Activity Recognition from User-Annotated Acceleration Data
	 * Ravi, N. et al. - Activity recognition from accelerometer data
	 * Huynh, T. et al. - Analyzing features for activity recognition
	 * Lara, Oscar D. et al. - A Survey on Human Activity Recognition using Wearable Sensors
	 * Chen, Yen-Ping et al. - Online classifier construction algorithm for human activity detection using a tri-axial accelerometer
	 * Hanai, Yuya et al. - Haar-Like Filtering for Human Activity Recognition Using 3D Accelerometer
	 */
	float SearchFeaturesHead::getMean(std::vector<float> values)
	{
		return MathTools::GetMean(values);
	}

	/*
	* Standard deviation for acceleration value
	*
	* based on
	* Bao, Ling et al. - Activity Recognition from User-Annotated Acceleration Data
	* Ravi, N. et al. - Activity recognition from accelerometer data
	* Huynh, T. et al. - Analyzing features for activity recognition
	* Lara, Oscar D. et al. - A Survey on Human Activity Recognition using Wearable Sensors
	* Chen, Yen-Ping et al. - Online classifier construction algorithm for human activity detection using a tri-axial accelerometer
	* Hanai, Yuya et al. - Haar-Like Filtering for Human Activity Recognition Using 3D Accelerometer
	*/
	float SearchFeaturesHead::getStdDeviation(std::vector<float> values)
	{
		return MathTools::GetStdDeviation(values);
	}

	/*
	* Energy for acceleration value
	*
	* based on
	* Bao, Ling et al. - Activity Recognition from User-Annotated Acceleration Data
	* Ravi, N. et al. - Activity recognition from accelerometer data
	* Huynh, T. et al. - Analyzing features for activity recognition
	* Lara, Oscar D. et al. - A Survey on Human Activity Recognition using Wearable Sensors
	* Chen, Yen-Ping et al. - Online classifier construction algorithm for human activity detection using a tri-axial accelerometer
	* Hanai, Yuya et al. - Haar-Like Filtering for Human Activity Recognition Using 3D Accelerometer
	*/
	float SearchFeaturesHead::getEnergy(std::vector<float> values)
	{
		float energy = 0;

		ssi_size_t sampleNumber = static_cast<ssi_size_t>(values.size());

		// Copy data for FFT
		ssi_real_t* dataCopy = new ssi_real_t[sampleNumber];
		for (ssi_size_t i = 0; i < sampleNumber; i++)
		{
			dataCopy[i] = values[i];
		}

		// FFT
		FFT fft(sampleNumber, 1);
		Matrix<ssi_real_t> fftInput(sampleNumber, 1, dataCopy);
		Matrix<ssi_real_t> fftOutput(1, fft.rfft);
		fft.transform(&fftInput, &fftOutput);

		// Calculate energy
		for (ssi_size_t i = 0; i < fft.rfft; i++)
		{
			energy += pow((double)fftOutput.data[i], (double)2);
		}

		energy = energy / static_cast<float>(fft.rfft);

		// delete dataCopy; // gets deleted in the matrix for some reason

		return energy;
	}

	/*
	* Correlation for acceleration value
	*
	* based on
	* Bao, Ling et al. - Activity Recognition from User-Annotated Acceleration Data
	* Ravi, N. et al. - Activity recognition from accelerometer data
	* Huynh, T. et al. - Analyzing features for activity recognition
	* Lara, Oscar D. et al. - A Survey on Human Activity Recognition using Wearable Sensors
	* Chen, Yen-Ping et al. - Online classifier construction algorithm for human activity detection using a tri-axial accelerometer
	* Hanai, Yuya et al. - Haar-Like Filtering for Human Activity Recognition Using 3D Accelerometer
	*/
	float SearchFeaturesHead::getCorrelation(std::vector<float> aValues, std::vector<float> bValues)
	{
		float correlation = 0;
		float covariance = 0;
		float meanA = getMean(aValues);
		float meanB = getMean(bValues);
		float stdDeviationA = getStdDeviation(aValues);
		float stdDeviationB = getStdDeviation(bValues);

		size_t sampleCount = aValues.size();
		for (int i = 0; i < sampleCount; i++)
		{
			covariance += ((aValues[i]) - meanA) * ((bValues[i]) - meanB);
		}

		covariance = covariance / static_cast<float>(sampleCount);

		if (stdDeviationA * stdDeviationB != 0)
		{
			correlation = covariance / (stdDeviationA * stdDeviationB);
		}

		return correlation;
	}

	/*
	 * Displacement for acceleration value in meter
	 */
	float SearchFeaturesHead::getDisplacement(std::vector<float> values, ssi_time_t sr)
	{
		float displacement = 0;
		float a = 0;
		float velNew = 0;
		float velOld = 0;

		ssi_size_t sampleNumber = static_cast<ssi_size_t>(values.size());

		// Time for which the object moves with that acceleration
		float t = static_cast<float>(1.0 / sr);

		// Sum up displacement steps
		for (ssi_size_t i = 0; i < sampleNumber; i++)
		{
#if __ANDROID_API__
            a = (fabs(values[i]) < 0.1) ? 0 : values[i];
#else
			a = (abs(values[i]) < 0.1) ? 0 : values[i];
#endif
			// v1 = a * t + v0
			velNew = a * t + velOld;

			// d = v0 * t + 0.5 * a * t^2  or  d = v0 * t + 0.5 * (v1 - v0) * t
			displacement += velOld * t + 0.5f * (velNew - velOld) * t;

			// Update v0
			velOld = velNew;
		}

		return displacement;
	}

	/*
	* Frequency domain entropy for acceleration value
	*
	* based on
	* Bao, Ling et al. - Activity Recognition from User-Annotated Acceleration Data
	* Huynh, T. et al. - Analyzing features for activity recognition
	* Lara, Oscar D. et al. - A Survey on Human Activity Recognition using Wearable Sensors
	* Khan, A. et al. - Accelerometer's position free human activity recognition using a hierarchical recognition model
	*
	* http://stackoverflow.com/questions/30418391/what-is-frequency-domain-entropy-in-fft-result-and-how-to-calculate-it
	* http://dsp.stackexchange.com/questions/23689/what-is-spectral-entropy
	*
	* Normalisation based on
	*
	* Zurlini, Giovanni et al. - Highlighting order and disorder in social--ecological landscapes to
	* foster adaptive capacity and sustainability
	*/
	float SearchFeaturesHead::getFrequencyDomainEntropy(std::vector<float> values)
	{
		float fde = 0;

		ssi_size_t sampleNumber = static_cast<ssi_size_t>(values.size());

		// Copy data for FFT
		ssi_real_t* dataCopy = new ssi_real_t[sampleNumber];
		for (ssi_size_t i = 0; i < sampleNumber; i++)
		{
			dataCopy[i] = values[i];
		}

		// 1. Calculate FFT
		FFT fft(sampleNumber, 1);
		Matrix<ssi_real_t> fftInput(sampleNumber, 1, dataCopy);
		Matrix<ssi_real_t> fftOutput(1, fft.rfft);
		fft.transform(&fftInput, &fftOutput);

		if (fft.rfft > 0)
		{
			std::vector<float> psd;

			// 2. Calculate Power Spectral Density
			for (ssi_size_t i = 0; i < fft.rfft; i++)
			{
				psd.push_back(pow((double)fftOutput.data[i], (double)2) / static_cast<float>(fft.rfft));
			}

			float psdSum = MathTools::GetSum(psd);

			if (psdSum > 0)
			{
				// 3. Normalize calculated PSD so that it can be viewed as a Probability Density Function
				for (ssi_size_t i = 0; i < fft.rfft; i++)
				{
					psd[i] = psd[i] / psdSum;
				}

				// 4. Calculate the Frequency Domain Entropy
				for (ssi_size_t i = 0; i < fft.rfft; i++)
				{
					if (psd[i] != 0)
					{
						fde += psd[i] * log(psd[i]);
					}
				}

				fde *= -1;
			}
		}

        if(_options.normalize) fde = (fde / log(fft.rfft)); //normalisation

        return fde;
	}

	/*
	* Skew for acceleration
	* 
	* based on
	* Herren, R. et al. - The prediction of speed and incline in outdoor running in humans using accelerometry
	*/
	float SearchFeaturesHead::getSkew(std::vector<float> values)
	{
		return MathTools::GetSkew(values);
	}

	/*
	* Kurtosis for acceleration
	*
	* based on
	* Herren, R. et al. - The prediction of speed and incline in outdoor running in humans using accelerometry
	* Lara, Oscar D. et al. - A Survey on Human Activity Recognition using Wearable Sensors
	*/
	float SearchFeaturesHead::getKurtosis(std::vector<float> values)
	{
		return MathTools::GetKurtosis(values);
	}

	/*
	* Interquartile range for acceleration
	*
	* based on
	* Lara, Oscar D. et al. - A Survey on Human Activity Recognition using Wearable Sensors
	* Chen, Yen-Ping et al. - Online classifier construction algorithm for human activity detection using a tri-axial accelerometer
	*/
	float SearchFeaturesHead::getIQR(std::vector<float> values)
	{
		return MathTools::GetIQR(values);
	}

	/*
	* Mean absolute deviation for acceleration
	*
	* based on
	* Lara, Oscar D. et al. - A Survey on Human Activity Recognition using Wearable Sensors
	* Chen, Yen-Ping et al. - Online classifier construction algorithm for human activity detection using a tri-axial accelerometer
	*/
	float SearchFeaturesHead::getMAD(std::vector<float> values)
	{
		return MathTools::GetMAD(values);
	}

	/*
	* Root mean square for acceleration
	*
	* based on
	* Chen, Yen-Ping et al. - Online classifier construction algorithm for human activity detection using a tri-axial accelerometer
	*/
	float SearchFeaturesHead::getRMS(std::vector<float> values)
	{
		return MathTools::GetRMS(values);
	}

	/*
	* Variance for acceleration
	*
	* based on
	* Chen, Yen-Ping et al. - Online classifier construction algorithm for human activity detection using a tri-axial accelerometer
	*/
	float SearchFeaturesHead::getVariance(std::vector<float> values)
	{
		return MathTools::GetVariance(values);
	}

    /*
    * Min for acceleration
    *
    */
    float SearchFeaturesHead::getMin(std::vector<float> values)
    {
        return MathTools::GetMin(values);
    }


    /*
    * Max for acceleration
    *
    */
    float SearchFeaturesHead::getMax(std::vector<float> values)
    {
        return MathTools::GetMax(values);
    }

    /*
    * Median for acceleration
    *
    */
    float SearchFeaturesHead::getMedian(std::vector<float> values)
    {
        return MathTools::GetMedian(values);
    }

    std::vector<float> SearchFeaturesHead::getMagnitudes(std::vector<float> xValues, std::vector<float> yValues, std::vector<float> zValues)
    {
        std::vector<float> magnitudes;

        if (xValues.size() == yValues.size() && yValues.size() == zValues.size())
        {
            for (ssi_size_t i = 0; i < xValues.size(); i++)
            {
#if __ANDROID_API__
                magnitudes.push_back( fsqrt(xValues[i] * xValues[i] + yValues[i]* yValues[i] + zValues[i]* zValues[i])));
#else
                magnitudes.push_back(sqrt( xValues[i] * xValues[i] + yValues[i]* yValues[i] + zValues[i]* zValues[i]));
#endif
            }
        }


    return magnitudes;
    }

    float SearchFeaturesHead::getFFTSum(std::vector<float> values, std::vector<bool>* indices)
    {
        float sum;


        ssi_size_t sampleNumber = static_cast<ssi_size_t>(values.size());

        // Copy data for FFT
        ssi_real_t* dataCopy = new ssi_real_t[sampleNumber];
        for (ssi_size_t i = 0; i < sampleNumber; i++)
        {
            dataCopy[i] = values[i];
        }

        // 1. Calculate FFT
        FFT fft(sampleNumber, 1);
        Matrix<ssi_real_t> fftInput(sampleNumber, 1, dataCopy);
        Matrix<ssi_real_t> fftOutput(1, fft.rfft);
        fft.transform(&fftInput, &fftOutput);

        if (fft.rfft > 0)
        {

            // 2. Calculate Power Spectral Density
            for (ssi_size_t i = 0; i < fft.rfft; i++)
            {
                sum+=fftOutput.data[i];
            }


        }

        //if(_options.normalize) sum = fft.nfft*Norm; //normalisation

        return sum;
    }


	/*
	* Signal magnitude area for acceleration
	*
	* based on
	* Khan, A. et al. - Accelerometer's position free human activity recognition using a hierarchical recognition model
	*/
	float SearchFeaturesHead::getSignalMagnitudeArea(std::vector<float> xValues, std::vector<float> yValues, std::vector<float> zValues)
	{
		float sma = 0;

		if (xValues.size() == yValues.size() && yValues.size() == zValues.size())
		{
			for (ssi_size_t i = 0; i < xValues.size(); i++)
			{
#if __ANDROID_API__
                sma += fabs(xValues[i]) + fabs(yValues[i]) + fabs(zValues[i]);
#else
				sma += abs(xValues[i]) + abs(yValues[i]) + abs(zValues[i]);
#endif
			}
		}

		return sma;
	}

	/*
	* Haar-like filter for acceleration
	*
	* based on
	* Hanai, Yuya et al. - Haar-Like Filtering for Human Activity Recognition Using 3D Accelerometer
	*/
	float SearchFeaturesHead::getHaarFilter(std::vector<float> values)
	{
		float haar = 0;

		// Sizes in number of samples
        if(values.size() == 0)
        {
            ssi_wrn("vector has no elements");
            return 0.0;
        }
		int wFrame = (int) values.size();
		int wFilter = (int) (0.2 * wFrame);
		int wShift = (int) (0.5 * wFilter);
		int N = (wFrame - wFilter) / wShift + 1;
		
		float filterValue = 0;

		for (int n = 0; n < N; n++)
		{
			filterValue = 0;

			for (int k = 0; k < wFilter; k++)
			{
				if (n * wShift + k < wFrame)
				{
					if (k < wFilter / 2)
					{
						// Left side of haar filter
						filterValue -= values[n * wShift + k];
					}
					else
					{
						// Right side of haar filter
						filterValue += values[n * wShift + k];
					}
				}
			}
#if __ANDROID_API__
            haar += fabs(filterValue);
#else
			haar += abs(filterValue);
#endif
			}

		return haar;
	}

	/*
	* Biaxial Haar-like filter for acceleration
	*
	* based on
	* Hanai, Yuya et al. - Haar-Like Filtering for Human Activity Recognition Using 3D Accelerometer
	*/
	float SearchFeaturesHead::getHaarFilterBiaxial(std::vector<float> aValues, std::vector<float> bValues)
	{
		float haarBiaxial = 0;

		// Sizes in number of samples
        if(aValues.size() == 0)
        {
            ssi_wrn("vector has no elements");
            return 0.0;
        }
        int wFrame = (int)aValues.size();
		int wFilter = (int)(0.2 * wFrame);
		int wShift = (int)(0.5 * wFilter);
		int N = (wFrame - wFilter) / wShift + 1;

		float aFilterValue = 0;
		float bFilterValue = 0;

		for (int n = 0; n < N; n++)
		{
			aFilterValue = 0;
			bFilterValue = 0;

			for (int k = 0; k < wFilter; k++)
			{
				if (n * wShift + k < wFrame)
				{
					if (k < wFilter / 2)
					{
						// Left side of haar filter
						aFilterValue -= aValues[n * wShift + k];
						bFilterValue -= bValues[n * wShift + k];
					}
					else
					{
						// Right side of haar filter
						aFilterValue += aValues[n * wShift + k];
						bFilterValue += bValues[n * wShift + k];
					}
				}
			}
#if __ANDROID_API__
			haarBiaxial += fabs(aFilterValue - bFilterValue);
#else
            haarBiaxial += abs(aFilterValue - bFilterValue);
#endif
		}

		return haarBiaxial;
	}

	/*
	* Crest factor for acceleration
	*
	* based on
	* Rahman, Shah et al. - Unintrusive eating recognition using Google glass
	*/
	float SearchFeaturesHead::getCrest(std::vector<float> values)
	{
		return MathTools::GetCrest(values);
	}

	/*
	* Spectral flux for acceleration
	*
	* based on
	* Rahman, Shah et al. - Unintrusive eating recognition using Google glass
	* Lu, Hong et al. - SoundSense: Scalable Sound Sensing for People-Centric Applications on Mobile Phones
	*/
	float SearchFeaturesHead::getSpectralFlux(std::vector<float> values)
	{
		float spectralFlux = 0;

		ssi_size_t sampleNumber = static_cast<ssi_size_t>(values.size());

		// Copy data for FFT
		ssi_real_t* dataCopy = new ssi_real_t[sampleNumber];
		for (ssi_size_t i = 0; i < sampleNumber; i++)
		{
			dataCopy[i] = values[i];
		}

		// 1. Calculate FFT
		FFT fft(sampleNumber, 1);
		Matrix<ssi_real_t> fftInput(sampleNumber, 1, dataCopy);
		Matrix<ssi_real_t> fftOutput(1, fft.rfft);
		fft.transform(&fftInput, &fftOutput);

		if (fft.rfft > 0)
		{
			float previousValue = 0;
			float currentValue = 0;

			for (ssi_size_t i = 0; i < fft.rfft; i++)
			{
				currentValue = fftOutput.data[i];

				spectralFlux += pow((double)currentValue - previousValue, (double)2);

				previousValue = fftOutput.data[i];
			}
		}

		return spectralFlux;
	}

	/*
	* Spectral centroid for acceleration
	*
	* based on
	* Rahman, Shah et al. - Unintrusive eating recognition using Google glass
	* Lu, Hong et al. - SoundSense: Scalable Sound Sensing for People-Centric Applications on Mobile Phones
	*/
	float SearchFeaturesHead::getSpectralCentroid(std::vector<float> values)
	{
		float spectralCentroid = 0;

		ssi_size_t sampleNumber = static_cast<ssi_size_t>(values.size());

		// Copy data for FFT
		ssi_real_t* dataCopy = new ssi_real_t[sampleNumber];
		for (ssi_size_t i = 0; i < sampleNumber; i++)
		{
			dataCopy[i] = values[i];
		}

		// 1. Calculate FFT
		FFT fft(sampleNumber, 1);
		Matrix<ssi_real_t> fftInput(sampleNumber, 1, dataCopy);
		Matrix<ssi_real_t> fftOutput(1, fft.rfft);
		fft.transform(&fftInput, &fftOutput);

		if (fft.rfft > 0)
		{
			float sumTop = 0;
			float sumBottom = 0;

			for (ssi_size_t i = 0; i < fft.rfft; i++)
			{
				sumTop += i * pow((double)fftOutput.data[i], (double)2);
				sumBottom += pow((double)fftOutput.data[i], (double)2);
			}

			if (sumBottom > 0)
			{
				spectralCentroid = sumTop / sumBottom;
			}
		}

		return spectralCentroid;
	}

	/*
	* Spectral rolloff for acceleration
	*
	* based on
	* Rahman, Shah et al. - Unintrusive eating recognition using Google glass
	* Lu, Hong et al. - SoundSense: Scalable Sound Sensing for People-Centric Applications on Mobile Phones
	*/
	int SearchFeaturesHead::getSpectralRolloff(std::vector<float> values)
	{
		int spectralRolloff = 0;
		float threshold = 0.93f;

		ssi_size_t sampleNumber = static_cast<ssi_size_t>(values.size());

		// Copy data for FFT
		ssi_real_t* dataCopy = new ssi_real_t[sampleNumber];
		for (ssi_size_t i = 0; i < sampleNumber; i++)
		{
			dataCopy[i] = values[i];
		}

		// 1. Calculate FFT
		FFT fft(sampleNumber, 1);
		Matrix<ssi_real_t> fftInput(sampleNumber, 1, dataCopy);
		Matrix<ssi_real_t> fftOutput(1, fft.rfft);
		fft.transform(&fftInput, &fftOutput);

		if (fft.rfft > 0)
		{
			float fftSumTotal = 0;
			float fftSum = 0;

			for (ssi_size_t i = 0; i < fft.rfft; i++)
			{
				fftSumTotal += fftOutput.data[i];
			}

			for (ssi_size_t i = 0; i < fft.rfft; i++)
			{
				fftSum += fftOutput.data[i];

				if (fftSum / fftSumTotal >= threshold)
				{
					spectralRolloff = i;
					break;
				}
			}
		}

		return spectralRolloff;
	}

	/*
	* Discrete cosine transform for acceleration
	*
	* based on
	* He, Zhenyu et al. - Activity recognition from acceleration data based on discrete consine transform and svm
	*/
	float SearchFeaturesHead::getDiscreteCosineTransform(std::vector<float> values, int k)
	{
		float dct = 0;
		float N = static_cast<float>(values.size());

		if (N > 0)
		{
			if (k == 0)
			{
				dct = MathTools::GetSum(values) / sqrt(N);
			}
			else
			{
				for (size_t i = 0; i < values.size(); i++)
				{
					dct += (float) (values[i] * cos(((2 * i + 1) * k * PI) / (2 * N)));
				}

				dct *= sqrt(2 / N);
			}
		}

		return dct;
	}

	/***********************************************************************************************\
	|* Helper functions
	\***********************************************************************************************/


	/*
	 * Helper function to get the values as a list
	 */
	std::vector<float> SearchFeaturesHead::getValues(ssi_real_t *ptr_in, ssi_size_t sample_number, ssi_size_t sample_dimension, int dimension)
	{
		std::vector<float> values;

		for (ssi_size_t i = 0; i < sample_number; i++)
		{
			values.push_back(ptr_in[i * sample_dimension + dimension]);
		}

		return values;
	}
}
