// Main.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2011/10/11
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

#include "ssi.h"
using namespace ssi;

bool ex_address (void *arg);
bool ex_fake (void *arg);
ssi_char_t string[SSI_MAX_CHAR];

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

    Factory::RegisterDLL ("ssiframe");
    Factory::RegisterDLL ("ssievent");
    Factory::RegisterDLL ("ssiioput");

    ssi_random_seed ();

	Exsemble ex;
	ex.add(&ex_address, 0, "ADDRESS", "How to use 'EventAddress' to compose an event address.");
	ex.add(&ex_fake, 0, "FAKE", "How to use 'FakeSignal' for simple pipeline testing.");
	ex.show();

    Factory::Clear ();

#ifdef USE_SSI_LEAK_DETECTOR
    }
    _CrtDumpMemoryLeaks();
#endif

    return 0;
}

bool ex_address (void *arg) {

    EventAddress ea;

    ea.print ();

    ea.setAddress ("e1@s1,s2");
    ea.print ();

    ea.setEvents ("e2,e3");
    ea.print ();

    ea.setSender ("s3");
    ea.print ();

    ea.setAddress ("e4,e5@s4");
    ea.print ();

    ea.clear ();
    ea.setAddress ("e1@s1,s2");
    ea.print ();

    return true;
}
bool ex_fake (void *arg) {

	ITheFramework *frame = Factory::GetFramework ();

	Decorator *decorator = ssi_create (Decorator, 0, true);
	frame->AddDecorator(decorator);

	ITheEventBoard *board = Factory::GetEventBoard ();

	ssi_event_t e_class;
	ssi_event_init(e_class, SSI_ETYPE_STRING, Factory::AddString("sender"), Factory::AddString("class"), 0, 0, SSI_MAX_CHAR);

	FakeSignal *random = ssi_create(FakeSignal, 0, true);
	random->getOptions()->type = FakeSignal::SIGNAL::RANDOM;
	ITransformable *random_p = frame->AddProvider(random, "random");
	frame->AddSensor(random);

	FileSampleWriter *writer = ssi_create(FileSampleWriter, 0, true);
	writer->getOptions()->type = File::ASCII;
	writer->getOptions()->setClasses("A;B");
	writer->getOptions()->setUser("user");
	frame->AddConsumer(random_p, writer, "1.0s");
	board->RegisterListener(*writer, "class@");

	ssi_event_init(e_class, SSI_ETYPE_STRING, Factory::AddString("sender"), Factory::AddString("class"), 0, 0, SSI_MAX_CHAR);

    random = ssi_create(FakeSignal, 0, true);
    random->getOptions()->type = FakeSignal::SIGNAL::RANDOM;
    random_p = frame->AddProvider(random, "random");
    frame->AddSensor(random);

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
	
	return true;
}
