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

#include <sstream>
#include <jni.h>
#include "ssjsensorchannel.h"
#include <vector>


extern "C"
{
void Java_hcm_ssj_mobileSSI_MobileSSIConsumer_pushData(JNIEnv* env, jobject thiz, jobject stream, jint id);
void Java_hcm_ssj_mobileSSI_MobileSSIConsumer_setSensor(JNIEnv* env, jobject thiz,jobject ssjSensor, jobject ssjStream, jint id);
void Java_hcm_ssj_mobileSSI_MobileSSIConsumer_setStreamOptions(JNIEnv* env, jobject thiz, jint id, jint num, jint dim, jint type, jdouble sr );
}

namespace ssi {


class ssjSensor;
class ssjSensorContainer;

struct SSJSensorNeccessities
{
int id;
double sr;
int num;
int dim;
int type;
ssjSensor* sensor;
/*
Look for the bare necessities
The simple bare necessities
Forget about your worries and your strife
I mean the bare necessities
Old Mother Nature's recipes
That brings the bare necessities of life
*/
};

int AttachJavaThread(JNIEnv *& env );





                class ssjSensor : public ISensor, public Thread {

	public:

		class Options : public OptionList {

		public:

                        Options() : pollMs(100), sr(100), streamType(1), streamDim(1), sampleNum(1), fullSSJ(true), ID(0){


                                addOption ("sr", &sr, 1, SSI_DOUBLE, "sample rate in Hz");
                                addOption("ssjProviderClass",&ssjSensorClassName, SSI_MAX_CHAR, SSI_CHAR ,"Name of the SSJ Sensor");
                                addOption("streamType", &streamType, 1, SSI_INT, "type of stream" );
                                addOption("streamDim", &streamDim, 1, SSI_INT, "number of stream dimensions");
                                addOption("sampleNum", &sampleNum, 1, SSI_INT, "number of samples in stream" );
                                addOption("fullSSJ", &fullSSJ,1,SSI_BOOL, "is a full ssj pipeline running?");
                                addOption("ssjID", &ID, 1, SSI_INT, "id to connect ssi and ssj sensors");


			}




                        ssi_object_t getType () { return SSI_SENSOR; };


                        ssi_char_t ssjSensorClassName[SSI_MAX_CHAR];
			ssi_char_t sname[SSI_MAX_CHAR];
                        ssi_time_t sr;
                        ssi_int_t ID;
                        ssi_int_t streamType;
                        ssi_int_t streamDim;
                        ssi_int_t sampleNum;
                        bool fullSSJ;

			ssi_size_t pollMs;

		};

	public:
                static ssjSensor* getSsjSensorsFromJava(int id);
                static const ssi_char_t *GetCreateName() { return "ssi_sensor_ssjSensor"; };
                static IObject *Create(const ssi_char_t *file) { return new ssjSensor(file); };
                                ~ssjSensor();

		Options *getOptions() { return &_options; };
		const ssi_char_t *getName() { return GetCreateName(); };
                const ssi_char_t *getInfo() { return "ssj sensor"; };


                char* getSsjSensorName()
                {
                    return _options.ssjSensorClassName;
                }
                void run();
                void flush();
                void terminate();
                void enter();
                void setHaveSSJ()
                {
                    haveSSJ=true;
                }
                ssi_size_t getChannelSize ();
                IChannel *getChannel (ssi_size_t index);
                bool setProvider (const ssi_char_t *name, IProvider *provider);
                void setSsjSensor(jobject ssjSensor, jobject ssjStream, JNIEnv* env)
                {

                    this->env=env;
                    ssjSensorObj=ssjSensor;
                    ssjStreamObj=ssjStream;
                }
                void ssiSsjPush(JNIEnv* env, jobject thiz, jobject ssjStream, jint id);
                bool connect ();
                bool disconnect ();
                bool start();
                bool stop();



                //event sender




		//event sender

		ITheFramework	*_frame;


                bool stopLoop=false;


                void startSensors();


	protected:


                IProvider* sensorProvider;
                IChannel* sensorChannel;
                ssjSensor(const ssi_char_t *file = 0);
		ssi_char_t *_file;
		Options _options;
                float* dataBuffer;
                bool haveSSJ=false;

		static char ssi_log_name[];
                //ssj sensor we are listening to
                JavaVM *				g_JavaVM=0;
                JNIEnv *env;
                jobject ssjSensorObj;
                //ssjStream is needed to call ssj enter/flush
                jobject ssjStreamObj;

                static ssjSensor* instanceForJava;




	};
//todo multiple ssj sensors
        class ssjSensorContainer
        {
        public:
            static void addInstance( SSJSensorNeccessities* inst );
            static SSJSensorNeccessities* getInstance( int id );
            static void remInstance( int id );
            static ssjSensorContainer* getContainer();
            static int size()
            {
                return sensorOptions->size();
            }
        private:
            static std::vector<SSJSensorNeccessities*>* sensorOptions;
            static ssjSensorContainer* instance;
            ssjSensorContainer() {

            }

        };


}


#endif
