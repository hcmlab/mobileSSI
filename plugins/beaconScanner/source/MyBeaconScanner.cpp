// MyBeaconScanner.cpp
// author: Simon Flutura <simon.flutura@informatik.uni-augsburg.de>, Andreas Seiderer <seiderer@hcm-lab.de>
// created: 12/5/2015
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

#include "MyBeaconScanner.h"
#include "base/Factory.h"

#include <iostream>
#include <serial/serial.h>

#define SLIP_START  0xAB
#define SLIP_END  0xBC
#define SLIP_ESC  0xCD
#define SLIP_ESC_START  SLIP_START+1
#define SLIP_ESC_END  SLIP_END+1
#define SLIP_ESC_ESC  SLIP_ESC+1


#define SYNCWORD_POS 0
#define HEADER_LEN_POS 0
#define PAYLOAD_LEN_POS HEADER_LEN_POS+1
#define PROTOVER_POS PAYLOAD_LEN_POS+1
#define PACKETCOUNTER_POS PROTOVER_POS+1
#define ID_POS PACKETCOUNTER_POS+2

#define BLE_HEADER_LEN_POS ID_POS+1
#define FLAGS_POS BLE_HEADER_LEN_POS+1
#define CHANNEL_POS FLAGS_POS+1
#define RSSI_POS CHANNEL_POS+1
// OS Specific sleep
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
using namespace std;

namespace ssi {

	char MyBeaconScanner::ssi_log_name[] = "beaconscan";

	MyBeaconScanner::MyBeaconScanner(const ssi_char_t *file)
		: _file(0), _elistener(0), my_serial(0) {

		if (file) {
			if (!OptionList::LoadXML(file, _options)) {
				OptionList::SaveXML(file, _options);
			}
			_file = ssi_strcpy(file);
		}

		ssi_event_init(_event, SSI_ETYPE_STRING);
		ssi_event_adjust(_event, 100);

		ssi_event_init(_event_BLE, SSI_ETYPE_STRING);
		ssi_event_adjust(_event_BLE, 100);
	}

	MyBeaconScanner::~MyBeaconScanner() {

		if (my_serial) {
			if (my_serial->isOpen())
				my_serial->close();
			delete my_serial;
		}


		if (_file) {
			OptionList::SaveXML(_file, _options);
			delete[] _file;
		}

		ssi_event_destroy(_event);
		ssi_event_destroy(_event_BLE);
	}


	double MyBeaconScanner::accuracy(int rssi, int txPower)
	{
		if (rssi == 0)
			return -1.0;

		double ratio = rssi*1.0 / txPower;
		if (ratio < 1.0){

			return pow(ratio, 10);
		}
		else
		{
			double accuracy = (0.89976*pow(ratio, 7.7095) + 0.111);

			return accuracy;
		}
	}

	//https://github.com/lijunxyz/nRF24_BLE/blob/master/Arduino/nRF24_BLE_advertizer_demo/nRF24_BLE_advertizer_demo.ino
	void btLeCrc(const uint8_t* data, uint8_t len, uint8_t* dst){
		// implementing CRC with LFSR
		uint8_t v, t, d;

		while (len--){
			d = *data++;
			for (v = 0; v < 8; v++, d >>= 1){
				t = dst[0] >> 7;
				dst[0] <<= 1;
				if (dst[1] & 0x80) dst[0] |= 1;
				dst[1] <<= 1;
				if (dst[2] & 0x80) dst[1] |= 1;
				dst[2] <<= 1;

				if (t != (d & 1)){
					dst[2] ^= 0x5B;
					dst[1] ^= 0x06;
				}
			}
		}
	}


	uint8_t  swapbits(uint8_t a){
		// reverse the bit order in a single byte
		uint8_t v = 0;
		if (a & 0x80) v |= 0x01;
		if (a & 0x40) v |= 0x02;
		if (a & 0x20) v |= 0x04;
		if (a & 0x10) v |= 0x08;
		if (a & 0x08) v |= 0x10;
		if (a & 0x04) v |= 0x20;
		if (a & 0x02) v |= 0x40;
		if (a & 0x01) v |= 0x80;
		return v;
	}


