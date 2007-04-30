#include <windows.h>
#include "IrrlichtDrawDevice.h"
#include "SWFMovie.hpp"

extern void message_log(const char* format, ...);
extern void error_log(const char *format, ...);
extern void swfload(SWFMovie *swf, const char *name);

#include "base/utility.h"
#include "base/container.h"
#include "base/tu_file.h"
#include "base/tu_types.h"
#include "base/image.h"

using namespace gameswf;

// ------------------------------------------------------------
// SWF UI 処理用
// ------------------------------------------------------------

/**
 * SWF UI 用変数初期化
 */
void
tTVPIrrlichtDrawDevice::initSWF()
{
	uiSWF = NULL;
}

/**
 * SWF UI 変数破棄
 */
void
tTVPIrrlichtDrawDevice::deinitSWF()
{
	delete uiSWF;
}

/**
 * SWF UI のロード
 */
void
tTVPIrrlichtDrawDevice::loadSWF(const char *name)
{
	delete uiSWF;
	uiSWF = new SWFMovie();
	uiSWF->load(name);
	//swfload(uiSWF, name);
}

/**
 * SWF の描画処理
 */
void
tTVPIrrlichtDrawDevice::drawSWF(tjs_uint64 tick, int x, int y, int width, int height)
{
	if (uiSWF) {
		gameswf::set_render_handler(this);
		if (first) {
			prevTick = tick;
			first = false;
		}
		uiSWF->update((int)(tick - prevTick));
		uiSWF->draw(x, y, width, height);
		prevTick = tick;
	}
}

// --------------------------------------------------------
// SWF 関連機能
// --------------------------------------------------------

/**
 * ビットマップ情報
 */
struct bitmap_info_irr : public gameswf::bitmap_info
{
	bitmap_info_irr();
	bitmap_info_irr(int width, int height, Uint8* data);
	bitmap_info_irr(image::rgb* im);
	bitmap_info_irr(image::rgba* im);
	~bitmap_info_irr() {
	}
};

/**
 * コンストラクタ
 */
bitmap_info_irr::bitmap_info_irr()
{
}

/**
 * コンストラクタ
 */
bitmap_info_irr::bitmap_info_irr(int width, int height, Uint8* data)
{
}

/**
 * コンストラクタ
 */
bitmap_info_irr::bitmap_info_irr(image::rgb* im)
{
}

/**
 * コンストラクタ
 */
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
	/** なにもしない */
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
	m_display_width  = fabsf(x1 - x0);
	m_display_height = fabsf(y1 - y0);

	if (driver) {
		// 現在の状態をバックアップ
		m_projection_bak = driver->getTransform(irr::video::ETS_PROJECTION); 
		m_view_bak       = driver->getTransform(irr::video::ETS_VIEW); 
		m_world_bak      = driver->getTransform(irr::video::ETS_WORLD);

		irr::core::matrix4 prj, view, world;
		prj.makeIdentity();
		view.makeIdentity();
		world.makeIdentity(); 

		float gsWidthDiv  = 1.0f / viewport_width;
		float gsHeightDiv = 1.0f / viewport_height;
		
		prj(0,0) = 2.0f / (x1 - x0);
		prj(1,1) = -2.0f / (y1 - y0);
		prj(3,0) = -((x1 + x0) / (x1 - x0));
		prj(3,1) = ((y1 + y0) / (y1 - y0));
		
		view(0,0) = viewport_width * gsWidthDiv;
	    view(1,1) = viewport_height * gsHeightDiv;
		view(3,0) = -1.0f + viewport_x0 * 2.0f * gsWidthDiv + viewport_width * gsWidthDiv;
		view(3,1) = 1.0f - viewport_y0 * 2.0f * gsHeightDiv - viewport_height * gsHeightDiv;
		
		driver->setTransform(irr::video::ETS_PROJECTION, prj);
		driver->setTransform(irr::video::ETS_VIEW, view); 
		driver->setTransform(irr::video::ETS_WORLD, world); 
		
		// ビューポート指定
		irr::core::rect<irr::s32> area(viewport_x0, viewport_y0, viewport_x0 + viewport_width, viewport_y0 + viewport_height);
		driver->setViewPort(area);

		// 背景塗りつぶし XXX

		// 描画マテリアル指定
		// 光源処理を外す
		irr::video::SMaterial material;
		material.Lighting = false;
		driver->setMaterial(material);
	}
}

