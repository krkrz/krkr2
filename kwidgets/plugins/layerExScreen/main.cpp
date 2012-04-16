#include <windows.h>
#include "../ksupport/ncbind.hpp"

#ifdef _DEBUG
#define LOG(log) TVPAddLog(log)
#endif
#ifndef _DEBUG
#define LOG(log) {}
#endif

//	プロパティの取得
BOOL getValue(iTJSDispatch2* elm, tjs_char* key, tTJSVariant& value);

//	iTJSDispatch2から値を取り出す
BOOL	getValue(iTJSDispatch2* elm, tjs_char* key, tTJSVariant& value)
{
	if(elm==NULL)
	{
		LOG(L"getValue faild. (elm == NULL)");
		return false;
	}
	tjs_error	isvalid	= elm->IsValid(0, key, NULL, elm);
	LOG(L"elm->IsValid = "+ttstr(isvalid));
	if(isvalid==TJS_S_FALSE)	// isvalid != TJS_S_TRUE とすると、正しくても失敗してしまう
								// (Window.HWND を取ろうとすると、TJS_E_NOTIMPL(-1002) が返ってくる)
	{
		LOG(L"getValue faild. (elm is invalid)");
		return false;
	}
	if(TJS_SUCCEEDED(elm->PropGet(0, key, NULL, &value, elm)))
		return true;
	else
	{
		LOG(L"getValue falid. (can't get property["+ttstr(key)+L"])");
		return false;
	}
}

class LayerExScreen
{
private:
	//	primaryLayer 上の座標を求める
	static tjs_int getLayerAbsolutePos(iTJSDispatch2 * obj, tjs_char * prop)
	{
		tTJSVariant	value;
		tjs_int	tmp = 0;
		do
		{
			getValue(obj, prop, value);
			tmp	+= (tjs_int)value.AsInteger();
			getValue(obj, L"parent", value);
			obj		= value.AsObjectNoAddRef();
		}
		while(obj != NULL);
		return tmp;
	}

	//	レイヤーのウィンドウのウィンドウハンドルを求める
	static HWND	getLayerWindowHWND(iTJSDispatch2 * obj)
	{
		tTJSVariant	value;
		getValue(obj, L"window", value);
		obj	= value.AsObjectNoAddRef();
		getValue(obj, L"HWND", value);
		return (HWND)value.AsInteger();
	}

	//	レイヤーの拡大倍率を取得
	static tjs_real getWindowZoom(iTJSDispatch2 * obj)
	{
		tTJSVariant	value;
		getValue(obj, L"window", value);
		obj	= value.AsObjectNoAddRef();
		getValue(obj, L"zoomNumer", value);
		tjs_int	numer = (tjs_int)value.AsInteger();
		getValue(obj, L"zoomDenom", value);
		tjs_int	denom = (tjs_int)value.AsInteger();
		return (tjs_real)numer / denom;
	}

public:
	LayerExScreen(){}

	//	スクリーン上のX座標を取得
	static tjs_error TJS_INTF_METHOD screenLeft(
		tTJSVariant	*result,
		tjs_int numparams,
		tTJSVariant **param,
		iTJSDispatch2 *objthis)
	{
		if(result == NULL)
			return TJS_S_OK;

		tjs_int	left = getLayerAbsolutePos(objthis, L"left");	//	primaryLayer 上での座標を求める
		HWND hWnd = getLayerWindowHWND(objthis);	//	ウィンドウハンドルを取得

		//	数値の指定があれば、その分加える
		if(numparams >= 1 && param[0]->Type() == tvtInteger)
			left	+= (tjs_int)param[0]->AsInteger();

		//	レイヤー拡大倍率が変更されていれば、その分座標を変更する
		left	= (tjs_int)(left * getWindowZoom(objthis));

		//	スクリーン上の座標へ変換して返す
		POINT	pt;
		pt.x	= left;
		pt.y	= 0;
		ClientToScreen(hWnd, &pt);
		*result	= tTVInteger(pt.x);

		return TJS_S_OK;
	}

	//	スクリーン上のY座標を取得
	static tjs_error TJS_INTF_METHOD screenTop(
		tTJSVariant	*result,
		tjs_int numparams,
		tTJSVariant **param,
		iTJSDispatch2 *objthis)
	{
		if(result == NULL)
			return TJS_S_OK;

		tjs_int	top = getLayerAbsolutePos(objthis, L"top");	//	primaryLayer 上での座標を求める
		HWND hWnd = getLayerWindowHWND(objthis);	//	ウィンドウハンドルを取得

		//	数値の指定があれば、その分加える
		if(numparams >= 1 && param[0]->Type() == tvtInteger)
			top	+= (tjs_int)param[0]->AsInteger();

		//	レイヤー拡大倍率が変更されていれば、その分座標を変更する
		top		= (tjs_int)(top * getWindowZoom(objthis));

		//	スクリーン上の座標へ変換して返す
		POINT	pt;
		pt.x	= 0;
		pt.y	= top;
		ClientToScreen(hWnd, &pt);
		*result	= tTVInteger(pt.y);

		return TJS_S_OK;
	}
};


NCB_ATTACH_CLASS(LayerExScreen, Layer)
{
	//	将来的にはプロパティにしたい
	RawCallback("getScreenLeft", &Class::screenLeft, 0);
	RawCallback("getScreenTop", &Class::screenTop, 0);
};

