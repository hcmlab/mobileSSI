// Main.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2015/05/21
// Copyright (C) 2007-14 University of Augsburg, Lab for Human Centered Multimedia
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

#include "ssi.h"
#include "ssiml/include/ssiml.h"
#include "audio/include/ssiaudio.h"
using namespace ssi;

#ifdef USE_SSI_LEAK_DETECTOR
#include "SSI_LeakWatcher.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

ssi_char_t _workdir[SSI_MAX_CHAR];

struct params_s {
	ssi_char_t *taskname; // task name (extracted from output name of sample list)
	ssi_char_t *source_dir; // source dir of raw files
	ssi_char_t *list_path; // path of annotation file	
	ssi_char_t *target_dir; // path to final sample list
	bool anno_has_header; // annotation file has a header
	int anno_index_file; // index of filename in annotation
	int anno_index_class; // index of class in annnoation
	ssi_size_t n_classes; // number of mapping
	ssi_char_t *mapping_s; // final class names (seperated by comma)
	ssi_char_t *classes_s; // class names in annotation file (seperated by comma)
	ssi_char_t **mapping; // final class names (split)
	ssi_char_t **classes; // class names in annotation file (split)
	bool raw_format; // audio files are in raw format
	int sample_rate; // sample rate if audio files in raw format
	ssi_char_t *delim; // delim character in annotation file
	double silence; // silence between wav files	
	int n_test; // test files
	ssi_char_t *srcurl;
	bool merge_files;
	ssi_char_t *merge_name;
};

bool COMBINE(params_s &params);
bool VAD(params_s &params);
bool MERGE(params_s &params);

