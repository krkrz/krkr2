#include "vcl.h"
#include "Classes.hpp"
#include <map>
#include <stdio.h>

//----------------------------------------------------------------------------
void _ErrMes()
{  //エラーメッセージの表示
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
	    NULL, GetLastError(),
	    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // デフォルト言語
	    (LPTSTR) &lpMsgBuf, 0, NULL);

	char buf[4096];
	sprintf(buf, "Debug.message(\"%s\")", lpMsgBuf);
	OutputDebugString(buf);
	LocalFree( lpMsgBuf );
}


#ifdef _DEBUG
#define ErrMes() _ErrMes()
#else
#define ErrMes() (0)
#endif

//----------------------------------------------------------------------------

static std::map<HWND,TWndMethod> g_mapWndProc;

static LRESULT CALLBACK
WndProc_AllocateHWnd(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Messages::TMessage tMessage = { msg, wparam, lparam, 0 };
	if ( g_mapWndProc.find(hwnd) != g_mapWndProc.end() )
	{
		g_mapWndProc[hwnd](tMessage);
		return tMessage.Result;
	}
	else if ( g_mapWndProc.find(0) != g_mapWndProc.end() )
	{
		// ウィンドウ配列の 0 版は、初期化生成中のウィンドウ。
		// しかるに、デフォルトに任せる
		return DefWindowProc(hwnd, msg, wparam, lparam);
//		g_mapWndProc[0](tMessage);
//		return tMessage.Result;
	}

	return 0;
}

