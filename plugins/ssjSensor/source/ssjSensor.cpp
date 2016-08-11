// AndroidSensors.cpp
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

#include "ssjSensor.h"

#include "frame/include/TheFramework.h"
#include "base/Factory.h"
#include "base/ISensor.h"


extern "C"
{

    jint JNI_OnLoad(JavaVM* aVm, void* aReserved)
    {
        return JNI_VERSION_1_6;
    }

    ssi::ssjSensor* checkSsjSensorUp(int id)
    {
        ssi_wrn("java calling ssi for ssjsensor");
        bool framework_running=false;

        ssi::ITheFramework *frame = ssi::Factory::GetFramework ();

        if(frame)
        {
            if(frame->GetElapsedTimeMs())
                framework_running=true;
        }

        if(framework_running)
        {

            ssi_wrn("java calling ssi for ssjsensor: fw running");
            ssi::ssjSensor* ajavaSensors= ssi::ssjSensor::getSsjSensorsFromJava(id);



                    if(ajavaSensors)
                    {
                        ssi_wrn("we have ssjsensor");
                        return ajavaSensors;
                    }
                }

       return NULL;



    }


    void Java_hcm_ssj_mobileSSI_MobileSSIConsumer_pushData(JNIEnv* env, jobject thiz, jobject stream, jint id)
    {
        ssi::ssjSensor* ssjSptr=NULL;

        if(checkSsjSensorUp(id))
        {
               ssjSptr=checkSsjSensorUp(id);
               //update DataBuffer
               ssjSptr->ssiSsjPush(env, thiz, stream, id);
               ssi_wrn("push for %d", id);



        }

    }

    void Java_hcm_ssj_mobileSSI_MobileSSIConsumer_setSensor (JNIEnv* env, jobject thiz, jobject ssjSensor, jobject ssjStream, jint id)
    {

        ssi::ssjSensor* ssjSptr=NULL;

        if(checkSsjSensorUp(id))
        {


               ssjSptr=checkSsjSensorUp(id);
                if(!ssjSensor)
                {
                    ssi_wrn("ssj set: stream or sensor NULL %d", id);

                }
               ssjSptr->setSsjSensor(ssjSensor, ssjStream, env);

               ssjSptr->setHaveSSJ();
               ssi_wrn("ssj sensor set %d", id);

        }else
        {
            ssi_wrn("NOT ssj sensor set %d", id);
        }
    }

}


void Java_hcm_ssj_mobileSSI_MobileSSIConsumer_setStreamOptions(JNIEnv* env, jobject thiz, jint id, jint num, jint dim, jint type, jdouble sr )
{
if(!ssi::ssjSensorContainer::getInstance(id))
{
    ssi::SSJSensorNeccessities* bare = new ssi::SSJSensorNeccessities();
    bare->dim=dim;
    bare->num=num;
    bare->id=id;
    bare->sensor=NULL;
    bare->sr=sr;
    bare->type=type;
    ssi::ssjSensorContainer::addInstance(bare);
    ssi_wrn("java adds %d", id);

}

}

namespace ssi {
    ssjSensor* ssjSensor::instanceForJava=0;


    char ssjSensor::ssi_log_name[] = "ssjsensor";

    ssjSensor* ssjSensor::getSsjSensorsFromJava(int id)
    {

        return ssjSensorContainer::getInstance(id)->sensor;

    }

    int AttachJavaThread ( JNIEnv *& env )
    {
        int ret = -1;


        return ret;
    }

    ssjSensor::ssjSensor(const ssi_char_t *file)
                : _file(0)
		{


		if (file) {
			if (!OptionList::LoadXML(file, _options)) {
				OptionList::SaveXML(file, _options);
			}
			_file = ssi_strcpy(file);
            ssi_wrn("file %s", _file);
		}

        {
            bool iDnullOrTaken =false;
            if(!ssjSensorContainer::getInstance(_options.ID))iDnullOrTaken=true;
            else
            if(ssjSensorContainer::getInstance(_options.ID)->sensor)iDnullOrTaken=true;

            if(iDnullOrTaken)
            {
                int offset=0;

                for(int i=0;i < ssjSensorContainer::size();)
                {
                    if(ssjSensorContainer::getInstance(i+offset))
                    {
                        i++;
                    }
                    else offset++;
                    if(!ssjSensorContainer::getInstance(i+offset));
                    else if(!ssjSensorContainer::getInstance(i+offset)->sensor)
                    {
                        _options.ID=i+offset;
                        ssjSensorContainer::getInstance(_options.ID)->sensor=this;
                        ssi_wrn("C++ takes %d", _options.ID);
                        break;
                    }
                }

            }
            else
            {
            ssjSensorContainer::getInstance(_options.ID)->sensor=this;

            ssi_wrn("C++ finds %d", _options.ID);
            }
            int i=ssjSensorContainer::getInstance(_options.ID)->num;
            int j=ssjSensorContainer::getInstance(_options.ID)->dim;


        }
//if(type)

        dataBuffer= new float[
                ssjSensorContainer::getInstance(_options.ID)->num*
                ssjSensorContainer::getInstance(_options.ID)->dim];

        //according to type
        sensorChannel=new SSJFloatChannel(ssjSensorContainer::getInstance(_options.ID)->num, ssjSensorContainer::getInstance(_options.ID)->dim);
        sensorProvider=0;



	}

