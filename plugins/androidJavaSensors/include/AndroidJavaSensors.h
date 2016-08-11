// AndroidJavaSensors.h
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

#ifndef SSI_ANDROIDJAVASENSORS_H
#define SSI_ANDROIDJAVASENSORS_H
//make androidJavaSensors singleton, so we can access object from java

#define NJSENSORS 3
#include "base/IObject.h"
#include "ioput/option/OptionList.h"
#include "event/EventAddress.h"
#include "thread/Lock.h"

#include "base/Factory.h"


#include <jni.h>


extern "C"
{
	void Java_de_hcmlab_ssi_android_1xmlpipe_SensorCollectorService_addEvent
													(JNIEnv* env,
													jobject thiz,
													jstring in_eventSender,
													jstring in_eventName,
													jstring in_eventText
													);
													
													
	void get_AndroidJavaSensorsFromJava();


}


namespace ssi
{

	class AndroidJavaSensors: public IObject
	{
		public:
		
		class Options : public OptionList {

			public:

				Options() : pollMs(100) {
					setSender("AndroidJavaSensors");
					setAllSensorsOn();
					addOption("sname", sname, SSI_MAX_CHAR, SSI_CHAR, "name of sender (if sent to event board)");

					addOption("activesensors", activeSensors, NJSENSORS, SSI_BOOL, "activate / disable sensors [battery, bluetooth, wlan]");
				}

				void setSender(const ssi_char_t *sname) {
					if (sname) {
						ssi_strcpy(this->sname, sname);
					}
				}

				void setSensors(bool *list) {
					if (list) {
						memcpy(activeSensors, list, sizeof(bool) * NJSENSORS);
					}
				}

				void setAllSensorsOn() {
					for (int i = 0; i < NJSENSORS; i++)
						activeSensors[i] = true;
				}

				void setAllSensorsOff() {
					for (int i = 0; i < NJSENSORS; i++)
						activeSensors[i] = false;
				}

				ssi_char_t sname[SSI_MAX_CHAR];

				bool activeSensors[NJSENSORS];
				ssi_size_t pollMs;

			};
		
		public:
		
		static AndroidJavaSensors* getAndroidJavaSensorsFromJava()
		{
			if(!androidJavaSensorsInstance) return NULL;
			else
			{
				return androidJavaSensorsInstance;
			}
		}
		static AndroidJavaSensors* getAndroidJavaSensors(const ssi_char_t *file)
		{
			if(!androidJavaSensorsInstance)
			{
				androidJavaSensorsInstance=new AndroidJavaSensors(file);
			}
			return androidJavaSensorsInstance;
		}
		
		static const ssi_char_t *GetCreateName() { return "ssi_events_AnJSensors"; };
		static IObject *Create(const ssi_char_t *file) { return getAndroidJavaSensors(file); };
		~AndroidJavaSensors();

		Options *getOptions() { return &_options; };
		const ssi_char_t *getName() { return GetCreateName(); };
		const ssi_char_t *getInfo() { return "android java sensors"; };

		bool setEventListener(IEventListener *listener);
		const ssi_char_t *getEventAddress()
		{
			return _event_address.getAddress();
		}
		
		IEventListener	*_elistener;
		EventAddress	_event_address;
		
		ssi_event_t	_event_batt, _event_bt, _event_wlan;
		ITheFramework	*_frame;

		void addJavaEvent(const char* sender, const char* name, const char* text);
		 
		private:
		
		AndroidJavaSensors(AndroidJavaSensors const&)   = delete;
		void operator=(AndroidJavaSensors const&)  = delete;
		
		protected:
		
		static AndroidJavaSensors* androidJavaSensorsInstance;
		
		AndroidJavaSensors(const ssi_char_t *file=0);

		ssi_char_t *_file;
		Options _options;
		static char ssi_log_name[];

	};


}//namespace ssi



#endif
