#include "SWFMovie.hpp"

#include "cairo.h"
#include "base/utility.h"
#include "base/container.h"
#include "base/tu_file.h"
#include "base/tu_types.h"
#include "base/image.h"
#include "gameswf/gameswf_impl.h"
#include "gameswf/gameswf_types.h"

using namespace gameswf;

// •`‰æƒ^[ƒQƒbƒg
static cairo_t *cairo_target;

struct bitmap_info_cairo : public gameswf::bitmap_info
{
	bitmap_info_cairo();
	bitmap_info_cairo(int width, int height, Uint8* data);
	bitmap_info_cairo(image::rgb* im);
	bitmap_info_cairo(image::rgba* im);
	~bitmap_info_cairo() {
	}
};

bitmap_info_cairo::bitmap_info_cairo()
{
}

bitmap_info_cairo::bitmap_info_cairo(int width, int height, Uint8* data)
{
}

bitmap_info_cairo::bitmap_info_cairo(image::rgb* im)
{
}

bitmap_info_cairo::bitmap_info_cairo(image::rgba* im)
{
}

struct render_handler_cairo : public gameswf::render_handler
{
	~render_handler_cairo() {}

	virtual bitmap_info* create_bitmap_info_empty() {
		return NULL;
	}
	
	virtual bitmap_info *create_bitmap_info_alpha(int w, int h, unsigned char* data) {
		return NULL;
	}
	
	virtual bitmap_info *create_bitmap_info_rgb(image::rgb* im) {
		return NULL;
	}
	
	virtual bitmap_info *create_bitmap_info_rgba(image::rgba* im) {
		return NULL;
	}
	
	virtual void delete_bitmap_info(bitmap_info* bi) {
	}
	
	// Bracket the displaying of a frame from a movie.
	// Fill the background color, and set up default
	// transforms, etc.
	virtual void	begin_display(rgba background_color,
								  int viewport_x0, int viewport_y0,
								  int viewport_width, int viewport_height,
								  float x0, float x1, float y0, float y1) {
		if (cairo_target) {

		}
	}

	virtual void	end_display() {
		if (cairo_target) {

		}
	}
		
	// Geometric and color transforms for mesh and line_strip rendering.
	virtual void	set_matrix(const matrix& m) {
		if (cairo_target) {
		}
	}
	
	virtual void	set_cxform(const cxform& cx) {
		if (cairo_target) {
		}
	}
		
	// Draw triangles using the current fill-style 0.
	// Clears the style list after rendering.
	//
	// coords is a list of (x,y) coordinate pairs, in
	// triangle-strip order.  The type of the array should
	// be Sint16[vertex_count*2]
	virtual void	draw_mesh_strip(const void* coords, int vertex_count) {
		if (cairo_target) {
		}
	}

	// As above, but coords is in triangle list order.
	virtual void	draw_triangle_list(const void* coords, int vertex_count) {
		if (cairo_target) {
		}
	}
		
	// Draw a line-strip using the current line style.
	// Clear the style list after rendering.
	//
	// Coords is a list of (x,y) coordinate pairs, in
	// sequence.  Each coord is a 16-bit signed integer.
	virtual void	draw_line_strip(const void* coords, int vertex_count) {
		if (cairo_target) {
		}
	}
		
	// Set line and fill styles for mesh & line_strip
	// rendering.
	enum bitmap_wrap_mode {
		WRAP_REPEAT,
		WRAP_CLAMP
	};

	virtual void	fill_style_disable(int fill_side) {
		if (cairo_target) {
		}
	}
	
	virtual void	fill_style_color(int fill_side, rgba color) {
		if (cairo_target) {
		}
	}

	virtual void	fill_style_bitmap(int fill_side, const bitmap_info* bi, const matrix& m, bitmap_wrap_mode wm) {
		if (cairo_target) {
		}
	}
		
	virtual void	line_style_disable() {
		if (cairo_target) {
		}
	}

	virtual void	line_style_color(rgba color) {
		if (cairo_target) {
		}
	}
	
	virtual void	line_style_width(float width) {
		if (cairo_target) {
		}
	}	

	// Special function to draw a rectangular bitmap;
	// intended for textured glyph rendering.  Ignores
	// current transforms.
	virtual void	draw_bitmap(
		const matrix&		m,
		const bitmap_info*	bi,
		const rect&		coords,
		const rect&		uv_coords,
		rgba			color) {
		if (cairo_target) {
		}
	}
	
	virtual void set_antialiased(bool enable) {
		if (cairo_target) {
		}
	}
	
	virtual void begin_submit_mask() {
		if (cairo_target) {
		}
	}
	
	virtual void end_submit_mask() {
		if (cairo_target) {
		}
	}

	virtual void disable_mask() {
		if (cairo_target) {
		}
	}
};

SWFMovie::SWFMovie()
{
}

SWFMovie::~SWFMovie()
{
	if (m) {
	}
	if (md) {
	}
}

void
SWFMovie::load(const char *name)
{
	md = gameswf::create_library_movie(name);
	if (md == NULL) {
	} else {
		m = gameswf::create_library_movie_inst(md);
		if (m == NULL) {
		}
	}
}

void
SWFMovie::setFrame(int frame)
{
	if (!m) {
		return;
	}
}

void
SWFMovie::draw(cairo_t *cairo)
{
	//gameswf::set_current_root(m);
	m->display();
}
