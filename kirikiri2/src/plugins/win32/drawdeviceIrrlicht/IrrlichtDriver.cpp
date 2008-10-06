#include "IrrlichtDriver.h"
#include "ncbind/ncbind.hpp"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace io;
using namespace gui;
 
IrrlichtDriver::IrrlichtDriver() : driver(NULL)
{
}

IrrlichtDriver::IrrlichtDriver(irr::video::IVideoDriver *driver) : driver(driver)
{
	if (driver) {
		driver->grab();
	}
}

IrrlichtDriver::IrrlichtDriver(const IrrlichtDriver &orig) : driver(orig.driver)
{
	if (driver) {
		driver->grab();
	}
}
	
IrrlichtDriver::~IrrlichtDriver()
{
	if (driver) {
		driver->drop();
		driver = NULL;
	}
}

/**
 * 画像のキャプチャ
 * @param dest 格納先レイヤ
 */
void
IrrlichtDriver::captureScreenShot(iTJSDispatch2 *dest)
{
	// レイヤインスタンス以外ではエラー

	if (dest->IsInstanceOf(TJS_IGNOREPROP,NULL,NULL,L"Layer",dest) != TJS_S_TRUE) {
		TVPThrowExceptionMessage(L"dest is not layer");
	}
	// 画像をキャプチャ
	IImage *screen;
	if (driver == NULL || (screen = driver->createScreenShot()) == NULL) {
		TVPThrowExceptionMessage(L"can't get screenshot");
	}

	dimension2d<s32> size = screen->getDimension();
	int width  = size.Width;
	int height = size.Height;

	ncbPropAccessor layer(dest);
	layer.SetValue(L"width",  width);
	layer.SetValue(L"height", height);
	layer.SetValue(L"imageLeft",  0);
	layer.SetValue(L"imageTop",   0);
	layer.SetValue(L"imageWidth",  width);
	layer.SetValue(L"imageHeight", height);

	unsigned char *buffer = (unsigned char*)layer.GetValue(L"mainImageBufferForWrite", ncbTypedefs::Tag<tjs_int>());
	int pitch = layer.GetValue(L"mainImageBufferPitch", ncbTypedefs::Tag<tjs_int>());

	// 画像データのコピー
	if (screen->getColorFormat() == ECF_A8R8G8B8) {
		unsigned char *src = (unsigned char*)screen->lock();
		int slen   = width * 4;
		int spitch = screen->getPitch();
		for (int y=0;y<height;y++) {
			memcpy(buffer, src, slen);
			src    += spitch;
			buffer += pitch;
		}
		screen->unlock();
	} else {
		for (int y=0;y<height;y++) {
			u32 *line = (u32 *)buffer;
			for (int x=0;x<width;x++) {
				*line++ = screen->getPixel(x, y).color;
				buffer += pitch;
			}
		}
	}

	dest->FuncCall(0, L"update", NULL, NULL, 0, NULL, dest);
	screen->drop();
}