int main(int argc, char **argv) {

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

		char info[1024];
		ssi_sprint(info, "\n%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

#if !_DEBUG && defined _MSC_VER && _MSC_VER == 1900	
		const ssi_char_t *default_source = "https://github.com/hcmlab/ssi/raw/master/bin/x64/vc140";
#else
		const ssi_char_t *default_source = "";
#endif

		ssi_sprint(ssi_log_name, "vadanno___");		

		//**** READ COMMAND LINE ****//

		CmdArgParser cmd;
		cmd.info(info);

		params_s params;
		params.srcurl = 0;
		params.taskname = 0;
		params.source_dir = 0;
		params.list_path = 0;
		params.target_dir = 0;
		params.anno_has_header = false;
		params.anno_index_file = 0;
		params.anno_index_class = 1;
		params.n_classes = 0;
		params.mapping_s = 0;
		params.classes_s = 0;
		params.mapping = 0;
		params.classes = 0;
		params.raw_format = false;
		params.sample_rate = 0;
		params.delim = 0;
		params.silence = 0;
		params.merge_files = false;
		params.merge_name = 0;

		cmd.addText("\nArguments:");
		cmd.addSCmdArg("srcDir", &params.source_dir, "source directory with audio files (filenames in <list> can be relative to this directory)");
		cmd.addICmdArg("sampleRate", &params.sample_rate, "sample rate (all wav files in source directory have to be sampled at this rate)");
		cmd.addSCmdArg("dstDir", &params.target_dir, "output directory where combined wav files and annotation will be stored");
		cmd.addSCmdArg("list", &params.list_path, "path to csv file(s) containing filenames and class labels, see <fileIndex> and <classIndex> (if several files separate by ';')");
		cmd.addSCmdArg("classes", &params.classes_s, "class names separated by ; as they occur in <list> (classes not listed here will be ignored)");

		cmd.addText("\nOptions:");
		cmd.addSCmdOption("-mapping", &params.mapping_s, "", "optional mapping to new class names");
		cmd.addBCmdOption("-merge", &params.merge_files, false, "merge wav files and annotations afterwards");
		cmd.addSCmdOption("-mergeName", &params.merge_name, "merge", "name of merged files");
		cmd.addBCmdOption("-header", &params.anno_has_header, false, "<list> file starts with a header");
		cmd.addSCmdOption("-delim", &params.delim, ";", "delimiter character in <list> file");
		cmd.addICmdOption("-fileIndex", &params.anno_index_file, 0, "column index of filename in annotation file");
		cmd.addICmdOption("-classIndex", &params.anno_index_class, 1, "column index of class in annotation file");
		cmd.addBCmdOption("-rawAudio", &params.raw_format, false, "audio files are in raw format");
		cmd.addDCmdOption("-silence", &params.silence, 0, "add silence between audio files (in seconds)");
		cmd.addICmdOption("-nTest", &params.n_test, 0, "take out n files per class and create a test wav (will be stored in output directory as 'test.wav')");
		cmd.addSCmdOption("-url", &params.srcurl, default_source, "override default url for downloading missing dlls and dependencies");

		if (cmd.read(argc, argv)) {

			// set directories
			FilePath exepath_fp(argv[0]);
			ssi_getcwd(SSI_MAX_CHAR, _workdir);
			ssi_char_t exedir[SSI_MAX_CHAR];
			if (exepath_fp.isRelative()) {
#if _WIN32|_WIN64
				ssi_sprint(exedir, "%s\\%s", _workdir, exepath_fp.getDir());
#else
				ssi_sprint(exedir, "%s/%s", _workdir, exepath_fp.getDir());
#endif
			}
			else {
				strcpy(exedir, exepath_fp.getDir());
			}
			ssi_print("download source=%s\ndownload target=%s\n\n", params.srcurl, exedir);
			Factory::SetDownloadDirs(params.srcurl, exedir);

			Factory::RegisterDLL("audio");
			Factory::RegisterDLL("event");
			Factory::RegisterDLL("ioput");

			FilePath fp(params.target_dir);
			params.taskname = ssi_strcpy(fp.getName());

			// split classes and mapping
			params.n_classes = ssi_split_string_count(params.classes_s, ';');
			params.classes = new ssi_char_t *[params.n_classes];
			params.mapping = new ssi_char_t *[params.n_classes];
			ssi_split_string(params.n_classes, params.classes, params.classes_s, ';');

			if (params.mapping_s[0] != '\0') {
				ssi_split_string(params.n_classes, params.mapping, params.mapping_s, ';');
			}
			else {
				for (ssi_size_t i = 0; i < params.n_classes; i++) {
					params.mapping[i] = ssi_strcpy(params.classes[i]);
				}
			}

			if (COMBINE(params) && VAD(params) && MERGE(params)) {
				ssi_msg(SSI_LOG_LEVEL_BASIC, "OK");
			}
		}

		delete[] params.taskname;
		delete[] params.source_dir;
		delete[] params.list_path;
		delete[] params.classes_s;
		delete[] params.mapping_s;
		for (ssi_size_t i = 0; i < params.n_classes; i++) {
			delete[] params.mapping[i];
			delete[] params.classes[i];
		}
		delete[] params.classes;
		delete[] params.mapping;
		delete[] params.target_dir;
		delete[] params.delim;
		delete[] params.srcurl;
		delete[] params.merge_name;

		Factory::Clear();

#ifdef USE_SSI_LEAK_DETECTOR
	}
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}

