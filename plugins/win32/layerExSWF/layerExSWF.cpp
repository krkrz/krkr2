#include "layerExSWF.hpp"

layerExSWF::layerExSWF(DispatchT obj) : layerExBase(obj)
{
	width = 0;
	height = 0;
	buffer = 0;
	pitch = 0;
	surface = NULL;
	cairo = NULL;
}

layerExSWF::~layerExSWF()
{
	cairo_destroy(cairo);
	cairo_surface_destroy(surface);
}


/**
 * リセット処理
 */
void
layerExSWF::reset() {

	// 基本処理
	layerExBase::reset();
	
	// レイヤの情報変更があったか判定
	if (width != _width ||
		height != _height ||
		buffer != _buffer ||
		pitch != _pitch) {

		width  = _width;
		height = _height;
		buffer = _buffer;
		pitch  = _pitch;
		
		// cairo 用コンテキストの再生成
		cairo_destroy(cairo);
		cairo_surface_destroy(surface);
		surface = cairo_image_surface_create_for_data((BYTE*)_buffer, CAIRO_FORMAT_ARGB32, width, height, pitch);
		cairo = cairo_create(surface);
		if (cairo_status(cairo) != CAIRO_STATUS_SUCCESS) {
			TVPThrowExceptionMessage(L"can't create cairo context");
			cairo_destroy(cairo);
			cairo = NULL;
		}
	}
}

void
layerExSWF::drawTest(double x, double y, double w, double h)
{
	if (cairo) {
		cairo_set_source_rgb (cairo, 0.25, 0.25, 0.25); /* dark gray */
		cairo_rectangle (cairo, x, y, w, h);
		cairo_fill (cairo);
	}
}

////////////////////////////////////////////////////////////////////////////////
/// ncBind 用マクロ

#include "ncbind/ncbind.hpp"

NCB_GET_INSTANCE_HOOK(layerExSWF)
{
	// インスタンスゲッタ
	NCB_INSTANCE_GETTER(objthis) { // objthis を iTJSDispatch2* 型の引数とする
		
		ClassT* obj = GetNativeInstance(objthis);	// ネイティブインスタンスポインタ取得
		if (!obj) {
			obj = new ClassT(objthis);				// ない場合は生成する
			SetNativeInstance(objthis, obj);		// objthis に obj をネイティブインスタンスとして登録する
		}
		if (obj) obj->reset();						// メソッドを呼ぶ前に必ず呼ばれる
		return (_obj = obj);						//< デストラクタで使用したい場合はプライベート変数に保存
	}

	// デストラクタ（実際のメソッドが呼ばれた後に呼ばれる）
	~NCB_GET_INSTANCE_HOOK_CLASS () {
		if (_obj) _obj->redraw();					// メソッドを呼んだ後に必ず呼ばれる
	}

private:
	ClassT *_obj;
}; // 実体は class 定義なので ; を忘れないでね

// フックつきアタッチ
NCB_ATTACH_CLASS_WITH_HOOK(layerExSWF, Layer) {
	NCB_METHOD(drawTest);
}
