#include "WindowExBase.h"

int NI_WindowExBase::classId;

iTJSDispatch2 * NI_WindowExBase::_HWNDProp = NULL;
iTJSDispatch2 * NI_WindowExBase::_innerWidthProp = NULL;
iTJSDispatch2 * NI_WindowExBase::_innerHeightProp = NULL;
iTJSDispatch2 * NI_WindowExBase::_primaryLayerProp = NULL;

iTJSDispatch2 * NI_WindowExBase::_leftProp   = NULL;
iTJSDispatch2 * NI_WindowExBase::_topProp    = NULL;
iTJSDispatch2 * NI_WindowExBase::_widthProp  = NULL;
iTJSDispatch2 * NI_WindowExBase::_heightProp = NULL;
iTJSDispatch2 * NI_WindowExBase::_pitchProp  = NULL;
iTJSDispatch2 * NI_WindowExBase::_bufferProp = NULL;
iTJSDispatch2 * NI_WindowExBase::_updateProp = NULL;

void
NI_WindowExBase::init(iTJSDispatch2 *windowobj, iTJSDispatch2 *layerobj)
{
	// プロパティ取得
	tTJSVariant var;

	if (TJS_FAILED(windowobj->PropGet(TJS_IGNOREPROP, TJS_W("HWND"), NULL, &var, windowobj))) {
		TVPThrowExceptionMessage(TJS_W("invoking of Window.HWND failed."));
	} else {
		_HWNDProp = var;
	}

	if (TJS_FAILED(windowobj->PropGet(TJS_IGNOREPROP, TJS_W("innerWidth"), NULL, &var, windowobj))) {
		TVPThrowExceptionMessage(TJS_W("invoking of Window.innerWidth failed."));
	} else {
		_innerWidthProp = var;
	}

	if (TJS_FAILED(windowobj->PropGet(TJS_IGNOREPROP, TJS_W("innerHeight"), NULL, &var, windowobj))) {
		TVPThrowExceptionMessage(TJS_W("invoking of Window.innerHeight failed."));
	} else {
		_innerHeightProp = var;
	}

	if (TJS_FAILED(windowobj->PropGet(TJS_IGNOREPROP, TJS_W("primaryLayer"), NULL, &var, windowobj))) {
		TVPThrowExceptionMessage(TJS_W("invoking of Window.primaryLayer failed."));
	} else {
		_primaryLayerProp = var;
	}
	
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

void
NI_WindowExBase::unInit()
{
	// レイヤ用プロパティの解放
	if (_leftProp)   _leftProp->Release();
	if (_topProp)    _topProp->Release();
	if (_widthProp)  _widthProp->Release();
	if (_heightProp) _heightProp->Release();
	if (_bufferProp) _bufferProp->Release();
	if (_pitchProp)  _pitchProp->Release();
	if (_updateProp) _updateProp->Release();

	// ウインドウ用プロパティの解放
	if (_primaryLayerProp)   _primaryLayerProp->Release();
	if (_innerWidthProp) _innerWidthProp->Release();
	if (_innerHeightProp) _innerHeightProp->Release();
	if (_HWNDProp) _HWNDProp->Release();
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
 * 内部記録状態の初期化用。例外をおこすので TVPDoTryBlock 用に分離
 */

void
NI_WindowExBase::_reset()
{
	// プライマリレイヤを取得
	// 失敗した場合は例外になる
	tTJSVariant primaryLayer;
	_primaryLayerProp->PropGet(0, NULL, NULL, &primaryLayer, _windowobj);
	
	if (primaryLayer.Type() == tvtObject) {
		iTJSDispatch2 *layerobj = primaryLayer.AsObjectNoAddRef();
		_width  = (int)getPropValue(_widthProp, layerobj);
		_height = (int)getPropValue(_heightProp, layerobj);
		_buffer = (unsigned char *)getPropValue(_bufferProp, layerobj);
		_pitch  = (int)getPropValue(_pitchProp, layerobj);
	}
}

//---------------------------------------------------------------------------
// 例外処理用

// 例外をおこすブロック
static void TJS_USERENTRY TryBlock1(void * data)
{
	NI_WindowExBase *winBase = (NI_WindowExBase*)data;
	winBase->_reset();
}

// 例外時処理
static bool TJS_USERENTRY CatchBlock1(void * data, const tTVPExceptionDesc & desc)
{
	NI_WindowExBase *winBase = (NI_WindowExBase*)data;
	winBase->_width  = 0;
	winBase->_height = 0;
	winBase->_pitch  = 0;
	winBase->_buffer = NULL;
	TVPAddLog(TJS_W("exception type:") + desc.type + TJS_W(", message:") + desc.message);
	return false;
}

// 完了後処理
static void TJS_USERENTRY FinallyBlock1(void * data)
{
	// nothing to do
}
//---------------------------------------------------------------------------

/**
 * 内部状態を初期化する
 */
void
NI_WindowExBase::reset(iTJSDispatch2 *windowobj)
{
	_windowobj = windowobj; // no add ref
	_hwnd = (HWND)getPropValue(_HWNDProp, windowobj);
	_innerWidth  = getPropValue(_innerWidthProp, windowobj);
	_innerHeight = getPropValue(_innerHeightProp, windowobj);
	// 例外がおこる場合があるのでブロッキング
	TVPDoTryBlock(TryBlock1, CatchBlock1, FinallyBlock1, (void*)this);
}

/**
 * ウインドウ拡張のネイティブインスタンスの取得
 * @param objthis this
 * @param create true なら存在しない時に生成
 */
NI_WindowExBase *
NI_WindowExBase::getNative(iTJSDispatch2 *objthis, bool create)
{
	NI_WindowExBase *_this = NULL;
	if (TJS_FAILED(objthis->NativeInstanceSupport(TJS_NIS_GETINSTANCE,
												  classId, (iTJSNativeInstance**)&_this)) && create) {
		_this = new NI_WindowExBase();
		if (TJS_FAILED(objthis->NativeInstanceSupport(TJS_NIS_REGISTER,
													  classId, (iTJSNativeInstance **)&_this))) {
			delete _this;
			_this = NULL;
		}
	}
	return _this;
}
	
/**
 * コンストラクタ
 */
NI_WindowExBase::NI_WindowExBase()
{
	_width  = 0;
	_height = 0;
	_pitch  = 0;
	_buffer = NULL;
}
