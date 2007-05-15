#pragma once

//----------------------------------------------------------------------------
namespace Messages {
	struct TMessage
	{
		int Msg;
		WPARAM WParam;
		LPARAM LParam;
		HRESULT Result;
	};
	struct TWMKey           { int Msg; HRESULT Result; };
	struct TWMMouseActivate
	{
		int Msg;
		int HitTestCode;
		HRESULT Result;
	};
	struct TWMMove          { int Msg; HRESULT Result; };
	struct TWMSetFocus
	{
		int Msg;
		HWND FocusedWnd;
		HRESULT Result;
	};
	struct TWMKillFocus     { int Msg; HRESULT Result; };
	struct TWMEnterMenuLoop { int Msg; HRESULT Result; };
	struct TWMExitMenuLoop  { int Msg; HRESULT Result; };
	struct TWMKeyDown
	{
		int Msg;
		unsigned int KeyData;
		HRESULT Result;
	};
	struct TWMNCLButtonDown { int Msg; HRESULT Result; };
	struct TWMNCRButtonDown { int Msg; HRESULT Result; };
	struct TWMMouse
	{
	public:
		int Msg;
		int XPos;
		int YPos;
		HRESULT Result;
	};

	struct TWMScroll
	{
	public:
		int Msg;
		short ScrollCode;
		short Pos;
		HWND ScrollBar;
		HRESULT Result;
	};
	typedef TWMScroll TWMHScroll;
	typedef TWMScroll TWMVScroll;
};
using namespace Messages;
