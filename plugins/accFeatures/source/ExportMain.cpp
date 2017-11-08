// ExportMain.cpp
// author: Michael Dietz <michael.dietz@informatik.uni-augsburg.de>
// created: 20/6/2016
// Copyright (C) University of Augsburg, Lab for Human Centered Multimedia
//
// *************************************************************************************************
//
// This file is part of Smart Sensor Integration (SSI) developed at the 
// Lab for Multimedia Concepts and Applications of the University of Augsburg
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

#include "ssisearchfeatures.h"
#include "StreamTransformer.h"
#include "base/Factory.h"

#ifndef DLLEXP
#if _WIN32|_WIN64
#define DLLEXP extern "C" __declspec( dllexport )
#else
#define DLLEXP extern "C" __attribute__((visibility("default")))
#endif
#endif

DLLEXP bool Register (ssi::Factory *factory, FILE *logfile, ssi::IMessage *message) {

	ssi::Factory::SetFactory (factory);

	if (logfile) {
		ssiout = logfile;
	}
	if (message) {
		ssimsg = message;
	}

	bool result = true;
	
	//result = ssi::Factory::Register(ssi::SearchFeaturesGaze::GetCreateName(), ssi::SearchFeaturesGaze::Create) && result;
	result = ssi::Factory::Register(ssi::SearchFeaturesHead::GetCreateName(), ssi::SearchFeaturesHead::Create) && result;
	//result = ssi::Factory::Register(ssi::SearchFeaturesPupil::GetCreateName(), ssi::SearchFeaturesPupil::Create) && result;
	result = ssi::Factory::Register(ssi::StreamTransformer::GetCreateName(), ssi::StreamTransformer::Create) && result;
	
	return result;
}
