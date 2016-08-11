// Main.cpp
// author: Simon Flutura <simon.flutura@informatik.uni-augsburg.de>
// created: 12/5/2015
// Copyright (C) University of Augsburg, Lab for Human Centered Multimedia
//
// *************************************************************************************************
//
// This file is part of Smart Sensor Integration (SSI) developed at the 
// Lab for Multimedia Concepts and Applications of the University of Augsburg
//
// This library is free software; you can redistribute itand/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or any laterversion.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along withthis library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
//*************************************************************************************************

#include "ssi.h"
#include "ssibeaconScanner.h"
#include "MyEventListener.h"
using namespace ssi;

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

int main () {

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

    Factory::RegisterDLL ("ssibeaconscanner");
	Factory::RegisterDLL ("ssiframe");
	Factory::RegisterDLL ("ssievent");

	Factory::Register (MyEventListener::GetCreateName (), MyEventListener::Create);

	MyBeaconScanner *sender = ssi_create (MyBeaconScanner, "beaconScanner", true);
	sender->getOptions()->port = 17;
	sender->getOptions()->blePackets = true;
	//sender->getOptions()->crcCheck = true;

	//object->start();

	ITheFramework *frame = Factory::GetFramework ();

	Decorator *decorator = ssi_create (Decorator, 0, true);
	frame->AddDecorator(decorator);

	ITheEventBoard *board = Factory::GetEventBoard ();

	frame->AddRunnable(sender);
	board->RegisterSender(*sender);

	EventMonitor* monitor = ssi_create_id (EventMonitor, 0, "monitor");	
	board->RegisterListener(*monitor);

	//MyEventListener *listener = ssi_create (MyEventListener, 0, true);
	//board->RegisterListener(*listener, sender->getEventAddress());	

	decorator->add("console", 0, 0, 650, 800);
	decorator->add("plot*", 650, 0, 400, 400);
	decorator->add("monitor*", 650, 400, 400, 400);

	board->Start();
	frame->Start();
	frame->Wait();
	frame->Stop();
	board->Stop();
	frame->Clear();
	board->Clear();

	ssi_print ("\n\n\tpress a key to quit\n");
	getchar ();

	Factory::Clear ();

#ifdef USE_SSI_LEAK_DETECTOR
	}
	_CrtDumpMemoryLeaks();
#endif
	
	return 0;
}

