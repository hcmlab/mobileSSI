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

#ifndef SSI_SEARCHFEATURES_GAZE_H
#define SSI_SEARCHFEATURES_GAZE_H

#include "base/IFeature.h"
#include "ioput/option/OptionList.h"
#include "MathTools.h"
#include "signal/SignalCons.h"

namespace ssi {

/*
* Number of pixels before gaze movement counts as a saccade (exclude microsaccades/jitter)
*
* based on
* Castelhano, M. et al. - Viewing task influences eye movement control during active scene perception
*/
#define SEARCHFEATURES_GAZE_SACCADE_THRESHOLD 8.0f 

#define SEARCHFEATURES_GAZE_DIM_SCENEGAZE_X 0
#define SEARCHFEATURES_GAZE_DIM_SCENEGAZE_Y 1
#define SEARCHFEATURES_GAZE_DIM_COUNT       2

struct Saccade {
	float startX;
	float startY;
	float endX;
	float endY;
	float duration;
	float length;
};

struct Fixation {
	float x;
	float y;
	float duration;
};

	class SearchFeaturesGaze : public IFeature {

	public:

		class Options : public OptionList {

		public:

			Options() {
				addOption("fixationCount", &fixationCount, 1, SSI_BOOL, "Number of fixations");
				addOption("fixationDurationTotal", &fixationDurationTotal, 1, SSI_BOOL, "Total duration of all fixations (in seconds)");
				addOption("fixationDurationAvg", &fixationDurationAvg, 1, SSI_BOOL, "Average duration of one fixation (in seconds)");
				addOption("fixationDurationMin", &fixationDurationMin, 1, SSI_BOOL, "Minimum fixation duration (in seconds)");
				addOption("fixationDurationMax", &fixationDurationMax, 1, SSI_BOOL, "Maximum fixation duration (in seconds)");
				addOption("fixationDurationMedian", &fixationDurationMedian, 1, SSI_BOOL, "Median fixation duration (in seconds)");
				addOption("fixationDurationStdDeviation", &fixationDurationStdDeviation, 1, SSI_BOOL, "Standard deviation of fixation duration (in seconds)");
				addOption("fixationDurationSkew", &fixationDurationSkew, 1, SSI_BOOL, "Skew of fixation duration");
				addOption("fixationDurationKurtosis", &fixationDurationKurtosis, 1, SSI_BOOL, "Kurtosis of fixation duration");
				addOption("fixationDurationRange", &fixationDurationRange, 1, SSI_BOOL, "Range of fixation duration");
				addOption("fixationDispersion", &fixationDispersion, 1, SSI_BOOL, "Root mean square of the distances of each fixation from the average position of all fixations");
				addOption("saccadeLengthAvg", &saccadeLengthAvg, 1, SSI_BOOL, "Average length of one saccade (in pixels)");
				addOption("saccadeLengthMin", &saccadeLengthMin, 1, SSI_BOOL, "Minimum length of one saccade (in pixels)");
				addOption("saccadeLengthMax", &saccadeLengthMax, 1, SSI_BOOL, "Maximum length of one saccade (in pixels)");
				addOption("saccadeLengthMedian", &saccadeLengthMedian, 1, SSI_BOOL, "Median length of one saccade (in pixels)");
				addOption("saccadeLengthStdDeviation", &saccadeLengthStdDeviation, 1, SSI_BOOL, "Standard deviation of saccade length (in pixels)");
				addOption("saccadeLengthSkew", &saccadeLengthSkew, 1, SSI_BOOL, "Skew of saccade length");
				addOption("saccadeLengthKurtosis", &saccadeLengthKurtosis, 1, SSI_BOOL, "Kurtosis of saccade length");
				addOption("saccadeLengthRange", &saccadeLengthRange, 1, SSI_BOOL, "Range of saccade length");
				addOption("saccadeLengthTotal", &saccadeLengthTotal, 1, SSI_BOOL, "Total saccade length");
				addOption("saccadeDurationAvg", &saccadeDurationAvg, 1, SSI_BOOL, "Average duration of one saccade (in seconds)");
				addOption("saccadeDurationMin", &saccadeDurationMin, 1, SSI_BOOL, "Minimum duration of one saccade (in seconds)");
				addOption("saccadeDurationMax", &saccadeDurationMax, 1, SSI_BOOL, "Maximum duration of one saccade (in seconds)");
				addOption("saccadeDurationMedian", &saccadeDurationMedian, 1, SSI_BOOL, "Median duration of one saccade (in seconds)");
				addOption("saccadeDurationStdDeviation", &saccadeDurationStdDeviation, 1, SSI_BOOL, "Standard deviation of saccade duration (in seconds)");
				addOption("saccadeDurationSkew", &saccadeDurationSkew, 1, SSI_BOOL, "Skew of saccade duration");
				addOption("saccadeDurationKurtosis", &saccadeDurationKurtosis, 1, SSI_BOOL, "Kurtosis of saccade duration");
				addOption("saccadeDurationRange", &saccadeDurationRange, 1, SSI_BOOL, "Range of saccade duration");
				addOption("saccadeDurationTotal", &saccadeDurationTotal, 1, SSI_BOOL, "Total duration of all saccades (in seconds)");
				addOption("saccadeCount", &saccadeCount, 1, SSI_BOOL, "Number of saccades");
				addOption("durationRatio", &durationRatio, 1, SSI_BOOL, "Fixation duration / saccade duration ratio");
				addOption("wordBookSize", &wordBookSize, 1, SSI_BOOL, "Size of the wordbooks");
				addOption("wordBookMax", &wordBookMax, 1, SSI_BOOL, "Maximum occurrence count of one word in each wordbook");
				addOption("wordBookDiff", &wordBookDiff, 1, SSI_BOOL, "Difference between maximum and minimum occurrence in each wordbook");
				addOption("wordBookVar", &wordBookVar, 1, SSI_BOOL, "Variance of all occurrence counts in each wordbook");
				addOption("wordBookMean", &wordBookMean, 1, SSI_BOOL, "Mean of all occurrence counts in each wordbook");
				addOption("viewCovered", &viewCovered, 1, SSI_BOOL, "Percentage of view covered with fixations");
				addOption("fixationGroups", &fixationGroups, 1, SSI_BOOL, "Number of fixation groups");
			}

