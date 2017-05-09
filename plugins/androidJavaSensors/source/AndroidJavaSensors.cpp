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
//#include <android/log.h>

extern "C"
{

bool checkExc(JNIEnv* env) {
 if(env->ExceptionCheck()) {
  env->ExceptionDescribe(); // writes to logcat
  env->ExceptionClear();
  return true;
 }
 ssi_wrn("jni exception occured");
 return false;
}


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *pjvm, void *reserved) {


    return JNI_VERSION_1_6;
}


        //event
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

                //if(!ssi::Factory::isFactoryNull())
		
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



        //stream
        void Java_de_hcmlab_ssi_android_1xmlpipe_SensorCollectorService_updateData
                                                                                                        (JNIEnv* env,
                                                                                                        jobject thiz,
                                                                                                        jstring in_modality,
                                                                                                        jdouble x,
                                                                                                        jdouble y,
                                                                                                        jdouble z
                                                                                                        )
        {
                        const char *modality = env->GetStringUTFChars(in_modality, 0);
                        double _x=x, _y=y, _z=z;


                //if(!ssi::Factory::isFactoryNull())

                { bool framework_running=false;
                    ssi::ITheFramework *frame = ssi::Factory::GetFramework ();

                    if(frame)
                    {
                        if(frame->GetElapsedTimeMs())
                            framework_running=true;
                    }

                    if(framework_running)
                    {

                        ssi::AndroidJavaSensors* ajavaSensors= ssi::AndroidJavaSensors::getAndroidJavaSensorsFromJava();



                        if(ajavaSensors)
                        {
                                ajavaSensors->updateData(modality, _x, _y, _z );
                        }

                    }


                }


                        env->ReleaseStringUTFChars(in_modality, modality);

        }








    jboolean Java_de_hcmlab_ssi_android_1xmlpipe_SensorCollectorService_initC(JNIEnv* env,
                                                                     jobject thiz)
    {


         bool framework_running=false;
            ssi::ITheFramework *frame = ssi::Factory::GetFramework ();

            if(frame)
            {
                if(frame->GetElapsedTimeMs())
                    framework_running=true;
            }

            if(framework_running)
            {

                JavaVM* jvm;
                env->GetJavaVM(&jvm);


                auto randomClass = env->FindClass("de/hcmlab/ssi/android_xmlpipe/SensorCollectorService");
                auto SensorCollectorServiceClass= reinterpret_cast<jclass>( env->NewGlobalRef(randomClass));
                jmethodID sendByteMethod = env->GetStaticMethodID( SensorCollectorServiceClass, "sendByteBroadcast", "([B)V");
                ssi::AndroidJavaSensors::setJMembers(jvm, SensorCollectorServiceClass, sendByteMethod);
                ssi_wrn("SET JAVA OBJ");
                return JNI_TRUE;



            }



        ssi_wrn("FAILED SET JAVA OBJ");
        return JNI_FALSE;
    }

    jboolean jni_attach_thread(JNIEnv** env, JavaVM* jVM)
    {
        if (jVM->GetEnv( (void**) env, JNI_VERSION_1_4) != JNI_OK)
        {

            jVM->AttachCurrentThread( (void**) env, NULL);

            if (jVM->GetEnv( (void**) env, JNI_VERSION_1_4) != JNI_OK)
            {
                ssi_wrn(  "android_java_callback: failed to obtain current JNI environment");
            }

            return JNI_TRUE;
        }

        return JNI_FALSE;
    }

    void jni_detach_thread(JavaVM* jVM)
    {
        jVM->DetachCurrentThread();
    }


} //extern c

namespace ssi
{
	//initialize
	AndroidJavaSensors* AndroidJavaSensors::androidJavaSensorsInstance=NULL;

