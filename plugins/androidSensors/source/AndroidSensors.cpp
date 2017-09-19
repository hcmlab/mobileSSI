// AndroidSensors.cpp
// author: Simon Flutura <simon.flutura@informatik.uni-augsburg.de>
// created: 25/3/2015
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

#include "AndroidSensors.h"
#include "definitions.hpp"
#include "frame/include/TheFramework.h"
#include "base/Factory.h"
#include "base/ISensor.h"
#include <android_native_app_glue.h>
#include <android/log.h>

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






namespace ssi {

	char AndroidSensors::ssi_log_name[] = "andsensors";




	AndroidSensors::AndroidSensors(const ssi_char_t *file)
				: _file(0)
		{


		if (file) {
                        if (!OptionList::LoadXML(file, &_options)) {
                                OptionList::SaveXML(file, &_options);
			}
			_file = ssi_strcpy(file);
		}

				sensorManager=ASensorManager_getInstance();

				for(int i=0; i< NSENSORS; i++)
				{
						sensorChannels[i]=0;
						sensorProviders[i]=0;
				}


				if (_options.activeSensors[SENSOR_INDEX_ACCELEROMETER]) {

					sensorChannels[SENSOR_INDEX_ACCELEROMETER]=new AccelerometerChannel();
					}
				if (_options.activeSensors[SENSOR_INDEX_GYROSCOPE ]) {

					sensorChannels[SENSOR_INDEX_GYROSCOPE]=new GyroscopeChannel();
				 }

				if (_options.activeSensors[SENSOR_INDEX_LIGHT]) {

					sensorChannels[SENSOR_INDEX_LIGHT]=new LightChannel();
				}

				if (_options.activeSensors[SENSOR_INDEX_PROXIMITY]) {

					sensorChannels[SENSOR_INDEX_PROXIMITY]=new ProximityChannel();

				}

				if (_options.activeSensors[SENSOR_INDEX_MAGNETIC_FIELD]) {

					sensorChannels[SENSOR_INDEX_MAGNETIC_FIELD]=new MagnetometerChannel();

				}
				if (_options.activeSensors[SENSOR_INDEX_HEART_RATE]) {

					sensorChannels[SENSOR_INDEX_HEART_RATE]=new HeartRateChannel();

				}



	}


	void AndroidSensors::terminate()
	{
		stopLoop=true;
	}
		void AndroidSensors::flush()
		{
				stopLoop=true;
		}
	void AndroidSensors::enter()
	{

	}
		bool AndroidSensors::start()
		{
			return Thread::start();
		}
		bool AndroidSensors::stop()
		{
			  return Thread::stop();
		}

	void AndroidSensors::run()
	{

				//enableSensors();
		if (_options.activeSensors[SENSOR_INDEX_ACCELEROMETER])
		{
			tryAddAndroidSensor(SENSOR_TYPE_LINEAR_ACCELERATION, true);
			enableSensors(SENSOR_ACCELEROMETER);

		}

		if (_options.activeSensors[SENSOR_INDEX_GYROSCOPE ]) {
			 tryAddAndroidSensor(SENSOR_TYPE_GYROSCOPE, true);
			 enableSensors(SENSOR_GYROSCOPE);
		 }

		if (_options.activeSensors[SENSOR_INDEX_LIGHT]) {
			   tryAddAndroidSensor(SENSOR_TYPE_LIGHT, true);
			   enableSensors(SENSOR_LIGHT);
		}

		if (_options.activeSensors[SENSOR_INDEX_PROXIMITY]) {
			tryAddAndroidSensor(SENSOR_TYPE_PROXIMITY, true);
			enableSensors(SENSOR_PROXIMITY);
		}

		if (_options.activeSensors[SENSOR_INDEX_MAGNETIC_FIELD]) {
			tryAddAndroidSensor(SENSOR_TYPE_MAGNETIC_FIELD, true);
			enableSensors(SENSOR_MAGNETIC_FIELD);
		}
                if (_options.activeSensors[SENSOR_INDEX_HEART_RATE]) {
                        tryAddAndroidSensor(SENSOR_TYPE_HEART_RATE, true);
                        enableSensors(SENSOR_HEART_RATE);
                }


		event_loop();
	}


