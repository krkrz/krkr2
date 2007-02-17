#ifndef _layExCairo_hpp_
#define _layExCairo_hpp_

#include <windows.h>
#include "layerExBase.hpp"
#include "cairo.h"

/*
 * Cairo 描画用レイヤ
 */
struct layerExCairo : public layerExBase
{
private:
	GeometryT width;
	GeometryT height;
	BufferT buffer;
	PitchT pitch;
	cairo_surface_t *surface;
	cairo_t * cairo;

public:
	layerExCairo(DispatchT obj);
	~layerExCairo();
	
public:
	/**
	 * レイヤ情報リセット処理
	 */
	virtual void reset();

	/**
	 * 描画テスト
	 */
	void drawTest(double x, double y, double w, double h);
};

#endif
