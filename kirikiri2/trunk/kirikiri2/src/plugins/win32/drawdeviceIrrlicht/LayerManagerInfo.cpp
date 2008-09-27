#include <windows.h>
#include "IrrlichtDrawDevice.h"
#include "LayerManagerInfo.h"

/**
 * コンストラクタ
 */
LayerManagerInfo::LayerManagerInfo(irr::video::ITexture *texture, int layerWidth, int layerHeight)
	: texture(texture), layerWidth(layerWidth), layerHeight(layerHeight)
{
	destBuffer = NULL;
};

/**
 * デストラクタ
 */
LayerManagerInfo::~LayerManagerInfo()
{
}


/**
 * テクスチャをロックして描画領域情報を取得する
 */
void
LayerManagerInfo::lock()
{
	if (texture) {
		destBuffer = (unsigned char *)texture->lock();
		irr::core::dimension2d<irr::s32> size = texture->getSize();
		destWidth  = size.Width;
		destHeight = size.Height;
		destPitch  = texture->getPitch();
	} else {
		destBuffer = NULL;
	}
}

/**
 * ロックされたテクスチャにビットマップ描画を行う
 */
void
LayerManagerInfo::copy(tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
					   const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity)
{
	// bits, bitmapinfo で表されるビットマップの cliprect の領域を、x, y に描画する。

	if (destBuffer) {
		int srcPitch = -bitmapinfo->bmiHeader.biWidth * 4; // XXX きめうち
		unsigned char *srcBuffer = (unsigned char *)bits - srcPitch * (bitmapinfo->bmiHeader.biHeight - 1);
		int srcx   = cliprect.left;
		int srcy   = cliprect.top;
		int width  = cliprect.get_width();
		int height = cliprect.get_height();
		// クリッピング
		if (x < 0) {
			srcx  += x;
			width += x;
			x = 0;
		}
		if (x + width > destWidth) {
			width -= ((x + width) - destWidth);
		}
		if (y < 0) {
			srcy += y;
			height += y;
			y = 0;
		}
		if (y + height > destHeight) {
			height -= ((y + height) - destHeight);
		}
		unsigned char *src  = srcBuffer  + srcy * srcPitch  + srcx * 4;
		unsigned char *dest = destBuffer +    y * destPitch +    x * 4;
		for (int i=0;i<height;i++) {
			memcpy(dest, src, width * 4);
			src  += srcPitch;
			dest += destPitch;
		}
	}
}

/**
 * テクスチャのロックの解除
 */
void
LayerManagerInfo::unlock()
{
	if (texture) {
		texture->unlock();
		destBuffer = NULL;
	}
}