bool COMBINE(params_s &params) {

	ssi_msg(SSI_LOG_LEVEL_BASIC, "COMBINE AUDIO FILES (%s->%s)", params.source_dir, params.target_dir);

	// check if parameters are valid
	if (!ssi_exists_dir(params.source_dir)) {
		ssi_wrn("invalid source directory '%s'", params.source_dir);
		return false;
	}

	ssi_mkdir_r(params.target_dir, SSI_PATH_SEPARATOR);

	ssi_size_t n_annos = ssi_split_string_count(params.list_path, ';');
	if (n_annos == 0) {
		ssi_wrn("invalid list path '%s'", params.list_path);
		return false;
	}

	ssi_char_t **annos = new ssi_char_t *[n_annos];
	ssi_split_string(n_annos, annos, params.list_path, ';');

	// create wav per class
	WAVEFORMATEX wavformat = ssi_create_wfx(params.sample_rate, 1, sizeof(short));
	ssi_char_t **wavpath = new ssi_char_t *[params.n_classes];
	File **wavfile = new File *[params.n_classes];
	for (ssi_size_t i = 0; i < params.n_classes; i++) {
		wavpath[i] = new ssi_char_t[SSI_MAX_CHAR];
		ssi_sprint(wavpath[i], "%s/%s.wav", params.target_dir, params.mapping[i]);
		wavfile[i] = 0;
		wavfile[i] = File::CreateAndOpen(File::BINARY, File::WRITE, wavpath[i]);
		if (!ssi_exists(wavpath[i])) {
			ssi_wrn("invalid wav path '%s'", wavpath[i]);
			return false;
		}
		WavTools::WriteWavHeader(*wavfile[i], wavformat, 0); // we put 0 here cause we don't know the final number of samples yet
		WavTools::WriteWavChunk(*wavfile[i], wavformat, 0); // we put 0 here cause we don't know the final number of samples yet
	}

	// create test wav and counter
	ssi_char_t *wavtestpath = 0;
	File *wavtestfile = 0;
	if (params.n_test > 0)
	{
		wavtestpath = ssi_strcat(params.target_dir, "/test.wav");
		wavtestfile = File::CreateAndOpen(File::BINARY, File::WRITE, wavtestpath);
		if (ssi_exists(wavtestpath))
		{
			WavTools::WriteWavHeader(*wavtestfile, wavformat, 0); // we put 0 here cause we don't know the final number of samples yet
			WavTools::WriteWavChunk(*wavtestfile, wavformat, 0); // we put 0 here cause we don't know the final number of samples yet
		}
	}
	int *testcounter = new int[params.n_classes];
	for (ssi_size_t i = 0; i < params.n_classes; i++)
	{
		testcounter[i] = params.n_test;
	}

	// silence
	ssi_size_t n_silence = 0;
	short *silence = 0;
	if (params.silence > 0) {
		n_silence = ssi_cast(ssi_size_t, params.silence * params.sample_rate);
		silence = new short[n_silence];
		memset(silence, 0, n_silence * sizeof(short));
	}

	for (ssi_size_t i = 0; i < n_annos; i++) {

		if (!ssi_exists(annos[i])) {
			ssi_wrn("invalid annotation path '%s'", annos[i]);
			return false;
		}

		// read annotation file
		FileCSV anno;
		anno.parseFile(annos[i], params.delim[0], params.anno_has_header);
		FileCSV::Column label = anno[params.anno_index_class];
		FileCSV::Column file = anno[params.anno_index_file];
		FileCSV::Column::iterator it_label = label.begin();
		FileCSV::Column::iterator it_file = file.begin();
		FILE *fp = 0;
		size_t n_bytes = 0;
		ssi_byte_t *bytes = 0;
		for (; it_label != label.end(); it_label++, it_file++) {

			ssi_msg(SSI_LOG_LEVEL_BASIC, "process %s[%s]", *it_file, *it_label);
			ssi_char_t *audiopath = 0;

			if (ssi_exists(*it_file))
			{
				audiopath = ssi_strcpy(*it_file);
			}
			else
			{
				audiopath = ssi_strcat(params.source_dir, "/", *it_file);
			}

			if (!ssi_exists(audiopath)) {
				ssi_wrn("invalid path '%s'", audiopath);
				return false;
			}

			if (params.raw_format) {
				for (ssi_size_t i = 0; i < params.n_classes; i++) {
					if (ssi_strcmp(params.classes[i], *it_label)) {
						fp = fopen(audiopath, "rb");
						fseek(fp, 0, SEEK_END);
						n_bytes = ftell(fp);
						fseek(fp, 0, SEEK_SET);
						bytes = new ssi_byte_t[n_bytes];
						fread(bytes, sizeof(ssi_byte_t), n_bytes, fp);
						if (testcounter[i] > 0)
						{
							wavtestfile->write(bytes, sizeof(ssi_byte_t), n_bytes);
						}
						else
						{
							wavfile[i]->write(bytes, sizeof(ssi_byte_t), n_bytes);
						}						
						fclose(fp);
						delete[] bytes;

						if (n_silence > 0) {
							if (testcounter[i] > 0)
							{
								wavtestfile->write(silence, sizeof(short), n_silence);
							}
							else
							{
								wavfile[i]->write(silence, sizeof(short), n_silence);
							}
						}
						
						testcounter[i]--;

						break;
					}
				}
			}
			else {
				for (ssi_size_t i = 0; i < params.n_classes; i++) {
					if (ssi_strcmp(params.classes[i], *it_label)) {
						ssi_stream_t audio;
						if (WavTools::ReadWavFile(audiopath, audio, false))
						{
							if ((int)audio.sr != params.sample_rate)
							{
								ssi_wrn("sample rates do not match '%d!=%d'", (int)audio.sr, params.sample_rate);
							}
							if (testcounter[i] > 0)
							{
								wavtestfile->write(audio.ptr, sizeof(ssi_byte_t), audio.tot);
							}
							else
							{
								wavfile[i]->write(audio.ptr, sizeof(ssi_byte_t), audio.tot);
							}							
							ssi_stream_destroy(audio);

							if (n_silence > 0)
							{			
								if (testcounter[i] > 0)
								{
									wavtestfile->write(silence, sizeof(short), n_silence);
								}
								else
								{
									wavfile[i]->write(silence, sizeof(short), n_silence);
								}								
							}
						}

						testcounter[i]--;

						break;
					}
				}
			}
			delete[] audiopath;
		}
	}

	// we close the wav files
	for (ssi_size_t i = 0; i < params.n_classes; i++) {
		delete wavfile[i];
	}
	delete[] wavfile;
	delete wavtestfile;
	delete[] testcounter;

	// and finally repair them to set the correct sample numbers
	for (ssi_size_t i = 0; i < params.n_classes; i++) {
		ssi_stream_t data;
		if (!(WavTools::RepairWavFile(wavpath[i], data) && WavTools::WriteWavFile(wavpath[i], data)))		
		{
			ssi_wrn("could not write '%s'", wavpath[i]);
			ssi_remove(wavpath[i]);
		}
		ssi_stream_destroy(data);
	}
	if (wavtestpath)
	{
		ssi_stream_t data;
		WavTools::RepairWavFile(wavtestpath, data);
		WavTools::WriteWavFile(wavtestpath, data);
		ssi_stream_destroy(data);
	}

	// clean up
	for (ssi_size_t i = 0; i < params.n_classes; i++) {
		delete[] wavpath[i];
	}
	delete[] wavpath;
	delete[] wavtestpath;

	for (ssi_size_t i = 0; i < n_annos; i++) {
		delete[] annos[i];
	}
	delete[] annos;

	return true;
}

