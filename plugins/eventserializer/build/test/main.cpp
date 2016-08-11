// TestMain.cpp (TEST)
// author: Florian Lingenfelser <florian.lingenfelser@informatik.uni-augsburg.de>
// created: 2011/10/20
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

#include "ssi.h"
#include "ssieventserializer.h"
#include "opensmile/include/ssiopensmile.h"
#include "audio/include/ssiaudio.h"
#include "signal/include/ssisignal.h"
#include "model/include/ssimodel.h"
using namespace ssi;

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

ssi_char_t string[SSI_MAX_CHAR];

void ex_eventseializer();

int main (int argc, char *argv[]) {

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("ssiframe.dll");
	Factory::RegisterDLL ("ssievent.dll");
	Factory::RegisterDLL ("ssisignal.dll");
	Factory::RegisterDLL ("ssimodel.dll");	
	Factory::RegisterDLL ("ssimouse.dll");
	Factory::RegisterDLL ("ssigraphic.dll");
	Factory::RegisterDLL ("ssieventserializer.dll");
	Factory::RegisterDLL ("ssiopensmile.dll");
	Factory::RegisterDLL ("ssiaudio.dll");

	ex_eventseializer();

	ssi_print ("\n\n\tpress a key to quit\n");
	getchar ();

	Factory::Clear ();

#ifdef USE_SSI_LEAK_DETECTOR
	}
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}

void ex_eventseializer(){

	ITheFramework *frame = Factory::GetFramework ("frame");

	Decorator *decorator = ssi_create (Decorator, 0, true);
	frame->AddDecorator(decorator);

	ITheEventBoard *board = Factory::GetEventBoard ("board");

	Mouse *mouse = ssi_create (Mouse, "mouse", true);
	mouse->getOptions()->mask = Mouse::LEFT;
	mouse->getOptions()->flip = true;
	mouse->getOptions()->scale = false;
	ITransformable *cursor_p = frame->AddProvider(mouse, SSI_MOUSE_CURSOR_PROVIDER_NAME);
	ITransformable *button_p = frame->AddProvider(mouse, SSI_MOUSE_BUTTON_PROVIDER_NAME);	
	frame->AddSensor(mouse);

	ZeroEventSender *ezero = ssi_create(ZeroEventSender, "ezero", true);
	ezero->getOptions()->mindur = 0;
	ezero->getOptions()->eager = true;
	frame->AddConsumer(button_p, ezero, "5");
	board->RegisterSender(*ezero);

	EventSerializer *evs = ssi_create(EventSerializer, 0, true);
	evs->getOptions()->dimension = 1;
	evs->getOptions()->sr = 5;
	evs->getOptions()->setEventAddress(ezero->getEventAddress());
	ITransformable* evs_p = frame->AddProvider(evs, SSI_EVENTSERIALIZER_EVENT_PROVIDER_NAME);
	frame->AddSensor(evs);

	EventMonitor *monitor = ssi_create_id (EventMonitor, "monitor", "monitor");	
	board->RegisterListener(*monitor, 0, 10000);

	SignalPainter* paint = 0;

	paint = ssi_create_id (SignalPainter, 0, "plot");
	paint->getOptions()->size = 10.0;
	frame->AddConsumer(button_p, paint, "1");

	paint = ssi_create_id (SignalPainter, 0, "plot");
	paint->getOptions()->size = 10;
	frame->AddConsumer(evs_p, paint, "1");

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

}
