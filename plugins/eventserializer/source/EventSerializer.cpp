// EventSerializer.cpp
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

#include "EventSerializer.h"

#ifdef USE_SSI_LEAK_DETECTOR
#include "SSI_LeakWatcher.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


namespace ssi {

ssi_char_t *EventSerializer::ssi_log_name = "evserial_";

EventSerializer::EventSerializer (const ssi_char_t *file) 
	: m_event_provider (0), 
	_file (0), 
	ssi_log_level (SSI_LOG_LEVEL_DEFAULT), 
	_timer (0), 
	_buffer (0), 
	_listener(0) {	
			
	if (file) {
		if (!OptionList::LoadXML(file, &_options)) {
			OptionList::SaveXML(file, &_options);
		}
		_file = ssi_strcpy (file);
	}

}

EventSerializer::~EventSerializer () {

	if (_file) {
		OptionList::SaveXML(_file, &_options);
		delete[] _file;
	}
}

bool EventSerializer::setProvider (const ssi_char_t *name, IProvider *provider) {


	if (strcmp (name, SSI_EVENTSERIALIZER_EVENT_PROVIDER_NAME) == 0) {
		setEventProvider (provider);
		return true;
	}
	ssi_wrn ("unkown provider name '%s'", name);

	return false;
}

void EventSerializer::setEventProvider (IProvider *event_provider) {

	if (m_event_provider) {
		ssi_wrn ("event provider already set");
	}

	m_event_provider = event_provider;
	if (m_event_provider) {
		m_event_channel.stream.sr = _options.sr;
		m_event_channel.stream.dim = _options.dimension;
		m_event_provider->init (&m_event_channel);
		ssi_msg (SSI_LOG_LEVEL_DETAIL, "event provider set");
	}
}

bool EventSerializer::connect () {

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "try to connect sensor...");

	_buffer_size = SSI_EVENTSERIALIZER_BUFFER_SIZE;
	_buffer = new ssi_byte_t[_buffer_size];

	//set up event listener
	ITheEventBoard * eventboard = Factory::GetEventBoard ();
	_listener = ssi_create(ESListener, 0, true);
	eventboard->RegisterListener (*_listener, _options.eventAddr);

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "connected");

	// set thread name
	Thread::setName (getName ());

	return true;
}

void EventSerializer::run () {

	if (!_timer) {
		_timer = new Timer (1.0/_options.sr);
	}

	enter();
	process();
	provide();

	_timer->wait ();
}

void EventSerializer::enter () {

}

void EventSerializer::process () {

	if (m_event_provider && _listener) 
	{
		ssi_real_t val;
		ssi_real_t* out_ptr = ssi_pcast(ssi_real_t, _buffer);
		ssi_size_t sample_num = 0;

		std::vector<ssi_event_t*> events = _listener->getEvents();
		if(events.size() == 0)
		{
			//no events, send 0
			for (ssi_size_t i = 0; i < _options.dimension; i++) {
				*out_ptr++ = 0;
			}
			++sample_num;

			return;
		}
		

		ITheEventBoard *board = ssi::Factory::GetEventBoard ();
		
		std::vector<ssi_event_t*>::iterator i = events.begin();
		std::vector<ssi_event_t*>::iterator i_end = events.end();
		while(i != i_end)
		{	
			ssi_event_t *e = (*i);

			switch (e->type) {
				case SSI_ETYPE_EMPTY:
					
					if(1 != _options.dimension)
						ssi_wrn("dimension mismatch (received %d, expected %d)", 1, _options.dimension);

					val = 1;
					memcpy(out_ptr, &val, sizeof(ssi_real_t)); out_ptr += sizeof(ssi_real_t);
					++sample_num;

					break;

				case SSI_ETYPE_STRING:
					
					if(1 != _options.dimension)
						ssi_wrn("dimension mismatch (received %d, expected %d)", 1, _options.dimension);
					
					val = 1;
					memcpy(out_ptr, &val, sizeof(ssi_real_t)); out_ptr += sizeof(ssi_real_t);
					++sample_num;

					break;

				case SSI_ETYPE_TUPLE: {
						ssi_size_t n = e->tot / sizeof (ssi_real_t);
						if(n != _options.dimension)
							ssi_wrn("dimension mismatch (received %d, expected %d)", n, _options.dimension);

						memcpy(out_ptr, e->ptr, n* sizeof(ssi_real_t)); out_ptr += n* sizeof(ssi_real_t);
						++sample_num;
					}
					break;	

				case SSI_ETYPE_MAP: {
						ssi_size_t n = e->tot / sizeof (ssi_event_map_t);
						ssi_event_map_t *p = ssi_pcast (ssi_event_map_t, e->ptr);
					
						if(_options.tupleMaxValue)
						{							
							if(_options.dimension != 1)
								ssi_wrn("dimension mismatch (received %d, expected %d)", 1, _options.dimension);

							ssi_real_t max_val = 0;
							ssi_size_t max_i = 0;
							for (ssi_size_t i = 0; i < n; i++) {
								if (p[i].value > max_val)
								{
									max_val = p[i].value;
									max_i = i;
								}
							}

							memcpy(out_ptr, &max_val, sizeof(ssi_real_t)); out_ptr += sizeof(ssi_real_t);
							++sample_num;
						}
						else
						{
							if(n != _options.dimension)
								ssi_wrn("dimension mismatch (received %d, expected %d)", n, _options.dimension);

							for (ssi_size_t i = 0; i < n; i++) {
								*out_ptr++ = p[i].value;
							}
							++sample_num;
						}
					}
					break;	

				default:
					break;
			}	

			//ww can handle only one event at a time
			if(sample_num > 0)
			{
				if(sample_num > 1)
					ssi_wrn("too many events. Some have been ignored");

				break;
			}
			++i;
		}

		if(sample_num == 0)
		{
			//no events, send 0
			for (ssi_size_t i = 0; i < _options.dimension; i++) {
				*out_ptr++ = 0;
			}
			++sample_num;
		}
	}
}	

void EventSerializer::provide () {

	if (m_event_provider) {
		m_event_provider->provide(_buffer, 1);
	}	
}

bool EventSerializer::disconnect () {

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "try to disconnect sensor...");

	delete _timer; _timer = 0;
	delete[] _buffer; _buffer = 0;

	ssi_msg (SSI_LOG_LEVEL_DETAIL, "sensor disconnected");

	return true;
}	

}