bool VAD(params_s &params) {

	ssi_msg(SSI_LOG_LEVEL_BASIC, "APPLY VAD (%s)", params.target_dir);

	// check if parameters are valid
	if (!ssi_exists_dir(params.target_dir)) {
		ssi_wrn("invalid wav directory '%s'", params.target_dir);
		return false;
	}

	ssi_char_t wavpath[SSI_MAX_CHAR];
	ssi_char_t annopath[SSI_MAX_CHAR];

	ssi_char_t *activity_options_path = ssi_strcat(params.source_dir, "\\activity.option");
	AudioActivity *activity = 0;
	if (ssi_exists(activity_options_path))
	{
		activity = ssi_create(AudioActivity, activity_options_path, false);
	}
	else
	{
		activity = ssi_create(AudioActivity, activity_options_path, false);
		activity->getOptions()->threshold = 0.05;
	}
	delete[] activity_options_path;

	TriggerEventSender *trigger = 0;
	ssi_char_t *trigger_options_path = ssi_strcat(params.source_dir, "\\trigger.option");
	if (ssi_exists(trigger_options_path))
	{
		trigger = ssi_create(TriggerEventSender, trigger_options_path, false);
	}
	else
	{
		trigger = ssi_create(TriggerEventSender, trigger_options_path, false);
		trigger->getOptions()->setAddress("vad@audio");
		trigger->getOptions()->triggerType = TriggerEventSender::TRIGGER::NOT_EQUAL;
		trigger->getOptions()->minDuration = 0.1;
		trigger->getOptions()->maxDuration = 5.0;
		trigger->getOptions()->incDuration = 5.0;
		trigger->getOptions()->hangInSamples = 3;
		trigger->getOptions()->hangOutSamples = 10;
	}
	delete[] trigger_options_path;

	Annotation annotation;
	annotation.setDiscreteScheme(params.taskname, params.n_classes, (const ssi_char_t **)params.mapping);

	for (ssi_size_t i = 0; i < params.n_classes; i++) {

		ssi_sprint(wavpath, "%s/%s.wav", params.target_dir, params.mapping[i]);
		if (!ssi_exists(wavpath)) {
			ssi_wrn("could not read '%s'", wavpath);
			continue;
		}

		// read data
		ssi_stream_t data;
		WavTools::ReadWavFile(wavpath, data, true);

		// apply activity
		ssi_stream_t vad;
		SignalTools::Transform(data, vad, *activity, ssi_cast(ssi_size_t, 0.03*data.sr + 0.5), ssi_cast(ssi_size_t, 0.015*data.sr + 0.5));

		// store annotations
		annotation.clear();
		ssi_sprint(annopath, "%s/%s", params.target_dir, params.mapping[i]);
		FileAnnotationWriter *writer = ssi_create(FileAnnotationWriter, 0, false);
		writer->getOptions()->setDefaultLabel(params.mapping[i]);
		writer->setAnnotation(&annotation);
		trigger->setEventListener(writer);
		writer->listen_enter();
		SignalTools::Consume(vad, *trigger, ssi_cast(ssi_size_t, 0.1*vad.sr + 0.5));
		writer->listen_flush();
		annotation.save(annopath, File::ASCII);
		delete writer;

		// clean up
		ssi_stream_destroy(vad);
		ssi_stream_destroy(data);
	}

	delete activity;
	delete trigger;

	return true;
}

