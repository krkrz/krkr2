#include <windows.h>
#include <stdio.h>
#include "tp_stub.h"

#include <irrlicht.h>
#include <iostream>
using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace io;
#pragma comment(lib, "Irrlicht.lib")

/**
 * ログ出力用
 */
static void log(const tjs_char *format, ...)
{
	va_list args;
	va_start(args, format);
	tjs_char msg[1024];
	_vsnwprintf(msg, 1024, format, args);
	TVPAddLog(msg);
	va_end(args);
}

//---------------------------------------------------------------------------

static void
addMember(iTJSDispatch2 *dispatch, const tjs_char *name, iTJSDispatch2 *member)
{
	tTJSVariant var = tTJSVariant(member);
	member->Release();
	dispatch->PropSet(
		TJS_MEMBERENSURE, // メンバがなかった場合には作成するようにするフラグ
		name, // メンバ名 ( かならず TJS_W( ) で囲む )
		NULL, // ヒント ( 本来はメンバ名のハッシュ値だが、NULL でもよい )
		&var, // 登録する値
		dispatch // コンテキスト
		);
}

static iTJSDispatch2*
getMember(iTJSDispatch2 *dispatch, const tjs_char *name)
{
	tTJSVariant val;
	if (TJS_FAILED(dispatch->PropGet(TJS_IGNOREPROP,
									 name,
									 NULL,
									 &val,
									 dispatch))) {
		ttstr msg = TJS_W("can't get member:");
		msg += name;
		TVPThrowExceptionMessage(msg.c_str());
	}
	return val.AsObject();
}

static bool
isValidMember(iTJSDispatch2 *dispatch, const tjs_char *name)
{
	return dispatch->IsValid(TJS_IGNOREPROP,
							 name,
							 NULL,
							 dispatch) == TJS_S_TRUE;
}

static void
delMember(iTJSDispatch2 *dispatch, const tjs_char *name)
{
	dispatch->DeleteMember(
		0, // フラグ ( 0 でよい )
		name, // メンバ名
		NULL, // ヒント
		dispatch // コンテキスト
		);
}

//---------------------------------------------------------------------------

/**
 * Irrlicht 情報クラス
 * ・Irrlicht のデバイス情報を保持します
 * ・Irrlicht のデバイスに対する描画情報を保持します
 */
class NI_Irrlicht : public tTJSNativeInstance, tTVPContinuousEventCallbackIntf
{

protected:
	// レイヤから情報を取得するためのプロパティ
	// 少しでも高速化するためキャッシュしておく
	static iTJSDispatch2 * _leftProp;
	static iTJSDispatch2 * _topProp;
	static iTJSDispatch2 * _widthProp;
	static iTJSDispatch2 * _heightProp;
	static iTJSDispatch2 * _pitchProp;
	static iTJSDispatch2 * _bufferProp;
	static iTJSDispatch2 * _updateProp;

public:
	// スタティック系の初期化処理
	static void init(iTJSDispatch2 *layerobj);
	static void unInit();
	
private:
	/// デバイス
	IrrlichtDevice *device;
	/// ドライバ
	video::IVideoDriver* driver;
	/// シーンマネージャ
	ISceneManager* smgr;

	/// レイヤ描画先テクスチャ
	ITexture *texture;

	/// レイヤ
	iTJSDispatch2 *layer;
	
public:

	/**
	 * コンストラクタ
	 */
	NI_Irrlicht() {
	}

