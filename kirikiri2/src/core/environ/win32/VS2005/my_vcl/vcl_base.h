#pragma once

//----------------------------------------------------------------------------
// 最初に warning消し
#pragma warning(disable: 4800)
#pragma warning(disable: 4996)

//warning C4561: '__fastcall' は '/clr' スイッチとの互換性がありません : '__stdcall' に変換しています。
// __fastcall は無しに。
#pragma warning(disable: 4561)

// 本来有用なワーニング君
//warning C4244: '=' : 'TJS::tjs_int64' から 'TJS::tjs_uint' への変換です。データが失われる可能性があります。
#pragma warning(disable: 4244)
//warning C4018: '<=' : signed と unsigned の数値を比較しようとしました。
#pragma warning(disable: 4018)
//warning C4333: '>>' : 右シフトの回数が多すぎます。データが失われる可能性があります
#pragma warning(disable: 4333)
//warning C4793: 'vararg' : 関数 'bool boost::detail::function::has_empty_target(...)' 用にネイティブ コードの生成が発生します
#pragma warning(disable: 4793)

// 表示不可能文字ワーニングを消す。ツンッ
#pragma warning(disable: 4819)


//----------------------------------------------------------------------------
// ヘッダインクルード
#include <wx/wxprec.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <vector>
#include <map>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <wx/wfstream.h>

#include <windows.h>

//----------------------------------------------------------------------------
// 呼び出し規約系
#define DYNAMIC virtual

// マネージコードとアンマネージコードのリンクで問題が起きるので、__fastcall は殺します
#define __fastcall

#define __published public
#define __closure
#define _export

#define PACKAGE

#define _stdcall(x) (_stdcall x)
#define __stdcall(x) (__stdcall x)
#define __cdecl(x) (__cdecl x)

#ifndef _USERENTRY
#define _USERENTRY __cdecl
#endif

//----------------------------------------------------------------------------
// ヘッダインクルード(自作分)
#include "vcl_enums.h"
#include "AnsiString.h"
#include "systobj.h"
#include "SysUtils.hpp"

