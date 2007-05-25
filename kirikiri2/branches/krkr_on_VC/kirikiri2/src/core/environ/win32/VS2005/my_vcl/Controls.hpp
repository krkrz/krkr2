#pragma once

#include "vcl_base.h"
#include "Classes.hpp"
#include "Menus.hpp"
#include "Graphics.hpp"
#include "Messages.hpp"

#include <wx/listctrl.h>

#define BEGIN_MESSAGE_MAP virtual bool HandleMessageMap(TMessage msg) {
#define VCL_MESSAGE_HANDLER(id,type,func) if (msg.Msg == id ) { func( *((type*)&msg) ); return true; }
#define END_MESSAGE_MAP(x) return false; }

#define CM_MOUSEENTER WM_USER+200
#define CM_MOUSELEAVE WM_USER+201
//----------------------------------------------------------------------------
typedef unsigned int TImeMode;

//----------------------------------------------------------------------------
typedef int TConstraintSize;
class TSizeConstraints : public System::TObject
{
public:
	PROPERTY(TConstraintSize,MinHeight);
};



//----------------------------------------------------------------------------
class TWinControl;
class TControl : public TComponent
{
protected:
	DYNAMIC void __fastcall DblClick();
	DYNAMIC void __fastcall MouseDown(TMouseButton button, TShiftState shift, int x, int y);
	DYNAMIC void __fastcall MouseUp(TMouseButton button, TShiftState shift, int x, int y);
	DYNAMIC void __fastcall MouseMove(TShiftState shift, int x, int y);
	DYNAMIC void __fastcall Resize(void);
	PROPERTY_abstract(AnsiString, Caption);
public:
	TControl(TComponent* Owner);
	virtual ~TControl();

	PROPERTY_abstract(TAlign, Align );
	PROPERTY_abstract(int, ClientWidth);
	PROPERTY_abstract(int, ClientHeight);
	PROPERTY_abstract(TSizeConstraints*, Constraints);
	PROPERTY_VAR1(TColor, Color );
	PROPERTY_abstract(bool, Enabled);
	PROPERTY_VAR0(TPopupMenu*, PopupMenu); // 右クリックしたときに初めて評価される
	PROPERTY_abstract(bool, Visible);
	PROPERTY_VAR0(TWinControl*, Parent); // サブクラスへのポインタ
	PROPERTY_VAR0(bool, ShowHint);
	PROPERTY_VAR0(TAnchors, Anchors);
__published:
	PROPERTY_VAR0(TCursor, Cursor);
	PROPERTY_VAR0(AnsiString, Hint);
	PROPERTY_abstract(int, Left  );
	PROPERTY_abstract(int, Top   );
	PROPERTY_abstract(int, Width );
	PROPERTY_abstract(int, Height);

	PROPERTY(AnsiString, Text);
	PROPERTY_VAR_R(TFont*, Font);

	virtual void __fastcall BringToFront() = 0;
	virtual TPoint ClientToScreen(TPoint) const = 0;
	virtual void Invalidate(void) = 0;
	virtual TPoint ScreenToClient(TPoint) const = 0;
	virtual void SetBounds(int left, int top, int width, int height);
	virtual LRESULT Perform(UINT msg, WPARAM wparam, LPARAM lparam) = 0;

	// イベント
	boost::function1<void, System::TObject*> OnClick;
	boost::function1<void, System::TObject*> OnDblClick;
	boost::function5<void, System::TObject*, TMouseButton, TShiftState, int, int> OnMouseDown;
	boost::function4<void, System::TObject*, TShiftState, int, int> OnMouseMove;
	boost::function5<void, System::TObject*, TMouseButton, TShiftState, int, int> OnMouseUp;

	boost::function1<void, System::TObject*> OnResize;
};

//----------------------------------------------------------------------------
class TGraphicControl : public TControl
{
protected:
	PROPERTY_VAR0(AnsiString, Caption);
public:
	TGraphicControl( TComponent* );
	virtual ~TGraphicControl();
	PROPERTY_VAR_R(TCanvas*, Canvas);

	// 仮想関数の実装
	PROPERTY_VAR0(TAlign, Align );
	PROPERTY_VAR0(int, ClientWidth);
	PROPERTY_VAR0(int, ClientHeight);
	PROPERTY_VAR0(TSizeConstraints*, Constraints);
	PROPERTY_VAR0(bool, Enabled);
	PROPERTY_VAR0(bool, Visible);
	PROPERTY_VAR0(int, Left  );
	PROPERTY_VAR0(int, Top   );
	PROPERTY_VAR0(int, Width );
	PROPERTY_VAR0(int, Height);

	virtual void __fastcall BringToFront();
	virtual TPoint ClientToScreen(TPoint) const;
	virtual void Invalidate(void);
	virtual TPoint ScreenToClient(TPoint) const;
	virtual LRESULT Perform(UINT msg, WPARAM wparam, LPARAM lparam);
};

//----------------------------------------------------------------------------
class TWinControl : public TControl
{
	friend TGraphicControl;
protected:
	wxWindow* m_wxWindow;
protected:
	virtual void __fastcall CreateParams(TCreateParams &Params);
	PROPERTY(AnsiString, Caption);
public:
	TWinControl( TComponent* );
	virtual ~TWinControl();
	TImeMode ImeMode;
	PROPERTY_VAR_R(HWND, Handle);

	struct TSysLocale{ bool FarEast; } SysLocale;
	TBrush * Brush;

	bool __fastcall SetImeCompositionWindow(Graphics::TFont* Font, int XPos, int YPos);
	void __fastcall ResetIme(void);
	void __fastcall SetIme(void);
	bool Focused(void);


	BEGIN_MESSAGE_MAP
	END_MESSAGE_MAP(TForm)

