#include <windows.h>
#include "IrrlichtDrawDevice.h"

extern void message_log(const char* format, ...);
extern void error_log(const char *format, ...);

// --------------------------------------------------------
// SWF 関連機能
// --------------------------------------------------------

#include "base/utility.h"
#include "base/container.h"
#include "base/tu_file.h"
#include "base/tu_types.h"
#include "base/image.h"

using namespace gameswf;

struct bitmap_info_irr : public gameswf::bitmap_info
{
	bitmap_info_irr();
	bitmap_info_irr(int width, int height, Uint8* data);
	bitmap_info_irr(image::rgb* im);
	bitmap_info_irr(image::rgba* im);
	~bitmap_info_irr() {
	}
};

bitmap_info_irr::bitmap_info_irr()
{
}

bitmap_info_irr::bitmap_info_irr(int width, int height, Uint8* data)
{
}

bitmap_info_irr::bitmap_info_irr(image::rgb* im)
{
}

bitmap_info_irr::bitmap_info_irr(image::rgba* im)
{
}

bitmap_info*
tTVPIrrlichtDrawDevice::create_bitmap_info_empty()
{
	return new bitmap_info_irr;
}

bitmap_info *
tTVPIrrlichtDrawDevice::create_bitmap_info_alpha(int w, int h, unsigned char* data)
{
	return new bitmap_info_irr(w, h, data);
}

bitmap_info *
tTVPIrrlichtDrawDevice::create_bitmap_info_rgb(image::rgb* im)
{
	return new bitmap_info_irr(im);
}

bitmap_info *
tTVPIrrlichtDrawDevice::create_bitmap_info_rgba(image::rgba* im)
{
	return new bitmap_info_irr(im);
}

void
tTVPIrrlichtDrawDevice::delete_bitmap_info(bitmap_info* bi)
{
	delete bi;
}

gameswf::YUV_video*
tTVPIrrlichtDrawDevice::create_YUV_video(int w, int h)
{
	return NULL;
}

void
tTVPIrrlichtDrawDevice::delete_YUV_video(gameswf::YUV_video* yuv)
{
}

// Bracket the displaying of a frame from a movie.
// Fill the background color, and set up default
// transforms, etc.
void
tTVPIrrlichtDrawDevice::begin_display(gameswf::rgba bc,
							  int viewport_x0, int viewport_y0,
							  int viewport_width, int viewport_height,
							  float x0, float x1, float y0, float y1)
{
	m_display_width = fabsf(x1 - x0);
	m_display_height = fabsf(y1 - y0);
	
	if (device) {
		// ビューポート補正
		double sx = (double)viewport_width / m_display_width;
		double sy = (double)viewport_height / m_display_height;
		double scale = sx < sy ? sx : sy;
		//cairo_matrix_init_scale(&viewport, scale, scale);
		//cairo_matrix_translate(&viewport, viewport_x0 - x0 * sx, viewport_y0 - y0 * sy);
		//cairo_set_matrix(ctarget, &viewport);
		
		// 背景塗りつぶし
		//			cairo_set_source_rgba(ctarget,
		//								  bc.m_r/255.0,
		//								  bc.m_g/255.0,
		//								  bc.m_b/255.0,
		//								  bc.m_a/255.0);
		//			cairo_rectangle(ctarget, x0, y0, m_display_width, m_display_height);
		//			cairo_fill(ctarget);
	}
}

void
tTVPIrrlichtDrawDevice::end_display()
{
}

// Geometric and color transforms for mesh and line_strip rendering.
void
tTVPIrrlichtDrawDevice::set_matrix(const matrix& m)
{
	//cairo_matrix_init(&m_current_matrix,
	//				  m.m_[0][0],
	//				  m.m_[1][0],
	//				  m.m_[0][1],
	//				  m.m_[1][1],
	//				  m.m_[0][2],
	//				  m.m_[1][2]);
}

void
tTVPIrrlichtDrawDevice::set_cxform(const cxform& cx)
{
	m_current_cxform = cx;
}


// Draw triangles using the current fill-style 0.
// Clears the style list after rendering.
//
// coords is a list of (x,y) coordinate pairs, in
// triangle-strip order.  The type of the array should
// be Sint16[vertex_count*2]
void
tTVPIrrlichtDrawDevice::draw_mesh_strip(const void* coords, int vertex_count)
{
	if (device) {
		Sint16 *c = (Sint16*)coords;
		int i = 0;
		//cairo_save(ctarget);
		//cairo_transform(ctarget, &m_current_matrix);
		//cairo_new_path(ctarget);
		while (i < vertex_count) {
			int n = i*2;
		//	cairo_move_to(ctarget, c[n  ], c[n+1]);
	//		cairo_line_to(ctarget, c[n+2], c[n+3]);
	//		cairo_line_to(ctarget, c[n+4], c[n+5]);
	//		cairo_close_path(ctarget);
			i++;
		}
	//	cairo_fill (ctarget);
	//	cairo_restore(ctarget);
	}
}

