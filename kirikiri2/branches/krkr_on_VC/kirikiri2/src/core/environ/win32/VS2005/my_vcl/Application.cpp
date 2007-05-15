#include "vcl.h"
#include "Application.h"

#include "tjsCommHead.h"
#include <memory>

HINSTANCE HInstance = NULL;
TApplication* Application;

// main関数なしに強引に何かをさせるトリック
static __THogeInstance<TApplication, &Application> __TApplicationInstance;

#include <MainFormUnit.h>

//#define USE_DGB_FORM

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// wxWidgetsのAPPを作る
// Define a new application type
class MyApp : public wxApp
{
protected:
	std::auto_ptr<wxFrame> m_autoFrame;
public:
    virtual bool OnInit();

    // we need to override this as the default behaviour only works when we're
    // running wxWidgets main loop, not MFC one
//    virtual void ExitMainLoop();
    wxFrame* CreateFrame();
	void OnIdle(wxIdleEvent& event);

	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(MyApp, wxApp)
	EVT_IDLE(MyApp::OnIdle)
END_EVENT_TABLE()

IMPLEMENT_APP_NO_MAIN(MyApp)


//----------------------------------------------------------------------------
bool MyApp::OnInit()
{
//	SetExitOnFrameDelete(false);

#ifdef USE_DBG_FORM
	m_autoFrame.reset( CreateFrame() );
#endif

    return true;
}

//----------------------------------------------------------------------------
void MyApp::OnIdle(wxIdleEvent& event)
{
	bool Done;  //（戻り値）ほかのアプリケーションに制御を移すとき true
	Application->OnIdle(Application, Done);
}

//----------------------------------------------------------------------------
#define HELLO_QUIT 1
#define HELLO_NEW  2
wxFrame *MyApp::CreateFrame()
{
	wxFrame *subframe = new wxFrame(NULL, -1, _T("Canvas Frame"), wxPoint(10, 10), wxSize(300, 300),
        wxDEFAULT_FRAME_STYLE);

	subframe->SetTitle(_T("wxWidgets canvas frame"));

	// Give it a status line
	subframe->CreateStatusBar();

	// Make a menubar
	wxMenu *file_menu = new wxMenu;

	file_menu->Append(HELLO_NEW, _T("&New MFC Window"));
	file_menu->Append(HELLO_QUIT, _T("&Close"));

	wxMenuBar *menu_bar = new wxMenuBar;

	menu_bar->Append(file_menu, _T("&File"));

	// Associate the menu bar with the frame
	subframe->SetMenuBar(menu_bar);
    subframe->Show(true);

	// Return the main frame window
	return subframe;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
TApplication::TApplication( TComponent* Owner ) : TComponent(Owner)
{
	Handle = NULL;
}

//----------------------------------------------------------------------------
TApplication::~TApplication()
{
	wxEntryCleanup();
}

//----------------------------------------------------------------------------
// OLE オートメーションへのポインタを初期化する
// OLE オートメーションを使わないアプリケーションでは何もしない。
void TApplication::Initialize(void)
{
	// wxEntryStart内で OleInitialize が呼ばれる際にエラーがでる。
	// ココより以前にCOMが初期化されちゃってるらしい。ダメっぽい
	// ので、強引に、COMをリリース
	try {
		OleUninitialize();// とりあえず、やっつけてみる
		CoUninitialize(); // とにかくやっつける
	} catch(...)
	{
		OutputDebugString("例外でたけど無視します...\n");
	}

	// mfctestサンプルを見ると IMPLEMENT_APP_NO_MAIN(MyApp) を使うやり方が載っている
//	WXDLLIMPEXP_BASE void wxSetInstance(HINSTANCE hInst);
//	wxSetInstance(wxApp::m_hInstance);

    wxApp::m_nCmdShow = TRUE;
	
	int argc = 0;
    wxChar **argv = NULL;
	::wxEntryStart(argc, argv);
    if ( !wxTheApp || !wxTheApp->CallOnInit() )
        throw;
}

//----------------------------------------------------------------------------
void TApplication::Terminate( void )
{
	wxTheApp->ExitMainLoop();
}

//----------------------------------------------------------------------------
// 発生した例外のメッセージを表示する
void TApplication::ShowException(Exception* e)
{
	::MessageBox( NULL, e->Message.c_str(), Title.c_str(), MB_ICONERROR );
}

//----------------------------------------------------------------------------
// 超嘘実装。CreateFormは、特定の条件でしか使用されません
void TApplication::CreateForm( const type_info * type, void *)
{
	// Application->CreateForm(__classid(TTVPMainForm), &TVPMainForm);
	// のみ。
	// しからば、以下のようにして済ます。ごめん
	if ( *type == typeid(TTVPMainForm) )
		TVPMainForm = new TTVPMainForm(this);

	// ハンドル変数が未初期化のときにこの関数呼ばれたなら、コレって、アレだよね
	if ( Handle == NULL )
		Handle = TVPMainForm->Handle;
}

//----------------------------------------------------------------------------
void TApplication::Run(void)
{
//#ifdef USE_DBG_FORM
	::wxWakeUpIdle();
	wxTheApp->OnRun();
//#endif
}

//----------------------------------------------------------------------------
void TApplication::ProcessMessages()
{
}

//----------------------------------------------------------------------------
void TApplication::HandleMessage()
{
}

//----------------------------------------------------------------------------
void TApplication::BringToFront()
{
}

//----------------------------------------------------------------------------
void TApplication::Minimize()
{
}

//----------------------------------------------------------------------------
void TApplication::Restore()
{
}

//----------------------------------------------------------------------------
// メッセージボックスを表示する
// MessageBox メソッドは、Windows API 関数の MessageBox 関数をカプセル化したものである。
int TApplication::MessageBox(const char* Text, char* Caption, int Flags)
{
	return ::MessageBox(NULL, Text, Caption, Flags);
}

//----------------------------------------------------------------------------
int TApplication::HintMouseMessage(TControl*, TMessage)
{
	return 0;
}

//----------------------------------------------------------------------------
void TApplication::CancelHint()
{
}

