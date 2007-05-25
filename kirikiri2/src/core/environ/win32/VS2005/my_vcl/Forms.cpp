#include "Forms.hpp"

class MyFrameTCustomForm : public wxFrame
{
private:
	TCustomForm * m_pForm;
	int m_nLastX, m_nLastY;
public:
	//------------------------------------
	MyFrameTCustomForm(TCustomForm * pForm) :
		wxFrame(NULL, -1, _T("Canvas Frame"), wxPoint(10, 10), wxSize(300, 300), wxDEFAULT_FRAME_STYLE)
	{
		m_pForm = pForm;
	}
	//------------------------------------
	// デストラクタが走る。MyFrameTCustomFormとwxFrameは対で動作する。
	// wxWidgetsのフレームワークから、直接このクラスのインスタンスが破壊されてしまうケースがある
	// その場合に、TFormのデストラクタ中で deleteが失敗して困る。
	// 理想は、片方を delete しようとしたら、互いに他方を deleteする構造か？　うまくいかない・・
	virtual ~MyFrameTCustomForm()
	{
		m_pForm = NULL;
	}
	//------------------------------------
	void OnPaint(wxPaintEvent& event)
	{
		wxPaintDC dc(this);
		int i, n = m_pForm->ComponentCount;
		for(i=0; i < n; i++)
		{
			TComponent* _p = m_pForm->Components[i];
			TPaintBox* p = dynamic_cast<TPaintBox*>(_p);
			if ( p && p->OnPaint ) p->OnPaint( p );
		}
		if ( m_pForm->OnPaint ) m_pForm->OnPaint( m_pForm );
	}
	//------------------------------------
	void OnClick()
	{
		int i, n = m_pForm->ComponentCount;
		for(i=0; i < n; i++)
		{
			TComponent* _p = m_pForm->Components[i];
			TPaintBox* p = dynamic_cast<TPaintBox*>(_p);
			if ( p && p->OnClick ) p->OnClick( p );
		}
		if ( m_pForm->OnClick ) m_pForm->OnClick( m_pForm );
	}
	//------------------------------------
	void OnMouseDown(wxMouseEvent& event)
	{
		TMouseButton btn;
		if (event.LeftDown())
		{
			btn = mbLeft;
			m_nLastX = event.m_x;
			m_nLastY = event.m_y;
		}
		else
		{
			m_nLastX = m_nLastY = -1;
			if (event.RightDown())  btn = mbRight;
			else if (event.MiddleDown()) btn = mbMiddle;
		}

		TShiftState shift;
		if (event.AltDown()  ) shift << ssAlt;
		if (event.ShiftDown()) shift << ssShift;

		int i, n = m_pForm->ComponentCount;
		for(i=0; i < n; i++)
		{
			TComponent* _p = m_pForm->Components[i];
			TPaintBox* p = dynamic_cast<TPaintBox*>(_p);
			if ( p && p->OnMouseDown ) p->OnMouseDown( p, btn, shift, event.m_x, event.m_y );
		}
		if ( m_pForm->OnMouseDown ) m_pForm->OnMouseDown( m_pForm, btn, shift, event.m_x, event.m_y );
		event.Skip();
	}
	//------------------------------------
	void OnMouseUp(wxMouseEvent& event)
	{
		TMouseButton btn;
		if (event.LeftUp())
		{
			btn = mbLeft;
			if ( abs(m_nLastX - event.m_x) < 4 && abs(m_nLastY - event.m_y) < 4 )
				OnClick();
		}
		else
		{
			m_nLastX = m_nLastY = -1;
			if (event.RightUp())  btn = mbRight;
			else if (event.MiddleUp()) btn = mbMiddle;
		}

		TShiftState shift;
		if (event.AltDown()  ) shift << ssAlt;
		if (event.ShiftDown()) shift << ssShift;

		int i, n = m_pForm->ComponentCount;
		for(i=0; i < n; i++)
		{
			TComponent* _p = m_pForm->Components[i];
			TPaintBox* p = dynamic_cast<TPaintBox*>(_p);
			if ( p && p->OnMouseUp ) p->OnMouseUp( p, btn, shift, event.m_x, event.m_y );
		}
		if ( m_pForm->OnMouseUp ) m_pForm->OnMouseUp( m_pForm, btn, shift, event.m_x, event.m_y );
	}
	//------------------------------------
	void OnLeftDClick(wxMouseEvent& event)
	{
		int i, n = m_pForm->ComponentCount;
		for(i=0; i < n; i++)
		{
			TComponent* _p = m_pForm->Components[i];
			TPaintBox* p = dynamic_cast<TPaintBox*>(_p);
			if ( p && p->OnDblClick ) p->OnDblClick( p );
		}
		if ( m_pForm->OnDblClick ) m_pForm->OnDblClick( m_pForm );
	}
	//------------------------------------
	void OnMotion(wxMouseEvent& event)
	{
		TShiftState shift;
		if (event.AltDown()  ) shift << ssAlt;
		if (event.ShiftDown()) shift << ssShift;

		int i, n = m_pForm->ComponentCount;
		for(i=0; i < n; i++)
		{
			TComponent* _p = m_pForm->Components[i];
			TPaintBox* p = dynamic_cast<TPaintBox*>(_p);
			if ( p && p->OnMouseMove ) p->OnMouseMove( p, shift, event.m_x, event.m_y );
		}
		if ( m_pForm->OnMouseMove ) m_pForm->OnMouseMove( m_pForm, shift, event.m_x, event.m_y );
	}