			bool fixationCount					= true, // Visual search feature
				fixationDurationTotal			= true,
				fixationDurationAvg				= true, // Visual search feature
				fixationDurationMin				= true,
				fixationDurationMax				= true,
				fixationDurationMedian			= true,
				fixationDurationStdDeviation	= true,
				fixationDurationSkew			= true,
				fixationDurationKurtosis		= true,
				fixationDurationRange			= true,
				fixationDispersion				= true,
				saccadeLengthAvg				= true, // Visual search feature
				saccadeLengthMin				= true,
				saccadeLengthMax				= true,
				saccadeLengthMedian				= true,
				saccadeLengthStdDeviation		= true,
				saccadeLengthSkew				= true,
				saccadeLengthKurtosis			= true,
				saccadeLengthRange				= true,
				saccadeLengthTotal				= true,
				saccadeDurationAvg				= true,
				saccadeDurationMin				= true,
				saccadeDurationMax				= true,
				saccadeDurationMedian			= true,
				saccadeDurationStdDeviation		= true,
				saccadeDurationSkew				= true,
				saccadeDurationKurtosis			= true,
				saccadeDurationRange			= true,
				saccadeDurationTotal			= true,
				saccadeCount					= true,
				durationRatio					= true,
				wordBookSize					= true,
				wordBookMax						= true,
				wordBookDiff					= true,
				wordBookVar						= true,
				wordBookMean					= true,
				viewCovered						= true,
				fixationGroups					= true;
		};

	public:

		static const ssi_char_t *GetCreateName() { return "SearchFeaturesGaze"; };
		static IObject *Create(const ssi_char_t *file) { return new SearchFeaturesGaze(file); };
		~SearchFeaturesGaze();

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
			if (sample_dimension_in != SEARCHFEATURES_GAZE_DIM_COUNT)
			{
				ssi_err("DIM is %i, expected %i\n", sample_dimension_in, SEARCHFEATURES_GAZE_DIM_COUNT);
			}

			ssi_size_t dim = 0;

			if (_options.fixationCount) dim++;
			if (_options.fixationDurationTotal) dim++;
			if (_options.fixationDurationAvg) dim++;
			if (_options.fixationDurationMin) dim++;
			if (_options.fixationDurationMax) dim++;
			if (_options.fixationDurationMedian) dim++;
			if (_options.fixationDurationStdDeviation) dim++;
			if (_options.fixationDurationSkew) dim++;
			if (_options.fixationDurationKurtosis) dim++;
			if (_options.fixationDurationRange) dim++;
			if (_options.fixationDispersion) dim++;
			if (_options.saccadeLengthAvg) dim++;
			if (_options.saccadeLengthMin) dim++;
			if (_options.saccadeLengthMax) dim++;
			if (_options.saccadeLengthMedian) dim++;
			if (_options.saccadeLengthStdDeviation) dim++;
			if (_options.saccadeLengthSkew) dim++;
			if (_options.saccadeLengthKurtosis) dim++;
			if (_options.saccadeLengthRange) dim++;
			if (_options.saccadeLengthTotal) dim++;
			if (_options.saccadeDurationAvg) dim++;
			if (_options.saccadeDurationMin) dim++;
			if (_options.saccadeDurationMax) dim++;
			if (_options.saccadeDurationMedian) dim++;
			if (_options.saccadeDurationStdDeviation) dim++;
			if (_options.saccadeDurationSkew) dim++;
			if (_options.saccadeDurationKurtosis) dim++;
			if (_options.saccadeDurationRange) dim++;
			if (_options.saccadeDurationTotal) dim++;
			if (_options.saccadeCount) dim++;
			if (_options.durationRatio) dim++;
			if (_options.wordBookSize) dim += 4;
			if (_options.wordBookMax) dim += 4;
			if (_options.wordBookDiff) dim += 4;
			if (_options.wordBookVar) dim += 4;
			if (_options.wordBookMean) dim += 4;
			if (_options.viewCovered) dim++;
			if (_options.fixationGroups) dim++;

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

