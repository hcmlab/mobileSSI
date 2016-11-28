// EventTimeWarp.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/05/26
// Copyright (C) University of Augsburg, Lab for Human Centered Multimedia
//
// *************************************************************************************************
//
// This file is part of Social Signal Interpretation (SSI) developed at the
// Lab for Human Centered Multimedia of the University of Augsburg
//
// This library is free software; you can redistribute itand/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or any laterversion.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FORA PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along withthis library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
//*************************************************************************************************

#include "EventTimeWarp.h"
#include "base/Factory.h"
#include "frame/include/TheFramework.h"

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif


namespace ssi {

ssi_char_t *EventTimeWarp::ssi_log_name = "thresesend";

EventTimeWarp::EventTimeWarp (const ssi_char_t *file)
: _hangover_in (0),
	_hangover_out (0),
	_samples_max_dur (0),
	_counter_max_dur (0),
	_max_dur (0),
	_min_dur (0),
	_loffset (0),
	_uoffset (0),
	_skip_on_max_dur (false),
	_file (0),
	_check (0),
	_elistener (0),
	ssi_log_level (SSI_LOG_LEVEL_DEFAULT) {

	if (file) {
		if (!OptionList::LoadXML (file, _options)) {
			OptionList::SaveXML (file, _options);
		}
		_file = ssi_strcpy (file);
	}

	ssi_event_init(_event);
}

EventTimeWarp::~EventTimeWarp () {

	if (_file) {
		OptionList::SaveXML (_file, _options);
		delete[] _file;
	}

	ssi_event_destroy (_event);
}

bool EventTimeWarp::setEventListener (IEventListener *listener) {


        ssi_event_init (_event, SSI_ETYPE_MAP);



	_elistener = listener;

	if (_options.address[0] != '\0') {

		SSI_OPTIONLIST_SET_ADDRESS(_options.address, _event_address, _event);

	} else {

		ssi_wrn("use of deprecated option 'sname' and 'ename', use 'address' instead")

			_event.sender_id = Factory::AddString(_options.sname);
		if (_event.sender_id == SSI_FACTORY_STRINGS_INVALID_ID) {
			return false;
		}
		_event.event_id = Factory::AddString(_options.ename);
		if (_event.event_id == SSI_FACTORY_STRINGS_INVALID_ID) {
			return false;
		}

		_event_address.setSender(_options.sname);
		_event_address.setEvents(_options.ename);
	}

    return true;
}

void EventTimeWarp::listen_enter() {}
void EventTimeWarp::listen_flush() {}

bool EventTimeWarp::update(IEvents &events, ssi_size_t n_new_events, ssi_size_t time_ms)
{
    if (!events.getSize() || !n_new_events){
        return false;
    }
    events.reset();
    ssi_event_t * event_source = 0;


    for (ssi_size_t i = 0; i < n_new_events; i++){

        //Go to the last new event
        event_source = events.next();

       _event=*event_source;

        _event.time = Factory::GetFramework ()->GetElapsedTimeMs();


        if (_options.address[0] != '\0') {

            SSI_OPTIONLIST_SET_ADDRESS(_options.address, _event_address, _event);

        } else {

            ssi_wrn("use of deprecated option 'sname' and 'ename', use 'address' instead")

                _event.sender_id = Factory::AddString(_options.sname);
            if (_event.sender_id == SSI_FACTORY_STRINGS_INVALID_ID) {
                return false;
            }
            _event.event_id = Factory::AddString(_options.ename);
            if (_event.event_id == SSI_FACTORY_STRINGS_INVALID_ID) {
                return false;
            }

            _event_address.setSender(_options.sname);
            _event_address.setEvents(_options.ename);
        }


        //if(event contains next video){

        _elistener->update(_event);

    //}
       //else nothing to do

    }



    return true;
}



}