	void AndroidSensors::event_loop()
		{

                    while (!stopLoop) {

                    if (stopLoop)
                    {

                                    break;

                    }
                    else;

                            // Read all pending events.
                            int ident;
                            int events;
                            struct android_poll_source* source;

                            int block = -1;	//if set to -1 block until new sensor values are available
                            while ((ident=ALooper_pollAll(_options.pollMs, NULL, &events, (void**)&source)) >= 0)
                            {
                                    //process apps input events


                                    // If a sensor has data, process it now.
                                    if (ident == LOOPER_ID)
                                    {
                                            get_sensor_events(0,events,(void*)this);
                                    }

                                    // Check if we are exiting.
                                    if (stopLoop)
                                    {
                                            break;
                                    }
                                    //_options.pollMs/2
                                    std::this_thread::sleep_for(std::chrono::milliseconds(20));


                            }
                    }


		}




	bool AndroidSensors::tryAddAndroidSensor(int type, bool isVector) {

		  const ASensor* sensor;
			if((sensor = ASensorManager_getDefaultSensor(sensorManager, type )) == NULL)
			{

						  return false;
			}
			else {

                                  sensorList.insert( std::pair<int,AndroidSensor*>(
                                                                                 type,
                                                                                 new AndroidSensor(
                                                                                                        type ,
                                                                                                        isVector,
                                                                                                        sensor)
                                                                        ) );


                                 {

                                      double minDelay = ASensor_getMinDelay(sensor);

                                      ssi_wrn("min delay %d: %f", type, minDelay);
                                      // We'd like to get 60 events per second (in us).
                                  }


						  return true;
			}
	}
  
  
	void AndroidSensors::enableSensors(SensorType sensor) {


			ALooper* looper=0;


                        looper = ALooper_forThread();


			if(looper == NULL) looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
			ALooper_addFd(looper, 0 , 0, ALOOPER_EVENT_INPUT, get_sensor_events, (void*)this);


			sensorEventQueue = ASensorManager_createEventQueue(sensorManager, looper, LOOPER_ID, NULL, NULL);

			double sampleRateUs = SAMPLE_RATE;

			std::map<int,AndroidSensor*>::iterator it;
			for (it=sensorList.begin(); it!=sensorList.end(); ++it)
			{
					 ASensorEventQueue_enableSensor(sensorEventQueue, it->second->asensor);

					 ASensorEventQueue_setEventRate(sensorEventQueue, it->second->asensor, sampleRateUs);

			}


	}

	void AndroidSensors::disableSensors(SensorType sensor) {
			std::map<int,AndroidSensor*>::iterator it;
			for (it=sensorList.begin(); it!=sensorList.end(); ++it) {
					ASensorEventQueue_disableSensor(sensorEventQueue, it->second->asensor);
			}


	}



	void AndroidSensors::android2ssiEvent(ASensorEvent event)
	{
			ITheFramework *frame = Factory::GetFramework ();



				if(event.type==SENSOR_TYPE_LINEAR_ACCELERATION)
				{

						ssi_real_t ptr[3];
						ptr[0]=event.acceleration.x;
						ptr[1]=event.acceleration.y;
						ptr[2]=event.acceleration.z;

						if(sensorProviders[SENSOR_INDEX_ACCELEROMETER])
									sensorProviders[SENSOR_INDEX_ACCELEROMETER]->provide(
										ssi_pcast(ssi_byte_t, ptr),
										1
										);

				}
				else if(event.type==SENSOR_TYPE_MAGNETIC_FIELD)
				 {
						 ssi_real_t ptr[3];
						 ptr[0]=event.magnetic.x;
						 ptr[1]=event.magnetic.y;
						 ptr[2]=event.magnetic.z;


						 if(sensorProviders[SENSOR_INDEX_MAGNETIC_FIELD])
									sensorProviders[SENSOR_INDEX_MAGNETIC_FIELD]->provide(
										 ssi_pcast(ssi_byte_t, ptr),
										 1
										 );

				 }
				else if(event.type==SENSOR_TYPE_GYROSCOPE)
				 {
						 ssi_real_t ptr[3];
						 ptr[0]=event.vector.x;
						 ptr[1]=event.vector.y;
						 ptr[2]=event.vector.z;


						 if(sensorProviders[SENSOR_INDEX_GYROSCOPE])
									sensorProviders[SENSOR_INDEX_GYROSCOPE]->provide(
										 ssi_pcast(ssi_byte_t, ptr),
										 1
										 );

				 }
				else if(event.type==SENSOR_TYPE_LIGHT )
				 {
						 ssi_real_t ptr;
						 ptr=event.light;


						 if(sensorProviders[SENSOR_INDEX_LIGHT])
									sensorProviders[SENSOR_INDEX_LIGHT]->provide(
										 ssi_pcast(ssi_byte_t, &ptr),
										 1
										 );


				 }

				else if(event.type==SENSOR_INDEX_PROXIMITY )
				 {
						 ssi_real_t ptr;
						 ptr=event.distance;


						 if(sensorProviders[SENSOR_INDEX_PROXIMITY])
									 sensorProviders[SENSOR_INDEX_PROXIMITY]->provide(
											 ssi_pcast(ssi_byte_t, &ptr),
											 1
											 );

				 }
				else if(event.type==SENSOR_INDEX_HEART_RATE )
				 {
						 ssi_real_t ptr;
						 ptr=event.heart_rate.bpm;

				ssi_wrn("new hr value:: %f\n", ptr);
						 if(sensorProviders[SENSOR_INDEX_HEART_RATE])
									sensorProviders[SENSOR_INDEX_HEART_RATE]->provide(
										 ssi_pcast(ssi_byte_t, &ptr),
										 1
										 );

				 }
	}

