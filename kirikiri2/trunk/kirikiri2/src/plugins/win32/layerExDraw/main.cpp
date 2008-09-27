#include "layerExDraw.hpp"
#include "ncbind/ncbind.hpp"

extern void initGdiPlus();
extern void deInitGdiPlus();

// ------------------------------------------------------
// 配列操作用
// ------------------------------------------------------

// Array クラスメンバ
static iTJSDispatch2 *ArrayCountProp   = NULL;   // Array.count

tjs_int64 getArrayCount(iTJSDispatch2 *array)
{
	tTJSVariant result;
	if (TJS_SUCCEEDED(ArrayCountProp->PropGet(0, NULL, NULL, &result, array))) {
		return result.AsInteger();
	}
	return 0;
}

iTJSDispatch2*
getMember(iTJSDispatch2 *dispatch, int num)
{
	tTJSVariant val;
	if (TJS_FAILED(dispatch->PropGetByNum(TJS_IGNOREPROP,
										  num,
										  &val,
										  dispatch))) {
		ttstr msg = TJS_W("can't get array index:");
		msg += num;
		TVPThrowExceptionMessage(msg.c_str());
	}
	return val.AsObject();
}

REAL 
getRealValue(iTJSDispatch2 *obj, const tjs_char *name)
{
	tTJSVariant val;
	if (TJS_FAILED(obj->PropGet(0,
								name,
								NULL,
								&val,
								obj))) {
		ttstr msg = TJS_W("can't get member:");
		msg += name;
		TVPThrowExceptionMessage(msg.c_str());
	}
	return (REAL)val.AsReal();
}

// ----------------------------------- クラスの登録

NCB_REGISTER_SUBCLASS(FontInfo) {
	NCB_CONSTRUCTOR((const tjs_char *, REAL, INT));
	NCB_PROPERTY_WO(familyName, setFamilyName);
	NCB_PROPERTY(emSize, getEmSize, setEmSize);
	NCB_PROPERTY(style, getStyle, setStyle);
};

NCB_REGISTER_SUBCLASS(Appearance) {
	NCB_CONSTRUCTOR(());
	NCB_METHOD(clear);
	NCB_METHOD(addSolidBrush);
	NCB_METHOD(addLinearGradientBrush);
	NCB_METHOD(addColorPen);
};

#define ENUM(n) Variant(#n, (int)n)

NCB_REGISTER_CLASS(GdiPlus)
{
// enums
	ENUM(FontStyleRegular);
	ENUM(FontStyleBold);
	ENUM(FontStyleItalic);
	ENUM(FontStyleBoldItalic);
	ENUM(FontStyleUnderline);
	ENUM(FontStyleStrikeout);

// statics
	NCB_METHOD(addPrivateFont);
	NCB_METHOD(showPrivateFontList);

// classes
	NCB_SUBCLASS(Font,FontInfo);
	NCB_SUBCLASS(Appearance,Appearance);
}

NCB_GET_INSTANCE_HOOK(LayerExDraw)
{
	// インスタンスゲッタ
	NCB_INSTANCE_GETTER(objthis) { // objthis を iTJSDispatch2* 型の引数とする
		ClassT* obj = GetNativeInstance(objthis);	// ネイティブインスタンスポインタ取得
		if (!obj) {
			obj = new ClassT(objthis);				// ない場合は生成する
			SetNativeInstance(objthis, obj);		// objthis に obj をネイティブインスタンスとして登録する
		}
		obj->reset();
		return obj;
	}
	// デストラクタ（実際のメソッドが呼ばれた後に呼ばれる）
	~NCB_GET_INSTANCE_HOOK_CLASS () {
	}
};


// フックつきアタッチ
NCB_ATTACH_CLASS_WITH_HOOK(LayerExDraw, Layer) {
	NCB_PROPERTY(updateWhenDraw, getUpdateWhenDraw, setUpdateWhenDraw);
	NCB_METHOD(clear);
	NCB_METHOD(drawArc);
	NCB_METHOD(drawPie);
	NCB_METHOD(drawBezier);
	NCB_METHOD(drawEllipse);
	NCB_METHOD(drawLine);
	NCB_METHOD(drawRectangle);
	NCB_METHOD(drawString);
	NCB_METHOD(drawImage);
}

// ----------------------------------- 起動・開放処理

/**
 * 登録処理前
 */
void PreRegistCallback()
{
	initGdiPlus();

	// Array.count を取得
	{
		tTJSVariant varScripts;
		TVPExecuteExpression(TJS_W("Array"), &varScripts);
		iTJSDispatch2 *dispatch = varScripts.AsObjectNoAddRef();
		tTJSVariant val;
		if (TJS_FAILED(dispatch->PropGet(TJS_IGNOREPROP,
										 TJS_W("count"),
										 NULL,
										 &val,
										 dispatch))) {
			TVPThrowExceptionMessage(L"can't get Array.count");
		}
		ArrayCountProp = val.AsObject();
	}
}

/**
 * 開放処理後
 */
void PostUnregistCallback()
{
	if (ArrayCountProp) {
		ArrayCountProp->Release();
		ArrayCountProp = NULL;
	}
	deInitGdiPlus();
}

NCB_PRE_REGIST_CALLBACK(PreRegistCallback);
NCB_POST_UNREGIST_CALLBACK(PostUnregistCallback);
