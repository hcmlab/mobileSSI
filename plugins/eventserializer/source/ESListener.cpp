// ESListener.cpp
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

#include "../include/ESListener.h"

namespace ssi {

char ESListener::ssi_log_name[] = "evseriall_";

ESListener::ESListener (const ssi_char_t *file)
	:	_file (0) {
		
	_buffer[0] = '\0';

	if (file) {
		if (!OptionList::LoadXML(file, &_options)) {
			OptionList::SaveXML(file, &_options);
		}
		_file = ssi_strcpy (file);
	}
}

ESListener::~ESListener () {	
}

void ESListener::listen_enter () {
}

bool ESListener::update (IEvents &events, ssi_size_t n_new_events, ssi_size_t time_ms) {

	for (ssi_size_t i = 0; i < n_new_events; i++) 
	{
		ssi_event_t *e = events.next ();
		_events.push_back(e);
	}

	return true;
}

void ESListener::listen_flush () {
}

}