	int AndroidSensors::get_sensor_events(int fd, int events, void* data) {


					ASensorEvent event;
					while (ASensorEventQueue_getEvents(((AndroidSensors*)data)->getSensorEventQueue(), &event, 1) > 0)
					{


							((AndroidSensors*)data)->android2ssiEvent(event);


					}

					return 1;
	}

	
	void AndroidSensors::startSensors(){




			ASensorList sensorlist;
			int num = ASensorManager_getSensorList(ASensorManager_getInstance(), &sensorlist);


			int i;
			for (i = 0; i < num; i++) {
							ASensorRef sensor = sensorlist[i];
							const int type = ASensor_getType(sensor);
							const char* id = ASensor_getName(sensor);
							const char* vendor = ASensor_getVendor(sensor);
							const int mindelay = (ASensor_getMinDelay(sensor) / 1000);
							const float resolution = ASensor_getResolution(sensor);

							}
			}


	AndroidSensors::~AndroidSensors() {

			if (_file) {
                                        OptionList::SaveXML(_file, &_options);
					delete[] _file;
			}

						for(int i=0; i< NSENSORS; i++)
								delete sensorChannels[i];





	}



		ssi_size_t AndroidSensors::getChannelSize () { return NSENSORS; }

		IChannel *AndroidSensors::getChannel (ssi_size_t index)
		{
		if (index < NSENSORS && index > 0) return sensorChannels[index];

		return 0;
		}





