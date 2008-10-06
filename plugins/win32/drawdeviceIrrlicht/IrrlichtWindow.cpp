#include "IrrlichtWindow.h"

#define KRKRDISPWINDOWCLASS "TScrollBox"

extern void message_log(const char* format, ...);
extern void error_log(const char *format, ...);

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace io;
using namespace gui;

/**
 * ウインドウプロシージャ
 */
static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// 参考: irrlicht/source/Irrlicht/CIrrDeviceWin32.cpp
	switch (message) {
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		return 0;
	case WM_ERASEBKGND:
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void registerWindowClass()
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof wcex);
	wcex.cbSize		= sizeof(WNDCLASSEX);
	wcex.style		= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= GetModuleHandle(NULL);
	wcex.hIcon		    = NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= CLASSNAME;
	wcex.hIconSm		= 0;
	RegisterClassEx(&wcex);
}

void unregisterWindowClass()
{
	UnregisterClass(CLASSNAME, GetModuleHandle(NULL));
}



// イベント処理
bool __stdcall
IrrlichtWindow::messageHandler(void *userdata, tTVPWindowMessage *Message)
{
	IrrlichtWindow *self = (IrrlichtWindow*)userdata;
	switch (Message->Msg) {
	case TVP_WM_DETACH:
		self->destroyWindow();
		break;
	case TVP_WM_ATTACH:
		self->createWindow((HWND)Message->LParam);
		break;
	default:
		break;
	}
	return false;
}

// ユーザメッセージレシーバの登録/解除
void
IrrlichtWindow::setReceiver(tTVPWindowMessageReceiver receiver, bool enable)
{
	tTJSVariant mode     = enable ? (tTVInteger)(tjs_int)wrmRegister : (tTVInteger)(tjs_int)wrmUnregister;
	tTJSVariant proc     = (tTVInteger)(tjs_int)receiver;
	tTJSVariant userdata = (tTVInteger)(tjs_int)this;
	tTJSVariant *p[] = {&mode, &proc, &userdata};
	if (window->FuncCall(0, L"registerMessageReceiver", NULL, NULL, 4, p, window) != TJS_S_OK) {
		if (enable) {
			TVPThrowExceptionMessage(L"can't regist user message receiver");
		}
	}
}

/**
 * ウインドウを生成
 * @param parent 親ウインドウ
 */
void
IrrlichtWindow::createWindow(HWND krkr)
{
	if (krkr && (parent = FindWindowEx(krkr, NULL, KRKRDISPWINDOWCLASS, NULL))) {
		hwnd = CreateWindow(CLASSNAME, "",
							WS_CHILD|WS_CLIPCHILDREN,
							left, top, width, height,
							parent, NULL, GetModuleHandle(NULL), NULL);
		_setPos();
		UpdateWindow(hwnd);
		attach(hwnd);
	}
}

/**
 * ウインドウを破棄
 */
void
IrrlichtWindow::destroyWindow()
{
	if (hwnd) {
		detach();
		DestroyWindow(hwnd);
		hwnd = 0;
	}
	parent = 0;
}

/**
 * コンストラクタ
 */
IrrlichtWindow::IrrlichtWindow(int driverType, iTJSDispatch2 *win, int left, int top, int width, int height)
	: IrrlichtBase((E_DRIVER_TYPE)driverType), window(NULL), parent(0), hwnd(0), visible(false)
{
	if (win->IsInstanceOf(0, NULL, NULL, L"Window", win) != TJS_S_TRUE) {
		TVPThrowExceptionMessage(L"must set window object");
	}

	window = win;
	window->AddRef();
	
	tTJSVariant krkrHwnd; // 親のハンドル
	if (window->PropGet(0, TJS_W("HWND"), NULL, &krkrHwnd, window) == TJS_S_OK) {
		HWND parent = FindWindowEx((HWND)(tjs_int)krkrHwnd, NULL, KRKRDISPWINDOWCLASS, NULL);
		if (parent) {
			RECT rect;
			GetClientRect(parent, &rect);
			this->left   = 0;
			this->top    = 0;
			this->width  = rect.right - rect.left;
			this->height = rect.bottom - rect.top;
		}
	}
	this->left   = left;
	this->top    = top;
	if (width != 0) {
		this->width  = width;
	}
	if (height != 0) {
		this->height = height;
	}
	setReceiver(messageHandler, true);
	createWindow((HWND)(tjs_int)krkrHwnd);
}

/**
 * デストラクタ
 */
IrrlichtWindow::~IrrlichtWindow()
{
	destroyWindow();
	if (window) {
		setReceiver(messageHandler, false);
		window->Release();
		window = NULL;
	}
}

// -----------------------------------------------------------------------
// 表示指定
// -----------------------------------------------------------------------

void
IrrlichtWindow::_setPos()
{
	if (hwnd) {
		MoveWindow(hwnd, left, top, width, height, FALSE);
		ShowWindow(hwnd, visible);
	}
}
	
void
IrrlichtWindow::setVisible(bool v)
{
	visible = v;
	_setPos();
}

bool
IrrlichtWindow::getVisible()
{
	return visible;
}

void
IrrlichtWindow::setLeft(int l)
{
	left = l;
	_setPos();
}

int
IrrlichtWindow::getLeft()
{
	return left;
}

void
IrrlichtWindow::setTop(int t)
{
	top = t;
	_setPos();
}

int
IrrlichtWindow::getTop()
{
	return top;
}

void
IrrlichtWindow::setWidth(int w)
{
	width = w;
	_setPos();
}

int
IrrlichtWindow::getWidth()
{
	return width;
}

void
IrrlichtWindow::setHeight(int h)
{
	height = h;
	_setPos();
}

int
IrrlichtWindow::getHeight()
{
	return height;
}
	
/**
 * 窓場所指定
 */	
void
IrrlichtWindow::setPos(int l, int t)
{
	left = l;
	top  = t;
	_setPos();
}

/**
 * 窓サイズ指定
 */	
void
IrrlichtWindow::setSize(int w, int h)
{
	width = w;
	height = h;
	_setPos();
}
