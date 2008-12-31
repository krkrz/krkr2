//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Script Event Handling and Dispatching
//---------------------------------------------------------------------------
#include "tjsCommHead.h"

#include "EventImpl.h"
#include "MainFormUnit.h"
#include "ThreadIntf.h"
#include "TickCount.h"
#include "TimerIntf.h"
#include "SysInitIntf.h"
#include "DebugIntf.h"
#include "WindowImpl.h"
#include <mmsystem.h>
#include <ddraw.h>


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
// tTVPContinuousHandlerCallLimitThread
//---------------------------------------------------------------------------
class tTVPContinuousHandlerCallLimitThread : public tTVPThread
{
	tjs_uint64 NextEventTick;
	tjs_uint64 Interval;
	tTVPThreadEvent Event;
	tTJSCriticalSection CS;

	bool Enabled;
	HWND UtilWindow;

public:
	tTVPContinuousHandlerCallLimitThread();
	~tTVPContinuousHandlerCallLimitThread();

protected:
	void Execute();

	void __fastcall UtilWndProc(Messages::TMessage &Msg)
	{
		Msg.Result =  DefWindowProc(UtilWindow, Msg.Msg, Msg.WParam, Msg.LParam);
		return;
	}

public:
	void SetEnabled(bool enabled);

	void SetInterval(tjs_uint64 interval) { Interval = interval; }

} static * TVPContinuousHandlerCallLimitThread = NULL;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTVPContinuousHandlerCallLimitThread::tTVPContinuousHandlerCallLimitThread()
	 : tTVPThread(true)
{
	NextEventTick = 0;
	Interval = (1<<TVP_SUBMILLI_FRAC_BITS)*1000/60; // default 60Hz
	Enabled = false;
	UtilWindow = AllocateHWnd(UtilWndProc);
	Resume();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTVPContinuousHandlerCallLimitThread::~tTVPContinuousHandlerCallLimitThread()
{
	Terminate();
	Resume();
	Event.Set();
	WaitFor();
	DeallocateHWnd(UtilWindow);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPContinuousHandlerCallLimitThread::Execute()
{
	while(!GetTerminated())
	{
		tjs_uint64 curtick = TVPGetTickCount() << TVP_SUBMILLI_FRAC_BITS;
		DWORD sleeptime;

		{	// thread-protected
			tTJSCriticalSectionHolder holder(CS);

			if(Enabled)
			{
				if(NextEventTick <= curtick)
				{
					TVPProcessContinuousHandlerEventFlag = true; // set flag to process event on next idle
					::PostMessage(UtilWindow, WM_APP+2, 0, 0);
					while(NextEventTick <= curtick) NextEventTick += Interval;
				}
				tjs_uint64 sleeptime_64 = NextEventTick - curtick;
				sleeptime = (DWORD)(sleeptime_64 >> TVP_SUBMILLI_FRAC_BITS) +
						((sleeptime_64 & ((1<<TVP_SUBMILLI_FRAC_BITS)-1))?1:0);
							// add 1 if fraction exists
			}
			else
			{
				sleeptime = 10000; // how long to sleep when disabled does not matter
			}


		}	// end-of-thread-protected

		if(sleeptime == 0) sleeptime = 1; // 0 will let thread sleeping forever ...
		Event.WaitFor(sleeptime);
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPContinuousHandlerCallLimitThread::SetEnabled(bool enabled)
{
	tTJSCriticalSectionHolder holder(CS);

	Enabled = enabled;
	if(enabled)
	{
		tjs_uint64 curtick = TVPGetTickCount() << TVP_SUBMILLI_FRAC_BITS;
		NextEventTick = ((curtick + 1) / Interval) * Interval;
		Event.Set();
	}
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
static tjs_int TVPContinousHandlerLimitFrequency = 0;
//---------------------------------------------------------------------------
void TVPBeginContinuousEvent()
{
	// read commandline options
	static tjs_int ArgumentGeneration = 0;
	if(ArgumentGeneration != TVPGetCommandLineArgumentGeneration())
	{
		ArgumentGeneration = TVPGetCommandLineArgumentGeneration();

		tTJSVariant val;
		if( TVPGetCommandLine(TJS_W("-contfreq"), &val) )
		{
			TVPContinousHandlerLimitFrequency = (tjs_int)val;
		}
	}

	if(!TVPGetWaitVSync())
	{
		if(TVPContinousHandlerLimitFrequency == 0)
		{
			// no limit
			// this notifies continuous calling of TVPDeliverAllEvents.
			if(TVPMainForm) TVPMainForm->BeginContinuousEvent();
		}
		else
		{
			// has limit
			if(!TVPContinuousHandlerCallLimitThread)
				TVPContinuousHandlerCallLimitThread = new tTVPContinuousHandlerCallLimitThread();
			TVPContinuousHandlerCallLimitThread->SetInterval(
				 (1<<TVP_SUBMILLI_FRAC_BITS)*1000/TVPContinousHandlerLimitFrequency);
			TVPContinuousHandlerCallLimitThread->SetEnabled(true);
		}
	}

	// if we wait vsync, the continuous handler will be executed at the every timing of
	// vsync.
}
//---------------------------------------------------------------------------
void TVPEndContinuousEvent()
{
	// anyway
	if(TVPContinuousHandlerCallLimitThread)
		TVPContinuousHandlerCallLimitThread->SetEnabled(false);

	// anyway
	if(TVPMainForm) TVPMainForm->EndContinuousEvent();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
static void TVPReleaseContinuousHandlerCallLimitThread()
{
	if(TVPContinuousHandlerCallLimitThread)
		delete TVPContinuousHandlerCallLimitThread,
			TVPContinuousHandlerCallLimitThread = NULL;
}
// to release TVPContinuousHandlerCallLimitThread at exit
static tTVPAtExit TVPTimerThreadUninitAtExit(TVP_ATEXIT_PRI_SHUTDOWN,
	TVPReleaseContinuousHandlerCallLimitThread);
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
static bool TVPWaitVSync = 0;
//---------------------------------------------------------------------------
bool TVPGetWaitVSync()
{
	static tjs_int ArgumentGeneration = 0;
	if(ArgumentGeneration != TVPGetCommandLineArgumentGeneration())
	{
		ArgumentGeneration = TVPGetCommandLineArgumentGeneration();
		TVPWaitVSync = false;

		tTJSVariant val;
		if(TVPGetCommandLine(TJS_W("-waitvsync"), &val))
		{
			ttstr str(val);
			if(str == TJS_W("yes"))
			{
				TVPWaitVSync = true;
				TVPEnsureDirectDrawObject();
			}
		}
	}
	return TVPWaitVSync;
}
//---------------------------------------------------------------------------e











//---------------------------------------------------------------------------
// VSync�p�̃^�C�~���O�𔭐������邽�߂̃X���b�h
//---------------------------------------------------------------------------
class tTVPVSyncTimingThread : public tTVPThread
{
	DWORD SleepTime;
	tTVPThreadEvent Event;
	tTJSCriticalSection CS;
	DWORD VSyncInterval; //!< VSync �̊Ԋu(�Q�l�l)
	DWORD LastVBlankTick; //!< �Ō�� vblank �̎���

	bool Enabled;
	HWND UtilWindow;

public:
	tTVPVSyncTimingThread();
	~tTVPVSyncTimingThread();

protected:
	void Execute();

	void __fastcall UtilWndProc(Messages::TMessage &Msg);

public:
	void MeasureVSyncInterval(); // VSyncInterval ���v������
} static * TVPVSyncTimingThread = NULL;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTVPVSyncTimingThread::tTVPVSyncTimingThread()
	 : tTVPThread(true)
{
	SleepTime = 1;
	LastVBlankTick = 0;
	VSyncInterval = 16; // ����l�B
	Enabled = false;
	UtilWindow = AllocateHWnd(UtilWndProc);
	MeasureVSyncInterval();
	Resume();
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
tTVPVSyncTimingThread::~tTVPVSyncTimingThread()
{
	Terminate();
	Resume();
	Event.Set();
	WaitFor();
	DeallocateHWnd(UtilWindow);
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPVSyncTimingThread::Execute()
{
	while(!GetTerminated())
	{
		// SleepTime �� LastVBlankTick �𓾂�
		DWORD sleep_time, last_vblank_tick;
		{	// thread-protected
			tTJSCriticalSectionHolder holder(CS);
			sleep_time = SleepTime;
			last_vblank_tick = LastVBlankTick;
		}

		// SleepTime ������
		// LastVBlankTick ����N�Z���ASleepTime ������
		DWORD sleep_start_tick = timeGetTime();

		DWORD sleep_time_adj = sleep_start_tick - last_vblank_tick;

		if(sleep_time_adj < sleep_time)
		{
			Sleep(sleep_time - sleep_time_adj);
		}
		else
		{
			// ���ʁA���C���X���b�h���� Event.Set() �����Ȃ�΁A
			// �^�C���X���C�X(������10ms) ���I��鍠��
			// �����ɗ��Ă���͂��ł���B
			// sleep_time �͒ʏ� 10ms ��蒷���̂ŁA
			// �����ɗ�����Ăُ͈̂�B
			// ��قǃV�X�e�����d������ԂɂȂ��Ă�ƍl������B
			// �����ŗ��đ����� �C�x���g���|�X�g����킯�ɂ͂����Ȃ��̂�
			// �K���Ȏ���(�{���ɓK��) ����B
			Sleep(5);
		}

		// �C�x���g���|�X�g����
		::PostMessage(UtilWindow, WM_APP+2, 0, (LPARAM)sleep_start_tick);

		Event.WaitFor(0x7fffffff); // vsync �܂ő҂�
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void __fastcall tTVPVSyncTimingThread::UtilWndProc(Messages::TMessage &Msg)
{
	if(Msg.Msg != WM_APP+2)
	{
		Msg.Result =  DefWindowProc(UtilWindow, Msg.Msg, Msg.WParam, Msg.LParam);
		return;
	}

	// tTVPVSyncTimingThread ���瓊����ꂽ���b�Z�[�W

	// ���� vblank ���H
	IDirectDraw2 * DirectDraw2 = TVPGetDirectDrawObjectNoAddRef();
	BOOL in_vblank = false;
	if(DirectDraw2)
		DirectDraw2->GetVerticalBlankStatus(&in_vblank);

	// ���Ԃ��`�F�b�N
	bool drawn = false;
//	DWORD vblank_wait_start = timeGetTime();

	// VSync �҂����s��
	bool delayed = false;
	if(!drawn)
	{
		if(!in_vblank)
		{
			// vblank ���甲����܂ő҂�
			DWORD timeout_target_tick = timeGetTime() + 1;

			BOOL in_vblank = false;
			do
			{
				DirectDraw2->GetVerticalBlankStatus(&in_vblank);
			} while(in_vblank && (long)(timeGetTime() - timeout_target_tick) <= 0);

			// vblank �ɓ��܂ő҂�
			in_vblank = true;
			do
			{
				DirectDraw2->GetVerticalBlankStatus(&in_vblank);
			} while(!in_vblank && (long)(timeGetTime() - timeout_target_tick) <= 0);

			if((int)(timeGetTime() - timeout_target_tick) > 0)
			{
				// �t���[���X�L�b�v�����������ƍl���Ă悢
				delayed  =true;
			}
		}
	}

//	DWORD vblank_wait_end = timeGetTime();

	// �^�C�}�̎��Ԍ��_��ݒ肷��
	if(!delayed)
	{
		tTJSCriticalSectionHolder holder(CS);
		LastVBlankTick = timeGetTime(); // ���ꂪ���ɖ��鎞�Ԃ̋N�Z�_�ɂȂ�
	}
	else
	{
		tTJSCriticalSectionHolder holder(CS);
		LastVBlankTick += VSyncInterval; // ���ꂪ���ɖ��鎞�Ԃ̋N�Z�_�ɂȂ�(����������)
		if((long) (timeGetTime() - (LastVBlankTick + SleepTime)) <= 0)
		{
			// ��������A���ɋN���悤�Ƃ��鎞�Ԃ����łɉߋ��Ȃ̂Ŗ���܂���
			LastVBlankTick = timeGetTime(); // �����I�ɍ��̎����ɂ��܂�
		}
	}

	// ��ʂ̍X�V���s�� (DrawDevice��Show���\�b�h���Ă�)
	if(!drawn) TVPDeliverDrawDeviceShow();

	// ���� vsync �҂����s�����O�A���ł� vblank �ɓ���Ă����ꍇ�́A
	// �҂��Ԃ����������ƌ������Ƃł���
	if(in_vblank)
	{
		// ���̏ꍇ�� SleepTime �����炷
		tTJSCriticalSectionHolder holder(CS);
		if(SleepTime > 8) SleepTime --;
	}
	else
	{
		// vblank �Ŗ��������ꍇ�͓�̏ꍇ���l������
		// 1. vblank �O������
		// 2. vblank �ゾ����
		// �ǂ������͕�����Ȃ���
		// SleepTime �𑝂₷�B���������ꂪ VSyncInterval �𒴂���͂��͂Ȃ��B
		tTJSCriticalSectionHolder holder(CS);
		SleepTime ++;
		if(SleepTime > VSyncInterval) SleepTime = VSyncInterval;
	}

	// �^�C�}���N������
	Event.Set();

	// ContinuousHandler ���Ă�
	// ����͏\���Ȏ��Ԃ��Ƃ��悤�Avsync �҂��̒���ɌĂ΂��
	TVPProcessContinuousHandlerEventFlag = true; // set flag to invoke continuous handler on next idle

/*
static DWORD last_report_tick;

if(timeGetTime() > last_report_tick + 5000)
{
	last_report_tick = timeGetTime();
	TVPAddLog(TJS_W("SleepTime : ") + ttstr((int)SleepTime));
	TVPAddLog(TJS_W("VSyncInterval : ") + ttstr((int)VSyncInterval));
	TVPAddLog(TJS_W("VSync wait time : ") + ttstr((int)(vblank_wait_end - vblank_wait_start)));
}
*/
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void tTVPVSyncTimingThread::MeasureVSyncInterval()
{
	TVPEnsureDirectDrawObject();

	DWORD vsync_interval = 10000;

	// vsync ���� ms �œ���B
	// ms �P�ʂȂ̂ł��܂萳�m�Ȓl�͓����Ȃ����A�܂����ɖ��Ȃ����ƂƂ���B

	// �܂��ADirectDraw ���g�p�\�ȏꍇ�A WaitForVerticalBlank ���邢��
	// GetScanLine �� busy loop �ŊĎ����Ď��𓾂邱�Ƃ������B
	IDirectDraw2 * dd2 = TVPGetDirectDrawObjectNoAddRef();
	if(dd2)
	{
		// �Q�l: http://hpcgi1.nifty.com/MADIA/Vcbbs/wwwlng.cgi?print+200605/06050028.txt

		// ����ɂ��Ă� GetScanLine �͐M�p�Ȃ�Ȃ�
		// ����Ő���Ɏ��𓾂��Ȃ������������Ēf�O
		DWORD start_tick;
		DWORD timeout;

		DWORD last_sync_tick;
		int repeat_count;
/*
		// �܂��AGetScanLine �ɂ����̎擾�����݂�
		DWORD last_scanline = 65536;

		// ������ɖ߂�܂ŋ󃋁[�v
		// �������炪�{���̌v���B
		last_sync_tick = timeGetTime();
		timeout = 250;
		start_tick = timeGetTime();
		repeat_count = 0;
		while(timeGetTime() - start_tick < timeout)
		{
			DWORD scanline = 65536;
			if(FAILED(dd2->GetScanLine(&scanline))) scanline = 65536;
			if(scanline < last_scanline && last_scanline - scanline > 100)
			{
				// ������ɖ߂���
				// �O��`�F�b�N�����ʒu�����O�ɒl���߂����ꍇ��
				// ���A�����Ƃ݂Ȃ�
				// �O��Ɣ�ׂ�100���C���ȏ�߂��Ă邱�Ƃ��m�F����B
				// ����� W.Dee �̊� (GeForce 7600 GT) �ŁA�Ȃ���
				// �܂�ɃX�L�������C����1�����߂邱�Ƃ�����Ƃ������ۂ�
				// ���������߁B
				// ���������̑΍���Ƃ��Ă��܂Ƃ��Ɏ����擾�ł��Ȃ���������c�c
				DWORD tick = timeGetTime();
				if(repeat_count > 2)
				{
					// �ŏ��̐���̌��ʂ͎̂Ă�
					// �ŏ��̊Ԋu���L�^����
					if(tick - last_sync_tick < vsync_interval)
						vsync_interval = tick - last_sync_tick;
				}
				last_sync_tick = tick;
				repeat_count ++;
			}
			last_scanline = scanline;
		}

		TVPAddLog(TJS_W("Rough VSync interval measured by GetScanLine() : " + ttstr((int)vsync_interval)));

		// vsync ���͓K�؂��ۂ��H
		if(vsync_interval < 6 || vsync_interval > 66)
		{
			TVPAddLog(TJS_W("VSync interval by GetScanLine() seems to be strange, trying WaitForVerticalBlank() ..."));
*/
			// �ǂ�����
			vsync_interval = 10000;
			// WaitForVerticalBlank �ɂ�鑪������݂�
			// �ŏ���vblank��҂�
			dd2->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
			// ���񂩃��[�v���񂵂ĊԊu�𑪒肷��
			timeout = 250;
			last_sync_tick = timeGetTime();
			start_tick = timeGetTime();
			repeat_count = 0;
			while(timeGetTime() - start_tick < timeout)
			{
				// vblank ���甲����܂ő҂�
				BOOL in_vblank = false;
				do
				{
					dd2->GetVerticalBlankStatus(&in_vblank);
				} while(in_vblank && timeGetTime() - start_tick < timeout);

				DWORD aux_wait = timeGetTime();
				while(timeGetTime() - aux_wait < 2) ;
					// 1ms�`2ms�قǂ܂�
					// �ǂ����A�Z��Ԃ� vblank �ɓ������ vblank ���甲�����肷��悤��
					// ���ʂ������邱�Ƃ�����B
					// �ڂ�������͕�����Ȃ����A�����ɓK���ȃE�F�C�g����邱�Ƃ�
					// �Ȃ�Ƃ��Ώ������݂�B

				// vblank �ɓ��܂ő҂�
				in_vblank = true;
				do
				{
					dd2->GetVerticalBlankStatus(&in_vblank);
				} while(!in_vblank && timeGetTime() - start_tick < timeout);

				DWORD tick = timeGetTime();
				if(repeat_count > 2)
				{
					// �ŏ��̐���̌��ʂ͎̂Ă�
					// �ŏ��̊Ԋu���L�^����
					// �ŏ��̊Ԋu���L�^����
					if(tick - last_sync_tick < vsync_interval)
						vsync_interval = tick - last_sync_tick;
				}
				last_sync_tick = tick;
				repeat_count ++;
			}

			TVPAddLog(TJS_W("Rough VSync interval measured by GetVerticalBlankStatus() : " + ttstr((int)vsync_interval)));
/*
		}
*/
	}


	// vsync ���͓K�؂��ۂ��H
	if(!dd2 || vsync_interval < 6 || vsync_interval > 66)
	{
		// �ǂ�������ł� vsync �������܂��Ƃ�Ă��Ȃ����ۂ�
		// �����Ȃ�ƁA���� API �ɂ��擾�B
		// �Q�l: http://www.interq.or.jp/moonstone/person/del/zenact01.htm
		DWORD vsync_rate = 0;

		OSVERSIONINFO osvi;
		ZeroMemory(&osvi, sizeof(osvi));
		osvi.dwOSVersionInfoSize = sizeof(osvi);
		GetVersionEx(&osvi);

		if(osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			HDC dc = GetDC(0);
			vsync_rate = GetDeviceCaps(dc, VREFRESH);
			ReleaseDC(0, dc);
		}
		else if(osvi.dwMajorVersion == 4 && osvi.dwMinorVersion >= 10) // Windows 98 or lator
		{
			tTVP_devicemodeA dm;
			ZeroMemory(&dm, sizeof(tTVP_devicemodeA));
			dm.dmSize = sizeof(tTVP_devicemodeA);
			dm.dmDriverExtra = 0;
			EnumDisplaySettings(NULL, ENUM_REGISTRY_SETTINGS, reinterpret_cast<DEVMODE*>(&dm));
			vsync_rate = dm.dmDisplayFrequency;
		}

		if(vsync_rate != 0)
			vsync_interval = 1000 / vsync_rate;
		else
			vsync_interval = 0;

		TVPAddLog(TJS_W("Rough VSync interval read from API : " + ttstr((int)vsync_interval)));
	}

	// vsync ���͓K�؂��ۂ��H
	if(vsync_interval < 6 || vsync_interval > 66)
	{
		TVPAddLog(TJS_W("Rough VSync interval still seems wrong, assuming default value (16)"));
		vsync_interval = 16;
	}

	VSyncInterval = vsync_interval;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TVPEnsureVSyncTimingThread()
{
	// (�����K�v�Ȃ��) VSyncTimingThread ���쐬����
	if(TVPGetWaitVSync())
	{
		if(!TVPVSyncTimingThread)
			TVPVSyncTimingThread = new tTVPVSyncTimingThread();
	}
	else
	{
		TVPReleaseVSyncTimingThread();
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void TVPReleaseVSyncTimingThread()
{
	if(TVPVSyncTimingThread)
		delete TVPVSyncTimingThread,
			TVPVSyncTimingThread = NULL;
}
//---------------------------------------------------------------------------
// to release TVPContinuousHandlerCallLimitThread at exit
static tTVPAtExit TVPVSyncTimingThreadUninitAtExit(TVP_ATEXIT_PRI_SHUTDOWN,
	TVPReleaseVSyncTimingThread);
//---------------------------------------------------------------------------