	bool MyBeaconScanner::printIbeaconPackage(uint8_t* package, int packetLen)
	{
		bool isBeacon = false;
		bool isBLE = false;
		int index = 0;


		if ((package[PAYLOAD_LEN_POS + 1] + package[HEADER_LEN_POS + 1]) != packetLen - 1)
		{
			//printf("payloadlen:%d headerlen: %d packetLen: %d\n",package[PAYLOAD_LEN_POS+1], package[HEADER_LEN_POS+1],  packetLen -1);
			return false;
		}

		for (int i = 0; i < packetLen - 2; i++) {
			if (package[i] == 2 && package[i + 1] == 21)
			{
				index = i;
				isBeacon = true;
			}

			if (_options.blePackets && (package[i] == 1 && package[i + 1] == 5)) {
				index = i;
				isBLE = true;
			}

		}
		if (isBeacon)
		{

			double a = accuracy(package[RSSI_POS + 1] * -1, package[packetLen - 4] - 256);


			char data[100];
			for (int i = 0; i < 100; i++) data[i] = 0;
			char address[50];

			uint8_t addr_0 = package[index - 8];
			uint8_t addr_1 = package[index - 9];
			uint8_t addr_2 = package[index - 10];
			uint8_t addr_3 = package[index - 11];
			uint8_t addr_4 = package[index - 12];
			uint8_t addr_5 = package[index - 13];

			sprintf(address, "%X %X %X %X %X %X", addr_0, addr_1, addr_2, addr_3, addr_4, addr_5);

			sprintf(data, "[\"%s\", %d, %d, %f]", address, package[RSSI_POS + 1], package[packetLen - 4], a);
			//data[99] = '\0';


			ssi_event_adjust(_event, strlen(data) + 1);

			ssi_strcpy(_event.ptr, data);

			_event.time = Factory::GetFramework()->GetElapsedTimeMs();
			if (_elistener)
				_elistener->update(_event);
		}
		else if (isBLE)
		{			

			if (index - 10 < 0)
				return false;

			char lenpayload = package[index - 9];	//total bytes of the advertising payload + 6 bytes for the BLE mac address.

			//CRC
			if (_options.crcCheck) {

				uint8_t buf_this[40];

				buf_this[0] = package[index - 10];
				buf_this[1] = package[index - 9];
				//remove third value (0) for crc (found out by testing -> error?)

				for (int c = 2; c < lenpayload + 2; c++)
				{
					buf_this[c] = package[(index - 7) + (c - 2)];
				}

				//this is the same what is done in nrf24 Arduino firmware
				uint8_t crc[3] = { 0x55, 0x55, 0x55 };
				btLeCrc(buf_this, lenpayload + 2, crc);

				for (int i = 0; i < 3; i++)
					crc[i] = swapbits(crc[i]);

				int crc_index = index - 10 + lenpayload + 3;

				//compare calculated crc with received crc values
				if (package[crc_index] != crc[0] || package[crc_index + 1] != crc[1] || package[crc_index + 2] != crc[2]) {
					ssi_wrn("%i: CRC error in BLE packet -> dropped!", Factory::GetFramework()->GetElapsedTimeMs());
						return false;
				}
			}


			char PDU_type = package[index - 10];

			int namelen = package[index + 2] - 1;

			if (namelen < 0 || namelen >= packetLen)
				return false;

			std::string manufacturer(reinterpret_cast<char const*>(&package[index + 4]), namelen);


			int index_payloadlen = index + namelen + 4;

			if (index_payloadlen < 0 || index_payloadlen >= packetLen)
				return false;

			int payloadlen = package[index_payloadlen];

			std::string payload_str;
			stringstream ss;

			for (int i = 0; i < payloadlen; i++) {
				uint8_t val = package[index_payloadlen + 1 + i];
				ss << std::to_string(val);

				if (i < payloadlen-1)
					ss << ",";
			}
			payload_str = ss.str();

			uint8_t addr_0 = package[index - 2];
			uint8_t addr_1 = package[index - 3];
			uint8_t addr_2 = package[index - 4];
			uint8_t addr_3 = package[index - 5];
			uint8_t addr_4 = package[index - 6];
			uint8_t addr_5 = package[index - 7];


			stringstream data;
			data << "[\"";
			data << hex << (int)addr_0 << " " << hex << (int)addr_1 << " " << hex << (int)addr_2 << " " << hex << (int)addr_3 << " " << hex << (int)addr_4 << " " << hex << (int)addr_5 << "\", ";
			data << manufacturer << ", [" << payload_str << "]]";


			std::string datastr = data.str();

			ssi_event_adjust(_event_BLE, datastr.length() + 1);
			ssi_strcpy(_event_BLE.ptr, datastr.c_str());

			_event_BLE.time = Factory::GetFramework()->GetElapsedTimeMs();
			if (_elistener)
				_elistener->update(_event_BLE);


		}

		//if(isBeacon)printf("########################################################## %d, %d : %x huraaaa!\n", packetLen, index, package[index]);
		//if(isBeacon)for(int i=0; i< packetLen; i++)printf(" %x ",package[i]);
		//if(isBeacon)printf("\n##########################################################huraaaa!\n");
		return true;
	}