		int getFixationCount(std::vector<Fixation> fixations);
		float getFixationDurationTotal(std::vector<float> fixationDurations);
		float getFixationDurationAvg(std::vector<float> fixationDurations);
		float getFixationDurationMin(std::vector<float> fixationDurations);
		float getFixationDurationMax(std::vector<float> fixationDurations);
		float getFixationDurationMedian(std::vector<float> fixationDurations);
		float getFixationDurationStdDeviation(std::vector<float> fixationDurations);
		float getFixationDurationSkew(std::vector<float> fixationDurations);
		float getFixationDurationKurtosis(std::vector<float> fixationDurations);
		float getFixationDurationRange(std::vector<float> fixationDurations);
		float getFixationDispersion(std::vector<Fixation> fixations);
		float getSaccadeLengthAvg(std::vector<float> saccadeLengths);
		float getSaccadeLengthMin(std::vector<float> saccadeLengths);
		float getSaccadeLengthMax(std::vector<float> saccadeLengths);
		float getSaccadeLengthMedian(std::vector<float> saccadeLengths);
		float getSaccadeLengthStdDeviation(std::vector<float> saccadeLengths);
		float getSaccadeLengthSkew(std::vector<float> saccadeLengths);
		float getSaccadeLengthKurtosis(std::vector<float> saccadeLengths);
		float getSaccadeLengthRange(std::vector<float> saccadeLengths);
		float getSaccadeLengthTotal(std::vector<float> saccadeLengths);
		float getSaccadeDurationAvg(std::vector<float> saccadeDurations);
		float getSaccadeDurationMin(std::vector<float> saccadeDurations);
		float getSaccadeDurationMax(std::vector<float> saccadeDurations);
		float getSaccadeDurationMedian(std::vector<float> saccadeDurations);
		float getSaccadeDurationStdDeviation(std::vector<float> saccadeDurations);
		float getSaccadeDurationSkew(std::vector<float> saccadeDurations);
		float getSaccadeDurationKurtosis(std::vector<float> saccadeDurations);
		float getSaccadeDurationRange(std::vector<float> saccadeDurations);
		float getSaccadeDurationTotal(std::vector<float> saccadeDurations);
		int getSaccadeCount(std::vector<Saccade> saccades);
		float getDurationRatio(std::vector<float> fixationDurations, std::vector<float> saccadeDurations);
		int getWordBookSize(std::map<std::string, int> wordBook);
		int getWordBookMax(std::map<std::string, int> wordBook);
		int getWordBookDiff(std::map<std::string, int> wordBook);
		float getWordBookVar(std::map<std::string, int> wordBook);
		float getWordBookMean(std::map<std::string, int> wordBook);
		std::vector<float> getFixationCoverage(std::vector<Fixation> fixations);

		virtual void print();

	protected:

		SearchFeaturesGaze(const ssi_char_t *file = 0);
		ssi_char_t *_file;
		Options _options;
		static char ssi_log_name[];

		bool isFixation(ssi_real_t x1, ssi_real_t y1, ssi_real_t x2, ssi_real_t y2);
		std::vector<float> getSaccadeLenghts(std::vector<Saccade> saccades);
		std::vector<float> getSaccadeDurations(std::vector<Saccade> saccades);
		std::vector<float> getFixationDurations(std::vector<Fixation> fixations);
		std::vector<Saccade> getSaccades(ssi_real_t *ptr_in, ssi_size_t sample_number, ssi_size_t sample_dimension, ssi_time_t sr);
		std::vector<Fixation> getFixations(ssi_real_t *ptr_in, ssi_size_t sample_number, ssi_size_t sample_dimension, ssi_time_t sr);
		std::map<std::string, int> getWordBook(std::vector<Saccade> saccades, int wordLength, float lengthThreshold);
	};

}

#endif
