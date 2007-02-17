#include "layerExSWF.hpp"

#include "gameswf/gameswf.h"
#include "base/utility.h"
#include "base/container.h"
#include "base/tu_file.h"
#include "base/tu_types.h"

layerExSWF::layerExSWF(DispatchT obj) : layerExBase(obj)
{
	width = 0;
	height = 0;
	buffer = 0;
	pitch = 0;
	surface = NULL;
	cairo = NULL;

	// ------------------------------------
	// SWF 用パラメータ
	// ------------------------------------

	s_scale = 1.0f;
	s_antialiased = false;
	s_bit_depth = 16;
	s_background = true;
	s_measure_performance = false;
	s_event_thread = false;
}

layerExSWF::~layerExSWF()
{
	cairo_destroy(cairo);
	cairo_surface_destroy(surface);
}


static void
message_log(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	char msg[1024];
	_vsnprintf(msg, 1024, format, args);
	TVPAddLog(ttstr(msg));
	va_end(args);
}

static void
log_callback(bool error, const char* message)
{
	if (error) {
		TVPAddImportantLog(ttstr(message));
	} else {
		message_log(message);
	}
}

static tu_file*	file_opener(const char* url)
{
	return new tu_file(url, "rb");
}

static void	fs_callback(gameswf::movie_interface* movie, const char* command, const char* args)
{
	// FS コマンド用
	// 最終的に TJS 呼び出しに置換するべし
	message_log("fs_callback: '");
	message_log(command);
	message_log("' '");
	message_log(args);
	message_log("'\n");
}

static void	test_progress_callback(unsigned int loaded_tags, unsigned int total_tags)
{
	// プログレスバー表示用
	// 最終的に TJS の固有メソッド呼び出しを入れる
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

void PreRegistCallbackTest()
{
}

void PostRegistCallbackTest()
{
	gameswf::register_file_opener_callback(file_opener);
	gameswf::register_fscommand_callback(fs_callback);
	gameswf::register_log_callback(log_callback);
}

void PreUnregistCallbackTest()
{
	gameswf::clear();
}

void PostUnregistCallbackTest()
{
}

NCB_PRE_REGIST_CALLBACK(   PreRegistCallbackTest);
NCB_POST_REGIST_CALLBACK(  PostRegistCallbackTest);
NCB_PRE_UNREGIST_CALLBACK( PreUnregistCallbackTest);
NCB_POST_UNREGIST_CALLBACK(PostUnregistCallbackTest);
