#include "vcl.h"
#include "Controls.hpp"

//----------------------------------------------------------------------------
void SetCaptureControl(TControl*)
{
}

//----------------------------------------------------------------------------
// メッセージダイアログボックスを表示する
int MessageDlg( const AnsiString& Msg, TMsgDlgType DlgType, TMsgDlgButtons Buttons, int HelpCtx )
{
	//enum TMsgDlgType { mtWarning, mtError, mtInformation, mtConfirmation, mtCustom };
	//MB_ICONEXCLAMATION (または MB_ICONWARNING)  感嘆符アイコンを表示します。 
	//MB_ICONINFORMATION (または MB_ICONASTERISK)  吹き出しの中に小文字の「 i 」があるアイコンを表示します。 
	//MB_ICONQUESTION 疑問符のアイコンを表示します。 
	//MB_ICONSTOP (または MB_ICONERROR、または MB_ICONHAND)  

	UINT uType = 0;
	AnsiString strCaption;
	switch( DlgType )
	{
	default:
	case mtWarning:      uType = MB_ICONWARNING;     strCaption = "警告";   break;
	case mtError:        uType = MB_ICONERROR;       strCaption = "エラー"; break; 
	case mtInformation:  uType = MB_ICONINFORMATION; strCaption = "情報";   break;
	case mtConfirmation: uType = MB_ICONQUESTION;    strCaption = "確認";   break;
	case mtCustom:       uType = 0;                  strCaption = "未実装"; break;
	}
	// ボタンの選別は更にだるい ので、略
	//enum { mbYes=0, mbNo=1, mbOk=2, mbOK=2, mbCancel=3, mbAbort=4, mbRetry=5, mbIgnore=6, mbALL=7, mbNoToAll=8, mbYesToAll=9, mbHelp=10 };
	//MB_ABORTRETRYIGNORE [ 中止 ]、[ 再試行 ]、[ 無視 ] プッシュボタンを表示します。 
	//MB_OK [OK] プッシュボタンを表示します。デフォルトです。 
	//MB_OKCANCEL [OK]、[ キャンセル ] プッシュボタンを表示します。 
	//MB_RETRYCANCEL [ 再試行 ]、[ キャンセル ] プッシュボタンを表示します。 
	//MB_YESNO [ はい ]、[ いいえ ] プッシュボタンを表示します。 
	//MB_YESNOCANCEL 
	if ( Buttons.Contains(mbYes) | Buttons.Contains(mbNo) )
	{
		if ( Buttons.Contains(mbCancel) ) 
			uType |= MB_YESNOCANCEL;
		else
			uType |= MB_YESNO;
	}
	else if ( Buttons.Contains(mbOk) )
	{
		if ( Buttons.Contains(mbCancel) ) 
			uType |= MB_OKCANCEL;
		else
			uType |= MB_OK;
	}
	else if ( Buttons.Contains(mbCancel) )
		uType |= MB_RETRYCANCEL;
	else
		uType |= MB_ABORTRETRYIGNORE;

	// ダイアログボックスを出す
	int nRes = ::MessageBox(NULL, Msg.c_str(), strCaption.c_str(), uType ); 

	// 戻り値の張替え
	//enum { mbYes=0, mbNo=1, mbOk=2, mbOK=2, mbCancel=3, mbAbort=4, mbRetry=5, mbIgnore=6, mbALL=7, mbNoToAll=8, mbYesToAll=9, mbHelp=10 };
	switch( nRes )
	{
	case IDABORT:  return mbNoToAll;
	case IDCANCEL: return mbCancel;
	case IDIGNORE: return mbYesToAll;
	case IDNO:     return mbNo;
	case IDOK:     return mbOk;
	case IDRETRY:  return mbRetry;
	case IDYES:    return mbYes;
	}
	return mbOk;
}

//----------------------------------------------------------------------------
bool InputQuery(const AnsiString& ACaption, const AnsiString& APrompt, AnsiString& AValue)
{
	AValue = "";
	return false;
}

//----------------------------------------------------------------------------
void Win32NLSEnableIME(HWND hwnd, BOOL b)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
TControl::TControl(TComponent* owner) : TComponent(owner)
{
	m_Parent = dynamic_cast<TWinControl*>(owner); // 初期の親は、オーナーですよ
	m_Font = new TFont();
}

