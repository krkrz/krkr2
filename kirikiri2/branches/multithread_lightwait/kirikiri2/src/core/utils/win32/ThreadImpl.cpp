//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

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


//---------------------------------------------------------------------------
tjs_int TVPDrawThreadNum = 1;
struct ThreadInfo {
  bool readyToExit;
  HANDLE thread;
  HANDLE pingEvent;
  HANDLE pongEvent;
  TVP_THREAD_TASK_FUNC  lpStartAddress;
  TVP_THREAD_PARAM lpParameter;
};
static std::vector<ThreadInfo*> TVPThreadList;
static std::vector<HANDLE> TVPPongEventList;
static std::vector<tjs_int> TVPProcesserIdList;
static tjs_int TVPRunningThreadCount;
static tjs_int TVPThreadTaskNum, TVPThreadTaskCount;

//---------------------------------------------------------------------------
static tjs_int GetProcesserNum(void)
{
  static tjs_int processor_num = 0;
  if (! processor_num) {
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    processor_num = info.dwNumberOfProcessors;
  }
  return processor_num;
}

tjs_int TVPGetProcessorNum(void)
{
  return GetProcesserNum();
}

//---------------------------------------------------------------------------
tjs_int TVPGetThreadNum(void)
{
  tjs_int threadNum = TVPDrawThreadNum ? TVPDrawThreadNum : GetProcesserNum();
  threadNum = std::min(threadNum, TVPMaxThreadNum);
  return threadNum;
}

//---------------------------------------------------------------------------
static DWORD WINAPI ThreadLoop(LPVOID p)
{
  ThreadInfo *threadInfo = (ThreadInfo*)p;
  for(;;) {
    SignalObjectAndWait(threadInfo->pongEvent, threadInfo->pingEvent, INFINITE, FALSE);
    if (threadInfo->readyToExit)
      break;
    (threadInfo->lpStartAddress)(threadInfo->lpParameter);
  }

  DeleteObject(threadInfo->pongEvent);
  DeleteObject(threadInfo->pingEvent);
  delete threadInfo;
  ExitThread(0);

  return TRUE;
}
//---------------------------------------------------------------------------
void TVPBeginThreadTask(tjs_int taskNum)
{
  TVPThreadTaskNum = taskNum;
  TVPThreadTaskCount = 0;
  TVPRunningThreadCount = 0;
  tjs_int extraThreadNum = TVPGetThreadNum() - 1;
  if (TVPProcesserIdList.empty()) {
    DWORD processAffinityMask, systemAffinityMask;
    GetProcessAffinityMask(GetCurrentProcess(),
                           &processAffinityMask,
                           &systemAffinityMask);
    for (tjs_int i = 0; i < MAXIMUM_PROCESSORS; i++) {
      if (processAffinityMask & (1 << i))
        TVPProcesserIdList.push_back(i);
    }
    if (TVPProcesserIdList.empty())
      TVPProcesserIdList.push_back(MAXIMUM_PROCESSORS);
  }
  while (TVPThreadList.size() < extraThreadNum) {
    ThreadInfo *threadInfo = new ThreadInfo();
    threadInfo->readyToExit = false;
    threadInfo->pingEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    threadInfo->pongEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    threadInfo->thread = CreateThread(NULL, 0, ThreadLoop, threadInfo, CREATE_SUSPENDED, NULL);
    SetThreadIdealProcessor(threadInfo->thread, TVPProcesserIdList[TVPThreadList.size() % TVPProcesserIdList.size()]);
    ResumeThread(threadInfo->thread);
    WaitForSingleObject(threadInfo->pongEvent, INFINITE);
    TVPThreadList.push_back(threadInfo);
    TVPPongEventList.push_back(threadInfo->pongEvent);
  }
  while (TVPThreadList.size() > extraThreadNum) {
    ThreadInfo *threadInfo = TVPThreadList.back();
    threadInfo->readyToExit = true;
    SetEvent(threadInfo->pingEvent);
    TVPThreadList.pop_back();
    TVPPongEventList.pop_back();
  }
}

//---------------------------------------------------------------------------
void TVPExecThreadTask(TVP_THREAD_TASK_FUNC func, TVP_THREAD_PARAM param)
{
  if (++TVPThreadTaskCount == TVPThreadTaskNum) {
    func(param);
    return;
  }
  ThreadInfo *threadInfo;

  if (TVPRunningThreadCount < TVPThreadList.size()) {
    threadInfo = TVPThreadList[TVPRunningThreadCount++];
  } else {
    DWORD retval = WaitForMultipleObjects(TVPPongEventList.size(),
                                          &(TVPPongEventList[0]),
                                          FALSE,
                                          INFINITE);
    threadInfo = TVPThreadList[ retval - WAIT_OBJECT_0 ];
  }
  threadInfo->lpStartAddress = func;
  threadInfo->lpParameter = param;
  SetEvent(threadInfo->pingEvent);
}
//---------------------------------------------------------------------------
void TVPEndThreadTask(void) 
{
  if (TVPRunningThreadCount) {
    WaitForMultipleObjects(TVPRunningThreadCount,
                           &(TVPPongEventList[0]),
                           TRUE,
                           INFINITE);
  }
}
//---------------------------------------------------------------------------
