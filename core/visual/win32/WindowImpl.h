//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// "Window" TJS Class implementation
//---------------------------------------------------------------------------


#ifndef WindowImplH
#define WindowImplH

#include "WindowIntf.h"


/*[*/
//---------------------------------------------------------------------------
// window message receivers
//---------------------------------------------------------------------------
enum tTVPWMRRegMode { wrmRegister=0, wrmUnregister=1 };
#pragma pack(push, 4)
struct tTVPWindowMessage
{
	unsigned int Msg; // window message
	int WParam;  // WPARAM
	int LParam;  // LPARAM
	int Result;  // result
};
#pragma pack(pop)
typedef bool (__stdcall * tTVPWindowMessageReceiver)
	(void *userdata, tTVPWindowMessage *Message);

#define TVP_WM_DETACH (WM_USER+106)  // before re-generating the window
#define TVP_WM_ATTACH (WM_USER+107)  // after re-generating the window


/*]*/
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// Mouse Cursor Management
//---------------------------------------------------------------------------
extern tjs_int TVPGetCursor(const ttstr & name);
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// Utility functions
//---------------------------------------------------------------------------
tjs_uint32 TVPGetCurrentShiftKeyState();
HWND TVPGetModalWindowOwnerHandle();
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// Color Format Detection
//---------------------------------------------------------------------------
extern tjs_int TVPDisplayColorFormat;
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// Screen Mode management
//---------------------------------------------------------------------------
class IDirectDraw2;
class IDirectDrawSurface;
class IDirectDrawClipper;
extern void TVPTestDisplayMode(tjs_int w, tjs_int h, tjs_int & bpp);
extern void TVPSwitchToFullScreen(HWND window, tjs_int w, tjs_int h);
extern void TVPRevertFromFullScreen(HWND window);
extern void TVPEnsureDirectDrawObject();
extern IDirectDraw2 * TVPGetDirectDrawObjectNoAddRef();
extern IDirectDrawSurface * TVPGetDDPrimarySurfaceNoAddRef();
extern void TVPSetDDPrimaryClipper(IDirectDrawClipper * clipper);
extern bool TVPUseChangeDisplaySettings;
extern void TVPMinimizeFullScreenWindowAtInactivation();
extern void TVPRestoreFullScreenWindowAtActivation();
//---------------------------------------------------------------------------










//---------------------------------------------------------------------------
// tTJSNI_Window : Window Native Instance
//---------------------------------------------------------------------------
class TTVPWindowForm;
class tTJSNI_Window : public tTJSNI_BaseWindow
{
	TTVPWindowForm *Form;

public:
	tTJSNI_Window();
	tjs_error TJS_INTF_METHOD Construct(tjs_int numparams, tTJSVariant **param,
		iTJSDispatch2 *tjs_obj);
	void TJS_INTF_METHOD Invalidate();
	bool CloseFlag;

public:
	bool CanDeliverEvents() const; // tTJSNI_BaseWindow::CanDeliverEvents override

public:
	TTVPWindowForm * GetForm() const { return Form; }
	void NotifyWindowClose();

	void SendCloseMessage();

	void TickBeat();

//-- event control
	virtual void PostInputEvent(const ttstr &name, iTJSDispatch2 * params);  // override

//-- interface to layer manager
	void NotifyLayerResize(); // is called from primary layer

	void SetDefaultMouseCursor(); // set window mouse cursor to default
	void SetMouseCursor(tjs_int cursor); // set window mouse cursor
	void GetCursorPos(tjs_int &x, tjs_int &y);
	void SetCursorPos(tjs_int x, tjs_int y);
	void SetHintText(const ttstr & text);
	void SetAttentionPoint(tTJSNI_BaseLayer *layer,
		tjs_int l, tjs_int t);
	void DisableAttentionPoint();
	void SetImeMode(tTVPImeMode mode);
	void SetDefaultImeMode(tTVPImeMode mode);
	tTVPImeMode GetDefaultImeMode() const;
	void ResetImeMode();


//-- methods from tTVPDrawable
	void DrawCompleted(const tTVPRect &destrect,
		tTVPBaseBitmap *bmp, const tTVPRect &cliprect,
		tTVPLayerType type, tjs_int opacity);

//-- update managment
	void BeginUpdate(const tTVPComplexRect &rects);
	void EndUpdate();

//-- interface to MenuItem object
	TMenuItem *  GetRootMenuItem();
	void SetMenuBarVisible(bool b);
	bool GetMenuBarVisible() const;

//-- interface to VideoOverlay object
public:
	HWND GetSurfaceWindowHandle();
	HWND GetWindowHandle(tjs_int &ofsx, tjs_int &ofsy);

	void ReadjustVideoRect();
	void WindowMoved();
	void DetachVideoOverlay();

//-- interface to plugin
	HWND GetWindowHandleForPlugin();
	void RegisterWindowMessageReceiver(tTVPWMRRegMode mode,
		void * proc, const void *userdata);

//-- methods
	void Close();
	void OnCloseQueryCalled(bool b);

	void BeginMove();
	void BringToFront();
	void Update(tTVPUpdateType);
	void ShowModal();

	void HideMouseCursor();

//-- properties
	bool GetVisible() const;
	void SetVisible(bool s);

	void GetCaption(ttstr & v) const;
	void SetCaption(const ttstr & v);

	void SetWidth(tjs_int w);
	tjs_int GetWidth() const;
	void SetHeight(tjs_int h);
	tjs_int GetHeight() const;
	void SetSize(tjs_int w, tjs_int h);

	void SetLeft(tjs_int l);
	tjs_int GetLeft() const;
	void SetTop(tjs_int t);
	tjs_int GetTop() const;
	void SetPosition(tjs_int l, tjs_int t);

	void SetLayerLeft(tjs_int l);
	tjs_int GetLayerLeft() const;
	void SetLayerTop(tjs_int t);
	tjs_int GetLayerTop() const;
	void SetLayerPosition(tjs_int l, tjs_int t);


	void SetInnerSunken(bool b);
	bool GetInnerSunken() const;

	void SetInnerWidth(tjs_int w);
	tjs_int GetInnerWidth() const;
	void SetInnerHeight(tjs_int h);
	tjs_int GetInnerHeight() const;

	void SetInnerSize(tjs_int w, tjs_int h);

	void SetBorderStyle(tTVPBorderStyle st);
	tTVPBorderStyle GetBorderStyle() const;

	void SetStayOnTop(bool b);
	bool GetStayOnTop() const;

	void SetShowScrollBars(bool b);
	bool GetShowScrollBars() const;

	void SetFullScreen(bool b);
	bool GetFullScreen() const;

	void SetUseMouseKey(bool b);
	bool GetUseMouseKey() const;

	void SetTrapKey(bool b);
	bool GetTrapKey() const;

	void SetMaskRegion(tjs_int threshold);
	void RemoveMaskRegion();

	void SetMouseCursorState(tTVPMouseCursorState mcs);
    tTVPMouseCursorState GetMouseCursorState() const;

	void SetFocusable(bool b);
	bool GetFocusable();

protected:
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#endif