	void MyBeaconScanner::enter()
	{
		int i = 0;
		char port[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    #if _WIN32||_WIN64
		ssi_sprint(port, "COM%u", _options.port);
    #else
            ssi_sprint(port, "ttyUSB%u", _options.port);
    #endif

		int baud = 460800;

		//check if port exists
		std::vector<serial::PortInfo> ports = serial::list_ports();

		bool exists = false;
		for (int i = 0; i < ports.size(); i++)
		{
			if (ports[i].port.compare(port) == 0)
				exists = true;
		}

		if (!exists) ssi_err("Port %s not found!", port);

		my_serial = new serial::Serial(port, baud);
		my_serial->setRTS(true);
		//cout << "Is the serial port open?";
		//if(my_serial->isOpen())
		//  cout << " Yes." << endl;
		//else
		//  cout << " No." << endl;
	}


	void MyBeaconScanner::run()
	{

		// cout << "Timeout == 1000ms, asking for 1 more byte than written." << endl;
		vector<uint8_t> dataVec;

		dataVec.push_back(171);
		dataVec.push_back(6);
		dataVec.push_back(0);
		dataVec.push_back(1);
		dataVec.push_back(0);
		dataVec.push_back(0);
		dataVec.push_back(13);
		dataVec.push_back(188);


		int sendTN = 0;
		int bytesRead = 0;
		uint8_t rByte;
		const vector<uint8_t> dataCVec = dataVec;

		{
			vector<uint8_t> dataVec;
			dataVec.push_back(171);
			dataVec.push_back(6);
			dataVec.push_back(0);
			dataVec.push_back(1);
			dataVec.push_back(0);
			dataVec.push_back(0);
			dataVec.push_back(13);
			dataVec.push_back(188);

			dataVec.at(3) = i;//package number to little endian?

			const vector<uint8_t> dataCVec = dataVec;
			if (bytesRead == 0 && false){
				size_t bytes_wrote = my_serial->write(dataCVec);


			}
			uint8_t rbuffer[256];
			for (int i = 0; i < 256; i++)rbuffer[i] = 0;
			//TODO:
			// this is an old hound dog, clean up unnecessary stuff    
			int len = 1, packetLen = 0;

			for (int i = 0; i < 256; i++)
			{
				int ret = my_serial->read(&rByte, 1);
				//restart with 0 at SLIP_START
				if (i == 0 && rByte != SLIP_START)i--;

				else{
					if (rByte != SLIP_ESC) {
						assert(i >= 0 && i < 256);
						// if we dont have an escape sequence, cache data
						rbuffer[i] = rByte;
					}
					//ignore slip escape
					else i--;
					
					//reconstruct data without escape sequence
					if (rByte == SLIP_ESC_START)rByte = SLIP_START;
					if (rByte == SLIP_ESC_END)rByte = SLIP_ESC;

					if (!ret)
					{
						//did not receive anything, reset
						bytesRead = -5;
						for (int j = 0; j < 256; j++)
							rbuffer[j] = 0;
						break;
					}
					else
					{
						bytesRead = len;
					}


					if (rByte == SLIP_END)
					{
						packetLen = i;
						break;

					}
				}
			}
			printIbeaconPackage(rbuffer, packetLen);

			//todo bytesRead is statemachine
			for (int i = 0; i < 256 && bytesRead != -5; i++)
			{
				assert(i >= 0 && i < 256);

				//handle end sequence
				if (rbuffer[i] == SLIP_END)
				{

					if (bytesRead == -1)bytesRead--;
					if (bytesRead == -4)
					{
						bytesRead = 0;
						sendTN = 0;
					}
					if (i == 1)bytesRead = -1;


					break;
				}
				else if (rbuffer[i]){
					bytesRead = 1;

				}
				rbuffer[i] = 0;
			}

		}
		if (bytesRead == -5)
		{
			sendTN = 0;
			bytesRead = 0;
		}
		else
			if (bytesRead)sendTN++;

		i++;
	}


	void MyBeaconScanner::flush()
	{
		//char data[5] = { '\0', '\0', '\0', '\0', '\0' };
		//ssi_event_adjust(_event, 5);
		//ssi_strcpy(_event.ptr, data);


		//_event.tot = strlen(data)+1;
		//_event.tot_real = 100;

		//if (_elistener)
		//	_elistener->update(_event);
	}


	bool MyBeaconScanner::setEventListener(IEventListener *listener) {

		_elistener = listener;
		_event.sender_id = Factory::AddString("iBeaconScanner");
		if (_event.sender_id == SSI_FACTORY_STRINGS_INVALID_ID) {
			return false;
		}
		_event.event_id = Factory::AddString("iBeaconPackage");
		if (_event.event_id == SSI_FACTORY_STRINGS_INVALID_ID) {
			return false;
		}

		_event_BLE.sender_id = _event.sender_id;

		_event_BLE.event_id = Factory::AddString("BLEpackage");
		if (_event_BLE.event_id == SSI_FACTORY_STRINGS_INVALID_ID) {
			return false;
		}

		_event_address.setSender("iBeaconScanner");
		_event_address.setEvents("iBeaconPackage,BLEpackage");

		return true;
	}

}
