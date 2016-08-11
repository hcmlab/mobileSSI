// MyBeaconScanner.h
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

#pragma once

#ifndef SSI_BEACONSCANNER_MYBEACONSCANNER_H
#define SSI_BEACONSCANNER_MYBEACONSCANNER_H

#include "base/IObject.h"
#include "ioput/option/OptionList.h"
#include "event/EventAddress.h"
#include "thread/Thread.h"
#include <serial/serial.h>

namespace ssi {

	class MyBeaconScanner : public IObject, public Thread {

	public:

		class Options : public OptionList {

		public:

			Options()
				: blePackets(false), port(11), crcCheck(false) {

				string[0] = '\0';
				addOption ("port", &port, 1, SSI_SIZE, "port number");
				addOption("blePackets", &blePackets, 1, SSI_BOOL, "include BLE packets");
				addOption("crcCheck", &crcCheck, 1, SSI_BOOL, "crc check (just tested with nrf24 broadcast!)");

			}

			void setString(const ssi_char_t *string) {
				this->string[0] = '\0';
				if (string) {
					ssi_strcpy(this->string, string);
				}
			}

			bool blePackets;
			bool crcCheck;

			ssi_char_t string[SSI_MAX_CHAR];
			ssi_size_t port;
		};

	public:

		static const ssi_char_t *GetCreateName() { return "BeaconScanner"; };
		static IObject *Create(const ssi_char_t *file) { return new MyBeaconScanner(file); };
		~MyBeaconScanner();

		Options *getOptions() { return &_options; };
		const ssi_char_t *getName() { return GetCreateName(); };
		const ssi_char_t *getInfo() { return "iBeacon scanner for nordic Bluetooth LE Sniffer via serial port"; };

		bool printIbeaconPackage(uint8_t* package, int packetLen);
		double accuracy(int rssi, int txPower);

		void run();
		void flush();
		void enter();
		bool setEventListener(IEventListener *listener);

		const ssi_char_t *getEventAddress() {
			return _event_address.getAddress();
		}

	protected:

		MyBeaconScanner(const ssi_char_t *file = 0);
		ssi_char_t *_file;
		Options _options;
		static char ssi_log_name[];
		
		IEventListener *_elistener;
		ssi_event_t _event;
		ssi_event_t _event_BLE;
		EventAddress _event_address;
		int i;
		serial::Serial* my_serial;
	};

}

#endif
