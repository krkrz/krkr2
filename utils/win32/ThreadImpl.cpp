//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Thread base class
//---------------------------------------------------------------------------
#include "tjsCommHead.h"

#include <process.h>

#include "ThreadIntf.h"
#include "ThreadImpl.h"
#include "MsgIntf.h"



//---------------------------------------------------------------------------
// tTVPThread : a wrapper class for thread
//---------------------------------------------------------------------------
tTVPThread::tTVPThread(bool suspended)
{
	Terminated = false;
	Handle = NULL;
	ThreadId = 0;
	Suspended = suspended;

	Handle = (HANDLE) _beginthreadex(
		NULL, 0, StartProc, this, suspended ? CREATE_SUSPENDED : 0,
			(unsigned *)&ThreadId);

	if(Handle == INVALID_HANDLE_VALUE) TVPThrowInternalError;
}
//---------------------------------------------------------------------------
tTVPThread::~tTVPThread()
{
	CloseHandle(Handle);
}
//---------------------------------------------------------------------------
unsigned __stdcall tTVPThread::StartProc(void * arg)
{
	((tTVPThread*)arg)->Execute();
	return 0;
}
//---------------------------------------------------------------------------
void tTVPThread::WaitFor()
{
	WaitForSingleObject(Handle, INFINITE);
}
//---------------------------------------------------------------------------
tTVPThreadPriority tTVPThread::GetPriority()
{
	int n = GetThreadPriority(Handle);
	switch(n)
	{
	case THREAD_PRIORITY_IDLE:			return ttpIdle;
	case THREAD_PRIORITY_LOWEST:		return ttpLowest;
	case THREAD_PRIORITY_BELOW_NORMAL:	return ttpLower;
	case THREAD_PRIORITY_NORMAL:		return ttpNormal;
	case THREAD_PRIORITY_ABOVE_NORMAL:	return ttpHigher;
	case THREAD_PRIORITY_HIGHEST:		return ttpHighest;
	case THREAD_PRIORITY_TIME_CRITICAL:	return ttpTimeCritical;
	}

	return ttpNormal;
}
//---------------------------------------------------------------------------
void tTVPThread::SetPriority(tTVPThreadPriority pri)
{
	int npri = THREAD_PRIORITY_NORMAL;
	switch(pri)
	{
	case ttpIdle:			npri = THREAD_PRIORITY_IDLE;			break;
	case ttpLowest:			npri = THREAD_PRIORITY_LOWEST;			break;
	case ttpLower:			npri = THREAD_PRIORITY_BELOW_NORMAL;	break;
	case ttpNormal:			npri = THREAD_PRIORITY_NORMAL;			break;
	case ttpHigher:			npri = THREAD_PRIORITY_ABOVE_NORMAL;	break;
	case ttpHighest:		npri = THREAD_PRIORITY_HIGHEST;			break;
	case ttpTimeCritical:	npri = THREAD_PRIORITY_TIME_CRITICAL;	break;
	}

	SetThreadPriority(Handle, npri);
}
//---------------------------------------------------------------------------
void tTVPThread::Suspend()
{
	SuspendThread(Handle);
}
//---------------------------------------------------------------------------
void tTVPThread::Resume()
{
	while((tjs_int32)ResumeThread(Handle) > 1) ;
}
//---------------------------------------------------------------------------







//---------------------------------------------------------------------------
// tTVPThreadEvent
//---------------------------------------------------------------------------
tTVPThreadEvent::tTVPThreadEvent(bool manualreset)
{
	Handle = CreateEvent(NULL, manualreset?TRUE:FALSE, FALSE, NULL);
	if(!Handle) TVPThrowInternalError;
}
//---------------------------------------------------------------------------
tTVPThreadEvent::~tTVPThreadEvent()
{
	CloseHandle(Handle);
}
//---------------------------------------------------------------------------
void tTVPThreadEvent::Set()
{
	SetEvent(Handle);
}
//---------------------------------------------------------------------------
void tTVPThreadEvent::Reset()
{
	ResetEvent(Handle);
}
//---------------------------------------------------------------------------
bool tTVPThreadEvent::WaitFor(tjs_uint timeout)
{
	// wait for event;
	// returns true if the event is set, otherwise (when timed out) returns false.

	DWORD state = WaitForSingleObject(Handle, timeout == 0 ? INFINITE : timeout);

	if(state == WAIT_OBJECT_0) return true;
	return false;
}
//---------------------------------------------------------------------------


