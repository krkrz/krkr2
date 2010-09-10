//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <windows.h>
#include <list>
#include "tp_stub.h"

//---------------------------------------------------------------------------

// モニター個々のパラメータ保持
typedef struct _MONIAREA
{
	int nNo;			// モニター番号
	long lAreaSize;		// モニター上を占めるウィンドウのサイズ
	RECT rcMoniRect;	// モニターの矩形
}MONIAREA;

// モニター列挙コールバック関数への引数
typedef struct _DATA
{
	int	nMonitorCount;
	RECT rcWndRect;
	std::list<MONIAREA> listMoniArea;
}DATA;

// モニター列挙コールバック関数
static BOOL CALLBACK MonitorEnumProc( HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData )
{
	wchar_t	sStr[MAX_PATH];

	DATA* ptData = (DATA*)dwData;

	wsprintf(sStr, L"%d番目のモニタの位置 : left:%d top:%d  right:%d bottom:%d", ptData->nMonitorCount, lprcMonitor->left, lprcMonitor->top, lprcMonitor->right, lprcMonitor->bottom );
//	::MessageBox(NULL, sStr, L"Info", MB_OK);
	
	RECT rcWndRect = ptData->rcWndRect;
	RECT rcMoniRect = *lprcMonitor;

	// 最初に最長長さを保持しておく
	int nLenWidth = rcWndRect.right - rcWndRect.left; 
	
	{// 横
	
		// 左座標が矩形より左外にある
		if( rcWndRect.left < rcMoniRect.left )
			nLenWidth -= ( rcMoniRect.left - rcWndRect.left );
		// 右座標が矩形より左外にある
		if( rcWndRect.right > rcMoniRect.right )
			nLenWidth -= ( rcWndRect.right - rcMoniRect.right );
		
		if( nLenWidth < 0 )
			nLenWidth = 0;
	}
	
	// 最初に最長長さを保持しておく
	int nLenHeight = rcWndRect.bottom - rcWndRect.top;
	
	{// 縦
	
		// 上座標が矩形より右外にある
		if( rcWndRect.top < rcMoniRect.top )
			nLenHeight -= ( rcMoniRect.top - rcWndRect.top );
		// 下座標が矩形より右外にある
		if( rcWndRect.bottom > rcMoniRect.bottom )
			nLenHeight -= ( rcWndRect.bottom - rcMoniRect.bottom );

		if( nLenHeight < 0 )
			nLenHeight = 0;
	}

	MONIAREA aMoniArea;
	
	aMoniArea.nNo = ptData->nMonitorCount;
	aMoniArea.rcMoniRect = rcMoniRect;
	aMoniArea.lAreaSize = nLenWidth*nLenHeight;

	// ウィンドウの矩形保持
	ptData->listMoniArea.push_back( aMoniArea );
	
	wsprintf( sStr, L"%d番目のモニタにある面積 : %d", ptData->nMonitorCount, aMoniArea.lAreaSize );
//	::MessageBox(NULL, sStr, L"Info", MB_OK);

	ptData->nMonitorCount++;

	return TRUE;	// TRUEは探索継続，FALSEで終了
}