void
tTVPIrrlichtDrawDevice::end_display()
{
	if (driver) {
		driver->setTransform(irr::video::ETS_PROJECTION, m_projection_bak);
		driver->setTransform(irr::video::ETS_VIEW,       m_view_bak);
		driver->setTransform(irr::video::ETS_WORLD,      m_world_bak);
	}
}

// Geometric and color transforms for mesh and line_strip rendering.
void
tTVPIrrlichtDrawDevice::set_matrix(const matrix& m)
{
	m_current_matrix.makeIdentity();
	m_current_matrix(0,0) = m.m_[0][0];
	m_current_matrix(0,1) = m.m_[1][0];
	m_current_matrix(1,0) = m.m_[0][1];
	m_current_matrix(1,1) = m.m_[1][1];
	m_current_matrix(3,0) = m.m_[0][2];
	m_current_matrix(3,1) = m.m_[1][2];
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
	if (driver) {
		// かけかえ処理
		Sint16 *c = (Sint16*)coords;
		irr::core::array<irr::u16> indices;
		irr::core::array<irr::video::S3DVertex> vertices;
		for (int i=0; i < vertex_count; i++) {
			int n = i*2;
			indices.push_back(i);
			vertices.push_back(irr::video::S3DVertex(c[n], c[n+1], 0, 0,0,0, m_fill_color, 0, 0));
		}
		driver->setTransform(irr::video::ETS_WORLD, m_current_matrix);
		driver->drawVertexPrimitiveList(vertices.const_pointer(), vertex_count,
										indices.const_pointer(), vertex_count - 2, 
										irr::video::EVT_STANDARD, irr::scene::EPT_TRIANGLE_STRIP);
	}
}

// As above, but coords is in triangle list order.
void
tTVPIrrlichtDrawDevice::draw_triangle_list(const void *coords, int vertex_count)
{
	if (driver) {

		// かけかえ処理
		Sint16 *c = (Sint16*)coords;
		irr::core::array<irr::u16> indices;
		irr::core::array<irr::video::S3DVertex> vertices;
		for (int i=0; i < vertex_count; i++) {
			int n = i*2;
			indices.push_back(i);
			vertices.push_back(irr::video::S3DVertex(c[n], c[n+1], 0, 0,0,0, m_fill_color, 0, 0));
		}
		driver->setTransform(irr::video::ETS_WORLD, m_current_matrix);
		driver->drawVertexPrimitiveList(vertices.const_pointer(), vertex_count,
										indices.const_pointer(), vertex_count / 3, 
										irr::video::EVT_STANDARD, irr::scene::EPT_TRIANGLES);
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
	if (driver) {

		// かけかえ処理
		Sint16 *c = (Sint16*)coords;
		irr::core::array<irr::u16> indices;
		irr::core::array<irr::video::S3DVertex> vertices;
		for (int i=0; i < vertex_count; i++) {
			int n = i*2;
			indices.push_back(i);
			vertices.push_back(irr::video::S3DVertex(c[n], c[n+1], 0, 0,0,0, m_line_color, 0, 0));
		}
		driver->setTransform(irr::video::ETS_WORLD, m_current_matrix);
		driver->drawVertexPrimitiveList(vertices.const_pointer(), vertex_count,
										indices.const_pointer(), vertex_count - 1, 
										irr::video::EVT_STANDARD, irr::scene::EPT_LINE_STRIP);
	}
}

void
tTVPIrrlichtDrawDevice::fill_style_disable(int fill_side)
{
	if (driver) {
	}
}

void
tTVPIrrlichtDrawDevice::fill_style_color(int fill_side, gameswf::rgba c)
{
	m_fill_color.set(c.m_a, c.m_r, c.m_g, c.m_b);
}

void
tTVPIrrlichtDrawDevice::fill_style_bitmap(int fill_side, const gameswf::bitmap_info* bi, const gameswf::matrix& m, gameswf::render_handler::bitmap_wrap_mode wm)
{
	if (driver) {
	}
}

void
tTVPIrrlichtDrawDevice::line_style_disable()
{
	if (driver) {
	}
}

void
tTVPIrrlichtDrawDevice::line_style_color(gameswf::rgba c)
{
	m_line_color.set(c.m_a, c.m_r, c.m_g, c.m_b);
}

void
tTVPIrrlichtDrawDevice::line_style_width(float width)
{
	if (driver) {
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
	if (driver) {
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