    void ssjSensor::ssiSsjPush(JNIEnv* env, jobject thiz,  jobject ssjStream, jint id)
    {

        ITheFramework *frame = Factory::GetFramework ();

        ssi_wrn("jni provide");

        if(haveSSJ)ssi_wrn("haveSSJ");

        if(haveSSJ&&id==_options.ID)
        {
        ssi_wrn("jni provide code");

        jclass cls= (jclass)env->GetObjectClass( ssjStream);

        jmethodID mid = env->GetMethodID( cls, "ptrF", "()[F");
        //call ssj process

        //copy from ssj
        // Call the method, returns JObject (because Array is instance of Object)
        jobject mvdata = env->CallObjectMethod( ssjStream, mid);
        jfloatArray * arr = reinterpret_cast<jfloatArray*>(&mvdata);

        // Get the elements (you probably have to fetch the length of the array as well
        float * data = env->GetFloatArrayElements(*arr, NULL);


        for( int i=0; i< ssjSensorContainer::getInstance(_options.ID)->num*
                        ssjSensorContainer::getInstance(_options.ID)->dim; i++)
                dataBuffer[i]=data[i];

        for(int i=0; i < ssjSensorContainer::getInstance(_options.ID)->num*
            ssjSensorContainer::getInstance(_options.ID)->dim; i++){
                 //           ssi_wrn("dat ta da: %f :%d, %d ::%d\n",dataBuffer[i], _options.ID, ssjSensorContainer::size(), ssjSensorContainer::getInstance(_options.ID)->id);
        }

        sensorProvider->provide(
            ssi_pcast(ssi_byte_t, dataBuffer),
            ssjSensorContainer::getInstance(_options.ID)->num
            );

        // Don't forget to release it
        env->ReleaseFloatArrayElements(*arr, data, 0);


        }
    }
    void ssjSensor::terminate()
	{
		stopLoop=true;
	}
    void ssjSensor::flush()
    {
        //(*env)->FindClass(env, "HelloWorld");
    if(false)
    {
        jclass cls = env->GetObjectClass( ssjSensorObj);
        jmethodID mid= env->GetMethodID( cls, "flush", "()V");
        env->CallVoidMethod( ssjSensorObj, mid);
    }
    }
    void ssjSensor::enter()
	{
        //ssj enter
        //ssj first run
	}
    bool ssjSensor::start()
    {
        //ssjsensor start
        if(!_options.fullSSJ)
        return Thread::start();
        else return true;
    }
    bool ssjSensor::stop()
    {

        if(!_options.fullSSJ)
        //ssjsensor stop
        return Thread::stop();
        else return true;
    }


    void ssjSensor::run()
	{

/*
            ITheFramework *frame = Factory::GetFramework ();

            if(haveSSJ&&ssjStreamObj)
            {


            jclass cls = (jclass)env->GetObjectClass( ssjStreamObj->class_name);
            jmethodID mid = env->GetMethodID( cls, "ptr", "()[D");
            //call ssj process

            jclass cls2 = (jclass)env->NewWeakGlobalRef( ssjSensorObj);
            jmethodID midProcess = env->GetMethodID( cls, "ptr", "(Lhcm/ssj/core/stream/Stream;)V");

            env->CallVoidMethod( ssjSensorObj, midProcess, ssjStreamObj);

            //copy from ssj
            // Call the method, returns JObject (because Array is instance of Object)
            jobject mvdata = env->CallObjectMethod( ssjStreamObj, mid);
            jdoubleArray * arr = reinterpret_cast<jdoubleArray*>(&mvdata);

            // Get the elements (you probably have to fetch the length of the array as well
            double * data = env->GetDoubleArrayElements(*arr, NULL);

            for( int i=0; i< _options.sampleNum*_options.streamDim; i++)
                    dataBuffer[i]=data[i];

            // Don't forget to release it
            env->ReleaseDoubleArrayElements(*arr, data, 0);

            sensorProvider->provide(
                ssi_pcast(ssi_byte_t, &dataBuffer),
                1
                );

            }
            else
            {
                //ssi_wrn("ssj not there or stream null");
            }*/

        ssi_sleep(10);

	}







    ssjSensor::~ssjSensor() {

			if (_file) {
					OptionList::SaveXML(_file, _options);
					delete[] _file;
			}

        ssjSensorContainer::getContainer()->remInstance(_options.ID);

        delete sensorChannel;

        delete dataBuffer;


	}



        ssi_size_t ssjSensor::getChannelSize () { return 1; }

        IChannel *ssjSensor::getChannel (ssi_size_t index)
		{
                    return sensorChannel;

		return 0;
		}





        bool ssjSensor::setProvider (const ssi_char_t *name, IProvider *provider)
        {  ssi_wrn ("setProvider");

            if(strcmp (name, SSI_SSJ_PROVIDER_NAME) == 0)
			{
                if (sensorProvider) {
                        ssi_wrn ("provider already set");
				}


                sensorProvider = provider;

                if (sensorProvider) {


                        sensorChannel->getStreamPtr()->sr = ssjSensorContainer::getInstance(_options.ID)->sr;

                        sensorProvider->init (sensorChannel);

						return true;
                }
			}


        return false;
		}
        bool ssjSensor::connect ()
		{

			return true;
		}
        bool ssjSensor::disconnect ()
		{

			return true;
		}




}
