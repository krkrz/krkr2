//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Timer Object Implementation
//---------------------------------------------------------------------------
#ifndef TimerImplH
#define TimerImplH

#include "TimerIntf.h"

//---------------------------------------------------------------------------
// tTJSNI_Timer : Timer Native Instance
//---------------------------------------------------------------------------
class TTVPWindowForm;
class tTVPTimerThread;
class tTJSNI_Timer : public tTJSNI_BaseTimer
{
	typedef tTJSNI_BaseTimer inherited;
	friend class tTVPTimerThread;

	tjs_uint64 Interval;
	tjs_uint64 NextTick;
	bool Enabled;

	HWND UtilWindow; // a dummy window for receiving trigger from playing thread

public:
	tTJSNI_Timer();
	tjs_error TJS_INTF_METHOD Construct(tjs_int numparams, tTJSVariant **param,
		iTJSDispatch2 *tjs_obj);
	void TJS_INTF_METHOD Invalidate();

	void InternalSetInterval(tjs_uint64 n) { Interval = n; }
	void SetInterval(tjs_uint64 n);
	tjs_uint64 GetInterval() const;
		// { return Interval; }

	void SetNextTick(tjs_uint64 n) { NextTick = n; }
	tjs_uint64 GetNextTick() const;
		// { return NextTick; }
		// bcc 5.5.1 has an inliner bug of bad returning of int64...

	void InternalSetEnabled(bool b) { Enabled = b; }
	void SetEnabled(bool b);
	bool GetEnabled() const { return Enabled; }

	void Trigger(tjs_uint n);

private:
	void CancelTrigger();
	void __fastcall WndProc(Messages::TMessage &Msg);
		// UtilWindow's window procedure
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#endif
