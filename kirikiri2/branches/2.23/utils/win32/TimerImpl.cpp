//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Timer Object Implementation
//---------------------------------------------------------------------------
#include "tjsCommHead.h"
#pragma hdrstop

#include <syncobjs.hpp>
#include <mmsystem.h>
#include <EventIntf.h>
#include "TickCount.h"
#include "TimerImpl.h"
#include "SysInitIntf.h"
#include "ThreadIntf.h"
#include "MsgIntf.h"


//---------------------------------------------------------------------------

// TVP Timer class gives ability of triggering event on punctual interval.
// a large quantity of event at once may easily cause freeze to system,
// so we must trigger only porocess-able quantity of the event.


#define TVP_LEAST_TIMER_INTERVAL 3

//---------------------------------------------------------------------------
// tTVPTimerThread
//---------------------------------------------------------------------------
class tTVPTimerThread : public tTVPThread
{
	// thread for triggering punctual event.
	// normal Windows timer cannot call the timer callback routine at
	// too short interval ( roughly less than 50ms ).

	std::vector<tTJSNI_Timer *> List;
	tTVPThreadEvent Event;

public:

	tTJSCriticalSection TVPTimerCS;

	tTVPTimerThread();
	~tTVPTimerThread();

protected:
	void Execute();

private:
	void AddItem(tTJSNI_Timer * item);
	bool RemoveItem(tTJSNI_Timer *item);

public:
	void SetEnabled(tTJSNI_Timer *item, bool enabled); // managed by this class
	void SetInterval(tTJSNI_Timer *item, tjs_uint64 interval); // managed by this class

public:
	static void Init();
	static void Uninit();

	static void Add(tTJSNI_Timer * item);
	static void Remove(tTJSNI_Timer *item);

} static * TVPTimerThread = NULL;
//---------------------------------------------------------------------------
tTVPTimerThread::tTVPTimerThread() : tTVPThread(true)
{
	SetPriority(TVPLimitTimerCapacity ? ttpNormal : ttpHighest);
	Resume();
}
//---------------------------------------------------------------------------
__fastcall tTVPTimerThread::~tTVPTimerThread()
{
	Terminate();
	Resume();
	Event.Set();
	WaitFor();
}
//---------------------------------------------------------------------------
void __fastcall tTVPTimerThread::Execute()
{
	while(!GetTerminated())
	{
		tjs_uint64 step_next = (tjs_uint64)(tjs_int64)-1L; // invalid value
		tjs_uint64 curtick = TVPGetTickCount();
		DWORD sleeptime;

		{	// thread-protected
			tTJSCriticalSectionHolder holder(TVPTimerCS);

			std::vector<tTJSNI_Timer*>::iterator i;
			for(i = List.begin(); i!=List.end(); i ++)
			{
				tTJSNI_Timer * item = *i;

				if(!item->GetEnabled() || item->GetInterval() == 0) continue;

				if(item->GetNextTick() < curtick)
				{
					tjs_uint n = (curtick - item->GetNextTick()) /
						item->GetInterval();
					n++;
					if(n > 40)
					{
						// too large amount of event at once; discard rest
						item->Trigger(1);
						item->SetNextTick(curtick + item->GetInterval());
					}
					else
					{
						item->Trigger(n);
						item->SetNextTick(item->GetNextTick() +
							n * item->GetInterval());
					}
				}


				tjs_uint64 to_next = item->GetNextTick() - curtick;

				if(step_next == (tjs_uint64)(tjs_int64)-1L)
				{
					step_next = to_next;
				}
				else
				{
					if(step_next > to_next) step_next = to_next;
				}
			}


			if(step_next != (tjs_uint64)(tjs_int64)-1L)
			{
				// too large step_next must be diminished to size of DWORD.
				if(step_next >= 0x80000000)
					sleeptime = 0x7fffffff; // smaller value than step_next is OK
				else
					sleeptime = step_next;
			}
			else
			{
				sleeptime = INFINITE;
			}

			if(List.size() == 0) sleeptime = INFINITE;
		}	// end-of-thread-protected

		if(sleeptime != INFINITE && sleeptime < TVP_LEAST_TIMER_INTERVAL)
			sleeptime = TVP_LEAST_TIMER_INTERVAL;

		Event.WaitFor(sleeptime); // wait until sleeptime is elapsed or
									// Event->SetEvent() is executed.
	}
}
//---------------------------------------------------------------------------
void tTVPTimerThread::AddItem(tTJSNI_Timer * item)
{
	tTJSCriticalSectionHolder holder(TVPTimerCS);

	List.push_back(item);
}
//---------------------------------------------------------------------------
bool tTVPTimerThread::RemoveItem(tTJSNI_Timer *item)
{
	tTJSCriticalSectionHolder holder(TVPTimerCS);

	std::vector<tTJSNI_Timer *>::iterator i;
	i = std::find(List.begin(), List.end(), item);
	if(i != List.end()) List.erase(i);

	return List.size() != 0;
}
//---------------------------------------------------------------------------
void tTVPTimerThread::SetEnabled(tTJSNI_Timer *item, bool enabled)
{
	{ // thread-protected
		tTJSCriticalSectionHolder holder(TVPTimerCS);

		item->InternalSetEnabled(enabled);
		if(enabled)
		{
			item->SetNextTick(TVPGetTickCount() + item->GetInterval());
		}
		else
		{
			item->CancelEvents();
			item->CancelTrigger();
		}
	} // end-of-thread-protected

	if(enabled) Event.Set();
}
//---------------------------------------------------------------------------
void tTVPTimerThread::SetInterval(tTJSNI_Timer *item, tjs_uint64 interval)
{
	{ // thread-protected
		tTJSCriticalSectionHolder holder(TVPTimerCS);

		item->InternalSetInterval(interval);
		if(item->GetEnabled())
		{
			item->CancelEvents();
			item->CancelTrigger();
			item->SetNextTick(TVPGetTickCount() + item->GetInterval());
		}
	} // end-of-thread-protected

	if(item->GetEnabled()) Event.Set();

}
//---------------------------------------------------------------------------
void tTVPTimerThread::Init()
{
	if(!TVPTimerThread)
	{
		TVPStartTickCount(); // in TickCount.cpp
		TVPTimerThread = new tTVPTimerThread();
	}
}
//---------------------------------------------------------------------------
void tTVPTimerThread::Uninit()
{
	if(TVPTimerThread)
	{
		delete TVPTimerThread;
		TVPTimerThread = NULL;
	}
}
//---------------------------------------------------------------------------
static tTVPAtExit TVPTimerThreadUninitAtExit(TVP_ATEXIT_PRI_SHUTDOWN,
	tTVPTimerThread::Uninit);