	//------------------------------------
	int TranslateKey(int key)
	{
		switch(key)
		{
		case WXK_LEFT:  return VK_LEFT;
		case WXK_UP:    return VK_UP;
		case WXK_RIGHT: return VK_RIGHT;
		case WXK_DOWN:  return VK_DOWN;
		}
		return key;
	}
	//------------------------------------
	void OnKeyDown(wxKeyEvent& event)
	{
		Word key = TranslateKey( event.GetKeyCode() );

		if ( m_pForm->OnKeyDown )
			m_pForm->OnKeyDown( m_pForm, key, TShiftState() );
	}
	//------------------------------------
	void OnKeyUp(wxKeyEvent& event)
	{
		Word key = TranslateKey( event.GetKeyCode() );

		if ( m_pForm->OnKeyUp )
			m_pForm->OnKeyUp( m_pForm, key, TShiftState() );
	}
	//------------------------------------
	void OnKeyPress(wxKeyEvent& event)
	{
		char key = TranslateKey( event.GetKeyCode() );

		if ( m_pForm->OnKeyPress )
			m_pForm->OnKeyPress( m_pForm, key );
	}
	//------------------------------------
	void OnSize(wxSizeEvent& event)
	{
	}
	//------------------------------------
	void OnClose(wxCloseEvent& event)
	{
		OutputDebugString("OnClose\n");
		TCloseAction Action = caFree;
		if ( event.CanVeto() == false )
		{
			if ( m_pForm && m_pForm->OnClose )
				m_pForm->OnClose(m_pForm, Action);
			TCustomForm * p = m_pForm;
			m_pForm = NULL;
			delete p;
			return;
		}

		// いろいろ端折る
		// 最初にクローズ可能か問い合わせ
		// たぶん、この時点で全子ウィンドウに問い合わせがいるでしょうが、端折る
		bool CanClose = true;
		if ( m_pForm && m_pForm->OnCloseQuery )
			m_pForm->OnCloseQuery(m_pForm, CanClose);

		// 次に、クローズにトライ
		if ( CanClose == true )
		{
			if ( m_pForm && m_pForm->OnClose )
				m_pForm->OnClose(m_pForm, Action);
			switch( Action )
			{
			case caHide:
				Hide();
			case caNone:
				event.Veto();
				break;
			case caFree:
				TCustomForm * p = m_pForm;
				m_pForm = NULL;
				delete p;
				break;
			}
		}
		else
			event.Veto();
	}
	//------------------------------------
	virtual bool Destroy()
	{
		bool res = wxFrame::Destroy();

		if ( res && m_pForm && m_pForm->OnClose )
		{
			TCloseAction Action = caFree;
			m_pForm->OnClose(m_pForm, Action);
		}
		return res;
	}


	//------------------------------------
	WXLRESULT MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
	{
//		if ( message >= WM_USER && message < WM_USER + 200 )
		{
#ifdef _DEBUG
//			char buf[1024];
//			sprintf(buf, "WM_USER + %d(%xh)\n", message - WM_USER, message - WM_USER );
//			OutputDebugString(buf);
#endif
			TMessage msg = {message, wParam, lParam, 0};
			if ( m_pForm->HandleMessageMap(msg) == true )
				return msg.Result;
		}
		return wxFrame::MSWWindowProc(message, wParam, lParam);
	}

	//------------------------------------
	// TForm::Dispatchを呼ばれたときはコレが行く
	WXLRESULT MSWWindowProcForDispatch(WXUINT message, WXWPARAM wParam, WXLPARAM lParam)
	{
		return wxFrame::MSWWindowProc(message, wParam, lParam);
	}
	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(MyFrameTCustomForm, wxFrame)
	EVT_PAINT(MyFrameTCustomForm::OnPaint)
	EVT_SIZE(MyFrameTCustomForm::OnSize)
	EVT_CLOSE(MyFrameTCustomForm::OnClose)

	EVT_LEFT_DOWN   (MyFrameTCustomForm::OnMouseDown)
	EVT_RIGHT_DOWN  (MyFrameTCustomForm::OnMouseDown)
	EVT_MIDDLE_DOWN (MyFrameTCustomForm::OnMouseDown)
	EVT_LEFT_UP     (MyFrameTCustomForm::OnMouseUp)
	EVT_RIGHT_UP    (MyFrameTCustomForm::OnMouseUp)
	EVT_MIDDLE_UP   (MyFrameTCustomForm::OnMouseUp)
	EVT_LEFT_DCLICK (MyFrameTCustomForm::OnLeftDClick)
	EVT_MOTION      (MyFrameTCustomForm::OnMotion)