	/**
	 * コンストラクタ
	 */
	tjs_error TJS_INTF_METHOD Construct(tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *tjs_obj) {

		tjs_int width  = 800;
		tjs_int height = 600;
		ttstr title    = "test title";
		if (numparams >= 1 && param[0]->Type() != tvtVoid) {
			width = (tjs_int)*param[0];
		}
		if (numparams >= 2 && param[1]->Type() != tvtVoid) {
			height = (tjs_int)*param[1];
		}
		if (numparams >= 3 && param[2]->Type() != tvtVoid) {
			title = *param[2];
		}
		if (numparams >= 4 && param[3]->Type() == tvtObject) {
			layer = param[3]->AsObject();
		} else {
			layer = NULL;
		}

		device = createDevice(video::EDT_DIRECT3D9, core::dimension2d<s32>(width, height));
		if (device == NULL) {
			device = createDevice(video::EDT_DIRECT3D8, core::dimension2d<s32>(width, height));
			if (device == NULL) {
				device = createDevice(video::EDT_OPENGL, core::dimension2d<s32>(width, height));
			}
		}
		
		if (device == NULL) {
			TVPThrowExceptionMessage(L"Irrlicht デバイスの初期化に失敗しました");
		}

		device->setWindowCaption(title.c_str());
		driver = device->getVideoDriver();
		smgr   = device->getSceneManager();
		texture = driver->addTexture(core::dimension2d<s32>(1024, 1024), "layer", ECF_A8R8G8B8);

#if 0
		// 以下サンプルデータのロード処理
		ttstr dataPath = "sydney.md2";
		dataPath = TVPNormalizeStorageName(dataPath);
		TVPGetLocalName(dataPath);
		int len = dataPath.GetNarrowStrLen() + 1;
		char *str = new char[len];
		dataPath.ToNarrowStr(str, len);
		IAnimatedMesh *mesh = smgr->getMesh(str);
		IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode(mesh);
		if (node) {
			node->setMaterialFlag(EMF_LIGHTING, false);
			node->setMD2Animation ( scene::EMAT_STAND );
		}
		smgr->addCameraSceneNode(0, vector3df(0,30,-40), vector3df(0,5,0));
#else
		ttstr dataPath = "map-20kdm2.pk3";
		dataPath = TVPNormalizeStorageName(dataPath);
		TVPGetLocalName(dataPath);
		int len = dataPath.GetNarrowStrLen() + 1;
		char *str = new char[len];
		dataPath.ToNarrowStr(str, len);
		device->getFileSystem()->addZipFileArchive(str);

		scene::IAnimatedMesh* mesh = smgr->getMesh("20kdm2.bsp");
		scene::ISceneNode* node = 0;
		
		if (mesh)
			node = smgr->addOctTreeSceneNode(mesh->getMesh(0));
		
		if (node)
			node->setPosition(core::vector3df(-1300,-144,-1249));

		smgr->addCameraSceneNodeFPS();
		device->getCursorControl()->setVisible(false);
#endif
		
		// ワークプロックとして登録
		TVPAddContinuousEventHook(this);
		
		return S_OK;
	}

	void TJS_INTF_METHOD Invalidate() {
		// ワークプロックの解除
		TVPRemoveContinuousEventHook(this);
		device->drop();
		if (layer) {
			layer->Release();
		}
	}


protected:
	/**
	 * レイヤを描画する
	 * @param layerobj ターゲットになるレイヤ
	 */
	void drawLayer(iTJSDispatch2 *layerobj);
	
public:
	/**
	 * Continuous コールバック
	 * 吉里吉里が暇なときに常に呼ばれる
	 */
	virtual void TJS_INTF_METHOD OnContinuousCallback(tjs_uint64 tick) {
		if (driver) {
			device->getTimer()->tick();
			if (device->isWindowActive()) {
				driver->beginScene(true, true, video::SColor(255,100,101,140));
				smgr->drawAll();
				drawLayer(layer);
				driver->endScene();
			}
		}
	}

};

// プロパティ
iTJSDispatch2 * NI_Irrlicht::_leftProp   = NULL;
iTJSDispatch2 * NI_Irrlicht::_topProp    = NULL;
iTJSDispatch2 * NI_Irrlicht::_widthProp  = NULL;
iTJSDispatch2 * NI_Irrlicht::_heightProp = NULL;
iTJSDispatch2 * NI_Irrlicht::_pitchProp  = NULL;
iTJSDispatch2 * NI_Irrlicht::_bufferProp = NULL;
iTJSDispatch2 * NI_Irrlicht::_updateProp = NULL;

/**
 * プロパティ取得処理
 * @param layerobj レイヤクラスオブジェクト
 */
