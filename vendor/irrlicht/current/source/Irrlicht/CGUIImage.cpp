// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGUIImage.h"
#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"

namespace irr
{
namespace gui
{



//! constructor
CGUIImage::CGUIImage(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
: IGUIImage(environment, parent, id, rectangle), Texture(0), UseAlphaChannel(false)
{
	#ifdef _DEBUG
	setDebugName("CGUIImage");
	#endif
}



//! destructor
CGUIImage::~CGUIImage()
{
	if (Texture)
		Texture->drop();
}



//! sets an image
void CGUIImage::setImage(video::ITexture* image)
{
	if (Texture)
		Texture->drop();

	Texture = image;

	if (Texture)
		Texture->grab();
}



//! draws the element and its children
void CGUIImage::draw()
{
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();
	irr::video::IVideoDriver* driver = Environment->getVideoDriver();

	core::rect<s32> rect = AbsoluteRect;

	if (Texture)
		driver->draw2DImage(Texture, AbsoluteRect.UpperLeftCorner, 
			core::rect<s32>(core::position2d<s32>(0,0), Texture->getOriginalSize()),
			&AbsoluteClippingRect, video::SColor(255,255,255,255), UseAlphaChannel);
	else
		driver->draw2DRectangle(skin->getColor(EGDC_3D_DARK_SHADOW), AbsoluteRect, &AbsoluteClippingRect);

	IGUIElement::draw();
}


//! sets if the image should use its alpha channel to draw itself
void CGUIImage::setUseAlphaChannel(bool use)
{
	UseAlphaChannel = use;
}


//! Writes attributes of the element.
void CGUIImage::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0)
{
	IGUIImage::serializeAttributes(out,options);

	out->addTexture	("Texture",			Texture);
	out->addBool	("UseAlphaChannel",	UseAlphaChannel);

}

//! Reads attributes of the element
void CGUIImage::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
{
	IGUIImage::deserializeAttributes(in,options);

	setImage(in->getAttributeAsTexture("Texture"));
	setUseAlphaChannel(in->getAttributeAsBool("UseAlphaChannel"));

}




} // end namespace gui
} // end namespace irr

