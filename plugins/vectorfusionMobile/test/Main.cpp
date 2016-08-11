
#include "ssi.h"
#include "ssivectorfusion.h"
#include "signal/include/ssisignal.h"
using namespace ssi;
void ex_VectorFusionSimple(ssi_real_t FusionSpeed, ssi_real_t EventSpeed, bool Accel, bool DecayWeights){

    ITheFramework *frame = Factory::GetFramework();
    ITheEventBoard *board = Factory::GetEventBoard();

    //ssi_pcast(TheEventBoard, board)->getOptions()->update = 500;

	Mouse *mouse = ssi_factory_create (Mouse, "mouse", true);
	mouse->getOptions ()->mask = Mouse::LEFT;
	mouse->getOptions ()->flip = true;
	mouse->getOptions ()->scale = false;
	ITransformable *cursor_p = frame->AddProvider (mouse, SSI_MOUSE_CURSOR_PROVIDER_NAME);
	ITransformable *button_p = frame->AddProvider (mouse, SSI_MOUSE_BUTTON_PROVIDER_NAME);	
	frame->AddSensor (mouse);

	Normalize *norm = ssi_factory_create (Normalize, 0, true);
	norm->getOptions()->maxval = 1.0f;
	norm->getOptions()->minval = -1.0f;
	ITransformable *norm_t = frame->AddTransformer(cursor_p, norm, "0.1s");

	Selector *select_x = ssi_factory_create (Selector, 0, true);
	select_x->getOptions()->set(0);
	ITransformable *select_x_t = frame->AddTransformer(norm_t, select_x, "0.1s");

	FunctionalsEventSender *mean_x = ssi_factory_create(FunctionalsEventSender, 0, true);
	ssi_strcpy(mean_x->getOptions()->names, "mean");
	mean_x->getOptions()->setSenderName("MeanX");
	frame->AddConsumer(select_x_t, mean_x, "0.1s");
	board->RegisterSender(*mean_x);

	Selector *select_y = ssi_factory_create (Selector, 0, true);
	select_y->getOptions()->set(1);
	ITransformable *select_y_t = frame->AddTransformer(norm_t, select_y, "0.1s");

	FunctionalsEventSender *mean_y = ssi_factory_create(FunctionalsEventSender, 0, true);
	ssi_strcpy(mean_y->getOptions()->names, "mean");
	mean_y->getOptions()->setSenderName("MeanY");
	frame->AddConsumer(select_y_t, mean_y, "0.1s");
	board->RegisterSender(*mean_y);


	ssi_real_t p_size = 10.0f;
	//raw

	ZeroEventSender *ezero = ssi_factory_create(ZeroEventSender, "ezero", true);
	ezero->getOptions()->mindur = 0.2;
	frame->AddConsumer(button_p, ezero, "0.25s");
	board->RegisterSender(*ezero);

	TupleConverter *converter = ssi_create(TupleConverter, 0, true);
	converter->getOptions()->setSenderName("converter");
	converter->getOptions()->setEventName("tuple");
	board->RegisterListener(*converter, ezero->getEventAddress());
	board->RegisterSender(*converter);

	TupleMap *vec_send = ssi_pcast(TupleMap, Factory::Create(TupleMap::GetCreateName(), 0, true));
	vec_send->getOptions()->dimension = 2;
	vec_send->getOptions()->mapped = true;
	vec_send->getOptions()->setSenderName("Mouse");
	ssi_strcpy (vec_send->getOptions()->mapping, "1.0,1.0");
	board->RegisterListener(*vec_send, converter->getEventAddress());
	board->RegisterSender(*vec_send);

	EventAddress fusion_adress;
	fusion_adress.setAddress(vec_send->getEventAddress());
	VectorFusion *fusion = ssi_pcast(VectorFusion, Factory::Create(VectorFusion::GetCreateName()));
	fusion->getOptions()->dimension = 2;
    fusion->getOptions()->print = true;
	fusion->getOptions()->decay_type = EVector::DECAY_TYPE_LIN;
	fusion->getOptions()->gradient = .5f;
	fusion->getOptions()->fusionspeed = FusionSpeed;
	fusion->getOptions()->eventspeed = EventSpeed;
	fusion->getOptions()->threshold = .05f;
	fusion->getOptions()->accelerate = Accel;
	fusion->getOptions()->decay_weights = DecayWeights;
	fusion->getOptions()->update_ms = 100;
#ifndef HEADLESS
	fusion->getOptions()->setMove(400, 0, 400, 800);
#endif
	board->RegisterListener(*fusion, fusion_adress.getAddress());
	board->RegisterSender(*fusion);
		

	frame->Start ();
	board->Start ();

	frame->Wait ();
	board->Stop ();
	frame->Stop ();
	board->Clear ();
	frame->Clear ();	


}

int main (int argc, char *argv[]) {

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("ssiframe");
	Factory::RegisterDLL ("ssievent");
	Factory::RegisterDLL ("ssivectorfusion");
	Factory::RegisterDLL ("ssisignal");
	Factory::RegisterDLL ("ssimodel");	
	Factory::RegisterDLL ("ssimouse");
	Factory::RegisterDLL ("ssiioput");
	
	ex_VectorFusionSimple(0.1f, 0.2f, false, false);
	
		
	ssi_print ("\n\n\tpress a key to quit\n");
	getchar ();

	Factory::Clear ();

#ifdef USE_SSI_LEAK_DETECTOR
	}
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}