//HWND AllocateHWnd(WNDPROC lpfnWndProc)
HWND AllocateHWnd( TWndMethod Method )
{
	static int nClassID = 0;
	WNDCLASSEX wc = { 0 };
	HINSTANCE hInstance = GetModuleHandle(NULL);

	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc_AllocateHWnd;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;

	// クラス名を作る
	char szClassName[256] = {0};
	sprintf( szClassName, "MyHelloClass%.4x%.4x", hInstance, nClassID );
	nClassID++;
	wc.lpszClassName = szClassName;
	wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

	ATOM res = ::RegisterClassEx(&wc);
	if ( res == NULL )
	{
		return NULL;
	}
	// ウィンドウ生成時に呼ばれるように細工
	// ハンドルが分からないけど、リストには追加しなくちゃ
	g_mapWndProc[0] = Method;

	// ウィンドウ生成
	HWND hWnd = ::CreateWindow(
			wc.lpszClassName,    //ウィンドウクラス名
			"",		          	 //タイトル（任意）
			WS_OVERLAPPEDWINDOW, //ウィンドウの形
			CW_USEDEFAULT,       //水平位置
			CW_USEDEFAULT,       //垂直位置
			CW_USEDEFAULT,       //幅
			CW_USEDEFAULT,       //高さ
			NULL,                //親ウィンドウ 
			NULL,                //ウィンドウメニュー 
			hInstance,           //インスタンスハンドル
			NULL );              //補足情報
	if ( hWnd == NULL )
	{
		ErrMes();
	}

	g_mapWndProc[hWnd] = Method;
	g_mapWndProc.erase(0);

	//MyAddLog(TJS_W("created wnd: hHandle=%6x, szClassName=[%s]"), hWnd, ttstr(szClassName).c_str());
	return hWnd;
}
//----------------------------------------------------------------------------
void DeallocateHWnd( HWND hWnd )
{
	DestroyWindow(hWnd);
	g_mapWndProc.erase(hWnd);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//class TStrings : public System::TObject
//{
//private:
//	std::vector<AnsiString*> m_Strings;
//public:
//----------------------------------------------------------------------------
//	PROPERTY(AnsiString, Text);
void TStrings::setText(AnsiString str)
{
	m_Strings.clear();

	char * p0 = str.c_str();
	char * p1;
	char ch;

	while( *p0 != '\0' )
	{
		for(p1=p0; *p1 != '\0' && *p1 != '\r' && *p1 != '\n'; p1++) p1++;
		ch = *p1;
		m_Strings.push_back( AnsiString(p0) );
		if ( ch == '\0' )
			return;
		else if ( ch == '\r' && *(p1+1) == '\n' )
			p0 = p1 + 2;
		else
			p0 = p1 + 1;
	}
}
AnsiString TStrings::getText() const
{
	AnsiString str;
	if ( m_Strings.size() > 0 )
	{
		str = m_Strings[0];
		for(int i=1; i < m_Strings.size(); i++ )
		{
			str += "\r\n" + m_Strings[i];
		}
	}
	return str;
}


//----------------------------------------------------------------------------
//	PROPERTY_ARY(AnsiString, Strings);
void TStrings::setStrings(int Index, AnsiString s )
{
	m_Strings[Index] = s;
}
AnsiString TStrings::getStrings(int Index) const
{
	if ( Index < m_Strings.size() )
		return m_Strings[Index];
	return NULL;
}

//----------------------------------------------------------------------------
//	PROPERTY(int, Count);
void TStrings::setCount(int val )
{
	m_Strings.resize(val);
}

int TStrings::getCount( ) const
{
	return m_Strings.size();
}


//----------------------------------------------------------------------------
// 追加した位置を示すインデックス を返す
int TStrings::Add( const AnsiString& s )
{
	m_Strings.push_back(s);
	return m_Strings.size() - 1;
}

//----------------------------------------------------------------------------
void TStrings::Assign( TObject* Source )  // コピー元の TStringList オブジェクト
{
	TStrings * p = dynamic_cast<TStrings*>(Source);
	if ( p != NULL )
	{
		int n = p->m_Strings.size();
		m_Strings.resize( n );
		for(int i=0; i < n; i++)
			m_Strings[i] = p->m_Strings[i];
	}
}

//----------------------------------------------------------------------------
void TStrings::Delete( int Index )
{
	m_Strings.erase( m_Strings.begin() + Index );
}

//----------------------------------------------------------------------------
//リスト内を文字列検索する
// 成功しなかったとき -1
// なお、このメソッドは内部的には AnsiCompareText 関数を使う #AnsiCompareIC でもいいのかな？
int TStrings::IndexOf( const AnsiString& s )
{
	int j;
	std::vector<AnsiString>::iterator i = m_Strings.begin();
	for(i=m_Strings.begin(),j=0; i != m_Strings.end(); i++,j++)
	{
		if ( i->AnsiCompareIC(s) == 0 )
			return j;
	}
	return -1;
}

//----------------------------------------------------------------------------
void TStrings::Move( int CurIndex, int NewIndex )
{
	if ( CurIndex < m_Strings.size() && NewIndex < m_Strings.size() &&
		CurIndex >= 0 && NewIndex >= 0 && CurIndex != NewIndex )
	{
		AnsiString tmp = m_Strings[CurIndex];
		m_Strings[CurIndex] = m_Strings[NewIndex];
		m_Strings[NewIndex] = tmp;
	}
}

//----------------------------------------------------------------------------
void TStrings::SaveToFile(const AnsiString & FileName)
{
	FILE * fp = fopen( FileName.c_str(), "w" );
	if ( fp != NULL )
	{
		std::vector<AnsiString>::iterator i = m_Strings.begin();
		for(i=m_Strings.begin(); i != m_Strings.end(); i++)
			fprintf(fp, "%s\n", i->c_str() );
		fclose(fp);
	}
	
}

//----------------------------------------------------------------------------
void TStrings::BeginUpdate() // リストビューの更新を抑制する, 再描画を行うときは、 EndUpdate メソッドを実行する
{
}
//----------------------------------------------------------------------------
void TStrings::EndUpdate()
{
}

//----------------------------------------------------------------------------
void TStrings::Clear()
{
	m_Strings.clear();
}
//----------------------------------------------------------------------------
void TStrings::LoadFromFile(const AnsiString & FileName)
{
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//class TStringList : public TStrings
//	PROPERTY_ARY(AnsiString,Names);
//リスト内の「名前=値」形式の文字列の名前の部分を返す
AnsiString TStringList::getNames(int index) const
{
	if ( index >= 0 && index < Values.size())
	{
		//std::map<AnsiString,AnsiString> Values;
		std::map<AnsiString,AnsiString>::const_iterator i = Values.begin();
		for(int j=0; j < index; j++)
			i ++;
		return i->first;
	}
	else
		return AnsiString();
}
