// GenericSerial.cpp
// author: Andreas Seiderer <seiderer@hcm-lab.de>
// created: 9/3/2015
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

#include "GenericSerial.h"
#include "Serial.h"
#include <vector>

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

using namespace std;
namespace ssi {

static char ssi_log_name[] = "genericser";




GenericSerial::GenericSerial (const ssi_char_t *file) 
	: _serial_provider (0),
	_serial (0),
	_is_connected (false),
	_file (0),
	_serial_channel (0),
	_buffer (0), 
	_buffer_ptr (0),
	fpsValuecount(0),
	avgFps(0.0),
	ssi_log_level (SSI_LOG_LEVEL_DEFAULT) {
	
	getOptions()->setStartCMD(0);
	getOptions()->setStopCMD(0);
	getOptions()->setDeviceInstanceId(0);

	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}

	//possbile baud rates from windows
	baudRates[110]		= 110;
	baudRates[300]		= 300;
	baudRates[600]		= 600;
	baudRates[1200]		= 1200;
	baudRates[2400]		= 2400;
	baudRates[4800]		= 4800;
	baudRates[9600]		= 9600;
	baudRates[14400]	= 14400;
	baudRates[19200]	= 19200;
	baudRates[38400]	= 38400;
	baudRates[56000]	= 56000;
	baudRates[57600]	= 57600;
	baudRates[115200]	= 115200;
	baudRates[128000]	= 128000;
	baudRates[256000]	= 256000;

}

GenericSerial::~GenericSerial () {

	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}

	delete _serial_channel;
}

bool GenericSerial::setProvider (const ssi_char_t *name, IProvider *provider) {

	if (strcmp (name, SSI_GENERICSERIAL_PROVIDER_NAME) == 0) {
		setSerialProvider (provider);
		return true;
	}

	ssi_wrn ("unkown provider name '%s'", name);

	return false;
}

void GenericSerial::setSerialProvider (IProvider *provider) {

	if (_serial_provider) {
		ssi_wrn ("serial provider was already set");
	}

	_serial_provider = provider;
	if (_serial_provider) {		
		_serial_provider->init (getChannel (0));
		ssi_msg (SSI_LOG_LEVEL_DETAIL, "serial provider set");
	}
}

bool GenericSerial::connect () {

	//check for com port
    
    vector<serial::PortInfo> devices_found = serial::list_ports();
    
    vector<serial::PortInfo>::iterator iter = devices_found.begin();
    
    while( iter != devices_found.end() )
    {
        serial::PortInfo device = *iter++;
        
        printf( "(%s, %s, %s)\n", device.port.c_str(), device.description.c_str(),
               device.hardware_id.c_str() );
    }

    _serial=new Serial(_options.port, _options.baud);
	_frame_size = ssi_cast (ssi_size_t, _options.size * _options.sr + 0.5);
	_counter = 0;
	_buffer = new ssi_real_t[ _frame_size * _serial_channel->getStream().dim];
	_buffer_ptr = _buffer;
    _is_connected=_serial->IsConnected();

	return true;
}

void GenericSerial::run () {

	if (_serial && _is_connected) {

		int pos = 0;
		bool found = false;

		std::string str = "";

		//search for new line
		do {
			char c;
			int r = _serial->ReadData (&c, 1);
			str += c;
			if (r > 0 && c == '\n') {
				found = true;
			}
		} while (!found);


		//ssi_msg(SSI_LOG_LEVEL::SSI_LOG_LEVEL_BASIC, "%s", str.c_str());

		if (found) {
			std::vector<float> buffer;
			std::stringstream ss(str);

			float i;
			while (ss >> i) {
				buffer.push_back(i);

				if (ss.peek() == getOptions()->separator)
					ss.ignore();
			}

			//check if value count is ok
			if (buffer.size() == getOptions()->dim) {
				//copy from vector into outgoing buffer
				memcpy(_buffer_ptr, &buffer[0], buffer.size() * sizeof(float));
				_buffer_ptr += buffer.size();





				_counter++;
			}
			else {
				ssi_wrn("Value number (%d) from serial port differs from dim () -> dropping!", buffer.size(), getOptions()->dim);
			}

			buffer.clear();

		}


		if (_counter == _frame_size) {
			_counter = 0;
			if (_serial_provider) {
				_serial_provider->provide (ssi_pcast (char, _buffer), _frame_size);
				SSI_DBG (SSI_LOG_LEVEL_DEBUG, "serial data provided");

				
			}

			_buffer_ptr = _buffer;
		}
	}
}

bool GenericSerial::disconnect() {


	if (getOptions()->stopCMD != "") {
		ssi_msg(SSI_LOG_LEVEL_BASIC, "sending stop message ...");
		_serial->WriteData(getOptions()->startCMD, strlen(getOptions()->startCMD));
	}

	ssi_msg(SSI_LOG_LEVEL_BASIC, "try to disconnect sensor ...");

	if (!_serial || !_serial->IsConnected()) {
		ssi_wrn("sensor not connected");
		return false;
	}

	delete _serial; _serial = 0;

	if (_buffer) {
		delete[] _buffer; _buffer = 0;
		_buffer_ptr = 0;
	}

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "sensor disconnected");

	return true;
}



void GenericSerial::listen_enter()
{
	
}


void GenericSerial::listen_flush()
{
	
}

bool GenericSerial::update(IEvents& events, ssi_size_t n_new_events, ssi_size_t time_ms)
{
	for (int i = 0; i < n_new_events; i++) {
		ssi_event_t *e = events.next();

		if (e->type == SSI_ETYPE_STRING)
		{
			if (_serial && _is_connected) {
				_serial->WriteData(e->ptr, e->tot_real);
			}
		}
		else {
			ssi_wrn("Unsupported event type! SSI_ETYPE_STRING only!");
		}
	}

	return true;
}




}
