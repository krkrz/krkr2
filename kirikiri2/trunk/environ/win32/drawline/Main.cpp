//---------------------------------------------------------------------------
#include <windows.h>
#include "tp_stub.h"
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
// 直線描画を行う TJS 関数
//---------------------------------------------------------------------------
class tDrawLineFunction : public tTJSDispatch
{
	tjs_error TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
		tTJSVariant *result,
		tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
	{
		if(membername) return TJS_E_MEMBERNOTFOUND;

		// 引数 : レイヤ, 幅, 高さ

		if(numparams < 3) return TJS_E_BADPARAMCOUNT;

		iTJSDispatch2 * layerobj = param[0]->AsObjectNoAddRef();

		tjs_uint8 * bufferptr;
		tjs_int bufferpitch;
		tjs_int width;
		tjs_int height;
		tTVPDrawFace face;

		// 画像のサイズ、ピクセルバッファへのポインタ、ピッチを得る

		tTJSVariant val;
		static tjs_uint32 imageWidth_hint = 0;
		if(TJS_FAILED(layerobj->PropGet(0, TJS_W("imageWidth"), &imageWidth_hint, &val, layerobj)))
			TVPThrowExceptionMessage(TJS_W("invoking of Layer.imageWidth failed."));
		width = val;

		static tjs_uint32 imageHeight_hint = 0;
		if(TJS_FAILED(layerobj->PropGet(0, TJS_W("imageHeight"), &imageHeight_hint, &val, layerobj)))
			TVPThrowExceptionMessage(TJS_W("invoking of Layer.imageHeight failed."));
		height = val;

		static tjs_uint32 mainImageBufferForWrite_hint = 0;
		if(TJS_FAILED(layerobj->PropGet(0, TJS_W("mainImageBufferForWrite"), &mainImageBufferForWrite_hint,
			&val, layerobj)))
			TVPThrowExceptionMessage(TJS_W("invoking of Layer.mainImageBufferForWrite failed."));
		bufferptr = (tjs_uint8 *)(tjs_int)val;

		static tjs_uint32 mainImageBufferPitch_hint = 0;
		if(TJS_FAILED(layerobj->PropGet(0, TJS_W("mainImageBufferPitch"), &mainImageBufferPitch_hint,
			&val, layerobj)))
			TVPThrowExceptionMessage(TJS_W("invoking of Layer.mainImageBufferPitch failed."));
		bufferpitch = val;
/*
		bool hasalpha;
		static tjs_uint32 face_hint = 0;
		if(TJS_FAILED(layerobj->PropGet(0, TJS_W("face"), &face_hint,
			&val, layerobj)))
			TVPThrowExceptionMessage(TJS_W("invoking of Layer.face failed."));
		face = (tTVPDrawFace)(tjs_int)val;
		if(face == dfBoth)
			hasalpha = true;
		else if(face == dfMain)
			hasalpha = false;
		else
			TVPThrowExceptionMessage(TJS_W("not drawable face"));
*/

		// 直線を描画する
		tTJSVariant updatecoords[4];
		tTJSVariant *pupdatecoords[4] = { updatecoords, updatecoords +1, updatecoords +2, updatecoords +3 };


		// 

		val[0] = (tjs_int64)0;
		val[1] = (tjs_int64)0;
		val[2] = (tjs_int64)width;
		val[3] = (tjs_int64)height;
		static tjs_uint32 update_hint = 0;
		layerobj->FuncCall(0, L"update", &update_hint, NULL, 4, pval, layerobj);

		// 戻る
		return TJS_S_OK;
	}
} * DrawLineFunction;
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	return 1;
}
//---------------------------------------------------------------------------
static tjs_int GlobalRefCountAtInit = 0;
extern "C" HRESULT _stdcall _export V2Link(iTVPFunctionExporter *exporter)
{
	// スタブの初期化(必ず記述する)
	TVPInitImportStub(exporter);

	// TestFunction の作成と登録
	tTJSVariant val;

	// TJS のグローバルオブジェクトを取得する
	iTJSDispatch2 * global = TVPGetScriptDispatch();

	// 1 まずオブジェクトを作成
	DrawLineFunction = new tDrawLineFunction();

	// 2 DrawLine を tTJSVariant 型に変換
	val = tTJSVariant(DrawLineFunction);

	// 3 すでに val が DrawLineFunction を保持しているので、
	// DrawLineFunction は Release する
	DrawLineFunction->Release();


	// 4 global の PropSet メソッドを用い、オブジェクトを登録する
	global->PropSet(
		TJS_MEMBERENSURE, // メンバがなかった場合には作成するようにするフラグ
		TJS_W("drawLine"), // メンバ名 ( かならず TJS_W( ) で囲む )
		NULL, // ヒント ( 本来はメンバ名のハッシュ値だが、NULL でもよい )
		&val, // 登録する値
		global // コンテキスト ( global でよい )
		);


	// - global を Release する
	global->Release();

	// val をクリアする。
	// これは必ず行う。そうしないと val が保持しているオブジェクト
	// が Release されず、次に使う TVPPluginGlobalRefCount が正確にならない。
	val.Clear();


	// この時点での TVPPluginGlobalRefCount の値を
	GlobalRefCountAtInit = TVPPluginGlobalRefCount;
	TVPAddLog(TVPPluginGlobalRefCount);
	// として控えておく。TVPPluginGlobalRefCount はこのプラグイン内で
	// 管理されている tTJSDispatch 派生オブジェクトの参照カウンタの総計で、
	// 解放時にはこれと同じか、これよりも少なくなってないとならない。
	// そうなってなければ、どこか別のところで関数などが参照されていて、
	// プラグインは解放できないと言うことになる。

	return S_OK;
}
//---------------------------------------------------------------------------
extern "C" HRESULT _stdcall _export V2Unlink()
{
	// 吉里吉里側から、プラグインを解放しようとするときに呼ばれる関数。

	// もし何らかの条件でプラグインを解放できない場合は
	// この時点で E_FAIL を返すようにする。
	// ここでは、TVPPluginGlobalRefCount が GlobalRefCountAtInit よりも
	// 大きくなっていれば失敗ということにする。
	TVPAddLog(TVPPluginGlobalRefCount);
	if(TVPPluginGlobalRefCount > GlobalRefCountAtInit) return E_FAIL;
		// E_FAIL が帰ると、Plugins.unlink メソッドは偽を返す

	// TJS のグローバルオブジェクトに登録した drawLine 関数を削除する

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
			TJS_W("drawLine"), // メンバ名
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
//---------------------------------------------------------------------------