// listのsortコールバック
bool LessHeight(const MONIAREA& rLeft, const MONIAREA& rRight) { return rLeft.lAreaSize > rRight.lAreaSize; }


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class tAdjustMoniFunction : public tTJSDispatch
{
	// 吉里吉里で利用可能な関数を簡単に作成するには、
	// tTJSDispatch を継承させ、FuncCall を実装する。

	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);
} * AdjustMoniFunction;
	// テスト関数を保持
	// iTJSDispatch2 の派生物 ( tTJSDispatch や tAdjustMoniFunction もそう) はなる
	// べくヒープに確保 ( つまり、new で確保する ) ようにすべき。
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD tAdjustMoniFunction::FuncCall(
	tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
	tTJSVariant *result,
	tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
{
	// flag       : 呼び出しフラグ
	//              無視してかまわない
	// membername : メンバ名
	//              無視してかまわないが、一応
	if(membername) return TJS_E_MEMBERNOTFOUND;
	//              と記述しておくと良い
	// hint       : メンバ名ヒント
	//              本来はmembernameのハッシュ値だが、無視してかまわない
	// result     : 結果格納先
	//              NULL の場合は結果が必要ない時
	// numparams  : パラメータの数
	// param      : パラメータ
	// objthis    : コンテキスト
	//              クラスメソッドではないので無視してかまわない

	if(numparams == 0) return TJS_E_BADPARAMCOUNT;
		// この場合は引数が 0 であってはならないのでこのようにする


	wchar_t			sStr[MAX_PATH];
	iTJSDispatch2*	elm	= param[0]->AsObjectNoAddRef();
	tTJSVariant		val;
	RECT			rcWndRect;
	RECT			rcWndRect2;
	bool			bResRect=true;	// 戻り値はRECT辞書配列

	// HWNDで指定されている場合は、ウィンドウハンドルから矩形を得る
	if( elm->IsValid(0, L"window", NULL, elm) == TJS_S_TRUE )
	{
	
		// 辞書から値を取り出す
		TJS_SUCCEEDED( elm->PropGet(0, L"window", NULL, &val, elm) );
		
		tTJSVariant		valHWND;
		iTJSDispatch2*	tmp = val.AsObjectNoAddRef();
		TJS_SUCCEEDED( tmp->PropGet(0, L"HWND", NULL, &valHWND, tmp) );
		
		HWND hWnd = (HWND)(tjs_int)valHWND;
		::GetWindowRect( hWnd, &rcWndRect );
		
	}
	else
	// RECT指定の場合は、そのRECTから矩形を得る
	if( elm->IsValid(0, L"left", NULL, elm) == TJS_S_TRUE )
	{

		// 辞書から値を取り出す
		TJS_SUCCEEDED( elm->PropGet(0, L"left", NULL, &val, elm) );
		rcWndRect.left = (long)(tjs_int)val;
		
		// 辞書から値を取り出す
		TJS_SUCCEEDED( elm->PropGet(0, L"top", NULL, &val, elm) );
		rcWndRect.top = (long)(tjs_int)val;

		// right,bottom表記の場合
		if( elm->IsValid(0, L"right", NULL, elm) == TJS_S_TRUE )
		{
			
			// 辞書から値を取り出す
			TJS_SUCCEEDED( elm->PropGet(0, L"right", NULL, &val, elm) );
			rcWndRect.right = (long)(tjs_int)val;

			// 辞書から値を取り出す
			TJS_SUCCEEDED( elm->PropGet(0, L"bottom", NULL, &val, elm) );
			rcWndRect.bottom = (long)(tjs_int)val;
		}
		else
		// width,height表記の場合
		if( elm->IsValid(0, L"width", NULL, elm) == TJS_S_TRUE )
		{
		
			// 辞書から値を取り出す
			TJS_SUCCEEDED( elm->PropGet(0, L"width", NULL, &val, elm) );
			rcWndRect.right = rcWndRect.left + (long)(tjs_int)val;

			// 辞書から値を取り出す
			TJS_SUCCEEDED( elm->PropGet(0, L"height", NULL, &val, elm) );
			rcWndRect.bottom = rcWndRect.top + (long)(tjs_int)val;
		
		}
		
	}
	
	{// 調整したい矩形もある場合はこちらから
	
		// 調整したいHWNDがある場合は、その矩形も得る。戻り値は調整済み位置となる
		if( elm->IsValid(0, L"window2", NULL, elm) == TJS_S_TRUE )
		{

			// 辞書から値を取り出す
			TJS_SUCCEEDED( elm->PropGet(0, L"window2", NULL, &val, elm) );
			
			tTJSVariant		valHWND;
			iTJSDispatch2*	tmp = val.AsObjectNoAddRef();
			TJS_SUCCEEDED( tmp->PropGet(0, L"HWND", NULL, &valHWND, tmp) );
			
			HWND hWnd = (HWND)(tjs_int)valHWND;
			::GetWindowRect( hWnd, &rcWndRect2 );
		
			bResRect = false;	// 戻り値はPOS辞書配列
		}
		else
		// 調整したいRECTがある場合は、その矩形も得る。戻り値は調整済み位置となる
		if( elm->IsValid(0, L"left2", NULL, elm) == TJS_S_TRUE )
		{
			// 辞書から値を取り出す
			TJS_SUCCEEDED( elm->PropGet(0, L"left2", NULL, &val, elm) );
			rcWndRect2.left = (long)(tjs_int)val;
			
			// 辞書から値を取り出す
			TJS_SUCCEEDED( elm->PropGet(0, L"top2", NULL, &val, elm) );
			rcWndRect2.top = (long)(tjs_int)val;

			// right,bottom表記の場合
			if( elm->IsValid(0, L"right2", NULL, elm) == TJS_S_TRUE )
			{
				// 辞書から値を取り出す
				TJS_SUCCEEDED( elm->PropGet(0, L"right2", NULL, &val, elm) );
				rcWndRect2.right = (long)(tjs_int)val;

				// 辞書から値を取り出す
				TJS_SUCCEEDED( elm->PropGet(0, L"bottom2", NULL, &val, elm) );
				rcWndRect2.bottom = (long)(tjs_int)val;
			}
			else
			// width,height表記の場合
			if( elm->IsValid(0, L"width2", NULL, elm) == TJS_S_TRUE )
			{
				// 辞書から値を取り出す
				TJS_SUCCEEDED( elm->PropGet(0, L"width2", NULL, &val, elm) );
				rcWndRect2.right = rcWndRect2.left + (long)(tjs_int)val;

				// 辞書から値を取り出す
				TJS_SUCCEEDED( elm->PropGet(0, L"height2", NULL, &val, elm) );
				rcWndRect2.bottom = rcWndRect2.top + (long)(tjs_int)val;
			}

			bResRect = false;	// 戻り値はPOS辞書配列
		}

	}

	wsprintf(sStr, L"検査するウィンドウの矩形 left:%d top:%d  right:%d bottom:%d", rcWndRect.left, rcWndRect.top, rcWndRect.right, rcWndRect.bottom );
//	::MessageBox(NULL, sStr, L"Info", MB_OK);

	DATA data;

	data.nMonitorCount = 0;
	data.rcWndRect = rcWndRect;
	
	// モニタの列挙とウィンドウが占める面積の計算
	EnumDisplayMonitors( NULL, NULL, MonitorEnumProc, (LPARAM)&data );
	
	// 各モニタを占めるウィンドウの矩形
	data.listMoniArea.sort( LessHeight );

	
	std::list<MONIAREA>::iterator itBegin = data.listMoniArea.begin();
	wsprintf(sStr, L"%d番目のモニタ上にあります。\n矩形 left:%d top:%d  right:%d bottom:%d", (*itBegin).nNo, (*itBegin).rcMoniRect.left, (*itBegin).rcMoniRect.top, (*itBegin).rcMoniRect.right, (*itBegin).rcMoniRect.bottom );
//		::MessageBox(NULL, sStr, L"Info", MB_OK);

	RECT rcMoniRect = (*itBegin).rcMoniRect;

	// TJSの配列オブジェクト作成
	iTJSDispatch2* tjsArray = TJSCreateArrayObject();
	
	// 戻り値がRECT辞書配列の場合
	if( bResRect == true )
	{
		tTJSVariant var = (tjs_int)rcMoniRect.left;

		tjsArray->PropSet(
			TJS_MEMBERENSURE, // メンバがなかった場合には作成するようにするフラグ
			TJS_W("left"), // メンバ名 ( かならず TJS_W( ) で囲む )
			NULL, // ヒント ( 本来はメンバ名のハッシュ値だが、NULL でもよい )
			&var, // 登録する値
			tjsArray // コンテキスト ( global でよい )
			);

		var = (tjs_int)rcMoniRect.top;

		tjsArray->PropSet(
			TJS_MEMBERENSURE, // メンバがなかった場合には作成するようにするフラグ
			TJS_W("top"), // メンバ名 ( かならず TJS_W( ) で囲む )
			NULL, // ヒント ( 本来はメンバ名のハッシュ値だが、NULL でもよい )
			&var, // 登録する値
			tjsArray // コンテキスト ( global でよい )
			);

		var = (tjs_int)rcMoniRect.right;

		tjsArray->PropSet(
			TJS_MEMBERENSURE, // メンバがなかった場合には作成するようにするフラグ
			TJS_W("right"), // メンバ名 ( かならず TJS_W( ) で囲む )
			NULL, // ヒント ( 本来はメンバ名のハッシュ値だが、NULL でもよい )
			&var, // 登録する値
			tjsArray // コンテキスト ( global でよい )
			);

		var = (tjs_int)rcMoniRect.bottom;

		tjsArray->PropSet(
			TJS_MEMBERENSURE, // メンバがなかった場合には作成するようにするフラグ
			TJS_W("bottom"), // メンバ名 ( かならず TJS_W( ) で囲む )
			NULL, // ヒント ( 本来はメンバ名のハッシュ値だが、NULL でもよい )
			&var, // 登録する値
			tjsArray // コンテキスト ( global でよい )
			);
	}
	else
	// 戻り値がPOS辞書配列の場合
	{
		POINT ptAdjustPos = { rcWndRect2.left, rcWndRect2.top };
		if(rcWndRect2.left < rcMoniRect.left)		ptAdjustPos.x = rcMoniRect.left;
		if(rcWndRect2.top < rcMoniRect.top)			ptAdjustPos.y = rcMoniRect.top;
		if(rcWndRect2.right > rcMoniRect.right)		ptAdjustPos.x = rcMoniRect.right - (rcWndRect2.right - rcWndRect2.left);
		if(rcWndRect2.bottom > rcMoniRect.bottom)	ptAdjustPos.y = rcMoniRect.bottom - (rcWndRect2.bottom - rcWndRect2.top);

		tTJSVariant var = (tjs_int)ptAdjustPos.x;

		tjsArray->PropSet(
			TJS_MEMBERENSURE, // メンバがなかった場合には作成するようにするフラグ
			TJS_W("x"), // メンバ名 ( かならず TJS_W( ) で囲む )
			NULL, // ヒント ( 本来はメンバ名のハッシュ値だが、NULL でもよい )
			&var, // 登録する値
			tjsArray // コンテキスト ( global でよい )
			);

		var = (tjs_int)ptAdjustPos.y;

		tjsArray->PropSet(
			TJS_MEMBERENSURE, // メンバがなかった場合には作成するようにするフラグ
			TJS_W("y"), // メンバ名 ( かならず TJS_W( ) で囲む )
			NULL, // ヒント ( 本来はメンバ名のハッシュ値だが、NULL でもよい )
			&var, // 登録する値
			tjsArray // コンテキスト ( global でよい )
			);
	}

	// 戻り値に設定
	*result = tTJSVariant(tjsArray,tjsArray);

	tjsArray->Release();

	return TJS_S_OK;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	return 1;
}
//---------------------------------------------------------------------------
static tjs_int GlobalRefCountAtInit = 0;
extern "C" __declspec(dllexport) HRESULT __stdcall V2Link(iTVPFunctionExporter *exporter)
{
	// スタブの初期化(必ず記述する)
	TVPInitImportStub(exporter);

	// AdjustMoniFunction の作成と登録
	tTJSVariant val;

	// TJS のグローバルオブジェクトを取得する
	iTJSDispatch2 * global = TVPGetScriptDispatch();

	//-----------------------------------------------------------------------
	// 1 まずオブジェクトを作成
	AdjustMoniFunction = new tAdjustMoniFunction();

	// 2 AdjustMoniFunction を tTJSVariant 型に変換
	val = tTJSVariant(AdjustMoniFunction);

	// 3 すでに val が AdjustMoniFunction を保持しているので、AdjustMoniFunction は
	//   Release する
	AdjustMoniFunction->Release();


	// 4 global の PropSet メソッドを用い、オブジェクトを登録する
	global->PropSet(
		TJS_MEMBERENSURE, // メンバがなかった場合には作成するようにするフラグ
		TJS_W("AdjustMoni"), // メンバ名 ( かならず TJS_W( ) で囲む )
		NULL, // ヒント ( 本来はメンバ名のハッシュ値だが、NULL でもよい )
		&val, // 登録する値
		global // コンテキスト ( global でよい )
		);
	//-----------------------------------------------------------------------

	// - global を Release する
	global->Release();

	// もし、登録する関数が複数ある場合は 1 ～ 4 を繰り返す


	// val をクリアする。
	// これは必ず行う。そうしないと val が保持しているオブジェクト
	// が Release されず、次に使う TVPPluginGlobalRefCount が正確にならない。
	val.Clear();


	// この時点での TVPPluginGlobalRefCount の値を
	GlobalRefCountAtInit = TVPPluginGlobalRefCount;
	// として控えておく。TVPPluginGlobalRefCount はこのプラグイン内で
	// 管理されている tTJSDispatch 派生オブジェクトの参照カウンタの総計で、
	// 解放時にはこれと同じか、これよりも少なくなってないとならない。
	// そうなってなければ、どこか別のところで関数などが参照されていて、
	// プラグインは解放できないと言うことになる。

	return S_OK;
}
//---------------------------------------------------------------------------
extern "C" __declspec(dllexport) HRESULT __stdcall V2Unlink()
{
	// 吉里吉里側から、プラグインを解放しようとするときに呼ばれる関数。

	// もし何らかの条件でプラグインを解放できない場合は
	// この時点で E_FAIL を返すようにする。
	// ここでは、TVPPluginGlobalRefCount が GlobalRefCountAtInit よりも
	// 大きくなっていれば失敗ということにする。
	if(TVPPluginGlobalRefCount > GlobalRefCountAtInit) return E_FAIL;
		// E_FAIL が帰ると、Plugins.unlink メソッドは偽を返す

	// TJS のグローバルオブジェクトに登録した average 関数などを削除する

	// - まず、TJS のグローバルオブジェクトを取得する
	iTJSDispatch2 * global = TVPGetScriptDispatch();

	// - global の DeleteMember メソッドを用い、オブジェクトを削除する
	if(global)
	{
		// TJS 自体が既に解放されていたときなどは
		// global は NULL になり得るので global が NULL でない
		// ことをチェックする

		global->DeleteMember(
			0, // フラグ ( 0 でよい )
			TJS_W("AdjustMoni"), // メンバ名
			NULL, // ヒント
			global // コンテキスト
			);
			// 登録した関数が複数ある場合は これを繰り返す
	}

	// - global を Release する
	if(global) global->Release();

	// スタブの使用終了(必ず記述する)
	TVPUninitImportStub();

	return S_OK;
}

