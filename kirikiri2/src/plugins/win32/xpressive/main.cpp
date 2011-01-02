#include <windows.h>
#include "tp_stub.h"


class tTJSNativeClassForRegExp : public tTJSDispatch
{
public:
	tTJSNativeClassForRegExp(tjs_char const *className) : tTJSDispatch(), classname(className) {
		self = this;
		classobj = TJSCreateNativeClassForPlugin(className, _CreateNativeInstance);
	}

	static void Link(iTJSDispatch2 *global) {
		Initialize();
		if (self) {
			self->Setup(global);
		}
	}
	static void Unlink(iTJSDispatch2 *global) {
		if (self) {
			self->Terminate(global);
			self->Release();
		}
		self = 0;
	}

private:
	static tTJSNativeClassForRegExp * self;
	static iTJSNativeInstance * TJS_INTF_METHOD _CreateNativeInstance() {
		return self ? self->CreateNativeInstance() : NULL;
	}
	static void Initialize();
	void Setup(iTJSDispatch2 *global) {
		tTJSVariant val(classobj);
		global->PropSet(
			TJS_MEMBERENSURE, // メンバがなかった場合には作成するようにするフラグ
			classname, // メンバ名 ( かならず TJS_W( ) で囲む )
			NULL, // ヒント ( 本来はメンバ名のハッシュ値だが、NULL でもよい )
			&val, // 登録する値
			global // コンテキスト ( global でよい )
			);
		classobj->Release();
	}
	void Terminate(iTJSDispatch2 *global) {
		global->DeleteMember(
			0, // フラグ ( 0 でよい )
			classname, // メンバ名
			NULL, // ヒント
			global // コンテキスト
			);
	}

protected:
	tjs_char const *classname;
	tTJSNativeClassForPlugin * classobj;
	virtual iTJSNativeInstance *CreateNativeInstance() = 0;
};
tTJSNativeClassForRegExp* tTJSNativeClassForRegExp::self = 0;


// hacks

#define TJS_cdecl
#define TJS_strchr			wcschr

#define TJS_NATIVE_CLASSID_NAME tTJSNC_RegExp::ClassID
#define tTJSNativeClass tTJSNativeClassForRegExp

#define TJS_eTJSError TVPThrowExceptionMessage
inline void TVPThrowExceptionMessage(const tjs_nchar * msg) {
	ttstr msg_(msg);
	TVPThrowExceptionMessage(msg_.c_str());
}

#pragma warning (disable: 4996)
#pragma warning (disable: 4800)

#define TJS_USE_XPRESSIVE
#include "tjsRegExp.cpp"

void tTJSNativeClassForRegExp::Initialize()
{
	TJS::TJSCreateRegExpClass();
}


#define EXPORT(hr) extern "C" __declspec(dllexport) hr __stdcall

#ifdef _MSC_VER
#pragma comment(linker, "/EXPORT:V2Link=_V2Link@4")
#pragma comment(linker, "/EXPORT:V2Unlink=_V2Unlink@0")
#endif

//--------------------------------------
int WINAPI
DllEntryPoint(HINSTANCE /*hinst*/, unsigned long /*reason*/, void* /*lpReserved*/)
{
	return 1;
}

//---------------------------------------------------------------------------
static tjs_int GlobalRefCountAtInit = 0;

EXPORT(HRESULT) V2Link(iTVPFunctionExporter *exporter)
{
	// スタブの初期化(必ず記述する)
	TVPInitImportStub(exporter);

	// TJS のグローバルオブジェクトを取得する
	iTJSDispatch2 * global = TVPGetScriptDispatch();
	if (global) {

		// 登録処理
		tTJSNativeClassForRegExp::Link(global);

		// - global を Release する
		global->Release();
	}

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
EXPORT(HRESULT) V2Unlink()
{
	// 吉里吉里側から、プラグインを解放しようとするときに呼ばれる関数

	// もし何らかの条件でプラグインを解放できない場合は
	// この時点で E_FAIL を返すようにする。
	// ここでは、TVPPluginGlobalRefCount が GlobalRefCountAtInit よりも
	// 大きくなっていれば失敗ということにする。
	if (TVPPluginGlobalRefCount > GlobalRefCountAtInit) return E_FAIL;


	/*
		ただし、クラスの場合、厳密に「オブジェクトが使用中である」ということを
		知るすべがありません。基本的には、Plugins.unlink によるプラグインの解放は
		危険であると考えてください (いったん Plugins.link でリンクしたら、最後ま
		でプラグインを解放せず、プログラム終了と同時に自動的に解放させるのが吉)。
	*/

	// - まず、TJS のグローバルオブジェクトを取得する
	iTJSDispatch2 * global = TVPGetScriptDispatch();
	if(global) {

		// 後始末
		tTJSNativeClassForRegExp::Unlink(global);

		// - global を Release する
		global->Release();
	}

	// スタブの使用終了(必ず記述する)
	TVPUninitImportStub();

	return S_OK;
}
