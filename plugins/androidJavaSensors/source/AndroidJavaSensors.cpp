// AndroidJavaSensors.cpp
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


#include "AndroidJavaSensors.h"

#include "frame/include/TheFramework.h"
#include "base/Factory.h"
#include <sstream>
#include "string.h"
#include <android/log.h>

extern "C"
{
	
        jint JNI_OnLoad(JavaVM* aVm, void* aReserved)
        {
                return JNI_VERSION_1_6;
        }
	
	void Java_de_hcmlab_ssi_android_1xmlpipe_SensorCollectorService_addEvent
													(JNIEnv* env,
													jobject thiz,
													jstring in_eventSender,
													jstring in_eventName,
													jstring in_eventText
													)
	{
			const char *eventSender = env->GetStringUTFChars(in_eventSender, 0);
			const char *eventName = env->GetStringUTFChars(in_eventName, 0);
			const char *eventText = env->GetStringUTFChars(in_eventText, 0);

                if(!ssi::Factory::isFactoryNull())
		
                { bool framework_running=false;
                    ssi::ITheFramework *frame = ssi::Factory::GetFramework ();

                    if(frame)
                    {
                        if(frame->GetElapsedTimeMs())
                            framework_running=true;
                    }

                    if(framework_running)
                    {
			//ssi::AndroidJavaSensors::getAndroidJavaSensors();

			ssi::AndroidJavaSensors* ajavaSensors= ssi::AndroidJavaSensors::getAndroidJavaSensorsFromJava();
			
			ssi_print("called j sensor graffl\n");
			__android_log_print(ANDROID_LOG_INFO, "SSI", "|||||or must i w3arn %s %s %s",eventSender , eventName, eventText);
			
			if(ajavaSensors)
			{
				ajavaSensors->addJavaEvent(eventSender,eventName, eventText );
                        }

                    }
			
			
		}

		
			env->ReleaseStringUTFChars(in_eventSender, eventSender);
			env->ReleaseStringUTFChars(in_eventName, eventName);
			env->ReleaseStringUTFChars(in_eventText, eventText);
	}

} //extern c

namespace ssi
{
	//initialize
	AndroidJavaSensors* AndroidJavaSensors::androidJavaSensorsInstance=NULL;
	
	
	void AndroidJavaSensors::addJavaEvent(const char* sender, const char* name, const char* text)
	{
		ITheFramework *frame = Factory::GetFramework ();
		
		ssi_char_t *ptr = ssi_pcast(ssi_char_t, _event_batt.ptr);
		
		//char* eventStr=new char[strlen(sender)+strlen(name)+ strlen(text)+4];
		memset(ptr, 0, 64);
		strncpy(ptr, sender,64);
		strncat(ptr, ";",64);
		strncat(ptr, name,64);
		strncat(ptr, ";",64);
		strncat(ptr,text,64);
		
		
		


		_event_batt.time=frame->GetElapsedTimeMs();

		ssi_print("ow ow\n");
		__android_log_print(ANDROID_LOG_INFO, "SSI", ">>>>>>>>>>>>>>> or must i2 warn%s", ptr);
		
		if (_elistener)
		{
			__android_log_print(ANDROID_LOG_INFO, "SSI","}}}}}}}}}}}}}}LISTENER WE hAVe####\n");
				_elistener->update(_event_batt);
				
		}

	//delete[] eventStr;
	
	}
	
	bool AndroidJavaSensors::setEventListener(IEventListener *listener) {
		_elistener = listener;
  
		ssi_size_t sender_id = Factory::AddString(_options.sname);

		std::stringstream str;

		if (sender_id == SSI_FACTORY_STRINGS_INVALID_ID)
			return false;


		if (_options.activeSensors[0]||true) {


			_event_batt.sender_id = sender_id;
			_event_batt.event_id = Factory::AddString("battery");

			if (_event_batt.event_id == SSI_FACTORY_STRINGS_INVALID_ID)
				return false;

			str << "battery" << ",";
			_event_batt.prob = 1.0;
		}
               

		_event_address.setSender(_options.sname);
		_event_address.setEvents(str.str().c_str());



		return true;
	}



	
	AndroidJavaSensors::AndroidJavaSensors(const char* file)
	                : _file(0)
        {
		ssi_print("#### ey ey\n");
		__android_log_print(ANDROID_LOG_INFO, "SSI", "####or museyt i warn");

		if (file) {
			if (!OptionList::LoadXML(file, _options)) {
				OptionList::SaveXML(file, _options);
			}
			_file = ssi_strcpy(file);
		}
			if (_options.activeSensors[0]||true)
			{

				ssi_event_init(_event_batt, SSI_ETYPE_STRING);
				ssi_event_adjust(_event_batt, 64 * sizeof(ssi_char_t));
			}
		
	}
	AndroidJavaSensors::~AndroidJavaSensors()
	{
		androidJavaSensorsInstance=0;
	}
	
}
