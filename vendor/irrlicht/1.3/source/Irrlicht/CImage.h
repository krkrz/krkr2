// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IMAGE_H_INCLUDED__
#define __C_IMAGE_H_INCLUDED__

#include "IImage.h"
#include "rect.h"

namespace irr
{
namespace video  
{

//! IImage implementation with a lot of special image operations for
//! 16 bit A1R5G5B5/32 Bit A8R8G8B8 images, which are used by the SoftwareDevice.
class CImage : public IImage
{
public:

	//! constructor 
	CImage(ECOLOR_FORMAT format, IImage* imageToCopy);

	//! constructor 
	//! \param useForeignMemory: If true, the image will use the data pointer
	//! directly and own it from now on, which means it will also try to delete [] the
	//! data when the image will be destructed. If false, the memory will by copied.
	CImage(ECOLOR_FORMAT format, const core::dimension2d<s32>& size,
		void* data, bool ownForeignMemory=true, bool deleteMemory = true);

	//! constructor
	CImage(ECOLOR_FORMAT format, const core::dimension2d<s32>& size);

	//! constructor
	CImage(IImage* imageToCopy,
		const core::position2d<s32>& pos, const core::dimension2d<s32>& size);

	//! destructor
	virtual ~CImage();

	//! Lock function.
	virtual void* lock()
	{
		return Data;
	};

	//! Unlock function.
	virtual void unlock() {};

	//! Returns width and height of image data.
	virtual const core::dimension2d<s32>& getDimension();

	//! Returns bits per pixel. 
	virtual s32 getBitsPerPixel();

	//! Returns bytes per pixel
	virtual s32 getBytesPerPixel();

	//! Returns image data size in bytes
	virtual s32 getImageDataSizeInBytes();

	//! Returns image data size in pixels
	virtual s32 getImageDataSizeInPixels();

	//! returns mask for red value of a pixel
	virtual u32 getRedMask();

	//! returns mask for green value of a pixel
	virtual u32 getGreenMask();

	//! returns mask for blue value of a pixel
	virtual u32 getBlueMask();

	//! returns mask for alpha value of a pixel
	virtual u32 getAlphaMask();

	//! returns a pixel
	virtual SColor getPixel(s32 x, s32 y);

	//! sets a pixel
	virtual void setPixel(s32 x, s32 y, const SColor &color );

	//! returns the color format
	virtual ECOLOR_FORMAT getColorFormat() const;

	//! draws a rectangle
	void drawRectangle(const core::rect<s32>& rect, const SColor &color);

	//! copies this surface into another
	void copyTo(CImage* target, const core::position2d<s32>& pos);

	//! copies this surface into another
	void copyTo(CImage* target, const core::position2d<s32>& pos, const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect=0);

	//! copies this surface into another, using the alpha mask, an cliprect and a color to add with
	void copyToWithAlpha(CImage* target, const core::position2d<s32>& pos, const core::rect<s32>& sourceRect, const SColor &color, const core::rect<s32>* clipRect = 0);

	//! copies this surface into another, scaling it to fit it.
	void copyToScaling(CImage* target);

	//! copies this surface into another, scaling it to fit it, appyling a box filter
	void copyToScalingBoxFilter(CImage* target, s32 bias = 0);

	//! draws a line from to
	void drawLine(const core::position2d<s32>& from, const core::position2d<s32>& to, const SColor &color);
	
	//! fills the surface with black or white
	void fill(const SColor &color);

	//! returns pitch of image
	virtual u32 getPitch() const
	{
		return Pitch;
	}
	
	
private:

	//! assumes format and size has been set and creates the rest
	void initData();

	s32 getBitsPerPixelFromFormat();

	inline SColor getPixelBox ( s32 x, s32 y, s32 fx, s32 fy, s32 bias );

	void* Data;
	core::dimension2d<s32> Size;
	s32 BitsPerPixel;
	s32 BytesPerPixel;
	u32 Pitch;
	ECOLOR_FORMAT Format;

	bool DeleteMemory;

	u32 RedMask;
	u32 GreenMask;
	u32 BlueMask;
	u32 AlphaMask;
};

} // end namespace video
} // end namespace irr


#endif