    void AndroidJavaSensors::stringToJava(std::string str)
    {
        int byteCount=str.length();

       JNIEnv *env2;
       jboolean attached= jni_attach_thread(&env2, androidJavaSensorsInstance->jvm);
        if(attached == JNI_TRUE)
        {



            const jbyte* pNativeMessage = reinterpret_cast<const jbyte*>(str.c_str());
            jbyteArray bytes = env2->NewByteArray(byteCount);
            if(bytes){


                env2->SetByteArrayRegion(bytes, 0, byteCount, pNativeMessage);


                jclass cls=(jclass)androidJavaSensorsInstance->sensorCollectorServiceClass;
                jmethodID mid=androidJavaSensorsInstance->sendByteMethod;

                if(cls){


                    if(mid)
                    {

                      env2->CallStaticVoidMethod(cls, mid, bytes);


                    }

                }

            }

            //env->CallObjectMethod( javaObject, mid);
            //env2->ReleaseByteArrayElements(bytes, rawjBytes, 0);
            jni_detach_thread(jvm);

        }

    }


    bool AndroidJavaSensors::connect ()
            {

                    return true;
            }
    bool AndroidJavaSensors::disconnect ()
            {

                    return true;

    }
	
	
	void AndroidJavaSensors::addJavaEvent(const char* sender, const char* name, const char* text)
	{
		ITheFramework *frame = Factory::GetFramework ();
		
		ssi_char_t *ptr = ssi_pcast(ssi_char_t, _event_batt.ptr);
		

		memset(ptr, 0, 64);
		strncpy(ptr, sender,64);
		strncat(ptr, ";",64);
		strncat(ptr, name,64);
		strncat(ptr, ";",64);
		strncat(ptr,text,64);
		
		
		


		_event_batt.time=frame->GetElapsedTimeMs();


		
		if (_elistener)
		{

                if(_options.activeSensors[0]||_options.activeSensors[1]||_options.activeSensors[2])
                    _elistener->update(_event_batt);
				
		}


	
	}


