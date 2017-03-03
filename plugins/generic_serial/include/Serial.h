#ifndef SERIALCLASS_H_INCLUDED
#define SERIALCLASS_H_INCLUDED

#include <serial/serial.h>
#include <stdio.h>
#include <stdlib.h>

class Serial
{
    private:
        //Serial comm handler
        serial::Serial* hSerial;
        //Connection status
        bool connected;
        //Get various information about the connection
        uint32_t status;
        //Keep track of last error
        uint32_t errors;

    public:
        //Initialize Serial communication with the given COM port
        Serial(const char *portName, uint32_t speed = 115200);
        //Close the connection
        //NOTA: for some reason you can't connect again before exiting
        //the program and running it again
        ~Serial();
        //Read data in a buffer. The function return -1 when nothing could
        //be read, the number of bytes actually read.
        int ReadData(char *buffer, uint32_t nbChar);
        //Writes data from a buffer through the Serial connection
        //return true on success.
        bool WriteData(char *buffer, uint32_t nbChar);
        //Check if we are actually connected
        bool IsConnected();
};

#endif // SERIALCLASS_H_INCLUDED