//----------------------------------------------------------------------------
// 次に #define と typedef
// 定義のみ
#define PROPERTY(type,Name) public: virtual type get##Name() const; virtual void set##Name(type); \
	__declspec( property(get=get##Name, put=set##Name) ) type Name;

// 読出し専用
#define PROPERTY_R(type,Name) public: virtual type get##Name() const; \
	__declspec( property(get=get##Name) ) type Name;

// 定義のみ(配列)
#define PROPERTY_ARY(type,Name) public: virtual type get##Name(int) const; virtual void set##Name(int,type); \
	__declspec( property(get=get##Name, put=set##Name) ) type Name[];

// 定義のみ(配列) 読出し専用
#define PROPERTY_ARY_R(type,Name) public: virtual type get##Name(int) const; \
	__declspec( property(get=get##Name) ) type Name[];

// 定義のみ(純仮想関数)
#define PROPERTY_abstract(type,Name) public: virtual type get##Name() const =0; virtual void set##Name(type)=0; \
	__declspec( property(get=get##Name, put=set##Name) ) type Name;

// 変数とゲッタセッタの実装まで
#define PROPERTY_VAR0(type,Name) public: virtual type get##Name() const {return m_##Name;} virtual void set##Name(type _v){m_##Name=_v;} \
	protected: type m_##Name; public: __declspec( property(get=get##Name, put=set##Name) ) type Name;

// 変数とゲッタのみ実装まで
#define PROPERTY_VAR1(type,Name) public: virtual type get##Name() const {return m_##Name;} virtual void set##Name(type _v); \
	protected: type m_##Name; public: __declspec( property(get=get##Name, put=set##Name) ) type Name;

// 変数とゲッタのみ実装まで, セッタは定義自体無し
#define PROPERTY_VAR_R(type,Name) public: virtual type get##Name() const {return m_##Name;} \
	protected: type m_##Name; public: __declspec( property(get=get##Name) ) type Name;

// 特定の変数を対象にしたい
#define PROPERTY_VAR_sp(type,Name,Var) public: virtual type get##Name() const {return Var;} virtual void set##Name(type _v){Var=_v;} \
	__declspec( property(get=get##Name, put=set##Name) ) type Name;

//----------------------------------------------------------------------------
// main関数なしに強引に何かをさせるトリック1
template <class T, T** p>
class __THogeInstance
{
public:
	__THogeInstance()
	{
		*p = new T;
	}
	virtual ~__THogeInstance()
	{
		delete *p;
	}
};

// main関数なしに強引に何かをさせるトリック2
#define CALL_ON_STARTUP(x,y)     \
class x##StartUp         \
{                        \
public:                  \
	x##StartUp() { x(); }\
} x##StartUpInstance;

//----------------------------------------------------------------------------
// なんかこの手のクラスがVCLに多い
template <class T, int VMIN, int VMAX>
class Set
{
	bool m_Val[VMAX-VMIN+1];
public:
	typedef Set<T,VMIN,VMAX> _Type;
	Set() { memset(m_Val, 0, sizeof(bool) * (VMAX-VMIN+1) ); }
	Set( const _Type& s) { memcpy(m_Val, s.m_Val, sizeof(bool) * (VMAX-VMIN+1) ); }
	bool Contains(int V) { if ( V >= VMIN && V <= VMAX ) return m_Val[V-VMIN]; return false;  }
	_Type& operator<< (int V) { if ( V >= VMIN && V <= VMAX ) m_Val[V-VMIN] = true;  return *this; }
	_Type& operator>> (int V) { if ( V >= VMIN && V <= VMAX ) m_Val[V-VMIN] = false; return *this; }
	bool operator==(const _Type& V)
	{
		for(int i=0; i < VMAX-VMIN; i++)
			if ( m_Val[i] != V.m_Val[i] ) return false;
		return true;
	}
};

//----------------------------------------------------------------------------
// VCLではこの型定義があるらしい。VCには無い
typedef unsigned int dliNotification;
// 以下も BCB固有っぽ
#define MAXDRIVE MAX_PATH
#define MAXDIR MAX_PATH

#define _argv __argv
#define _argc __argc

#define __emit__(x,y,z) __asm _emit x; __asm _emit y; __asm _emit z;
//__pfnDliNotifyHook
//static FARPROC WINAPI DllLoadHook(dliNotification dliNotify,  DelayLoadInfo * pdli)
//dliNotification

//----------------------------------------------------------------------------
#define EVENT_FUNC1(x, y) boost::bind(boost::mem_fn(&x::y), this, _1)
#define EVENT_FUNC2(x, y) boost::bind(boost::mem_fn(&x::y), this, _1, _2)
#define EVENT_FUNC3(x, y) boost::bind(boost::mem_fn(&x::y), this, _1, _2, _3)
#define EVENT_FUNC4(x, y) boost::bind(boost::mem_fn(&x::y), this, _1, _2, _3, _4)
#define EVENT_FUNC5(x, y) boost::bind(boost::mem_fn(&x::y), this, _1, _2, _3, _4, _5)

//----------------------------------------------------------------------------
typedef Set<TFontStyle, fsBold, fsStrikeOut>  TFontStyles;
typedef Set<int,mbYes,mbHelp> TMsgDlgButtons;


//----------------------------------------------------------------------------
struct TCreateParams
{
  char*        Caption;          // ウインドウのキャプション
  int          Style;            // 同、スタイル（WS_xxx）
  int          ExStyle;          // 同、拡張スタイル（WS_EX_xxx）
  int          X;                // 同、左端の X 座標
  int          Y;                // 同、上端の Y 座標
  int          Width;            // 同、幅
  int          Height;           // 同、高さ
  HWND         WndParent;        // 親ウインドウのウインドウハンドル
  void*        Param;            // WM_CREATE メッセージの LParam として渡されるポインタ
  tagWNDCLASSA WindowClass;      // WNDCLASSEX 構造体
  char         WinClassName[64]; // ウインドウのクラス名
};

//----------------------------------------------------------------------------
struct TSearchRec
{
  int               Time;        // ファイルのタイムスタンプ
  int               Size;        // ファイルのサイズ（単位：バイト）
  int               Attr;        // ファイル属性（FileGetAttr 関数を参照）
  AnsiString        Name;        // ファイル名
  int               ExcludeAttr; // 含まないファイル属性
  int               FindHandle;  // 検索ハンドル
  _WIN32_FIND_DATAA FindData;    // WIN32_FIND_DATA 構造体
};

//----------------------------------------------------------------------------
class TPoint : public POINT
{
public:
	TPoint() { x = y = 0; }
	TPoint(int _x,int _y) { x = _x; y = _y; }
	TPoint(const POINT& pt) { x = pt.x; y = pt.y; }
};
//----------------------------------------------------------------------------
class TRect : public RECT
{
public:
	TRect() {}

	TRect(const TPoint& TL, const TPoint& BR)
	{
		left   = TL.x;
		top    = TL.y;
		right  = BR.x;
		bottom = BR.y;
	}

	TRect(int l, int t, int r, int b)
	{
		left   = l;
		top    = t;
		right  = r;
		bottom = b;
	}

	TRect(RECT& r)
	{
		left   = r.left;
		top    = r.top;
		right  = r.right;
		bottom = r.bottom;
	}

	int Width () const { return right  - left; }
	int Height() const { return bottom - top ; }

	bool operator ==(const TRect& rc) const
	{
		return left ==  rc.left  && top == rc.top &&
			right == rc.right && bottom == rc.bottom;
	}

	bool operator !=(const TRect& rc) const 
	{  return !(rc==*this); }

	PROPERTY_VAR_sp(LONG,Left  , left  );
	PROPERTY_VAR_sp(LONG,Top   , top   );
	PROPERTY_VAR_sp(LONG,Right , right );
	PROPERTY_VAR_sp(LONG,Bottom, bottom);
};


extern PACKAGE AnsiString __fastcall ParamStr(int Index);
extern AnsiString Trim(const AnsiString&);

extern int FindFirst(const AnsiString & Path, int Attr, TSearchRec & F );
extern int FindNext(TSearchRec & F);
extern void FindClose(TSearchRec & F);

extern HINSTANCE HInstance;

// min, maxが、std::以下にいるべきらしい。
// マクロは潰して、inline関数版に置き換える
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

namespace std
{
	template <class T1,class T2> inline T1 min(T2 a, T2 b) { return (a>b)?b:a; }
	template <class T1,class T2> inline T1 max(T2 a, T2 b) { return (a>b)?a:b; }
};

// その他、関数名を割り当てるマクロ置換
#define fnsplit _splitpath
#define strcasecmp _stricmp
#define strncasecmp _strnicmp

