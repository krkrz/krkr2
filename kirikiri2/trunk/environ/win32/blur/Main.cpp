//---------------------------------------------------------------------------
#include <windows.h>
#include "tp_stub.h"
#include <math.h>
//---------------------------------------------------------------------------



/*
	I_out = Σ(I_in*A_in) / ΣA_in
	A_out = Σ(A_in) / Σ1
*/

//---------------------------------------------------------------------------
// ぼかしを行うクラス
//---------------------------------------------------------------------------
class tBlur
{
	tjs_uint8 * DestBuffer;
	tjs_uint8 * SrcBuffer;
	tjs_int Pitch;
	tjs_int Width;
	tjs_int Height;
	tjs_int BlurWidth;
	tjs_int BlurHeight;
	bool HasAlpha;

	tjs_uint32 *  TmpBuf; // 作業領域
	tjs_uint32 ** TmpOrgLines; // 作業領域の各ラインの先頭へのポインタ
	tjs_uint32 ** TmpLines; // 作業領域 (TmpOrgLinesを並び替えた物)
	tjs_uint32 ** TmpLinePtr: // SrcLines + BlurHeight/2

public:
	tBlur(tjs_uint8 * destbuffer,
			tjs_uint8 *srcbuffer,
			tjs_int pitch,
			tjs_int width,
			tjs_int height,
			tjs_int blurwidth,
			tjs_int blurheight,
			bool hasalpha) :
				DestBuffer(destbuffer),
				SrcBuffer(srcbuffer),
				Pitch(pitch),
				Width(width),
				Height(height),
				BlurWidth(blurwidth),
				BlurHeight(blurheight),
				HasAlpha(hasalpha)
	{
		// SrcLines
		TmpBuf = NULL;
		TmpOrgLines = NULL;
		TmpLinePtr = NULL;
		try
		{
			int i;
			int pitch = (Width + 1) & ~ 1; // 偶数にアライン
			TmpBuf = new tjs_uint32 [BlurHeight * pitch];

			TmpOrgLines = new tjs_uint32 * [BlurHeight];
			for(i = 0; i < BlurHeight; i++) TmpOrgLines[i] = TmpBuf + pitch*i;
			TmpLines = new tjs_uint32 * [BlurHeight];

			TmpLinePtr = TmpLines + BlurHeight / 2;
		}
		catch(...)
		{
			Cleanup();
			throw;
		}
	}

	~tBlur()
	{
		Cleanup();
	}

private:
	void Cleanup()
	{
		if(TmpBuf) delete [] TmpBuf, TmpBuf = NULL;
		if(TmpOrgLines) delete [] TmpOrgLines, TmpOrgLines = NULL;
		if(TmpLines) delete [] TmpLines, TmpLines = NULL;
	}

private:

public:
	void DoBlur();
};
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// ぼかしを行う TJS 関数
//---------------------------------------------------------------------------
class tBlurImageFunction : public tTJSDispatch
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
		bool hasalpha;
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

			/*
				note:
				PropGet 等の "hint" (ヒント) 引数には tjs_uint32 型の変数へのポインタを渡す。
				TJS2 は、メンバ名を検索するときに、検索途中で算出された数値を hint に格納して返す。
				２回目以降は、再び hint に渡された値を用いて検索を行うため、処理が高速になる。
				実際はヒントはメンバ名のハッシュで、同じメンバ名ならば同じ数値になるが、
				同じヒントの値だからといって同じメンバ名であるとは限らない。
				ヒントの値が不正な場合は正しい値に自動的に修正されるが、初期値としては 0 を
				指定することが推奨される。
			*/


		BlurParam bp;
		bp.laybuf = bufferptr;
		bp.laybufpitch = bufferpitch;
		bp.laywidth = width;
		bp.layheight = height;
		bp.blurwidth = *param[1];
		bp.blurheight = *param[2];
		bp.hasalpha = hasalpha;

		// ぼかす
		BlurImage(bp);

		
		// 領域をアップデート
		{
			tTJSVariant val[4];
			tTJSVariant *pval[4] = { val, val +1, val +2, val +3 };

			val[0] = (tjs_int64)0;
			val[1] = (tjs_int64)0;
			val[2] = (tjs_int64)width;
			val[3] = (tjs_int64)height;
			static tjs_uint32 update_hint = 0;
			layerobj->FuncCall(0, L"update", &update_hint, NULL, 4, pval, layerobj);
		}


		// 戻る
		return TJS_S_OK;
	}
} * BlurImageFunction;
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
	BlurImageFunction = new tBlurImageFunction();

	// 2 BlurImageFunction を tTJSVariant 型に変換
	val = tTJSVariant(BlurImageFunction);

	// 3 すでに val が BlurImageFunction を保持しているので、
	// BlurImageFunction は Release する
	BlurImageFunction->Release();


	// 4 global の PropSet メソッドを用い、オブジェクトを登録する
	global->PropSet(
		TJS_MEMBERENSURE, // メンバがなかった場合には作成するようにするフラグ
		TJS_W("blurImage"), // メンバ名 ( かならず TJS_W( ) で囲む )
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

	// TJS のグローバルオブジェクトに登録した blurImage 関数を削除する

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
			TJS_W("blurImage"), // メンバ名
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



