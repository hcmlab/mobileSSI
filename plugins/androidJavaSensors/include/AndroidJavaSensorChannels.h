#ifndef ANDROIDJAVASENSORCHANNELS_H
#define ANDROIDJAVASENSORCHANNELS_H

#include "AndroidJavaSensors.h"
#include "base/ISensor.h"


namespace ssi{

#define SSI_ANDROIDJAVASENSORS_TYPE		ssi_real_t
#define SSI_ANDROIDJAVASENSORS_NAME	   "androidjava"
class AndroidJavaSensors;

class AndroidJavaChannel : public IChannel {

        friend class AndroidJavaSensors;

        public:


                AndroidJavaChannel () {
                    //todo MODALITIES::N_MODLAITIES
                        ssi_stream_init (stream, 0, 2, sizeof (SSI_ANDROIDJAVASENSORS_TYPE), SSI_REAL, 0);
                }
                ~AndroidJavaChannel () {
                        ssi_stream_destroy (stream);
                }

                const ssi_char_t *getName () { return SSI_ANDROIDJAVASENSORS_NAME; };
                const ssi_char_t *getInfo () { return "Reports gps"; };
                ssi_stream_t getStream () { return stream; };
                ssi_stream_t* getStreamPtr(){return &stream;};

        protected:
                ssi_stream_t stream;
};

}


#endif // ANDROIDJAVASENSORCHANNELS_H