//---------------------------------------------------------------------------
void tTVPTimerThread::Add(tTJSNI_Timer * item)
{
	// at this point, item->GetEnebled() must be false.

	Init();

	TVPTimerThread->AddItem(item);
}
//---------------------------------------------------------------------------
void tTVPTimerThread::Remove(tTJSNI_Timer *item)
{
	if(TVPTimerThread)
	{
		if(!TVPTimerThread->RemoveItem(item)) Uninit();
	}
}
//---------------------------------------------------------------------------






//---------------------------------------------------------------------------
// tTJSNI_Timer
//---------------------------------------------------------------------------
tTJSNI_Timer::tTJSNI_Timer()
{
	NextTick = 0;
	Interval = 1000;
	Enabled = false;

	UtilWindow = AllocateHWnd(WndProc);
	if(!UtilWindow) TVPThrowInternalError;
}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD tTJSNI_Timer::Construct(tjs_int numparams,
	tTJSVariant **param, iTJSDispatch2 *tjs_obj)
{
	inherited::Construct(numparams, param, tjs_obj);

	tTVPTimerThread::Add(this);
	return S_OK;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTJSNI_Timer::Invalidate()
{
	if(UtilWindow) DeallocateHWnd(UtilWindow);
	tTVPTimerThread::Remove(this);
	CancelTrigger();
	CancelEvents();
	inherited::Invalidate();  // this sets Owner = NULL
}
//---------------------------------------------------------------------------
void tTJSNI_Timer::SetInterval(tjs_uint64 n)
{
	TVPTimerThread->SetInterval(this, n);
}
//---------------------------------------------------------------------------
tjs_uint64 tTJSNI_Timer::GetInterval() const
{
	return Interval;
}
//---------------------------------------------------------------------------
tjs_uint64 tTJSNI_Timer::GetNextTick() const
{
	return NextTick;
}
//---------------------------------------------------------------------------
void tTJSNI_Timer::SetEnabled(bool b)
{
	TVPTimerThread->SetEnabled(this, b);
}
//---------------------------------------------------------------------------
void tTJSNI_Timer::Trigger(tjs_uint n)
{
	// this function is called by sub-thread.
	CancelTrigger();
	PostMessage(UtilWindow, WM_USER+1, 0, (long)n);
}
//---------------------------------------------------------------------------
void tTJSNI_Timer::CancelTrigger()
{
	// remove all trigger messages from message queue
	MSG msg;
	while(PeekMessage(&msg, UtilWindow, WM_USER, WM_USER+2, PM_REMOVE)) ;
}
//---------------------------------------------------------------------------
void __fastcall tTJSNI_Timer::WndProc(Messages::TMessage &Msg)
{
	// called indirectly from Trigger
	if(Msg.Msg == WM_USER + 1)
	{
//		if(!GetInputState() && !TVPGetInputEventCount())
//		{
			// we must give priority to input events
//			if(AreEventsInQueue())
//				Fire(1);
//			else
				Fire((tjs_uint)Msg.LParam);
			// here does not fire events more than once when the event is already
			// in queue to avoid hung-up.
//  		}
//		else
//		{
//			Fire(1);
//		}
	}
	else
	{
		Msg.Result =  DefWindowProc(UtilWindow, Msg.Msg, Msg.WParam, Msg.LParam);
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// tTJSNC_Timer
//---------------------------------------------------------------------------
tTJSNativeInstance *tTJSNC_Timer::CreateNativeInstance()
{
	return new tTJSNI_Timer();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// TVPCreateNativeClass_Timer
//---------------------------------------------------------------------------
tTJSNativeClass * TVPCreateNativeClass_Timer()
{
	return new tTJSNC_Timer();
}
//---------------------------------------------------------------------------

