#pragma once

#include "vcl_base.h"
#include "Controls.hpp"
#include "Menus.hpp"
#include "Application.h"
#include "Screen.h"
#include "Messages.hpp"

#include <memory>

//----------------------------------------------------------------------------
class TScrollingWinControl : public TWinControl
{
public:
	TScrollingWinControl(TComponent* owner);
};

//----------------------------------------------------------------------------
typedef unsigned char Byte;
class TCustomForm : public TScrollingWinControl
{
protected:
	wxFrame* m_wxFrame;
public:
	TCustomForm(TComponent* owner);
	virtual ~TCustomForm();
	virtual void Dispatch(void*);

	int ModalResult;

	PROPERTY_VAR0(bool, AlphaBlend);
	PROPERTY_VAR0(Byte, AlphaBlendValue);
//	PROPERTY_VAR1(bool, FreeOnTerminate);
	PROPERTY(TWindowState, WindowState);
	PROPERTY_VAR1(TBorderStyle, BorderStyle );
	PROPERTY_VAR0(TMainMenu*, Menu);
	PROPERTY_VAR0(TIcon*, Icon);
	PROPERTY_VAR0(TFormStyle, FormStyle);
	PROPERTY_VAR0(int, BorderIcons);

	// BCBのBorderStyelプロパティは以下の通り。krkr.eXeは、SetBorderStyleをオーバーライドしてる
	// オーバーライドされた SetBorderStyleの中で、描画用のDCが初期化されるので、そこが動くようがんばる。
	// __property TFormBorderStyle BorderStyle = {read=FBorderStyle, write=SetBorderStyle, stored=IsForm, default=2};
	virtual void SetBorderStyle(TFormBorderStyle Value);

	void SetFocus();
	HRESULT SetHWnd(int, HWND);
	int ShowModal();
	void __fastcall Close(void);
	bool __fastcall IsShortCut(Messages::TWMKey &Message);
	virtual void WndProc(TMessage&);
	void SetZOrder(bool);

	// イベントのデフォルトハンドラ(嘘)
	virtual void FormDestroy( TObject * Sender );
	virtual void FormClose(TObject *Sender, TCloseAction &Action);
	virtual void FormCloseQuery(TObject *Sender, bool &CanClose);

	// なんだかイベントハンドラ
	boost::function1<void, System::TObject*> OnPaint;
	boost::function1<void, System::TObject*> OnDestroy;
};

//----------------------------------------------------------------------------
class TForm : public TCustomForm
{
public:
	TForm(TComponent* owner);
	virtual ~TForm();
};

//----------------------------------------------------------------------------
class TScrollBar;
class TScrollBox : public TScrollingWinControl
{
public:
	TScrollBox(TComponent* owner);

	TScrollBar *HorzScrollBar;
	TScrollBar *VertScrollBar;
};
class TScrollBar : public TWinControl
{
public:
	TScrollBar(TComponent* owner);

	PROPERTY(int, Position);
};

//----------------------------------------------------------------------------
// TScrollingWinControl は略
class TCustomFrame : public TWinControl
{
public:
	TCustomFrame( TComponent* owner ) : TWinControl(owner){};
};
//----------------------------------------------------------------------------
class TFrame : public TCustomFrame
{
public:
	TFrame( TComponent* owner ) : TCustomFrame(owner){};
};