// As above, but coords is in triangle list order.
void
tTVPIrrlichtDrawDevice::draw_triangle_list(const void *coords, int vertex_count)
{
	if (device) {
		Sint16 *c = (Sint16*)coords;
		//cairo_save(ctarget);
		//cairo_transform(ctarget, &m_current_matrix);
		//cairo_new_path(ctarget);
		int i = 0;
		while (i < vertex_count) {
			int n = i*2;
			//cairo_move_to(ctarget, c[n  ], c[n+1]);
			//cairo_line_to(ctarget, c[n+2], c[n+3]);
			//cairo_line_to(ctarget, c[n+4], c[n+5]);
			//cairo_close_path(ctarget);
			i += 3;
		}
		//cairo_fill (ctarget);
		//cairo_restore(ctarget);
	}
}

// Draw a line-strip using the current line style.
// Clear the style list after rendering.
//
// Coords is a list of (x,y) coordinate pairs, in
// sequence.  Each coord is a 16-bit signed integer.
void
tTVPIrrlichtDrawDevice::draw_line_strip(const void* coords, int vertex_count)
{
	if (device) {
		Sint16 *c = (Sint16*)coords;
		//cairo_save(ctarget);
		//cairo_transform(ctarget, &m_current_matrix);
		//cairo_new_path(ctarget);
		//cairo_move_to(ctarget, c[0], c[1]);
		int i = 1;
		while (i < vertex_count) {
			int n = i*2;
			//cairo_line_to(ctarget, c[n], c[n+1]);
			i++;
		}
		//cairo_stroke(ctarget);
		//cairo_restore(ctarget);
	}
}

void
tTVPIrrlichtDrawDevice::fill_style_disable(int fill_side)
{
	if (device) {
	}
}

void
tTVPIrrlichtDrawDevice::fill_style_color(int fill_side, gameswf::rgba c)
{
	if (device) {
		//cairo_set_source_rgba(ctarget, c.m_r/255.0, c.m_g/255.0, c.m_b/255.0, c.m_a/255.0);
	}
}

void
tTVPIrrlichtDrawDevice::fill_style_bitmap(int fill_side, const gameswf::bitmap_info* bi, const gameswf::matrix& m, gameswf::render_handler::bitmap_wrap_mode wm)
{
	if (device) {
	}
}

void
tTVPIrrlichtDrawDevice::line_style_disable()
{
	if (device) {
	}
}

void
tTVPIrrlichtDrawDevice::line_style_color(gameswf::rgba c)
{
	if (device) {
		//cairo_set_source_rgba(ctarget, c.m_r/255.0, c.m_g/255.0, c.m_b/255.0, c.m_a/255.0);
	}
}

void
tTVPIrrlichtDrawDevice::line_style_width(float width)
{
	if (device) {
		//cairo_set_line_width(ctarget, width);
	}
}	

// Special function to draw a rectangular bitmap;
// intended for textured glyph rendering.  Ignores
// current transforms.
void
tTVPIrrlichtDrawDevice::draw_bitmap(const matrix&		m,
									const bitmap_info*	bi,
									const rect&		coords,
									const rect&		uv_coords,
									gameswf::rgba			color)
{
	if (device) {
	}
}

void
tTVPIrrlichtDrawDevice::set_antialiased(bool enable)
{
	m_enable_antialias = enable;
}

void
tTVPIrrlichtDrawDevice::begin_submit_mask()
{
}

void
tTVPIrrlichtDrawDevice::end_submit_mask()
{
}

void
tTVPIrrlichtDrawDevice::disable_mask()
{
}

// ---------------------------------------------------------------------------

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

/**
 * ファイルオープン処理
 */
static tu_file*
file_opener(const char* url)
{
	return new tu_file(url, "rb");
}

/**
 * FSコマンド呼び出し
 * 最終的に TJS 呼び出しに置換？
 * イベント処理に置換してもいいのかも
 */
static void
fs_callback(gameswf::movie_interface* movie, const char* command, const char* args)
{
	message_log("fs_callback: '");
	message_log(command);
	message_log("' '");
	message_log(args);
	message_log("'\n");
}

/**
 * プログレスバー表示用
 * 最終的に TJS の固有メソッド呼び出しを入れる？あるいは専用処理
 */
static void
test_progress_callback(unsigned int loaded_tags, unsigned int total_tags)
{
}

/**
 * gamswf の初期化
 */
void
initSWFMovie()
{
	gameswf::register_file_opener_callback(file_opener);
	gameswf::register_fscommand_callback(fs_callback);
	gameswf::register_log_callback(log_callback);
}

/**
 * gameswf の破棄
 */
void
destroySWFMovie()
{
	gameswf::clear();
}