bool MERGE(params_s &params)
{
	if (params.merge_files)
	{
		ssi_char_t wavpath[SSI_MAX_CHAR];
		ssi_char_t wavpath_merge[SSI_MAX_CHAR];
		ssi_char_t annopath[SSI_MAX_CHAR];
		ssi_char_t annopath_merge[SSI_MAX_CHAR];

		// create merged wav

		ssi_sprint(wavpath_merge, "%s/%s.wav", params.target_dir, params.merge_name);				
		WAVEFORMATEX wavformat = ssi_create_wfx(params.sample_rate, 1, sizeof(short));

		File *wavfile_merge = File::CreateAndOpen(File::BINARY, File::WRITE, wavpath_merge);
		if (!ssi_exists(wavpath_merge)) {
			ssi_wrn("invalid wav path '%s'", wavpath_merge);
			return false;
		}
		WavTools::WriteWavHeader(*wavfile_merge, wavformat, 0); // we put 0 here cause we don't know the final number of samples yet
		WavTools::WriteWavChunk(*wavfile_merge, wavformat, 0); // we put 0 here cause we don't know the final number of samples yet

		// create merged annotation

		ssi_sprint(annopath_merge, "%s/%s.annotation", params.target_dir, params.merge_name);
		Annotation annotation_merge;		
		annotation_merge.setDiscreteScheme(params.merge_name, params.n_classes, (const ssi_char_t **)params.mapping);

		// merge

		ssi_time_t offset = 0;

		for (ssi_size_t i = 0; i < params.n_classes; i++) {

			ssi_sprint(wavpath, "%s/%s.wav", params.target_dir, params.mapping[i]);
			if (!ssi_exists(wavpath)) {
				ssi_wrn("could not read '%s'", wavpath);
				continue;
			}

			// add wav data

			ssi_stream_t data;
			WavTools::ReadWavFile(wavpath, data, false);
			wavfile_merge->write(data.ptr, sizeof(ssi_byte_t), data.tot);

			// add annotations
		
			ssi_sprint(annopath, "%s/%s", params.target_dir, params.mapping[i]);
			Annotation annotation;
			annotation.load(annopath);			
			annotation.addOffset(offset, offset);
			annotation_merge.add(annotation);						

			// shift offset

			offset += data.num / data.sr;

			// clean up			
			
			ssi_stream_destroy(data);
		}

		// close wav file

		delete wavfile_merge;

		// save annotation

		annotation_merge.save(annopath_merge, File::ASCII);

		// repair merged wav file and save

		ssi_stream_t data;
		WavTools::RepairWavFile(wavpath_merge, data);
		WavTools::WriteWavFile(wavpath_merge, data);
		ssi_stream_destroy(data);
	}

	return true;
}