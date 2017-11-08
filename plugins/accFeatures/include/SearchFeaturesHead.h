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
#define SEARCHFEATURES_HEAD_DCT_DOEFFICIENTS 8

	class SearchFeaturesHead : public IFeature {

	public:

		class Options : public OptionList {

		public:

            Options() : mean_dev_norm(78.45), normalize(true) {

                addOption("normalize", &normalize, 1, SSI_BOOL, "Activate/Deactivate Normalization for mean, std. deviation, energy, correlation, entropy, irq, mad, rms and variance");
                addOption("mdNorm", &mean_dev_norm, 1, SSI_REAL, "Normalisation value for mean, std. deviation and energy");

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
				addOption("entropyX", &entropyX, 1, SSI_BOOL, "Frequency domain entropy for x-axis");
				addOption("entropyY", &entropyY, 1, SSI_BOOL, "Frequency domain entropy for y-axis");
				addOption("entropyZ", &entropyZ, 1, SSI_BOOL, "Frequency domain entropy for z-axis");
				addOption("skewX", &skewX, 1, SSI_BOOL, "Skew for x-axis");
				addOption("skewY", &skewY, 1, SSI_BOOL, "Skew for y-axis");
				addOption("skewZ", &skewZ, 1, SSI_BOOL, "Skew for z-axis");
				addOption("kurtosisX", &kurtosisX, 1, SSI_BOOL, "Kurtosis for x-axis");
				addOption("kurtosisY", &kurtosisY, 1, SSI_BOOL, "Kurtosis  for y-axis");
				addOption("kurtosisZ", &kurtosisZ, 1, SSI_BOOL, "Kurtosis for z-axis");
				addOption("iqrX", &iqrX, 1, SSI_BOOL, "Interquartile range for x-axis");
				addOption("iqrY", &iqrY, 1, SSI_BOOL, "Interquartile range for y-axis");
				addOption("iqrZ", &iqrZ, 1, SSI_BOOL, "Interquartile range for z-axis");
				addOption("madX", &madX, 1, SSI_BOOL, "Mean absolute deviation for x-axis");
				addOption("madY", &madY, 1, SSI_BOOL, "Mean absolute deviation for y-axis");
				addOption("madZ", &madZ, 1, SSI_BOOL, "Mean absolute deviation z-axis");
				addOption("rmsX", &rmsX, 1, SSI_BOOL, "Root mean square for x-axis");
				addOption("rmsY", &rmsY, 1, SSI_BOOL, "Root mean square for y-axis");
				addOption("rmsZ", &rmsZ, 1, SSI_BOOL, "Root mean square for z-axis");
				addOption("varianceX", &varianceX, 1, SSI_BOOL, "Variance for x-axis");
				addOption("varianceY", &varianceY, 1, SSI_BOOL, "Variance for y-axis");
				addOption("varianceZ", &varianceZ, 1, SSI_BOOL, "Variance for z-axis");
				addOption("signalMagnitudeArea", &signalMagnitudeArea, 1, SSI_BOOL, "Signal magnitude area for all axes");
				addOption("haarFilterX", &haarFilterX, 1, SSI_BOOL, "Haar-like filter for x-axis");
				addOption("haarFilterY", &haarFilterY, 1, SSI_BOOL, "Haar-like filter for y-axis");
				addOption("haarFilterZ", &haarFilterZ, 1, SSI_BOOL, "Haar-like filter for z-axis");
				addOption("haarFilterBiaxialXY", &haarFilterBiaxialXY, 1, SSI_BOOL, "Biaxial Haar-like filter between x and y-axis");
				addOption("haarFilterBiaxialYZ", &haarFilterBiaxialYZ, 1, SSI_BOOL, "Biaxial Haar-like filter between y and z-axis");
				addOption("haarFilterBiaxialZX", &haarFilterBiaxialZX, 1, SSI_BOOL, "Biaxial Haar-like filter between z and x-axis");
				addOption("crestX", &crestX, 1, SSI_BOOL, "Crest factor for x-axis");
				addOption("crestY", &crestY, 1, SSI_BOOL, "Crest factor for y-axis");
				addOption("crestZ", &crestZ, 1, SSI_BOOL, "Crest factor for z-axis");
				addOption("spectralFluxX", &spectralFluxX, 1, SSI_BOOL, "Spectral flux for x-axis");
				addOption("spectralFluxY", &spectralFluxY, 1, SSI_BOOL, "Spectral flux for y-axis");
				addOption("spectralFluxZ", &spectralFluxZ, 1, SSI_BOOL, "Spectral flux for z-axis");
				addOption("spectralCentroidX", &spectralCentroidX, 1, SSI_BOOL, "Spectral centroid for x-axis");
				addOption("spectralCentroidY", &spectralCentroidY, 1, SSI_BOOL, "Spectral centroid for y-axis");
				addOption("spectralCentroidZ", &spectralCentroidZ, 1, SSI_BOOL, "Spectral centroid for z-axis");
				addOption("spectralRolloffX", &spectralRolloffX, 1, SSI_BOOL, "Spectral rolloff for x-axis");
				addOption("spectralRolloffY", &spectralRolloffY, 1, SSI_BOOL, "Spectral rolloff for y-axis");
				addOption("spectralRolloffZ", &spectralRolloffZ, 1, SSI_BOOL, "Spectral rolloff for z-axis");
				addOption("dctX", &dctX, 1, SSI_BOOL, "Discrete cosine transform for x-axis");
				addOption("dctY", &dctY, 1, SSI_BOOL, "Discrete cosine transform for y-axis");
				addOption("dctZ", &dctZ, 1, SSI_BOOL, "Discrete cosine transform for z-axis");
                addOption("maxX", &maxX,	 1, SSI_BOOL,  "Max for X-Axis"	);
                addOption("minX", &minX,	 1, SSI_BOOL,  "MinX "	);
                addOption("medX", &medX,	 1, SSI_BOOL,  "MedX");
                addOption("fftSumX", &fftSumX, 1, SSI_BOOL, "fftSumX	");
                addOption("maxY", &maxY,	 1, SSI_BOOL,  "MaxY");
                addOption("minY", &minY,	 1, SSI_BOOL,  "MinY" );
                addOption("medY", &medY,	 1, SSI_BOOL,  "MedY");
                addOption("fftSumY", &fftSumY, 1, SSI_BOOL, "fftSumY	"	);
                addOption("maxZ", &maxZ,	 1, SSI_BOOL,  "MaxZ"	);
                addOption("minZ", &minZ,	 1, SSI_BOOL,  "MinZ "	);
                addOption("medZ", &medZ,	 1, SSI_BOOL,  "MedZ"	);
                addOption("fftSumZ", &fftSumZ, 1, SSI_BOOL, "fftSumZ	");
                addOption("magMean", &magMean, 1, SSI_BOOL, "MeanSum on Vector Length (Magnitude)");
                addOption("magStd", &magStd, 1, SSI_BOOL,  "StdVarSum on Vector Length (Magnitude)"	);
                addOption("magMax", &magMax, 1, SSI_BOOL,  "Max	Sum on Vector Length (Magnitude)");
                addOption("magMin", &magMin, 1, SSI_BOOL,  "Min	Sum on Vector Length (Magnitude)");
                addOption("magSemiQuartile", &magSemiQuartile, 1, SSI_BOOL, "IQR	Sum on Vector Length (Magnitude)");
                addOption("magMed", &magMed, 1, SSI_BOOL, "Median	Sum on Vector Length (Magnitude)");
                addOption("magFftSum",        &magFftSum,	 1, SSI_BOOL, "FFT Sum on Vector Length (Magnitude)");

			}

            ssi_real_t mean_dev_norm;
            bool normalize;

			bool meanX				= true,
				meanY				= true,
				meanZ				= true,
                stdDeviationX		= true,
                stdDeviationY		= true,
                stdDeviationZ		= true,
                energyX				= true,
                energyY				= true,
                energyZ				= true,
                correlationXY		= true,
                correlationXZ		= true,
                correlationYZ		= true,
                displacementX		= false,
                displacementY		= false,
                displacementZ		= false,
                entropyX			= false,
                entropyY			= false,
                entropyZ			= false,
                skewX				= false,
                skewY				= false,
                skewZ				= false,
                kurtosisX			= false,
                kurtosisY			= false,
                kurtosisZ			= false,
                iqrX				= true,
                iqrY				= true,
                iqrZ				= true,
                madX				= true,
                madY				= true,
                madZ				= true,
                rmsX				= true,
                rmsY				= true,
                rmsZ				= true,
                varianceX			= true,
                varianceY			= true,
                varianceZ			= true,
                signalMagnitudeArea	= false,
                haarFilterX			= false,
                haarFilterY			= false,
                haarFilterZ			= false,
                haarFilterBiaxialXY = false,
                haarFilterBiaxialYZ = false,
                haarFilterBiaxialZX = false,
                crestX				= false,
                crestY				= false,
                crestZ				= false,
                spectralFluxX		= false,
                spectralFluxY		= false,
                spectralFluxZ		= false,
                spectralCentroidX	= false,
                spectralCentroidY	= false,
                spectralCentroidZ	= false,
                spectralRolloffX	= false,
                spectralRolloffY	= false,
                spectralRolloffZ	= false,
                dctX				= false,
                dctY				= false,
                dctZ				= false,
                maxX				= true,
                minX				= true,
                medX				= false,
                fftSumX				= false,
                maxY				= true,
                minY				= true,
                medY				= false,
                fftSumY				= false,
                maxZ				= true,
                minZ				= true,
                medZ				= false,
                fftSumZ				= false,
                magMean				= true,
                magStd				= true,
                magMax				= true,
                magMin				= true,
                magSemiQuartile		= true,
                magMed				= false,
                magFftSum			= false;




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
			if (_options.entropyX) dim++;
			if (_options.entropyY) dim++;
			if (_options.entropyZ) dim++;
			if (_options.skewX) dim++;
			if (_options.skewY) dim++;
			if (_options.skewZ) dim++;
			if (_options.kurtosisX) dim++;
			if (_options.kurtosisY) dim++;
			if (_options.kurtosisZ) dim++;
			if (_options.iqrX) dim++;
			if (_options.iqrY) dim++;
			if (_options.iqrZ) dim++;
			if (_options.madX) dim++;
			if (_options.madY) dim++;
			if (_options.madZ) dim++;
			if (_options.rmsX) dim++;
			if (_options.rmsY) dim++;
			if (_options.rmsZ) dim++;
			if (_options.varianceX) dim++;
			if (_options.varianceY) dim++;
			if (_options.varianceZ) dim++;
			if (_options.signalMagnitudeArea) dim++;
			if (_options.haarFilterX) dim++;
			if (_options.haarFilterY) dim++;
			if (_options.haarFilterZ) dim++;
			if (_options.haarFilterBiaxialXY) dim++;
			if (_options.haarFilterBiaxialYZ) dim++;
			if (_options.haarFilterBiaxialZX) dim++;
			if (_options.crestX) dim++;
			if (_options.crestY) dim++;
			if (_options.crestZ) dim++;
			if (_options.spectralFluxX) dim++;
			if (_options.spectralFluxY) dim++;
			if (_options.spectralFluxZ) dim++;
			if (_options.spectralCentroidX) dim++;
			if (_options.spectralCentroidY) dim++;
			if (_options.spectralCentroidZ) dim++;
			if (_options.spectralRolloffX) dim++;
			if (_options.spectralRolloffY) dim++;
			if (_options.spectralRolloffZ) dim++;
			if (_options.dctX) dim += SEARCHFEATURES_HEAD_DCT_DOEFFICIENTS;
			if (_options.dctY) dim += SEARCHFEATURES_HEAD_DCT_DOEFFICIENTS;
			if (_options.dctZ) dim += SEARCHFEATURES_HEAD_DCT_DOEFFICIENTS;
            if(_options.maxX) dim++;
            if(_options.minX) dim++;
            if(_options.medX) dim++;
            if(_options.fftSumX) dim++;
            if(_options.maxY) dim++;
            if(_options.minY) dim++;
            if(_options.medY) dim++;
            if(_options.fftSumY) dim++;
            if(_options.maxZ) dim++;
            if(_options.minZ) dim++;
            if(_options.medZ) dim++;
            if(_options.fftSumZ) dim++;
            if(_options.magMean) dim++;
            if(_options.magStd) dim++;
            if(_options.magMax) dim++;
            if(_options.magMin) dim++;
            if(_options.magSemiQuartile) dim++;
            if(_options.magMed) dim++;
            if(_options.magFftSum ) dim++;

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
		float getFrequencyDomainEntropy(std::vector<float> values);
		float getSkew(std::vector<float> values);
		float getKurtosis(std::vector<float> values);
        float getMin(std::vector<float> values);
        float getMax(std::vector<float> values);
        float getMedian(std::vector<float> values);
        float getFFTSum(std::vector<float> values, std::vector<bool>* indices=NULL);
        std::vector<float> getMagnitudes(std::vector<float> xValues, std::vector<float> yValues, std::vector<float> zValues);
		float getIQR(std::vector<float> values);
		float getMAD(std::vector<float> values);
		float getRMS(std::vector<float> values);
		float getVariance(std::vector<float> values);
		float getSignalMagnitudeArea(std::vector<float> xValues, std::vector<float> yValues, std::vector<float> zValues);
		float getHaarFilter(std::vector<float> values);
		float getHaarFilterBiaxial(std::vector<float> aValues, std::vector<float> bValues);
		float getCrest(std::vector<float> values);
		float getSpectralFlux(std::vector<float> values);
		float getSpectralCentroid(std::vector<float> values);
		int getSpectralRolloff(std::vector<float> values);
		float getDiscreteCosineTransform(std::vector<float> values, int k);


	protected:

		SearchFeaturesHead(const ssi_char_t *file = 0);
		ssi_char_t *_file;
		Options _options;
		static char ssi_log_name[];

		std::vector<float> getValues(ssi_real_t *ptr_in, ssi_size_t sample_number, ssi_size_t sample_dimension, int dimension);

        bool _initEnergyNorm;
        float _normalizeEnergy;


        bool _initEntropyNorm;
        float _normalizeEntropy;
	};

}

#endif
