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

// 描画ターゲット
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
	// Enable/disable antialiasing.
	bool	m_enable_antialias;

	// Output size.
	float	m_display_width;
	float	m_display_height;
	
	gameswf::matrix	m_current_matrix;
	gameswf::cxform	m_current_cxform;

	virtual void set_antialiased(bool enable) {
		m_enable_antialias = enable;
	}
	
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

	gameswf::YUV_video*	create_YUV_video(int w, int h)
	{
		return NULL;
	}

	void	delete_YUV_video(gameswf::YUV_video* yuv)
	{
	}

	~render_handler_cairo()
	{
	}

	
	
	// Bracket the displaying of a frame from a movie.
	// Fill the background color, and set up default
	// transforms, etc.
	virtual void	begin_display(rgba background_color,
								  int viewport_x0, int viewport_y0,
								  int viewport_width, int viewport_height,
								  float x0, float x1, float y0, float y1) {

		m_display_width = fabsf(x1 - x0);
		m_display_height = fabsf(y1 - y0);

		if (cairo_target) {
			//
		}
	}

	virtual void	end_display() {
		if (cairo_target) {

		}
	}
		
	// Geometric and color transforms for mesh and line_strip rendering.
	virtual void	set_matrix(const matrix& m) {
		m_current_matrix = m;
	}
	
	virtual void	set_cxform(const cxform& cx) {
		m_current_cxform = cx;
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

	void fill_style_bitmap(int fill_side, const bitmap_info* bi, const matrix& m, render_handler::bitmap_wrap_mode wm) {
		
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

extern void message_log(const char* format, ...);
extern void error_log(const char *format, ...);

/**
 * ログ処理呼び出し
 */
static void
log_callback(bool error, const char* message)
{
	if (error) {
		error_log(message);
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

// レンダラ
static gameswf::render_handler *render = NULL;


/**
 * gamswf の初期化
 */
void
initSWFMovie()
{
	gameswf::register_file_opener_callback(file_opener);
	gameswf::register_fscommand_callback(fs_callback);
	gameswf::register_log_callback(log_callback);
	render = (gameswf::render_handler*)new render_handler_cairo;
	gameswf::set_render_handler(render);
}

/**
 * gameswf の破棄
 */
void
destroySWFMovie()
{
	gameswf::clear();
	delete render;
}

// ---------------------------------------------------------------
// ムービー情報処理クラス
// ---------------------------------------------------------------

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
	lastFrame = -1;
}

void
SWFMovie::draw(cairo_t *cairo, bool reseted, int width, int height)
{
	if (cairo && m) {
		cairo_target = cairo;
		int frame = m->get_current_frame();
		if (frame != lastFrame || reseted) {
			gameswf::set_current_root(m);
			m->set_display_viewport(0, 0, width, height);
			//m->set_background_alpha(s_background ? 1.0f : 0.05f);
			m->display();
		}
		lastFrame = frame;
	}
}

void
SWFMovie::update(int advance)
{
	if (m) {
		if (advance > 0) {
			m->advance(advance / 1000.0f);
		}
	}
}

void
SWFMovie::notifyMouse(int x, int y, int buttons)
{
	if (m) {
		m->notify_mouse_state(x, y, buttons);
	}
}

void
SWFMovie::play()
{
	if (m) {
		if (m->get_play_state() == gameswf::movie_interface::STOP) {
			m->set_play_state(gameswf::movie_interface::PLAY);
		}
	}
}

void
SWFMovie::stop()
{
	if (m) {
		if (m->get_play_state() == gameswf::movie_interface::PLAY) {
			m->set_play_state(gameswf::movie_interface::STOP);
		}
	}
}

void
SWFMovie::restart()
{
	if (m) {
		m->restart();
	}
}

void
SWFMovie::back()
{
	if (m) {
		m->goto_frame(m->get_current_frame()-1);
	}
}

void
SWFMovie::next()
{
	if (m) {
		m->goto_frame(m->get_current_frame()+1);
	}
}

void
SWFMovie::gotoFrame(int frame)
{
	if (m) {
		m->goto_frame(frame);
	}
}