         void AndroidJavaSensors::updateData(const char* modality, double x, double y, double z)
         {
             ssi_wrn("blubb: %s ", modality);

                if(strcmp(modality, "GPS")==0)
                {
                    ssi_wrn("blub");
                    databuffer[MODALITIES::GPS_X]=x;
                    databuffer[MODALITIES::GPS_Y]=y;
                ssi_wrn("blubb %f, %f",databuffer[MODALITIES::GPS_X],  databuffer[MODALITIES::GPS_Y]);
                   // sensorProvider->provide(
                   //             ssi_pcast(ssi_byte_t, databuffer), 1);

                }


         }



	
	bool AndroidJavaSensors::setEventListener(IEventListener *listener) {
		_elistener = listener;
  
		ssi_size_t sender_id = Factory::AddString(_options.sname);

		std::stringstream str;

		if (sender_id == SSI_FACTORY_STRINGS_INVALID_ID)
			return false;


        if (_options.activeSensors[0]) {


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



		if (file) {
			if (!OptionList::LoadXML(file, _options)) {
				OptionList::SaveXML(file, _options);
			}
			_file = ssi_strcpy(file);
		}

                ssi_event_init(_event_batt, SSI_ETYPE_STRING);
                ssi_event_adjust(_event_batt, 64 * sizeof(ssi_char_t));

                sensorChannel=new AndroidJavaChannel();


		
	}




        ssi_size_t AndroidJavaSensors::getChannelSize ()
        {
            return MODALITIES::N_MODALITIES;
        }

        IChannel *AndroidJavaSensors::getChannel (ssi_size_t index)
        {
            return sensorChannel;

        }





        bool AndroidJavaSensors::setProvider (const ssi_char_t *name, IProvider *provider)
        {


                        if (sensorProvider) {
                                        ssi_wrn ("already set");
                        }

                        if(strcmp (name, SSI_ANDROIDJAVASENSORS_NAME) == 0)

                        {

                            ssi_wrn ("java sensor p: %s:, %d",name, provider);
                            sensorProvider = provider;


                            if (sensorProvider) {


                                            sensorChannel->getStreamPtr()->sr = _options.sr;
                                            sensorProvider->init (sensorChannel);
                                            ssi_wrn ( "android java provider set");
                                            return true;
                            }
                        }

         return false;

        }

        void AndroidJavaSensors::listen_enter() {


        }

        bool AndroidJavaSensors::update(IEvents &events, ssi_size_t n_new_events, ssi_size_t time_ms) {


                for (int i = 0; i < n_new_events; i++) {
                        ssi_event_t *e = events.next();


                    bool sendEvent = false;

                    //filter out own messages?
                    if ((e->sender_id == _event.sender_id) && _event.sender_id!=NULL)
                    {

                            //if (!_options.send_own_events)
                                    sendEvent = false;
                           // else
                             //       sendEvent = true;
                    }
                    else
                    {

                            sendEvent = true;
                    }



                    if (sendEvent) {
                            rapidjson::StringBuffer s;
                            rapidjson::Writer<rapidjson::StringBuffer> writer(s);

                            writer.StartObject();


                            writer.String("type");
                            writer.String(SSI_ETYPE_NAMES[e->type]);

                            writer.String("sender");
                            writer.String(Factory::GetString(e->sender_id));

                            writer.String("name");
                            writer.String(Factory::GetString(e->event_id));

                            writer.String("time");
                            writer.Int(e->time);

                            writer.String("dur");
                            writer.Int(e->dur);

                            writer.String("tot");
                            writer.Int(e->tot);

                            writer.String("state");
                            writer.String(SSI_ESTATE_NAMES[e->state]);

                            writer.String("glue");
                            writer.Int(e->glue_id);

                            writer.String("prob");
                            writer.Double(e->prob);


                            writer.String("value");

                            if (e->type == SSI_ETYPE_STRING)
                            {
                                    writer.String(e->ptr);
                            }
                            else if (e->type == SSI_ETYPE_TUPLE)
                            {
                                    writer.StartArray();

                                    ssi_real_t *floatptr = ssi_pcast(ssi_real_t, e->ptr);

                                    for (int i = 0; i < e->tot / sizeof(ssi_real_t); i++)
                                            writer.Double(floatptr[i]);

                                    writer.EndArray();
                            }
                            else if (e->type == SSI_ETYPE_EMPTY)
                            {
                                    writer.Null();
                            }
                            else if (e->type == SSI_ETYPE_MAP)
                            {
                                    writer.StartArray();

                                    ssi_event_map_t *ptr = ssi_pcast(ssi_event_map_t, e->ptr);

                                    for (int i = 0; i < e->tot / sizeof(ssi_event_map_t); i++) {

                                            writer.StartObject();
                                                    writer.String(Factory::GetString(ptr[i].id));
                                                    writer.Double(ptr[i].value);
                                            writer.EndObject();

                                    }

                                    writer.EndArray();
                            }

                            else if (e->type == SSI_ETYPE_UNDEF)
                            {
                                /*
                                    unsigned char *convbuf = reinterpret_cast<unsigned char*>(e->ptr);
                                    char *base64_buf = new char[4 * (int)ceil(e->tot / 3.0)]; //http://stackoverflow.com/questions/9668863/predict-the-byte-size-of-a-base64-encoded-byte

                                    mg_base64_encode(convbuf, e->tot, base64_buf);

                                    writer.String(e->ptr, strlen(base64_buf));*/
                            }
                            else // unknown types!
                            {
                                    ssi_wrn("unknown event type!");
                                    writer.Null();
                            }


                            writer.EndObject();



                            if(sensorCollectorServiceClass&&sendByteMethod)
                            {
                                std::string str=s.GetString();
                                std::thread javaThread(&AndroidJavaSensors::stringToJava,this, str);

                                javaThread.join();
                            }
                    }
            }


                return true;
        }

        void  AndroidJavaSensors::listen_flush() {



        }


        void AndroidJavaSensors::terminate()
        {

        }
        void AndroidJavaSensors::flush()
        {

        }
        void AndroidJavaSensors::enter()
        {

        }
        bool AndroidJavaSensors::start()
        {
            //ssjsensor start

            return Thread::start();

        }
        bool AndroidJavaSensors::stop()
        {


            return Thread::stop();

        }

        //repeat data provided from java for stable samplerate
        void AndroidJavaSensors::run()
            {


            sensorProvider->provide(
                        ssi_pcast(ssi_byte_t, databuffer),
                        1);

            double sleepTime=(1000.0/_options.sr) -10.0;
            if(sleepTime<0)sleepTime=10.0;

            ssi_sleep(sleepTime);

            }






	AndroidJavaSensors::~AndroidJavaSensors()
	{
		androidJavaSensorsInstance=0;
	}
	
}
