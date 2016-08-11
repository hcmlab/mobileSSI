#pragma once

#define SAMPLE_RATE	(1000L/60)*1000
#define SAMPLE_RATE_10  100
#define DB_PATH "/sdcard/sensor_recordings.db"

#define AUDIO_SAMPLE_RATE_HZ  48000
#define AUDIO_RECORDER_FRAMES 2048	//should fit FFT size
#define AUDIO_FFT_SIZE		  2048

#define AUDIO_RECORD_RAW	  true


#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "libsensorcollector", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "libsensorcollector", __VA_ARGS__))



#define SENSOR_TYPE_ACCELEROMETER       			1
#define SENSOR_TYPE_MAGNETIC_FIELD      			2
#define SENSOR_TYPE_ORIENTATION         			3
#define SENSOR_TYPE_GYROSCOPE           			4
#define SENSOR_TYPE_LIGHT               			5
#define SENSOR_TYPE_PRESSURE            			6
#define SENSOR_TYPE_TEMPERATURE         			7
#define SENSOR_TYPE_PROXIMITY           			8
#define SENSOR_TYPE_GRAVITY             			9
#define SENSOR_TYPE_LINEAR_ACCELERATION 			10
#define SENSOR_TYPE_ROTATION_VECTOR     			11
#define SENSOR_TYPE_RELATIVE_HUMIDITY   			12
#define SENSOR_TYPE_AMBIENT_TEMPERATURE   			13
#define SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED                 14
#define SENSOR_TYPE_GAME_ROTATION_VECTOR 			15
#define SENSOR_TYPE_GYROSCOPE_UNCALIBRATED			16
#define SENSOR_TYPE_SIGNIFICANT_MOTION				17
#define SENSOR_TYPE_STEP_DETECTOR				18
#define SENSOR_TYPE_STEP_COUNTER				19
#define SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR                 20
#define SENSOR_TYPE_HEART_RATE                                  21

#define SENSOR_TYPE_ALL 					-1
