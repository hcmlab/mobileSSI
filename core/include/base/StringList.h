// StringList.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/11/04
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

#pragma once

#ifndef SSI_SIGNAL_STRINGLIST_H
#define SSI_SIGNAL_STRINGLIST_H

#include "base/String.h"

namespace ssi {

class StringList : public std::vector<String> {

public:

	StringList ();
	virtual ~StringList ();

	void add (const ssi_char_t *string);
	bool find(const ssi_char_t *string, bool case_sensitive = true, ssi_size_t n = 0);
	bool remove(const ssi_char_t *string, bool case_sensitive = true, ssi_size_t n = 0);
	bool parse(const ssi_char_t *string, ssi_char_t delim);
	const ssi_char_t *get(ssi_size_t index);
	void print (FILE *file);
};

}

#endif
