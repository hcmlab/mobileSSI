#ifndef ANDROIDSENSORCHANNELS_H
#define ANDROIDSENSORCHANNELS_H

#include "AndroidSensors.h"
#include "base/ISensor.h"
namespace ssi{


#define SSI_ANDROIDSENSORS_ACCELEROMETER_SAMPLE_TYPE		ssi_real_t
#define SSI_ANDROIDSENSORS_ACCELEROMETER_PROVIDER_NAME	"accelerometer"

#define SSI_ANDROIDSENSORS_MAGNETIC_FIELD_SAMPLE_TYPE		ssi_real_t
#define SSI_ANDROIDSENSORS_MAGNETIC_FIELD_PROVIDER_NAME	"magnetometer"

#define SSI_ANDROIDSENSORS_GYROSCOPE_SAMPLE_TYPE		ssi_real_t
#define SSI_ANDROIDSENSORS_GYROSCOPE_PROVIDER_NAME	"gyroscope"

#define SSI_ANDROIDSENSORS_LIGHT_SAMPLE_TYPE                    ssi_real_t
#define SSI_ANDROIDSENSORS_LIGHT_PROVIDER_NAME          "light"

#define SSI_ANDROIDSENSORS_PROXIMITY_SAMPLE_TYPE		ssi_real_t
#define SSI_ANDROIDSENSORS_PROXIMITY_PROVIDER_NAME	"proximity"

#define SSI_ANDROIDSENSORS_HEART_RATE_SAMPLE_TYPE		ssi_real_t
#define SSI_ANDROIDSENSORS_HEART_RATE_PROVIDER_NAME	"heartrate"


class AccelerometerChannel : public IChannel {

        friend class AndroidSensor;

        public:

                AccelerometerChannel () {
                        ssi_stream_init (stream, 0, 3, sizeof (SSI_ANDROIDSENSORS_ACCELEROMETER_SAMPLE_TYPE), SSI_REAL, 0);
                }
                ~AccelerometerChannel () {
                        ssi_stream_destroy (stream);
                }

                const ssi_char_t *getName () { return SSI_ANDROIDSENSORS_ACCELEROMETER_PROVIDER_NAME; };
                const ssi_char_t *getInfo () { return "Reports accelerometer, 3 axis, float values"; };
                ssi_stream_t getStream () { return stream; };
                ssi_stream_t* getStreamPtr(){return &stream;};

        protected:
                ssi_stream_t stream;
};

class MagnetometerChannel : public IChannel {

        friend class AndroidSensor;

        public:

                MagnetometerChannel () {
                        ssi_stream_init (stream, 0, 3, sizeof (SSI_ANDROIDSENSORS_MAGNETIC_FIELD_SAMPLE_TYPE), SSI_REAL, 0);
                }
                ~MagnetometerChannel () {
                        ssi_stream_destroy (stream);
                }

                const ssi_char_t *getName () { return SSI_ANDROIDSENSORS_MAGNETIC_FIELD_PROVIDER_NAME; };
                const ssi_char_t *getInfo () { return "Reports magnetometer, 3 axis, float values"; };
                ssi_stream_t getStream () { return stream; };
                 ssi_stream_t* getStreamPtr(){return &stream;};

        protected:
                ssi_stream_t stream;
};




class GyroscopeChannel : public IChannel {

        friend class AndroidSensor;

        public:

                GyroscopeChannel () {
                        ssi_stream_init (stream, 0, 3, sizeof (SSI_ANDROIDSENSORS_GYROSCOPE_SAMPLE_TYPE), SSI_REAL, 0);
                }
                ~GyroscopeChannel () {
                        ssi_stream_destroy (stream);
                }

                const ssi_char_t *getName () { return SSI_ANDROIDSENSORS_GYROSCOPE_PROVIDER_NAME; };
                const ssi_char_t *getInfo () { return "Reports gyroscope, 3 axis, float values"; };
                ssi_stream_t getStream () { return stream; };
                 ssi_stream_t* getStreamPtr(){return &stream;};

        protected:
                ssi_stream_t stream;
};




class LightChannel : public IChannel {

        friend class AndroidSensor;

        public:

                LightChannel () {
                        ssi_stream_init (stream, 0, 1, sizeof (SSI_ANDROIDSENSORS_LIGHT_SAMPLE_TYPE), SSI_REAL, 0);
                }
                ~LightChannel () {
                        ssi_stream_destroy (stream);
                }

                const ssi_char_t *getName () { return SSI_ANDROIDSENSORS_LIGHT_PROVIDER_NAME; };
                const ssi_char_t *getInfo () { return "Reports light, float value"; };
                ssi_stream_t getStream () { return stream; };
                 ssi_stream_t* getStreamPtr(){return &stream;};

        protected:
                ssi_stream_t stream;
};



class ProximityChannel : public IChannel {

        friend class AndroidSensor;

        public:

                ProximityChannel () {
                        ssi_stream_init (stream, 0, 1, sizeof (SSI_ANDROIDSENSORS_PROXIMITY_SAMPLE_TYPE), SSI_REAL, 0);
                }
                ~ProximityChannel () {
                        ssi_stream_destroy (stream);
                }

                const ssi_char_t *getName () { return SSI_ANDROIDSENSORS_PROXIMITY_PROVIDER_NAME; };
                const ssi_char_t *getInfo () { return "Reports accelerometer,  float value"; };
                ssi_stream_t getStream () { return stream; };
                 ssi_stream_t* getStreamPtr(){return &stream;};

        protected:
                ssi_stream_t stream;
};



class HeartRateChannel : public IChannel {

        friend class AndroidSensor;

        public:

                HeartRateChannel () {
                        ssi_stream_init (stream, 0, 1, sizeof (SSI_ANDROIDSENSORS_HEART_RATE_SAMPLE_TYPE), SSI_REAL, 0);
                }
                ~HeartRateChannel () {
                        ssi_stream_destroy (stream);
                }

                const ssi_char_t *getName () { return SSI_ANDROIDSENSORS_HEART_RATE_PROVIDER_NAME; };
                const ssi_char_t *getInfo () { return "Reports heart rate, float value"; };
                ssi_stream_t getStream () { return stream; };
                ssi_stream_t* getStreamPtr(){return &stream;};

        protected:
                ssi_stream_t stream;
};
}

#endif // ANDROIDSENSORCHANNELS_H

