#ifndef _swfMovie_hpp_
#define _swfMovie_hpp_

#include "cairo.h"
#include "gameswf/gameswf.h"

class SWFMovie
{
private:
	gameswf::movie_definition *md;
	gameswf::movie_interface *m;
	int movie_version;
	int movie_width;
	int movie_height;
	float movie_fps;
public:
	SWFMovie();
	~SWFMovie();
	void load(const char *name);
	void setFrame(int frame);
	void draw(cairo_t *cairo);
};

#endif
