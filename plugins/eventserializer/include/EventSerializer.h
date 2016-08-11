// EventSerializer.h
// author: Ionut Damian <damian@informatik.uni-augsburg.de>
// created: 2013/03/14
// Copyright (C) University of Augsburg, Lab for Human Centered Multimedia
//
// *************************************************************************************************
//
// This file is part of Social Signal Interpretation (SSI) developed at the 
// Lab for Human Centered Multimedia of the University of Augsburg
//
// This library is free software; you can redistribute itand/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or any laterversion.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FORA PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along withthis library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
//*************************************************************************************************

#pragma once

#ifndef SSI_SENSOR_EVENTSERIALIZER_H
#define	SSI_SENSOR_EVENTSERIALIZER_H

#include "base/ISensor.h"
#include "base/IProvider.h"
#include "thread/Timer.h"
#include "thread/Thread.h"
#include "ioput/option/OptionList.h"
#include "ioput/socket/Socket.h"
#include "base/Factory.h"

#include "ESListener.h"


namespace ssi {

#define SSI_EVENTSERIALIZER_CHANNEL_NUM 1
#define SSI_EVENTSERIALIZER_EVENT_CHANNEL_NUM 0
#define SSI_EVENTSERIALIZER_EVENT_PROVIDER_NAME "event"
#define SSI_EVENTSERIALIZER_BUFFER_SIZE 2048

class EventSerializer : public ISensor, public Thread {

public:
	friend class ESListener;
	
public:

	class EventChannel : public IChannel {
	
		friend class EventSerializer;
		
		public:
			EventChannel () {
				ssi_stream_init (stream, 0, 0, sizeof(ssi_real_t), SSI_REAL, 0); 
			}
			~EventChannel () {
				ssi_stream_destroy (stream);
			}
			const ssi_char_t *getName () { return SSI_EVENTSERIALIZER_EVENT_PROVIDER_NAME; };
			const ssi_char_t *getInfo () { return "xx"; };
			ssi_stream_t getStream () { return stream; };
		protected:
			ssi_stream_t stream;
	};

public:

	class Options : public OptionList {

	public:

		Options ()
			: sr (120.0), tupleMaxValue(false), dimension(0) {

			eventAddr[0] = '\0';

			addOption ("sr", &sr, 1, SSI_TIME, "update rate in hz");
			addOption ("tupleMaxValue", &tupleMaxValue, 1, SSI_BOOL, "compute max value from tuple events");
			addOption ("eventAddr", &eventAddr, SSI_MAX_CHAR, SSI_CHAR, "event address to listen");	
			addOption ("dimension", &dimension, 1, SSI_SIZE, "the number of values per event");
		};	

		void setEventAddress(const ssi_char_t* address)
		{
			strcpy(eventAddr, address);
		};

		ssi_time_t sr;
		ssi_size_t dimension;
		bool tupleMaxValue;
		ssi_char_t eventAddr[SSI_MAX_CHAR];
	};

public:
	
	static const ssi_char_t *GetCreateName () { return "EventSerializer"; };
	static IObject *Create (const ssi_char_t *file) { return new EventSerializer (file); };
	~EventSerializer ();
	EventSerializer::Options *getOptions () { return &_options; };
	const ssi_char_t *getName () { return GetCreateName (); };
	const ssi_char_t *getInfo () { return "xx"; };

	ssi_size_t getChannelSize () { return SSI_EVENTSERIALIZER_CHANNEL_NUM; };
	IChannel *getChannel (ssi_size_t index) { 
		switch (index) {
			case SSI_EVENTSERIALIZER_EVENT_CHANNEL_NUM:
				return &m_event_channel;		
		}
		return 0;
	};
	bool setProvider (const ssi_char_t *name, IProvider *provider);
	bool connect ();
	bool start () { return Thread::start (); };
	bool stop () { return Thread::stop (); };
	void run ();
	bool disconnect ();

	void setLogLevel (int level) {
		ssi_log_level = level;
	}

protected:

	EventSerializer (const ssi_char_t *file = 0);
	EventSerializer::Options _options;
	ssi_char_t *_file;

	ESListener *_listener;
		
	Timer *_timer;
	ssi_byte_t *_buffer;
	int _buffer_size;

	static ssi_char_t *ssi_log_name;
	int ssi_log_level;
	
	void enter();
	void process();
	void provide();	

	void setEventProvider (IProvider *event_provider);
	EventChannel m_event_channel;
	IProvider *m_event_provider;
};

}

#endif

