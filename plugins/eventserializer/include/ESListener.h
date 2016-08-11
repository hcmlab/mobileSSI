// ESListener.h
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

#ifndef _ESLISTENER_H
#define _ESLISTENER_H

#include "base/IObject.h"
#include "ioput/option/OptionList.h"

namespace ssi {

class ESListener : public IObject {

	class Options : public OptionList {

	public:

		Options () : dimension (0), nclasses (0) {

			setSenderName ("vsender");
			setEventName ("vevent");	

			addOption("dimension", &dimension, 1, SSI_INT, "dimension of output-vector");
			addOption("nclasses", &nclasses, 1, SSI_INT, "number of classes of mapped classifier");
			addOption("sname", sname, SSI_MAX_CHAR, SSI_CHAR, "name of sender");
			addOption("ename", ename, SSI_MAX_CHAR, SSI_CHAR, "name of event");

		};

		void setSenderName (const ssi_char_t *sname) {			
			if (sname) {
				ssi_strcpy (this->sname, sname);
			}
		}
		void setEventName (const ssi_char_t *ename) {
			if (ename) {
				ssi_strcpy (this->ename, ename);
			}
		}

		ssi_size_t dimension;
		ssi_size_t nclasses;
		ssi_char_t sname[SSI_MAX_CHAR];
		ssi_char_t ename[SSI_MAX_CHAR];		
	};

public:

	static const ssi_char_t *GetCreateName () { return "ESListener"; };
	static IObject *Create (const ssi_char_t *file) { return new ESListener (file); };
	~ESListener ();

	Options *getOptions () { return &_options; };
	const ssi_char_t *getName () { return GetCreateName (); };
	const ssi_char_t *getInfo () { return "takes events sent from classification models, mapps confidence values into the vector space and forwards them to the vector fusion."; };

	void listen_enter ();
	bool update (IEvents &events, ssi_size_t n_new_events, ssi_size_t time_ms);
	void listen_flush ();
		
	std::vector<ssi_event_t*> getEvents() 
	{
		std::vector<ssi_event_t*> out = _events;
		_events.clear();
		
		return out;
	};

protected:

	ESListener (const ssi_char_t *file = 0);
	Options _options;
	static char ssi_log_name[];
	ssi_char_t *_file;
	int ssi_log_level;

	ssi_char_t _buffer[SSI_MAX_CHAR];
	std::vector<ssi_event_t*> _events;	
};

}

#endif
