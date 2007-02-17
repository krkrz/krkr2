#ifndef _layExCairo_hpp_
#define _layExCairo_hpp_

#include <windows.h>
#include "layerExBase.hpp"

#include "cairo.h"

struct layerExSWF : public layerExBase
{
private:
	GeometryT width;
	GeometryT height;
	BufferT buffer;
	PitchT pitch;
	cairo_surface_t *surface;
	cairo_t * cairo;

public:
	layerExSWF(DispatchT obj);
	~layerExSWF();
	
public:
	/**
	 * レイヤ情報リセット処理
	 */
	virtual void reset();

	/**
	 * 描画テスト
	 */
	void drawTest(double x, double y, double w, double h);

	// --------------------------------------------------
	// SWF 関係情報
	// --------------------------------------------------
	
	float	s_scale;
	bool	s_antialiased;
	int	    s_bit_depth;
	bool	s_background;
	bool	s_measure_performance;
	bool	s_event_thread;

	int s_total_tags;
	int s_loaded_tags;
	
	int movie_version;
	int movie_width;
	int movie_height;
	float movie_fps;
	
};

#endif