	// イベント
	boost::function2<void, System::TObject*, char&> OnKeyPress;
	boost::function3<void, System::TObject*, Word&, TShiftState> OnKeyDown;
	boost::function3<void, System::TObject*, Word&, TShiftState> OnKeyUp;

	// 仮想関数の実装
	PROPERTY_VAR0(TAlign, Align );
	PROPERTY(int, ClientWidth);
	PROPERTY(int, ClientHeight);
	PROPERTY_VAR0(TSizeConstraints*, Constraints);
	PROPERTY(bool, Enabled);
	PROPERTY(bool, Visible);
	PROPERTY(int, Left  );
	PROPERTY(int, Top   );
	PROPERTY(int, Width );
	PROPERTY(int, Height);

	virtual void __fastcall BringToFront();
	virtual TPoint ClientToScreen(TPoint) const;
	virtual void Invalidate(void);
	virtual TPoint ScreenToClient(TPoint) const;
	virtual void SetBounds(int left, int top, int width, int height);
	virtual LRESULT Perform(UINT msg, WPARAM wparam, LPARAM lparam);
};

//----------------------------------------------------------------------------
class TCustomControl : public TWinControl
{
public:
	TCustomControl( TComponent* );
	virtual ~TCustomControl();
	PROPERTY_VAR_R(TCanvas*, Canvas);
};

//----------------------------------------------------------------------------
class TButton : public TWinControl
{
public:
	TButton( TComponent* );
	PROPERTY_VAR0(bool, Down);

	DYNAMIC void Click();
};
class TToolButton : public TButton
{
public:
	TToolButton( TComponent* );
};


//----------------------------------------------------------------------------
// TCustomImageList, TDragImageList は略
// Windows コモンコントロールのイメージリストコントロールを操作するコンポーネント
class TImageList : public TComponent
{
public:
	TImageList( TComponent* );
	virtual ~TImageList();
	void GetIcon( int Index, TIcon* Image );
};


//----------------------------------------------------------------------------
class TCustomListControl : public  TWinControl
{
public:
	TCustomListControl(TComponent* owner);
	PROPERTY_VAR0(int, ItemIndex);
};


//----------------------------------------------------------------------------
//TCustomTreeViewは略
class TTreeNodes;
class TTreeNode;
class TTreeView : public TWinControl
{
public:
	PROPERTY_VAR0(TTreeNodes*, Items);
	PROPERTY_VAR0(TTreeNode*, TopItem);
	PROPERTY_VAR0(TTreeNode*, Selected);
	void FullExpand();
};
class TTreeNode : public System::TObject
{
public:
	PROPERTY_VAR0(int, Count);
	PROPERTY_VAR0(void*, Data);
	PROPERTY_ARY_R(TTreeNode*, Item);
	PROPERTY_VAR0(AnsiString, Text);
	PROPERTY_VAR0(TTreeNode*, Parent);

	TTreeNode* GetNext();
};
class TTreeNodes : public System::TObject
{
public:
	PROPERTY_VAR0(int, Count);
	PROPERTY_ARY_R(TTreeNode*, Item);
	void BeginUpdate(); // リストビューの更新を抑制する, 再描画を行うときは、 EndUpdate メソッドを実行する
	void EndUpdate();
	TTreeNode* AddChild(TTreeNode * node, const AnsiString & s);
	TTreeNode* Add(TTreeNode * node, const AnsiString & s);
};

//----------------------------------------------------------------------------
class TMemo;
//----------------------------------------------------------------------------
// TCustomLable は略
class TLabel : public TGraphicControl
{
};


//----------------------------------------------------------------------------
#include "Dialogs.hpp"
//----------------------------------------------------------------------------
class TCustomStatusBar : public TWinControl
{
public:
	TCustomStatusBar( TComponent* );
};

//----------------------------------------------------------------------------
class TStatusPanel;
class TStatusPanels;
class TStatusBar : public TCustomStatusBar
{
public:
	TStatusBar( TComponent* );
	virtual ~TStatusBar();
	PROPERTY_VAR_R(TStatusPanels*, Panels);
};
class TStatusPanels : public TCollection
{
	friend TStatusBar;
	std::vector<TStatusPanel> m_Items;
public:
	PROPERTY_ARY_R(TStatusPanel*, Items);
};
class TStatusPanel : public TCollectionItem
{
public:
	PROPERTY(AnsiString, Text);
};
//----------------------------------------------------------------------------
// TCustomPanel は略
class TPanel : public TCustomControl
{
public:
	TPanel( TComponent* );
};
//----------------------------------------------------------------------------
class TFontDialog : public TCommonDialog
{
public:
	TFontDialog( TComponent* );
	PROPERTY_VAR_R(TFont*, Font);
	virtual bool Execute();
};
//----------------------------------------------------------------------------
class THintWindow : public TWinControl
{
};
//----------------------------------------------------------------------------
class TPaintBox : public TGraphicControl
{
public:
	TPaintBox(TComponent* Owner);

	// イベント
	boost::function1<void, System::TObject*> OnPaint;
};


//----------------------------------------------------------------------------
class TImage : public TGraphicControl
{
public:
	PROPERTY_VAR0(TPicture*, Picture);
};

//----------------------------------------------------------------------------
class TGroupBox : public TCustomControl
{
};
class TCheckBox : public TCustomControl
{
public:
	PROPERTY_VAR0(bool, Checked);
};

extern void SetCaptureControl(TControl*);
extern int MessageDlg(const AnsiString&, TMsgDlgType, TMsgDlgButtons, int );
extern bool InputQuery(const AnsiString&, const AnsiString&,AnsiString&);
extern void Win32NLSEnableIME(HWND, BOOL);
