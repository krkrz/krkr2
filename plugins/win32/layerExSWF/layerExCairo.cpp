#include "layerExCairo.hpp"

/**
 * コンストラクタ
 */
layerExCairo::layerExCairo(DispatchT obj) : layerExBase(obj)
{
	width = 0;
	height = 0;
	buffer = 0;
	pitch = 0;
	surface = NULL;
	cairo = NULL;
}

/**
 * デストラクタ
 */
layerExCairo::~layerExCairo()
{
	cairo_destroy(cairo);
	cairo_surface_destroy(surface);
}

/**
 * リセット処理
 */
void
layerExCairo::reset()
{
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
layerExCairo::drawTest(double x, double y, double w, double h)
{
	if (cairo) {
		cairo_set_source_rgb (cairo, 0.25, 0.25, 0.25); /* dark gray */
		cairo_rectangle (cairo, x, y, w, h);
		cairo_fill (cairo);
	}
}
