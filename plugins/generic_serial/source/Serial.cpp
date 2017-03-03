#include "Serial.h"
#include "SSI_Cons.h"
#include <string>

Serial::Serial(const char *portName, uint32_t speed )
{
    this->connected = false;

	char com [100];
	snprintf(com, 100, "\\\\.\\%s", portName);

    std:: string port(portName);
    
    this->hSerial= new serial::Serial(port, speed, serial::Timeout::simpleTimeout(1000));
    
    std::vector<serial::PortInfo> ports = serial::list_ports();

    bool exists = false;
    for (int i = 0; i < ports.size(); i++)
    {
        if (ports[i].port.compare(port) == 0)
            exists = true;
    }

    if (!exists) ssi_err("Port %s not found!", port);

    this->hSerial->setRTS(true);

    if(this->hSerial->isOpen())
            this->connected = true;

printf("####connected?: %d \n", this->connected);
}

Serial::~Serial()
{
    if(this->connected)
    {
        this->connected = false;
        delete this->hSerial;
        
    }
}

int Serial::ReadData(char *buffer, unsigned int nbChar)
{
    //Number of bytes we'll have read
    uint32_t bytesRead;
    //Number of bytes we'll really ask to read
    uint32_t toRead = nbChar;
    //Use the ClearCommError function to get status info on the Serial port
    std::string result;
    int i=0;

    while(toRead > 0)
    {
        result= hSerial->read();
        memcpy(buffer+i, result.c_str(),result.length());
        i+=result.length();
		toRead -= result.length();
    }

    return nbChar;
}


bool Serial::WriteData(char *buffer, unsigned int nbChar)
{
    uint32_t bytesSend;
    std::string sendString=std::string(buffer);

    //Try to write the buffer on the Serial port
   bytesSend= this->hSerial->write(sendString);
    
    return nbChar == bytesSend;
}

bool Serial::IsConnected()
{
    return this->connected;
}