		bool AndroidSensors::setProvider (const ssi_char_t *name, IProvider *provider)
		{

			ssi_wrn("provider set: %s\n", name);

			ssi_wrn("%s %s %s %s %s %s\n",
					SSI_ANDROIDSENSORS_ACCELEROMETER_PROVIDER_NAME,
					SSI_ANDROIDSENSORS_MAGNETIC_FIELD_PROVIDER_NAME,
					SSI_ANDROIDSENSORS_GYROSCOPE_PROVIDER_NAME,
					SSI_ANDROIDSENSORS_LIGHT_PROVIDER_NAME,
					SSI_ANDROIDSENSORS_PROXIMITY_PROVIDER_NAME,
					SSI_ANDROIDSENSORS_HEART_RATE_PROVIDER_NAME
					);
			if(strcmp (name, SSI_ANDROIDSENSORS_ACCELEROMETER_PROVIDER_NAME) == 0)
			{
				if (sensorProviders[SENSOR_INDEX_ACCELEROMETER]) {
						ssi_wrn ("already set");
				}


				sensorProviders[SENSOR_INDEX_ACCELEROMETER] = provider;
				if (sensorProviders[SENSOR_INDEX_ACCELEROMETER]) {


						sensorChannels[SENSOR_INDEX_ACCELEROMETER]->getStreamPtr()->sr = _options.sr;

						sensorProviders[SENSOR_INDEX_ACCELEROMETER]->init (sensorChannels[SENSOR_INDEX_ACCELEROMETER]);

						ssi_wrn ("ACCELEROMETER provider set");
						return true;
				}
			}
			else if(strcmp (name, SSI_ANDROIDSENSORS_MAGNETIC_FIELD_PROVIDER_NAME) == 0)
			{
				if (sensorProviders[SENSOR_INDEX_MAGNETIC_FIELD]) {
						ssi_wrn ("already set");
				}

				sensorProviders[SENSOR_INDEX_MAGNETIC_FIELD] = provider;
				if (sensorProviders[SENSOR_INDEX_MAGNETIC_FIELD]) {
						sensorChannels[SENSOR_INDEX_MAGNETIC_FIELD]->getStreamPtr()->sr = _options.sr;
						sensorProviders[SENSOR_INDEX_MAGNETIC_FIELD]->init (sensorChannels[SENSOR_INDEX_MAGNETIC_FIELD]);
						ssi_wrn ("MAGNETIC_FIELD provider set");
						return true;
				}
			}
			else if(strcmp (name, SSI_ANDROIDSENSORS_GYROSCOPE_PROVIDER_NAME) == 0)
			{
				if (sensorProviders[SENSOR_INDEX_GYROSCOPE]) {
						ssi_wrn ("already set");
				}

				sensorProviders[SENSOR_INDEX_GYROSCOPE] = provider;
				if (sensorProviders[SENSOR_INDEX_GYROSCOPE]) {
						sensorChannels[SENSOR_INDEX_GYROSCOPE]->getStreamPtr()->sr = _options.sr;
						sensorProviders[SENSOR_INDEX_GYROSCOPE]->init (sensorChannels[SENSOR_INDEX_GYROSCOPE]);
						ssi_wrn ("GYROSCOPE provider set");
						return true;
				}
			}
			else if(strcmp (name, SSI_ANDROIDSENSORS_LIGHT_PROVIDER_NAME) == 0)
			{
				if (sensorProviders[SENSOR_INDEX_LIGHT]) {
						ssi_wrn ("already set");
				}

				sensorProviders[SENSOR_INDEX_LIGHT] = provider;
				if (sensorProviders[SENSOR_INDEX_LIGHT]) {
						sensorChannels[SENSOR_INDEX_LIGHT]->getStreamPtr()->sr = _options.sr;
						sensorProviders[SENSOR_INDEX_LIGHT]->init (sensorChannels[SENSOR_INDEX_LIGHT]);
						ssi_wrn ( "LIGHT provider set");
						return true;
				}
			}
			else if(strcmp (name, SSI_ANDROIDSENSORS_PROXIMITY_PROVIDER_NAME) == 0)
			{
				if (sensorProviders[SENSOR_INDEX_PROXIMITY]) {
						ssi_wrn ("already set");
				}

				sensorProviders[SENSOR_INDEX_PROXIMITY] = provider;
				if (sensorProviders[SENSOR_INDEX_PROXIMITY]) {
						sensorChannels[SENSOR_INDEX_PROXIMITY]->getStreamPtr()->sr = _options.sr;
						sensorProviders[SENSOR_INDEX_PROXIMITY]->init (sensorChannels[SENSOR_INDEX_PROXIMITY]);
						ssi_wrn ( "PROXIMITY provider set");
						return true;
				}
			}
			else if(strcmp (name, SSI_ANDROIDSENSORS_HEART_RATE_PROVIDER_NAME) == 0)
			{
				if (sensorProviders[SENSOR_INDEX_HEART_RATE]) {
						ssi_wrn ("already set");
				}

				sensorProviders[SENSOR_INDEX_HEART_RATE] = provider;
				if (sensorProviders[SENSOR_INDEX_HEART_RATE]) {
						sensorChannels[SENSOR_INDEX_HEART_RATE]->getStreamPtr()->sr = _options.sr;
						sensorProviders[SENSOR_INDEX_HEART_RATE]->init (sensorChannels[SENSOR_INDEX_HEART_RATE]);
						ssi_wrn ( "HEART_RATE provider set");
						return true;
				}
			}

		return false;
		}
		bool AndroidSensors::connect ()
		{
			//register callback function for sensor events
			ALooper* looper = ALooper_forThread();

			if(looper == NULL) looper = ALooper_prepare(ALOOPER_POLL_CALLBACK);

			ALooper_addFd(looper, 0 , 0, ALOOPER_EVENT_INPUT, get_sensor_events, (void*)this);


			startSensors();
			return true;
		}
		bool AndroidSensors::disconnect ()
		{
			if (_options.activeSensors[SENSOR_INDEX_ACCELEROMETER])
			{	disableSensors(SENSOR_ACCELEROMETER);
;
			}
			if (_options.activeSensors[SENSOR_INDEX_MAGNETIC_FIELD])
			{	disableSensors(SENSOR_MAGNETIC_FIELD);

			}
			if (_options.activeSensors[SENSOR_INDEX_GYROSCOPE])
			{	disableSensors(SENSOR_GYROSCOPE);

			}
			if (_options.activeSensors[SENSOR_INDEX_LIGHT])
			{	disableSensors(SENSOR_LIGHT);

			}
			if (_options.activeSensors[SENSOR_INDEX_PROXIMITY])
			{	disableSensors(SENSOR_PROXIMITY);

			}
			if (_options.activeSensors[SENSOR_INDEX_HEART_RATE])
			{	disableSensors(SENSOR_HEART_RATE);

			}
			return true;
		}




}
