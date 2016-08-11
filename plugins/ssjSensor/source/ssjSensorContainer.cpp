#include "ssjSensor.h"
namespace ssi{

 ssjSensorContainer* ssjSensorContainer::instance=0;
 std::vector<SSJSensorNeccessities*>* ssjSensorContainer::sensorOptions=0;

        SSJSensorNeccessities* ssjSensorContainer::getInstance(int id)
        {
            getContainer();

            for(int i=0; i< sensorOptions->size(); i++)
            {
                if(sensorOptions->at(i)->id==id)
                    return sensorOptions->at(i);
            }
            return NULL;
        }
        void ssjSensorContainer::addInstance(SSJSensorNeccessities* inst)
        {
            sensorOptions->push_back(inst);
        }


        void ssjSensorContainer::ssjSensorContainer::remInstance(int id)
        {
            if (!instance) return;

            SSJSensorNeccessities* bare=getInstance(id);
            for(int i=0; i< sensorOptions->size(); i++)
            {
                if(sensorOptions->at(i)->id==id);
              sensorOptions->erase(ssjSensorContainer::sensorOptions->begin()+i);
            }

            delete bare;
            if(sensorOptions->empty())
            {
               delete instance;
                instance=NULL;
                ssi_wrn("REM INSTANCWE")
            }

        }


        ssjSensorContainer* ssjSensorContainer::ssjSensorContainer::getContainer()
        {
            if(!instance)
            {
                ssi_wrn("NEW INSTANCWE");
                instance=new ssjSensorContainer();
                sensorOptions=new std::vector<SSJSensorNeccessities*>();
            }
        return instance;
        }




}
