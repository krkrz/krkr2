//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Script Event Handling and Dispatching
//---------------------------------------------------------------------------
#include "tjsCommHead.h"

#include "EventImpl.h"
#include "MainFormUnit.h"



//---------------------------------------------------------------------------
// TVPInvokeEvents
//---------------------------------------------------------------------------
bool TVPEventInvoked = false;
void TVPInvokeEvents()
{
	if(TVPEventInvoked) return;
	TVPEventInvoked = true;
	if(TVPMainForm)
	{
		TVPMainForm->InvokeEvents();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// TVPEventReceived
//---------------------------------------------------------------------------
void TVPEventReceived()
{
	TVPEventInvoked = false;
	TVPMainForm->NotifyEventDelivered();
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// TVPCallDeliverAllEventsOnIdle
//---------------------------------------------------------------------------
void TVPCallDeliverAllEventsOnIdle()
{
	if(TVPMainForm)
	{
		TVPMainForm->CallDeliverAllEventsOnIdle();
	}
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// TVPBreathe
//---------------------------------------------------------------------------
static bool TVPBreathing = false;
void TVPBreathe()
{
	TVPEventDisabled = true; // not to call TVP events...
	TVPBreathing = true;
	try
	{
		Application->ProcessMessages(); // do Windows message pumping
	}
	catch(...)
	{
		TVPBreathing = false;
		TVPEventDisabled = false;
		throw;
	}

	TVPBreathing = false;
	TVPEventDisabled = false;
}
//---------------------------------------------------------------------------
bool TVPGetBreathing()
{
	// return whether now is in event breathing
	return TVPBreathing;
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// TVPSystemEventDisabledState
//---------------------------------------------------------------------------
void TVPSetSystemEventDisabledState(bool en)
{
	TVPMainForm->EventButton->Down = !en;
	if(!en) TVPDeliverAllEvents();
}
//---------------------------------------------------------------------------
bool TVPGetSystemEventDisabledState()
{
	return !TVPMainForm->EventButton->Down;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TVPBeginContinuousEvent()
{
	// this must begin calling TVPDeliverContinuousEvent
	if(TVPMainForm) TVPMainForm->BeginContinuousEvent();
}
//---------------------------------------------------------------------------
void TVPEndContinuousEvent()
{
	// this must stop calling TVPDeliverContinuousEvent
	if(TVPMainForm) TVPMainForm->EndContinuousEvent();
}
//---------------------------------------------------------------------------
