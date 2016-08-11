#ifndef ANDROIDSENSORCHANNELS_H
#define ANDROIDSENSORCHANNELS_H

#include "ssjSensor.h"
#include "base/ISensor.h"
namespace ssi{


//#define SSI_SSJ_SAMPLE_TYPE		ssi_real_t
#define SSI_SSJ_PROVIDER_NAME	"ssj"




class SSJFloatChannel : public IChannel {

        friend class ssjSensor;

        public:

                SSJFloatChannel (int num=1, int dim=1 ) {
                        ssi_stream_init (stream, num, dim, sizeof (float), SSI_REAL, 0);
                }
                ~SSJFloatChannel () {
                        ssi_stream_destroy (stream);
                }

                const ssi_char_t *getName () { return SSI_SSJ_PROVIDER_NAME; };
                const ssi_char_t *getInfo () { return "ssj"; };
                ssi_stream_t getStream () { return stream; };
                ssi_stream_t* getStreamPtr(){return &stream;};

        protected:
                ssi_stream_t stream;
};

}

#endif // ANDROIDSENSORCHANNELS_H

