// gameswf_render_handler_cairo

#include "gameswf.h"
#include "gameswf_types.h"
#include "base/image.h"
#include "base/utility.h"
#include <string.h>

struct bitmap_info_cairo : public gameswf::bitmap_info
{
	bitmap_info_cairo();
	bitmap_info_cairo(int width, int height, Uint8* data);
	bitmap_info_cairo(image::rgb* im);
	bitmap_info_cairo(image::rgba* im);
	~bitmap_info_cairo() {
	}
};

struct render_handler_cairo : public gameswf::render_handler
{
	// ターゲットレイヤを記録しておく必要あり

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
	}

	virtual void	end_display() {
	}
		
	// Geometric and color transforms for mesh and line_strip rendering.
	virtual void	set_matrix(const matrix& m) {
	}
	
	virtual void	set_cxform(const cxform& cx) {
	}
		
	// Draw triangles using the current fill-style 0.
	// Clears the style list after rendering.
	//
	// coords is a list of (x,y) coordinate pairs, in
	// triangle-strip order.  The type of the array should
	// be Sint16[vertex_count*2]
	virtual void	draw_mesh_strip(const void* coords, int vertex_count) {
	}

	// As above, but coords is in triangle list order.
	virtual void	draw_triangle_list(const void* coords, int vertex_count) {
	}
		
	// Draw a line-strip using the current line style.
	// Clear the style list after rendering.
	//
	// Coords is a list of (x,y) coordinate pairs, in
	// sequence.  Each coord is a 16-bit signed integer.
	virtual void	draw_line_strip(const void* coords, int vertex_count) {
	}
		
	// Set line and fill styles for mesh & line_strip
	// rendering.
	enum bitmap_wrap_mode {
		WRAP_REPEAT,
		WRAP_CLAMP
	};

	virtual void	fill_style_disable(int fill_side) {
	}
	
	virtual void	fill_style_color(int fill_side, rgba color) {
	}

	virtual void	fill_style_bitmap(int fill_side, const bitmap_info* bi, const matrix& m, bitmap_wrap_mode wm) {
	}
		
	virtual void	line_style_disable() {
	}

	virtual void	line_style_color(rgba color) {
	}
	
	virtual void	line_style_width(float width) {
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

	}
	
	virtual void set_antialiased(bool enable) {
	}
	
	virtual void begin_submit_mask() {
	}
	
	virtual void end_submit_mask() {
	}

	virtual void disable_mask() {
	}
};