void
NI_Irrlicht::init(iTJSDispatch2 *layerobj)
{
	// プロパティ取得
	tTJSVariant var;
	
	if (TJS_FAILED(layerobj->PropGet(TJS_IGNOREPROP, TJS_W("imageLeft"), NULL, &var, layerobj))) {
		TVPThrowExceptionMessage(TJS_W("invoking of Layer.imageLeft failed."));
	} else {
		_leftProp = var;
	}
	if (TJS_FAILED(layerobj->PropGet(TJS_IGNOREPROP, TJS_W("imageTop"), NULL, &var, layerobj))) {
		TVPThrowExceptionMessage(TJS_W("invoking of Layer.imageTop failed."));
	} else {
		_topProp = var;
	}
	if (TJS_FAILED(layerobj->PropGet(TJS_IGNOREPROP, TJS_W("imageWidth"), NULL, &var, layerobj))) {
		TVPThrowExceptionMessage(TJS_W("invoking of Layer.imageWidth failed."));
	} else {
		_widthProp = var;
	}
	if (TJS_FAILED(layerobj->PropGet(TJS_IGNOREPROP, TJS_W("imageHeight"), NULL, &var, layerobj))) {
		TVPThrowExceptionMessage(TJS_W("invoking of Layer.imageHeight failed."));
	} else {
		_heightProp = var;
	}
	if (TJS_FAILED(layerobj->PropGet(TJS_IGNOREPROP, TJS_W("mainImageBuffer"), NULL, &var, layerobj))) {
		TVPThrowExceptionMessage(TJS_W("invoking of Layer.mainImageBuffer failed."));
	} else {
		_bufferProp = var;
	}
	if (TJS_FAILED(layerobj->PropGet(TJS_IGNOREPROP, TJS_W("mainImageBufferPitch"), NULL, &var, layerobj))) {
		TVPThrowExceptionMessage(TJS_W("invoking of Layer.mainImageBufferPitch failed."));
	} else {
		_pitchProp = var;
	}
	if (TJS_FAILED(layerobj->PropGet(TJS_IGNOREPROP, TJS_W("update"), NULL, &var, layerobj))) {
		TVPThrowExceptionMessage(TJS_W("invoking of Layer.update failed."));
	} else {
		_updateProp = var;
	}
}

/**
 * プロパティ解放
 */
void
NI_Irrlicht::unInit()
{
	if (_leftProp)   _leftProp->Release();
	if (_topProp)    _topProp->Release();
	if (_widthProp)  _widthProp->Release();
	if (_heightProp) _heightProp->Release();
	if (_bufferProp) _bufferProp->Release();
	if (_pitchProp)  _pitchProp->Release();
	if (_updateProp) _updateProp->Release();
}

/// プロパティから int 値を取得する
static tjs_int64 getPropValue(iTJSDispatch2 *dispatch, iTJSDispatch2 *layerobj)
{
	tTJSVariant var;
	if(TJS_FAILED(dispatch->PropGet(0, NULL, NULL, &var, layerobj))) {
		TVPThrowExceptionMessage(TJS_W("can't get int value from property."));
	}
	return var;
}

/**
 * レイヤの描画処理
 * Irrlicht の空間に対してあるレイヤの内容を吸い出して描画する
 */
void
NI_Irrlicht::drawLayer(iTJSDispatch2 *layerobj)
{
	if (layerobj && texture) {

		// 更新

		// プロパティ取得
		{
			tTJSVariant var;
			if (TJS_SUCCEEDED(layerobj->PropGet(TJS_IGNOREPROP, TJS_W("onPaint"), NULL, &var, layerobj))) {
				var.AsObjectNoAddRef()->FuncCall(0, NULL, NULL, NULL, 0, NULL, layerobj);
			}
		}
		
		// レイヤパラメータ取得
		int srcWidth             = (int)getPropValue(_widthProp, layerobj);
		int srcHeight            = (int)getPropValue(_heightProp, layerobj);
		int srcPitch             = (int)getPropValue(_pitchProp, layerobj);
		unsigned char *srcBuffer = (unsigned char *)getPropValue(_bufferProp, layerobj);

		// テクスチャパラメータ取得
		core::dimension2d<s32> size = texture->getSize();
		int destWidth = size.Width;
		int destHeight = size.Height;
		int destPitch  = texture->getPitch();
		unsigned char *destBuffer = (unsigned char *)texture->lock();

		int width  = srcWidth > destWidth ? destWidth : srcWidth;
		int height = srcHeight > destHeight ? destHeight : srcHeight;
		for (int i=0;i<height;i++) {
			memcpy(destBuffer, srcBuffer, width * 4);
			srcBuffer += srcPitch;
			destBuffer += destPitch;
		}
		texture->unlock();

		// テクスチャ描画処理!!
		driver->draw2DImage(texture, core::position2d<s32>(0,0),
							core::rect<s32>(0,0,800,600), 0, 
							video::SColor(255,255,255,255), true);
	}
}


