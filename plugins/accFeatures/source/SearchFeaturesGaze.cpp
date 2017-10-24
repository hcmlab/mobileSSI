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

#include "SearchFeaturesGaze.h"

#include "ssiocv.h"

namespace ssi {

	char SearchFeaturesGaze::ssi_log_name[] = "SearchFeaturesGaze";

	SearchFeaturesGaze::SearchFeaturesGaze(const ssi_char_t *file) : _file(0) {

		if (file) {
			if (!OptionList::LoadXML(file, _options)) {
				OptionList::SaveXML(file, _options);
			}
			_file = ssi_strcpy(file);
		}
	}

	SearchFeaturesGaze::~SearchFeaturesGaze() {

		if (_file) {
			OptionList::SaveXML(_file, _options);
			delete[] _file;
		}
	}

	void SearchFeaturesGaze::transform_enter(ssi_stream_t &stream_in,
		ssi_stream_t &stream_out,
		ssi_size_t xtra_stream_in_num,
		ssi_stream_t xtra_stream_in[]) {

	}

	void SearchFeaturesGaze::transform(ITransformer::info info,
		ssi_stream_t &stream_in,
		ssi_stream_t &stream_out,
		ssi_size_t xtra_stream_in_num,
		ssi_stream_t xtra_stream_in[]) {

		ssi_size_t sample_dimension = stream_in.dim;
		ssi_size_t sample_number = info.frame_num + info.delta_num; // stream_in.num

		ssi_real_t *ptr_in = ssi_pcast(ssi_real_t, stream_in.ptr);
		ssi_real_t *ptr_out = ssi_pcast(ssi_real_t, stream_out.ptr);

		// Calculate fixations for window
		std::vector<Fixation> fixations = getFixations(ptr_in, sample_number, sample_dimension, stream_in.sr);

		// Calculate saccades for window
		std::vector<Saccade> saccades = getSaccades(ptr_in, sample_number, sample_dimension, stream_in.sr);

		// Get fixation duration
		std::vector<float> fixationDurations = getFixationDurations(fixations);

		// Get saccade duration
		std::vector<float> saccadeDurations = getSaccadeDurations(saccades);

		// Get saccade lenght
		std::vector<float> saccadeLengths = getSaccadeLenghts(saccades);

		if (_options.fixationCount)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getFixationCount(fixations));
		}

		if (_options.fixationDurationTotal)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getFixationDurationTotal(fixationDurations));
		}

		if (_options.fixationDurationAvg)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getFixationDurationAvg(fixationDurations));
		}

		if (_options.fixationDurationMin)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getFixationDurationMin(fixationDurations));
		}

		if (_options.fixationDurationMax)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getFixationDurationMax(fixationDurations));
		}

		if (_options.fixationDurationMedian)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getFixationDurationMedian(fixationDurations));
		}

		if (_options.fixationDurationStdDeviation)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getFixationDurationStdDeviation(fixationDurations));
		}

		if (_options.fixationDurationSkew)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getFixationDurationSkew(fixationDurations));
		}

		if (_options.fixationDurationKurtosis)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getFixationDurationKurtosis(fixationDurations));
		}

		if (_options.fixationDurationRange)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getFixationDurationRange(fixationDurations));
		}

		if (_options.fixationDispersion)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getFixationDispersion(fixations));
		}

		if (_options.saccadeLengthAvg)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getSaccadeLengthAvg(saccadeLengths));
		}

		if (_options.saccadeLengthMin)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getSaccadeLengthMin(saccadeLengths));
		}

		if (_options.saccadeLengthMax)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getSaccadeLengthMax(saccadeLengths));
		}

		if (_options.saccadeLengthMedian)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getSaccadeLengthMedian(saccadeLengths));
		}

		if (_options.saccadeLengthStdDeviation)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getSaccadeLengthStdDeviation(saccadeLengths));
		}

		if (_options.saccadeLengthSkew)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getSaccadeLengthSkew(saccadeLengths));
		}

		if (_options.saccadeLengthKurtosis)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getSaccadeLengthKurtosis(saccadeLengths));
		}

		if (_options.saccadeLengthRange)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getSaccadeLengthRange(saccadeLengths));
		}

		if (_options.saccadeLengthTotal)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getSaccadeLengthTotal(saccadeLengths));
		}

		if (_options.saccadeDurationAvg)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getSaccadeDurationAvg(saccadeDurations));
		}

		if (_options.saccadeDurationMin)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getSaccadeDurationMin(saccadeDurations));
		}

		if (_options.saccadeDurationMax)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getSaccadeDurationMax(saccadeDurations));
		}

		if (_options.saccadeDurationMedian)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getSaccadeDurationMedian(saccadeDurations));
		}

		if (_options.saccadeDurationStdDeviation)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getSaccadeDurationStdDeviation(saccadeDurations));
		}

		if (_options.saccadeDurationSkew)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getSaccadeDurationSkew(saccadeDurations));
		}

		if (_options.saccadeDurationKurtosis)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getSaccadeDurationKurtosis(saccadeDurations));
		}

		if (_options.saccadeDurationRange)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getSaccadeDurationRange(saccadeDurations));
		}

		if (_options.saccadeDurationTotal)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getSaccadeDurationTotal(saccadeDurations));
		}

		if (_options.saccadeCount)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getSaccadeCount(saccades));
		}

		if (_options.durationRatio)
		{
			*ptr_out++ = static_cast<ssi_real_t>(getDurationRatio(fixationDurations, saccadeDurations));
		}

		if (_options.wordBookSize || _options.wordBookMax || _options.wordBookDiff || _options.wordBookVar || _options.wordBookMean)
		{
			float lengthThreshold = SEARCHFEATURES_GAZE_SACCADE_THRESHOLD + 20;

			std::map<std::string, int> wordBook1 = getWordBook(saccades, 1, lengthThreshold);
			std::map<std::string, int> wordBook2 = getWordBook(saccades, 2, lengthThreshold);
			std::map<std::string, int> wordBook3 = getWordBook(saccades, 3, lengthThreshold);
			std::map<std::string, int> wordBook4 = getWordBook(saccades, 4, lengthThreshold);

			if (_options.wordBookSize)
			{
				*ptr_out++ = static_cast<ssi_real_t>(getWordBookSize(wordBook1));
				*ptr_out++ = static_cast<ssi_real_t>(getWordBookSize(wordBook2));
				*ptr_out++ = static_cast<ssi_real_t>(getWordBookSize(wordBook3));
				*ptr_out++ = static_cast<ssi_real_t>(getWordBookSize(wordBook4));
			}

			if (_options.wordBookMax)
			{
				*ptr_out++ = static_cast<ssi_real_t>(getWordBookMax(wordBook1));
				*ptr_out++ = static_cast<ssi_real_t>(getWordBookMax(wordBook2));
				*ptr_out++ = static_cast<ssi_real_t>(getWordBookMax(wordBook3));
				*ptr_out++ = static_cast<ssi_real_t>(getWordBookMax(wordBook4));
			}

			if (_options.wordBookDiff)
			{
				*ptr_out++ = static_cast<ssi_real_t>(getWordBookDiff(wordBook1));
				*ptr_out++ = static_cast<ssi_real_t>(getWordBookDiff(wordBook2));
				*ptr_out++ = static_cast<ssi_real_t>(getWordBookDiff(wordBook3));
				*ptr_out++ = static_cast<ssi_real_t>(getWordBookDiff(wordBook4));
			}

			if (_options.wordBookVar)
			{
				*ptr_out++ = static_cast<ssi_real_t>(getWordBookVar(wordBook1));
				*ptr_out++ = static_cast<ssi_real_t>(getWordBookVar(wordBook2));
				*ptr_out++ = static_cast<ssi_real_t>(getWordBookVar(wordBook3));
				*ptr_out++ = static_cast<ssi_real_t>(getWordBookVar(wordBook4));
			}

			if (_options.wordBookMean)
			{
				*ptr_out++ = static_cast<ssi_real_t>(getWordBookMean(wordBook1));
				*ptr_out++ = static_cast<ssi_real_t>(getWordBookMean(wordBook2));
				*ptr_out++ = static_cast<ssi_real_t>(getWordBookMean(wordBook3));
				*ptr_out++ = static_cast<ssi_real_t>(getWordBookMean(wordBook4));
			}

			wordBook1.clear();
			wordBook2.clear();
			wordBook3.clear();
			wordBook4.clear();
		}

		if (_options.viewCovered || _options.fixationGroups)
		{
			std::vector<float> result = getFixationCoverage(fixations);

			if (_options.viewCovered)
			{
				*ptr_out++ = static_cast<ssi_real_t>(result[0]);
			}
			
			if (_options.fixationGroups)
			{
				*ptr_out++ = static_cast<ssi_real_t>(result[1]);
			}

			result.clear();
		}

		fixations.clear();
		saccades.clear();
		fixationDurations.clear();
		saccadeDurations.clear();
		saccadeLengths.clear();
	}

	void SearchFeaturesGaze::transform_flush(ssi_stream_t &stream_in,
		ssi_stream_t &stream_out,
		ssi_size_t xtra_stream_in_num,
		ssi_stream_t xtra_stream_in[]) {

	}

	void SearchFeaturesGaze::print() {

		ssi_print("%s says: Hello!\n", "Searchfeatures");
	}

	/*
	 * Number of fixations
	 *
	 * based on
	 * Castelhano, Monica et al. - Viewing task influences eye movement control during active scene perception
	 * Coco, Moreno et al. - Classification of visual and linguistic tasks using eye-movement features
	 * Greene, Michelle R. et al. - Reconsidering Yarbus: a failure to predict observers' task from eye movement patterns
	 * Henderson, John et al. - Predicting cognitive state from eye movements
	 * Torralba, Antonio et al. - Contextual guidance of eye movements and attention in real-world scenes: the role of global features in object search
	 */
	int SearchFeaturesGaze::getFixationCount(std::vector<Fixation> fixations)
	{
		return static_cast<int>(fixations.size());
	}

	/*
	 * Calculate total duration (in seconds) of fixations in window
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 */
	float SearchFeaturesGaze::getFixationDurationTotal(std::vector<float> fixationDurations)
	{
		return MathTools::GetSum(fixationDurations);
	}

	/*
	 * Average duration (in seconds) of one fixation
	 *
	 * based on
	 * Castelhano, Monica et al. - Viewing task influences eye movement control during active scene perception
	 * Coco, Moreno et al. - Classification of visual and linguistic tasks using eye-movement features
	 * Greene, Michelle R. et al. - Reconsidering Yarbus: a failure to predict observers' task from eye movement patterns
	 * Mills, Mark et al. - Examining the influence of task set on eye movements and fixations
	 * Torralba, Antonio et al. - Contextual guidance of eye movements and attention in real-world scenes: the role of global features in object search
	 */
	float SearchFeaturesGaze::getFixationDurationAvg(std::vector<float> fixationDurations)
	{
		return MathTools::GetMean(fixationDurations);
	}

	/*
	 * Minimum fixation duration (in seconds)
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 */
	float SearchFeaturesGaze::getFixationDurationMin(std::vector<float> fixationDurations)
	{
		return MathTools::GetMin(fixationDurations);
	}

	/*
	 * Maximum fixation duration (in seconds)
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 */
	float SearchFeaturesGaze::getFixationDurationMax(std::vector<float> fixationDurations)
	{
		return MathTools::GetMax(fixationDurations);
	}

	/*
	 * Median fixation duration (in seconds)
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 */
	float SearchFeaturesGaze::getFixationDurationMedian(std::vector<float> fixationDurations)
	{
		return MathTools::GetMedian(fixationDurations);
	}

	/*
	 * Standard deviation of fixation duration (in seconds)
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 * Mills, Mark et al. - Examining the influence of task set on eye movements and fixations
	 * Torralba, Antonio et al. - Contextual guidance of eye movements and attention in real-world scenes: the role of global features in object search
	 */
	float SearchFeaturesGaze::getFixationDurationStdDeviation(std::vector<float> fixationDurations)
	{
		return MathTools::GetStdDeviation(fixationDurations);
	}

	/*
	 * Skew of fixation duration
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 */
	float SearchFeaturesGaze::getFixationDurationSkew(std::vector<float> fixationDurations)
	{
		return MathTools::GetSkew(fixationDurations);
	}

	/*
	 * Kurtosis of fixation duration
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 */
	float SearchFeaturesGaze::getFixationDurationKurtosis(std::vector<float> fixationDurations)
	{
		return MathTools::GetKurtosis(fixationDurations);
	}

	/*
	 * Range of fixation duration
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 */
	float SearchFeaturesGaze::getFixationDurationRange(std::vector<float> fixationDurations)
	{
		return MathTools::GetRange(fixationDurations);
	}

	/*
	 * Root mean square of the distances of each fixation from the average position of all fixations
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 */
	float SearchFeaturesGaze::getFixationDispersion(std::vector<Fixation> fixations)
	{
		float fixationDispersion = 0;
		float avgX = 0;
		float avgY = 0;

		size_t fixationCount = fixations.size();

		if (fixationCount > 0)
		{
			// Calculate average fixation position
			for (int i = 0; i < fixationCount; i++)
			{
				avgX += fixations[i].x;
				avgY += fixations[i].y;
			}

			avgX = avgX / fixationCount;
			avgY = avgY / fixationCount;

			// Calculate distances
			for (int i = 0; i < fixationCount; i++)
			{
				fixationDispersion += pow(fixations[i].x - avgX, 2) + pow(fixations[i].y - avgY, 2);
			}

			// Root mean square
			fixationDispersion = sqrt(fixationDispersion / (float) fixationCount);
		}

		return fixationDispersion;
	}

	/*
	 * Average length of one saccade (in pixels)
	 *
	 * based on
	 * Coco, Moreno et al. - Classification of visual and linguistic tasks using eye-movement features
	 * Greene, Michelle R. et al. - Reconsidering Yarbus: a failure to predict observers' task from eye movement patterns
	 * Henderson, John et al. - Predicting cognitive state from eye movements
	 * Mills, Mark et al. - Examining the influence of task set on eye movements and fixations
	 * Torralba, Antonio et al. - Contextual guidance of eye movements and attention in real-world scenes: the role of global features in object search
	 */
	float SearchFeaturesGaze::getSaccadeLengthAvg(std::vector<float> saccadeLengths)
	{
		return MathTools::GetMean(saccadeLengths);
	}

	/*
	 * Minimum length of one saccade (in pixels)
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 */
	float SearchFeaturesGaze::getSaccadeLengthMin(std::vector<float> saccadeLengths)
	{
		return MathTools::GetMin(saccadeLengths);
	}

	/*
	 * Maximum length of one saccade (in pixels)
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 */
	float SearchFeaturesGaze::getSaccadeLengthMax(std::vector<float> saccadeLengths)
	{
		return MathTools::GetMax(saccadeLengths);
	}

	/*
	 * Median length of one saccade (in pixels)
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 */
	float SearchFeaturesGaze::getSaccadeLengthMedian(std::vector<float> saccadeLengths)
	{
		return MathTools::GetMedian(saccadeLengths);
	}

	/*
	 * Standard deviation of saccade length (in pixels)
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 * Henderson, John et al. - Predicting cognitive state from eye movements
	 * Mills, Mark et al. - Examining the influence of task set on eye movements and fixations
	 * Torralba, Antonio et al. - Contextual guidance of eye movements and attention in real-world scenes: the role of global features in object search
	 */
	float SearchFeaturesGaze::getSaccadeLengthStdDeviation(std::vector<float> saccadeLengths)
	{
		return MathTools::GetStdDeviation(saccadeLengths);
	}

	/*
	 * Skew of saccade length
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 */
	float SearchFeaturesGaze::getSaccadeLengthSkew(std::vector<float> saccadeLengths)
	{
		return MathTools::GetSkew(saccadeLengths);
	}

	/*
	 * Kurtosis of saccade length
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 */
	float SearchFeaturesGaze::getSaccadeLengthKurtosis(std::vector<float> saccadeLengths)
	{
		return MathTools::GetKurtosis(saccadeLengths);
	}

	/*
	 * Range of saccade length
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 */
	float SearchFeaturesGaze::getSaccadeLengthRange(std::vector<float> saccadeLengths)
	{
		return MathTools::GetRange(saccadeLengths);
	}

	/*
	* Total saccade length
	*
	* based on
	* Castelhano, Monica et al. - Viewing task influences eye movement control during active scene perception
	*/
	float SearchFeaturesGaze::getSaccadeLengthTotal(std::vector<float> saccadeLengths)
	{
		return MathTools::GetSum(saccadeLengths);
	}

	/*
	 * Average duration of one saccade (in seconds)
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 */
	float SearchFeaturesGaze::getSaccadeDurationAvg(std::vector<float> saccadeDurations)
	{
		return MathTools::GetMean(saccadeDurations);
	}

	/*
	 * Minimum duration of one saccade (in seconds)
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 */
	float SearchFeaturesGaze::getSaccadeDurationMin(std::vector<float> saccadeDurations)
	{
		return MathTools::GetMin(saccadeDurations);
	}

	/*
	 * Maximum duration of one saccade (in seconds)
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 */
	float SearchFeaturesGaze::getSaccadeDurationMax(std::vector<float> saccadeDurations)
	{
		return MathTools::GetMax(saccadeDurations);
	}

	/*
	 * Median duration of one saccade (in seconds)
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 */
	float SearchFeaturesGaze::getSaccadeDurationMedian(std::vector<float> saccadeDurations)
	{
		return MathTools::GetMedian(saccadeDurations);
	}

	/*
	 * Standard deviation of saccade duration (in seconds)
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 */
	float SearchFeaturesGaze::getSaccadeDurationStdDeviation(std::vector<float> saccadeDurations)
	{
		return MathTools::GetStdDeviation(saccadeDurations);
	}

	/*
	 * Skew of saccade duration
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 */
	float SearchFeaturesGaze::getSaccadeDurationSkew(std::vector<float> saccadeDurations)
	{
		return MathTools::GetSkew(saccadeDurations);
	}

	/*
	 * Kurtosis of saccade duration
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 */
	float SearchFeaturesGaze::getSaccadeDurationKurtosis(std::vector<float> saccadeDurations)
	{
		return MathTools::GetKurtosis(saccadeDurations);
	}

	/*
	 * Range of saccade duration
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 */
	float SearchFeaturesGaze::getSaccadeDurationRange(std::vector<float> saccadeDurations)
	{
		return MathTools::GetRange(saccadeDurations);
	}

	/*
	* Total saccade duration (in seconds)
	*/
	float SearchFeaturesGaze::getSaccadeDurationTotal(std::vector<float> saccadeDurations)
	{
		return MathTools::GetSum(saccadeDurations);
	}

	/*
	 * Number of saccades
	 *
	 * based on
	 * Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	 */
	int SearchFeaturesGaze::getSaccadeCount(std::vector<Saccade> saccades)
	{
		return static_cast<int>(saccades.size());
	}

	/*
	* Fixation duration/saccade duration ratio
	*
	* based on
	* Bixler, Robert et al. - Toward Fully Automated Person-Independent Detection of Mind Wandering
	*/
	float SearchFeaturesGaze::getDurationRatio(std::vector<float> fixationDurations, std::vector<float> saccadeDurations)
	{
		float durationRatio = 0;
		float fixationDurationTotal = MathTools::GetSum(fixationDurations);
		float saccadeDurationTotal = MathTools::GetSum(saccadeDurations);

		if (saccadeDurationTotal > 0)
		{
			durationRatio = fixationDurationTotal / saccadeDurationTotal;
		}

		return durationRatio;
	}

	/*
	* Wordbook size
	*
	* based on
	* Bulling, A. et al. - Eye movement analysis for activity recognition using electrooculography
	*/
	int SearchFeaturesGaze::getWordBookSize(std::map<std::string, int> wordBook)
	{
		return static_cast<int>(wordBook.size());
	}

	/*
	* Maximum occurrence count of one word in the wordbook
	*
	* based on
	* Bulling, A. et al. - Eye movement analysis for activity recognition using electrooculography
	*/
	int SearchFeaturesGaze::getWordBookMax(std::map<std::string, int> wordBook)
	{
		int maxOccurrence = 0;

		std::map<std::string, int>::iterator it;
		for (it = wordBook.begin(); it != wordBook.end(); it++)
		{
			if (it->second > maxOccurrence)
			{
				maxOccurrence = it->second;
			}
		}

		return maxOccurrence;
	}

	/*
	* Difference between maximum and minimum occurrence in each wordbook
	*
	* based on
	* Bulling, A. et al. - Eye movement analysis for activity recognition using electrooculography
	*/
	int SearchFeaturesGaze::getWordBookDiff(std::map<std::string, int> wordBook)
	{
		int maxOccurrence = 0;
		int minOccurrence = INT_MAX;
		int diff = 0;

		std::map<std::string, int>::iterator it;
		for (it = wordBook.begin(); it != wordBook.end(); it++)
		{
			if (it->second > maxOccurrence)
			{
				maxOccurrence = it->second;
			}

			if (it->second < minOccurrence)
			{
				minOccurrence = it->second;
			}
		}

		if (maxOccurrence > minOccurrence)
		{
			diff = maxOccurrence - minOccurrence;
		}

		return diff;
	}

	/*
	* Variance of all occurrence counts in each wordbook
	*
	* based on
	* Bulling, A. et al. - Eye movement analysis for activity recognition using electrooculography
	*/
	float SearchFeaturesGaze::getWordBookVar(std::map<std::string, int> wordBook)
	{
		std::vector<float> occurrences;

		std::map<std::string, int>::iterator it;
		for (it = wordBook.begin(); it != wordBook.end(); it++)
		{
			occurrences.push_back((float) it->second);
		}

		return MathTools::GetVariance(occurrences);
	}

	/*
	* Mean of all occurrence counts in each wordbook
	*
	* based on
	* Bulling, A. et al. - Eye movement analysis for activity recognition using electrooculography
	*/
	float SearchFeaturesGaze::getWordBookMean(std::map<std::string, int> wordBook)
	{
		std::vector<float> occurrences;

		std::map<std::string, int>::iterator it;
		for (it = wordBook.begin(); it != wordBook.end(); it++)
		{
			occurrences.push_back((float) it->second);
		}

		return MathTools::GetMean(occurrences);
	}

	/*
	* Percentage of field of view covered with fixations
	*
	* based on
	* Castelhano, Monica et al. - Viewing task influences eye movement control during active scene perception
	*
	* and
	*
	* Number of fixation groups
	*
	* based on
	* Sadasivan, Sajay et al. - Use of Eye Movements as Feedforward Training for a Synthetic Aircraft Inspection Task
	*/
	std::vector<float> SearchFeaturesGaze::getFixationCoverage(std::vector<Fixation> fixations)
	{
		std::vector<float> covered;
		covered.push_back(0);
		covered.push_back(0);

		if (fixations.size() > 0)
		{
			// Initial view size
			int viewWidth = 640;
			int viewHeight = 480;

			// Increase view by those values
			int viewOffsetHorizontal = 320;
			int viewOffsetVertical = 240;

			viewWidth = viewOffsetHorizontal * 2 + viewWidth;
			viewHeight = viewOffsetVertical * 2 + viewHeight;

			int area = viewWidth * viewHeight;

			float minX = 0;
			float maxX = 0;
			float minY = 0;
			float maxY = 0;

			float xScale = 1;
			float yScale = 1;

			cv::Mat view = cv::Mat::zeros(viewHeight, viewWidth, CV_8U);

			// Remap coordinates to view and calculate min and max
			for (size_t i = 0; i < fixations.size(); i++)
			{
				fixations[i].x = (float)((int)fixations[i].x + viewOffsetHorizontal);
				fixations[i].y = (float)((int)fixations[i].y + viewOffsetVertical);

				if (fixations[i].x < minX)
				{
					minX = fixations[i].x;
				}

				if (fixations[i].x > maxX)
				{
					maxX = fixations[i].x;
				}

				if (fixations[i].y < minY)
				{
					minY = fixations[i].y;
				}

				if (fixations[i].y > maxY)
				{
					maxY = fixations[i].y;
				}
			}

			minX = abs(minX);
			minY = abs(minY);

			// Calculate scale factors
			if (maxX + minX > viewWidth)
			{
				xScale = (maxX + minX) / (float)(viewWidth - 1);
			}

			if (maxY + minY > viewHeight)
			{
				yScale = (maxY + minY) / (float)(viewHeight - 1);
			}

			int radius = 0;
			int x = 0;
			int y = 0;

			// Parameters for exponential function y = a / (1 + b * exp(-kx) )
			float maxRadius = 130;
			float a = 1;
			float b = 0.42f;
			float k = 1;

			// Draw circles around fixations
			for (size_t i = 0; i < fixations.size(); i++)
			{
				x = (int)((fixations[i].x + minX) / xScale);
				y = (int)((fixations[i].y + minY) / yScale);

				radius = (int) (a / (1 + b * exp(-1 * k * fixations[i].duration)) * maxRadius);

				cv::circle(view, cv::Point((int)fixations[i].x, (int)fixations[i].y), radius, cv::Scalar(1), CV_FILLED);
			}

			covered[0] = ((float) cv::sum(view)[0] / area) * 100;

			// Detect connected areas
			std::vector<std::vector<cv::Point>> contours;
			cv::findContours(view, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

			covered[1] = (float) contours.size();
		}

		return covered;
	}


	/***********************************************************************************************\
	|* Helper functions
	\***********************************************************************************************/


	/*
	* Helper function to calculate wordbook
	*/
	std::map<std::string, int> SearchFeaturesGaze::getWordBook(std::vector<Saccade> saccades, int wordLength, float lengthThreshold)
	{
		std::map<std::string, int> wordBook;

		std::string currentWord = "";
		std::string currentDirection = "";

		double angle = 0;

		for (size_t i = 0; i < saccades.size(); i++)
		{
			angle = atan2(saccades[i].endY - saccades[i].startY, saccades[i].endX - saccades[i].startX) * 180 / PI;
			
			currentDirection = "";

			if (saccades[i].length <= lengthThreshold)
			{
				// Inner directions
				if (angle >= -22.5 && angle < 22.5)
				{
					currentDirection = "r";
				}
				else if (angle >= 22.5 && angle < 67.5)
				{
					currentDirection = "b";
				}
				else if (angle >= 67.5 && angle < 112.5)
				{
					currentDirection = "u";
				}
				else if (angle >= 112.5 && angle < 157.5)
				{
					currentDirection = "n";
				}
				else if (angle >= 157.5 || angle < -157.5)
				{
					currentDirection = "l";
				}
				else if (angle >= -157.5 && angle < -112.5)
				{
					currentDirection = "j";
				}
				else if (angle >= -112.5 && angle < -67.5)
				{
					currentDirection = "d";
				}
				else if (angle >= -67.5 && angle < -22.5)
				{
					currentDirection = "f";
				}
				else
				{
					currentDirection = "x";
				}
			}
			else
			{
				// Outer directions
				if (angle >= -11.25 && angle < 11.25)
				{
					currentDirection = "R";
				}
				else if (angle >= 11.25 && angle < 33.75)
				{
					currentDirection = "C";
				}
				else if (angle >= 33.75 && angle < 56.25)
				{
					currentDirection = "B";
				}
				else if (angle >= 56.25 && angle < 78.75)
				{
					currentDirection = "A";
				}
				else if (angle >= 78.75 && angle < 101.25)
				{
					currentDirection = "U";
				}
				else if (angle >= 101.25 && angle < 123.75)
				{
					currentDirection = "O";
				}
				else if (angle >= 123.75 && angle < 146.25)
				{
					currentDirection = "N";
				}
				else if (angle >= 146.25 && angle < 168.75)
				{
					currentDirection = "M";
				}
				else if (angle >= 168.75 || angle < -168.75)
				{
					currentDirection = "L";
				}
				else if (angle >= -168.75 || angle < -146.25)
				{
					currentDirection = "K";
				}
				else if (angle >= -146.25 || angle < -123.75)
				{
					currentDirection = "J";
				}
				else if (angle >= -123.75 || angle < -101.25)
				{
					currentDirection = "H";
				}
				else if (angle >= -101.25 || angle < -78.75)
				{
					currentDirection = "D";
				}
				else if (angle >= -78.75 || angle < -56.25)
				{
					currentDirection = "G";
				}
				else if (angle >= -56.25 || angle < -33.75)
				{
					currentDirection = "F";
				}
				else if (angle >= -33.75 || angle < -11.25)
				{
					currentDirection = "E";
				}
				else
				{
					currentDirection = "X";
				}
			}


			currentWord += currentDirection;

			// Add word
			if (currentWord.length() == wordLength)
			{
				if (wordBook.find(currentWord) != wordBook.end())
				{
					// Insert new word
					wordBook.insert(std::make_pair(currentWord, 1));
				}
				else
				{
					// Increment word count
					wordBook[currentWord] += 1;
				}

				// Reset current word
				currentWord = "";
			}
		}

		return wordBook;
	}

	/*
	 * Helper function to get saccade lenghts as list
	 */
	std::vector<float> SearchFeaturesGaze::getSaccadeLenghts(std::vector<Saccade> saccades)
	{
		std::vector<float> saccadeLenghts;

		for (size_t i = 0; i < saccades.size(); i++)
		{
			saccadeLenghts.push_back(saccades[i].length);
		}

		return saccadeLenghts;
	}

	/*
	 * Helper function to get saccade durations as list
	 */
	std::vector<float> SearchFeaturesGaze::getSaccadeDurations(std::vector<Saccade> saccades)
	{
		std::vector<float> saccadeDurations;

		for (size_t i = 0; i < saccades.size(); i++)
		{
			saccadeDurations.push_back(saccades[i].duration);
		}

		return saccadeDurations;
	}

	/*
	 * Helper function to get fixation durations as list
	 */
	std::vector<float> SearchFeaturesGaze::getFixationDurations(std::vector<Fixation> fixations)
	{
		std::vector<float> fixationDurations;

		for (size_t i = 0; i < fixations.size(); i++)
		{
			fixationDurations.push_back(fixations[i].duration);
		}

		return fixationDurations;
	}

	/*
	 * Helper function to detect saccades
	 */
	std::vector<Saccade> SearchFeaturesGaze::getSaccades(ssi_real_t *ptr_in, ssi_size_t sample_number, ssi_size_t sample_dimension, ssi_time_t sr)
	{
		std::vector<Saccade> saccades;

		float avgSaccadeLength = 0;

		float x1 = 0;
		float y1 = 0;
		float x2 = 0;
		float y2 = 0;
		int i = 0;
		int j = 0;
		int start = 0;
		int end = 0;

		bool fixationDetected = false;

		for (ssi_size_t k = 0; k < sample_number; k++)
		{
			i = sample_dimension * k; // i = current sample
			j = i + sample_dimension; // j = next sample

			// Current sample
			x1 = ptr_in[i + SEARCHFEATURES_GAZE_DIM_SCENEGAZE_X];
			y1 = ptr_in[i + SEARCHFEATURES_GAZE_DIM_SCENEGAZE_Y];

			// Next sample
			x2 = ptr_in[j + SEARCHFEATURES_GAZE_DIM_SCENEGAZE_X];
			y2 = ptr_in[j + SEARCHFEATURES_GAZE_DIM_SCENEGAZE_Y];

			if (isFixation(x1, y1, x2, y2))
			{
				if (fixationDetected == false)
				{
					fixationDetected = true;

					// Saccade found (between two fixations)
					if (start > 0)
					{
						end = i;

						Saccade foundSaccade;

						foundSaccade.startX = ptr_in[start + SEARCHFEATURES_GAZE_DIM_SCENEGAZE_X];
						foundSaccade.startY = ptr_in[start + SEARCHFEATURES_GAZE_DIM_SCENEGAZE_Y];
						foundSaccade.endX = ptr_in[end + SEARCHFEATURES_GAZE_DIM_SCENEGAZE_X];
						foundSaccade.endY = ptr_in[end + SEARCHFEATURES_GAZE_DIM_SCENEGAZE_Y];

						foundSaccade.length = sqrt(pow(foundSaccade.endX - foundSaccade.startX, 2) + pow(foundSaccade.endY - foundSaccade.startY, 2));
						foundSaccade.duration = static_cast<float>(static_cast<float>(end - start) / sr / (float) sample_dimension);

						saccades.push_back(foundSaccade);

						start = 0;
						end = 0;
					}
				}

				start = j;
			}
			else
			{
				fixationDetected = false;
			}
		}

		return saccades;
	}

	/*
	 * Helper function to detect fixations
	 */
	std::vector<Fixation> SearchFeaturesGaze::getFixations(ssi_real_t *ptr_in, ssi_size_t sample_number, ssi_size_t sample_dimension, ssi_time_t sr)
	{
		std::vector<Fixation> fixations;

		int fixationSamples = 0;

		float x1 = 0;
		float y1 = 0;
		float x2 = 0;
		float y2 = 0;
		int i = 0;
		int j = 0;

		bool fixationDetected = false;

		for (ssi_size_t k = 0; k < sample_number; k++)
		{
			i = sample_dimension * k; // i = current sample
			j = i + sample_dimension; // j = next sample

			// Current sample
			x1 = ptr_in[i + SEARCHFEATURES_GAZE_DIM_SCENEGAZE_X];
			y1 = ptr_in[i + SEARCHFEATURES_GAZE_DIM_SCENEGAZE_Y];

			// Next sample
			x2 = ptr_in[j + SEARCHFEATURES_GAZE_DIM_SCENEGAZE_X];
			y2 = ptr_in[j + SEARCHFEATURES_GAZE_DIM_SCENEGAZE_Y];

			// if (ptr_in[j] < 0 || ptr_in[j + 1] < 0) break;

			if (isFixation(x1, y1, x2, y2))
			{
				if (fixationDetected == false)
				{
					Fixation fixation;
					fixation.x = x1;
					fixation.y = y1;
					fixation.duration = 0;

					fixations.push_back(fixation);

					// Reset fixation sample counter
					fixationSamples = 0;

					fixationDetected = true;
				}

				// Increase fixation sample counter
				fixationSamples++;

				fixations.back().duration = static_cast<float>((static_cast<float>(fixationSamples) / sr));
			}
			else
			{

				fixationDetected = false;
			}
		}

		return fixations;
	}

	/*
	 * Helper function to detect fixations
	 */
	bool SearchFeaturesGaze::isFixation(ssi_real_t x1, ssi_real_t y1, ssi_real_t x2, ssi_real_t y2)
	{
		bool isFixation = false;

		if (abs(x2 - x1) < SEARCHFEATURES_GAZE_SACCADE_THRESHOLD && abs(y2 - y1) < SEARCHFEATURES_GAZE_SACCADE_THRESHOLD)
		{
			isFixation = true;
		}

		return isFixation;
	}
}
