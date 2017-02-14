// AndroidSensors.h
// author: Simon Flutura <simon.flutura@informatik.uni-augsburg.de>
// created: 25/3/2015
// Copyright (C) 2007-14 University of Augsburg, Lab for Human Centered Multimedia
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

#ifndef SSI_ANDROIDSENSORS_MYANDROIDSENSORS_H
#define SSI_ANDROIDSENSORS_MYANDROIDSENSORS_H

#include "base/IObject.h"
#include "ioput/option/OptionList.h"
#include "event/EventAddress.h"
#include "thread/Lock.h"
#include "thread/Thread.h"
#include "base/Factory.h"
#include "base/IProvider.h"
#include <android/sensor.h>
#include <sstream>
#include <jni.h>
#include "androidsensorchannels.h"

#define NSENSORS 6

#define SENSOR_INDEX_ACCELEROMETER       0
#define SENSOR_INDEX_MAGNETIC_FIELD      1
#define SENSOR_INDEX_GYROSCOPE           2
#define SENSOR_INDEX_LIGHT               3
#define SENSOR_INDEX_PROXIMITY           4
#define SENSOR_INDEX_HEART_RATE          5




#define LOOPER_ID 3

extern "C"
{
void Java_de_hcmlab_ssi_android_1xmlpipe_DummyActivity_addEvent(JNIEnv* env, jobject thiz, jstring in_eventSender, jstring in_eventName, jstring in_eventText);
}

namespace ssi {



enum SensorType {
	SENSOR_UNKNOWN = -1,
	SENSOR_ACCELEROMETER      = 0,
	SENSOR_MAGNETIC_FIELD     = 1,
	SENSOR_GYROSCOPE          = 2,
	SENSOR_LIGHT              = 3,
	SENSOR_PROXIMITY          = 4,
        SENSOR_HEART_RATE         = 5,
	NUM_SENSOR_TYPE,
	ALL
};






 class AndroidSensor {
		public:
				AndroidSensor(int sensortype, bool isVector, const ASensor* asensor): sensortype(sensortype), isVector(isVector) , asensor(asensor) {
						memset(&lastEvent, 0, sizeof(ASensorEvent));

						lastEvent.type = sensortype;

				};

				ASensorEvent lastEvent;
				const ASensor* asensor;
				int sensortype;
				bool isVector;
};


 class AndroidSensors : public ISensor, public Thread {

	public:

		class Options : public OptionList {

		public:

                        Options() : pollMs(100), sr(100) {
				setSender("AndroidSensors");
				setAllSensorsOn();
                                activeSensors[SENSOR_HEART_RATE]=false;
				addOption("sname", sname, SSI_MAX_CHAR, SSI_CHAR, "name of sender (if sent to event board)");
				addOption("sensorpoll", &pollMs, 1, SSI_INT, "default sensor polling time in ms");
                                addOption ("sr", &sr, 1, SSI_DOUBLE, "sample rate in Hz");
                                addOption("activesensors", activeSensors, NSENSORS, SSI_BOOL, "activate / disable sensors [acc, gyr, prox, light, mag, heart]");
			}

			void setSender(const ssi_char_t *sname) {
				if (sname) {
					ssi_strcpy(this->sname, sname);
				}
			}

			void setSensors(bool *list) {
				if (list) {
					memcpy(activeSensors, list, sizeof(bool) * NSENSORS);
				}
			}

			void setAllSensorsOn() {
				for (int i = 0; i < NSENSORS; i++)
					activeSensors[i] = true;
			}

			void setAllSensorsOff() {
				for (int i = 0; i < NSENSORS; i++)
					activeSensors[i] = false;
			}




                        ssi_object_t getType () { return SSI_SENSOR; };


			ssi_char_t sname[SSI_MAX_CHAR];
                        ssi_time_t sr;

			bool activeSensors[NSENSORS];
			ssi_size_t pollMs;

		};

	public:

                static const ssi_char_t *GetCreateName() { return "AndSensors"; };
		static IObject *Create(const ssi_char_t *file) { return new AndroidSensors(file); };
				~AndroidSensors();

		Options *getOptions() { return &_options; };
		const ssi_char_t *getName() { return GetCreateName(); };
		const ssi_char_t *getInfo() { return "android sensors"; };

				void event_loop();
				//handle android input events as well
				//struct android_poll_source*
				void processInputEvents(void* source);
				void androidSensorsCollect();
				void enableSensors(SensorType sensor=ALL);
				void disableSensors(SensorType sensor=ALL);


				bool tryAddAndroidSensor(int type, bool isVector);
				void android2ssiEvent(ASensorEvent event);

				void run();
				void flush();
				void terminate();
				void enter();
                                ssi_size_t getChannelSize ();
                                IChannel *getChannel (ssi_size_t index);
                                bool setProvider (const ssi_char_t *name, IProvider *provider);
                                bool connect ();
                                bool disconnect ();
                                bool start();
                                bool stop();

				ASensorEventQueue* getSensorEventQueue(){return sensorEventQueue;}
				static int get_sensor_events(int fd, int events, void* data);

				//event sender




		//event sender
		IEventListener	*_elistener;
		EventAddress	_event_address;
				ssi_event_t	_event_acc, _event_gyr, _event_light, _event_prox, _event_mag;
		ITheFramework	*_frame;

				ASensorManager* sensorManager;
				ASensorEventQueue* sensorEventQueue;
				std::map<int,AndroidSensor*> sensorList;
				bool stopLoop=false;


				void startSensors();


	protected:

                IProvider* sensorProviders[NSENSORS];
                IChannel* sensorChannels[NSENSORS];

		AndroidSensors(const ssi_char_t *file = 0);
		ssi_char_t *_file;
		Options _options;
		static char ssi_log_name[];

				//struct android_app* app;




	};


}

#endif