//----------------------------------------------------------------------------
TControl::~TControl()
{
	delete m_Font;
}

//----------------------------------------------------------------------------
void __fastcall TControl::DblClick() {}
void __fastcall TControl::MouseDown(TMouseButton button, TShiftState shift, int x, int y) {}
void __fastcall TControl::MouseUp(TMouseButton button, TShiftState shift, int x, int y) {}
void __fastcall TControl::MouseMove(TShiftState shift, int x, int y) {}
void __fastcall TControl::Resize(void) {}

void TControl::SetBounds(int left, int top, int width, int height)
{
	Left   = left;
	Top    = top;
	Width  = width;
	Height = height;
}

//----------------------------------------------------------------------------
// 色設定変えて、再描画かける
//PROPERTY(TPopupMenu*, PopupMenu);
void TControl::setColor(TColor val)
{
	m_Color = val;
	Invalidate();
}

//----------------------------------------------------------------------------
// TEdit、TMemo、TComboBox オブジェクトの Text プロパティ
// したがって、TControl では処理が何も無い。(かといって、純仮想にもできない…
//PROPERTY(AnsiString, Text);
void TControl::setText(AnsiString str)
{
}
AnsiString TControl::getText() const
{
	return AnsiString();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//class TGraphicControl : public TControl
// オブジェクトを作成する場合は、new キーワードを使用し、直接このメソッドを呼び出してはならない。(らしい)
TGraphicControl::TGraphicControl( TComponent* owner ) : TControl(owner)
{
	m_Canvas = new TCanvas( new wxClientDC( getParent()->m_wxWindow ) );
}
TGraphicControl::~TGraphicControl()
{
	delete m_Canvas;
}
//	PROPERTY_VAR_R(TCanvas*, Canvas);
//	PROPERTY_VAR_R(TCanvas*, Canvas);

	// 仮想関数の実装
//	PROPERTY_VAR0(TAlign, Align );
//	PROPERTY_VAR0(int, ClientWidth);
//	PROPERTY_VAR0(int, ClientHeight);
//	PROPERTY_VAR0(bool, Enabled);
//	PROPERTY_VAR0(bool, Visible);
//	PROPERTY_VAR0(int, Left  );
//	PROPERTY_VAR0(int, Top   );
//	PROPERTY_VAR0(int, Width );
//	PROPERTY_VAR0(int, Height);
//	PROPERTY_VAR1(TWinControl*, Parent); // サブクラスへのポインタ

void TGraphicControl::BringToFront()
{
	getParent()->BringToFront();
}

//----------------------------------------------------------------------------
TPoint TGraphicControl::ClientToScreen(TPoint pt) const
{
	pt = getParent()->ClientToScreen(pt);
	pt.x += Left;
	pt.y += Top;
	return pt;
}
//----------------------------------------------------------------------------
void TGraphicControl::Invalidate(void)
{
	getParent()->Invalidate();
}
//----------------------------------------------------------------------------
TPoint TGraphicControl::ScreenToClient(TPoint pt) const
{
	pt = getParent()->ScreenToClient(pt);
	pt.x -= Left;
	pt.y -= Top;
	return pt;
}

//----------------------------------------------------------------------------
LRESULT TGraphicControl::Perform(UINT msg, WPARAM wparam, LPARAM lparam)
{
	return getParent()->Perform(msg, wparam, lparam);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//class TWinControl : public TControl
//----------------------------------------------------------------------------
// TWinControl オブジェクトのコンストラクタ
// オブジェクトを作成する場合は、new キーワードを使用し、直接このメソッドを呼び出してはならない。(らしい)
TWinControl::TWinControl(TComponent* owner) : TControl(owner)
{
	m_wxWindow = new wxWindow();
	Brush = new TBrush;
	// Createの機構と合わせて考えないと・・・
	//	m_Handle = (HWND)m_wxWindow->GetHandle();
}
//----------------------------------------------------------------------------
TWinControl::~TWinControl()
{
	delete m_wxWindow;
	delete Brush;
}
//----------------------------------------------------------------------------
//ウィンドウを作成するときの情報を保持する構造体を初期化する
void __fastcall TWinControl::CreateParams(TCreateParams &Params)
{
	memset(&Params, 0, sizeof(TCreateParams) );
}
//----------------------------------------------------------------------------
bool __fastcall TWinControl::SetImeCompositionWindow(Graphics::TFont* Font, int XPos, int YPos)
{
	return false;
}
//----------------------------------------------------------------------------
void __fastcall TWinControl::ResetIme(void) {}
//----------------------------------------------------------------------------
void __fastcall TWinControl::SetIme(void) {}
//----------------------------------------------------------------------------
bool TWinControl::Focused(void)
{
	wxWindow* pFocused = wxWindow::FindFocus();
	if ( pFocused && m_wxWindow->GetHandle() == pFocused->GetHandle() )
		return true;
	else
		return false;
}


//----------------------------------------------------------------------------
//	PROPERTY(AnsiString, Caption);
void TWinControl::setCaption( AnsiString val )
{
	m_wxWindow->SetLabel( val.c_str() );
}
AnsiString TWinControl::getCaption( ) const
{
	return m_wxWindow->GetLabel( ).c_str();
}


//PROPERTY_VAR0(TAlign, Align );
//----------------------------------------------------------------------------
//PROPERTY_VAR0(int, ClientWidth);
void TWinControl::setClientWidth(int v)
{
	wxSize sz = m_wxWindow->GetClientSize();
	sz.SetWidth(v);
	m_wxWindow->SetClientSize(sz);
}
int TWinControl::getClientWidth() const
{
	return m_wxWindow->GetClientSize().GetWidth();
}
//----------------------------------------------------------------------------
//PROPERTY_VAR0(int, ClientHeight);
void TWinControl::setClientHeight(int v)
{
	wxSize sz = m_wxWindow->GetClientSize();
	sz.SetHeight(v);
	m_wxWindow->SetClientSize(sz);
}
int TWinControl::getClientHeight() const
{
	return m_wxWindow->GetClientSize().GetHeight();
}
//----------------------------------------------------------------------------
//PROPERTY_VAR0(bool, Enabled);
void TWinControl::setEnabled(bool v)
{
	m_wxWindow->Show(v);
}
bool TWinControl::getEnabled() const
{
	return m_wxWindow->IsEnabled();
}

//----------------------------------------------------------------------------
//PROPERTY_VAR0(bool, Visible);
void TWinControl::setVisible(bool v)
{
	m_wxWindow->Show(v);
}
bool TWinControl::getVisible() const
{
	return m_wxWindow->IsShown();
}
//----------------------------------------------------------------------------
//PROPERTY(int, Left  );
void TWinControl::setLeft(int v)
{
	wxPoint pt = m_wxWindow->GetPosition();
	pt.x = v;
	m_wxWindow->Move( pt );
}
int TWinControl::getLeft() const
{
	return m_wxWindow->GetPosition().x;
}
//----------------------------------------------------------------------------
//PROPERTY(int, Top   );
void TWinControl::setTop(int v)
{
	wxPoint pt = m_wxWindow->GetPosition();
	pt.y = v;
	m_wxWindow->Move( pt );
}
int TWinControl::getTop() const
{
	return m_wxWindow->GetPosition().y;
}
//----------------------------------------------------------------------------
//PROPERTY(int, Width );
void TWinControl::setWidth(int v)
{
	wxSize sz = m_wxWindow->GetSize();
	sz.SetWidth(v);
	m_wxWindow->SetSize(sz);
}
int TWinControl::getWidth() const
{
	return m_wxWindow->GetSize().GetWidth();
}
//----------------------------------------------------------------------------
//PROPERTY(int, Height);
void TWinControl::setHeight(int v)
{
	wxSize sz = m_wxWindow->GetSize();
	sz.SetHeight(v);
	m_wxWindow->SetSize(sz);
}
int TWinControl::getHeight() const
{
	return m_wxWindow->GetSize().GetHeight();
}
//----------------------------------------------------------------------------
void TWinControl::BringToFront()
{
	m_wxWindow->Raise();
}

//----------------------------------------------------------------------------
TPoint TWinControl::ClientToScreen(TPoint pt) const
{
	int x = pt.x, y = pt.y;
	m_wxWindow->ClientToScreen(&x, &y);
	return TPoint(x,y);
}

//----------------------------------------------------------------------------
void TWinControl::Invalidate(void)
{
	m_wxWindow->Update();
}

//----------------------------------------------------------------------------
TPoint TWinControl::ScreenToClient(TPoint pt) const
{
	int x = pt.x, y = pt.y;
	m_wxWindow->ScreenToClient(&x, &y);
	return TPoint(x,y);
}

//----------------------------------------------------------------------------
void TWinControl::SetBounds(int left, int top, int width, int height)
{
	m_wxWindow->SetSize(left, top, width, height );
}

//----------------------------------------------------------------------------
// CWnd::SendMessage に相当する。さて、どーしたもんか
LRESULT TWinControl::Perform(UINT msg, WPARAM wparam, LPARAM lparam)
{
	return ::SendMessage( Handle, msg, wparam, lparam );
}




//----------------------------------------------------------------------------
//class TPaintBox : public TGraphicControl
TPaintBox::TPaintBox(TComponent* Owner) : TGraphicControl(Owner)
{
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//class TCustomControl : public TWinControl
TCustomControl::TCustomControl( TComponent* owner ) : TWinControl(owner)
{
	m_Canvas = new TCanvas( new wxWindowDC( m_wxWindow ) );
}
TCustomControl::~TCustomControl()
{
	delete m_Canvas;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
TButton::TButton( TComponent* owner) : TWinControl(owner)
{
}

//----------------------------------------------------------------------------
//void TButton::setDown(bool val)
//{
//}

//bool TButton::getDown() const
//{
//	return false;
//}

//----------------------------------------------------------------------------
void TButton::Click()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
TToolButton::TToolButton( TComponent* owner) : TButton(owner)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#include "Graphics.hpp"
//class TImageList : public TComponent
TImageList::TImageList( TComponent* owner) : TComponent(owner)
{
}
//----------------------------------------------------------------------------
TImageList::~TImageList()
{
}
//----------------------------------------------------------------------------
// Image には、有効な TIconのオブジェクトが渡ってくるので、そこへアイコンを描画してあげればよい
// が、めんどいのでしない
void TImageList::GetIcon( int Index, TIcon* Image )
{
}



//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
TCustomListControl::TCustomListControl(TComponent* owner) : TWinControl(owner)
{
}



//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//class TTreeView : public TWinControl
void TTreeView::FullExpand()
{
}

//----------------------------------------------------------------------------
//class TTreeNode : public System::TObject

//PROPERTY_ARY_R(TTreeNode*, Item);

TTreeNode* TTreeNode::GetNext()
{
	return NULL;
}

//----------------------------------------------------------------------------
//class TTreeNodes : public System::TObject
//	PROPERTY_ARY_R(TTreeNode*, Item);
void TTreeNodes::BeginUpdate()
{
}
void TTreeNodes::EndUpdate()
{
}
TTreeNode* TTreeNodes::AddChild(TTreeNode * node, const AnsiString & s)
{
	return NULL;
}
TTreeNode* TTreeNodes::Add(TTreeNode * node, const AnsiString & s)
{
	return NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
TCustomStatusBar::TCustomStatusBar( TComponent* owner ) : TWinControl(owner)
{
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
TStatusBar::TStatusBar( TComponent* owner ) : TCustomStatusBar(owner)
{
	m_Panels = NULL;
	m_Panels = new TStatusPanels();
	m_Panels->m_Items.resize(10);
}

TStatusBar::~TStatusBar()
{
	if ( m_Panels )
		delete m_Panels;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TStatusPanel::setText(AnsiString v)
{
}
AnsiString TStatusPanel::getText() const
{
	return "";
}


//----------------------------------------------------------------------------
TStatusPanel* TStatusPanels::getItems(int n) const
{
	if ( n >= m_Items.size() )
		return NULL;
	return const_cast<TStatusPanel*>( &(m_Items[n]) );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
TPanel::TPanel( TComponent* owner ) : TCustomControl(owner)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
TFontDialog::TFontDialog( TComponent* owner ) : TCommonDialog(owner)
{
}

//----------------------------------------------------------------------------
bool TFontDialog::Execute()
{
	return false;
}