//---------------------------------------------------------------------------
/*
	これは NI_Irrlicht のオブジェクトを作成して返すだけの関数です。
	後述の TJSCreateNativeClassForPlugin の引数として渡します。
*/
static iTJSNativeInstance * TJS_INTF_METHOD Create_NI_Irrlicht()
{
	return new NI_Irrlicht();
}
//---------------------------------------------------------------------------
/*
	TJS2 のネイティブクラスは一意な ID で区別されている必要があります。
	これは後述の TJS_BEGIN_NATIVE_MEMBERS マクロで自動的に取得されますが、
	その ID を格納する変数名と、その変数をここで宣言します。
	初期値には無効な ID を表す -1 を指定してください。
*/
#define TJS_NATIVE_CLASSID_NAME ClassID_Irrlicht
static tjs_int32 TJS_NATIVE_CLASSID_NAME = -1;
//---------------------------------------------------------------------------
/*
	TJS2 用の「クラス」を作成して返す関数です。
*/
static iTJSDispatch2 * Create_NC_Irrlicht()
{
	// クラスオブジェクト生成
	tTJSNativeClassForPlugin * classobj =
		TJSCreateNativeClassForPlugin(TJS_W("Irrlicht"), Create_NI_Irrlicht);

	// メンバ登録
	TJS_BEGIN_NATIVE_MEMBERS(/*TJS class name*/Irrlicht)

		TJS_DECL_EMPTY_FINALIZE_METHOD

		TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL(
			/*var.name*/_this,
			/*var.type*/NI_Irrlicht,
			/*TJS class name*/Irrlicht)
		{
			// NI_Irrlicht::Construct にも内容を記述できるので
			// ここでは何もしない
			return TJS_S_OK;
		}
		TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/Irrlicht)

	TJS_END_NATIVE_MEMBERS

	/*
		この関数は classobj を返します。
	*/
	return classobj;
}

#undef TJS_NATIVE_CLASSID_NAME

//---------------------------------------------------------------------------

#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason,
	void* lpReserved)
{
	return 1;
}

//---------------------------------------------------------------------------
static tjs_int GlobalRefCountAtInit = 0;
extern "C" HRESULT _stdcall V2Link(iTVPFunctionExporter *exporter)
{
	// スタブの初期化
	TVPInitImportStub(exporter);

	// TJS のグローバルオブジェクトを取得する
	iTJSDispatch2 * global = TVPGetScriptDispatch();
	if (global) {
		addMember(global, TJS_W("Irrlicht"), Create_NC_Irrlicht());

		// Layer クラスオブジェクトを取得
		tTJSVariant varScripts;
		TVPExecuteExpression(TJS_W("Layer"), &varScripts);
		iTJSDispatch2 *dispatch = varScripts.AsObjectNoAddRef();
		if (dispatch) {
			NI_Irrlicht::init(dispatch);
		}

		global->Release();
	}
			
	GlobalRefCountAtInit = TVPPluginGlobalRefCount;

	return S_OK;
}
//---------------------------------------------------------------------------
extern "C" HRESULT _stdcall V2Unlink()
{
	if (TVPPluginGlobalRefCount > GlobalRefCountAtInit) return E_FAIL;

	// プロパティ開放
	NI_Irrlicht::unInit();
	
	// - まず、TJS のグローバルオブジェクトを取得する
	iTJSDispatch2 * global = TVPGetScriptDispatch();
	if (global)	{
		delMember(global, TJS_W("Irrlicht"));
		global->Release();
	}

	// スタブの使用終了	TVPUninitImportStub();
	return S_OK;
}