	EVT_KEY_DOWN    (MyFrameTCustomForm::OnKeyDown)
	EVT_KEY_UP      (MyFrameTCustomForm::OnKeyUp)
	EVT_CHAR        (MyFrameTCustomForm::OnKeyPress)
END_EVENT_TABLE()


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
TScrollingWinControl::TScrollingWinControl(TComponent* owner) : TWinControl(owner)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
TCustomForm::TCustomForm(TComponent* Owner) : TScrollingWinControl(Owner)
{
	m_wxFrame = new MyFrameTCustomForm(this);
	m_Handle = (HWND)m_wxFrame->GetHandle();
	m_wxFrame->SetTitle(_T("wxWidgets canvas frame"));

	// 下のクラスのためにポインタセット
	if ( m_wxWindow )
		m_wxWindow->Destroy();
	m_wxWindow = m_wxFrame;
	m_wxWindow->Show();
	// Give it a status line
	//frame->CreateStatusBar();

	// Make a menubar
	//wxMenu *file_menu = new wxMenu;

	//file_menu->Append(HELLO_NEW, _T("&New MFC Window"));
	//file_menu->Append(HELLO_QUIT, _T("&Close"));

	//wxMenuBar *menu_bar = new wxMenuBar;

	//menu_bar->Append(file_menu, _T("&File"));

	// Associate the menu bar with the frame
	//subframe->SetMenuBar(menu_bar);
}

//----------------------------------------------------------------------------
TCustomForm::~TCustomForm()
{
	// いまだにクローズされていないのだったら、イベント発行しておく
	m_wxWindow->Destroy();
	m_wxWindow = NULL;

	// VCLオブジェクトを潰すときは、デストロイ
	if ( OnDestroy )
		OnDestroy(this);
}


//----------------------------------------------------------------------------
void TCustomForm::Dispatch(void*p)
{
	TMessage* pm = (TMessage*)p;
	pm->Result = 
		dynamic_cast<MyFrameTCustomForm*>(m_wxFrame)->MSWWindowProcForDispatch(pm->Msg, pm->WParam, pm->LParam);
}

//	PROPERTY_VAR1(bool, FreeOnTerminate);
//----------------------------------------------------------------------------
//	PROPERTY(TWindowState, WindowState);
//enum TWindowState { wsNormal=0, wsMinimized=1, wsMaximized=2 };
// 主に、最大化されているか、標準状態にあるか　のみ知りたい　のかな。
void TCustomForm::setWindowState( TWindowState state )
{
	if ( state == wsNormal )
		m_wxWindow->Show();
}
TWindowState TCustomForm::getWindowState( void ) const
{
	if ( m_wxWindow->IsShown() )
		return wsNormal;
	else
		return wsMinimized;
}
//	PROPERTY_VAR0(TBorderStyle, BorderStyle );
//	PROPERTY_VAR0(unsigned int, BorderStyle);
//	PROPERTY_VAR1(TMainMenu*, Menu);
//	PROPERTY_VAR1(TIcon*, Icon);

//----------------------------------------------------------------------------
void TCustomForm::SetFocus()
{
}
//----------------------------------------------------------------------------
HRESULT TCustomForm::SetHWnd(int, HWND)
{
	return S_OK;
}
//----------------------------------------------------------------------------
int TCustomForm::ShowModal()
{
	m_wxWindow->MakeModal();
	return 0;
}
//----------------------------------------------------------------------------
void __fastcall TCustomForm::Close(void)
{
}

//----------------------------------------------------------------------------
bool __fastcall TCustomForm::IsShortCut(Messages::TWMKey &Message)
{
	return false;
}
//----------------------------------------------------------------------------
void TCustomForm::WndProc(TMessage& message)
{
	message.Result = DefWindowProc(Handle, message.Msg, message.WParam, message.LParam);
}
//----------------------------------------------------------------------------
void TCustomForm::SetZOrder(bool)
{
}


//----------------------------------------------------------------------------
// ボーダーのセッタ。先頭小文字のものは、マクロで作る関数
// 先頭大文字のものは、VCLのインターフェース
void TCustomForm::setBorderStyle(TBorderStyle v)
{
	SetBorderStyle(v);
}
void TCustomForm::SetBorderStyle(TFormBorderStyle Value)
{
	m_BorderStyle = Value;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
TForm::TForm(TComponent* owner) : TCustomForm(owner)
{
	// BCB用コードを極力崩さないためには、
	// ここで、サブクラスのメンバを初期化する必要がある。
	// 初期化用関数の配列をグローバルで持てばよいのかのう？

	// typeid演算子をthisに適用すると、TFormを返すので、サブクラスのタイプ同定には使えない
	// dynamic_castも、所詮内部で類似のことをしているのでダメ
}

//----------------------------------------------------------------------------
TForm::~TForm()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
TScrollBox::TScrollBox(TComponent* owner) : TScrollingWinControl(owner)
{
	HorzScrollBar = new TScrollBar(this);
	VertScrollBar = new TScrollBar(this);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
TScrollBar::TScrollBar(TComponent* owner) : TWinControl(owner)
{
}

//----------------------------------------------------------------------------
void TScrollBar::setPosition(int val)
{
}

//----------------------------------------------------------------------------
int TScrollBar::getPosition() const
{
	return 0;
}
