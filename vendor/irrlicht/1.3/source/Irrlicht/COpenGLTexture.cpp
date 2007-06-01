// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "irrTypes.h"
#include "COpenGLTexture.h"
#include "COpenGLDriver.h"
#include "os.h"
#include "CColorConverter.h"

#include "irrString.h"

namespace irr
{
namespace video
{

const bool checkFBOStatus(COpenGLDriver* Driver)
{
#ifdef GL_EXT_framebuffer_object
	GLenum status = Driver->extGlCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

	switch (status)
	{
		//Our FBO is perfect, return true
		case GL_FRAMEBUFFER_COMPLETE_EXT:
			return true;

		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
			os::Printer::log("FBO has invalid read buffer", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
			os::Printer::log("FBO has invalid draw buffer", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
			os::Printer::log("FBO has one or several incomplete image attachments", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			os::Printer::log("FBO has one or several image attachments with different internal formats", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			os::Printer::log("FBO has one or several image attachments with different dimensions", ELL_ERROR);
			break;

// not part of fbo_object anymore, but won't harm as it is just a return value
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT
		case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
			os::Printer::log("FBO has a duplicate image attachment", ELL_ERROR);
			break;
#endif

		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
			os::Printer::log("FBO missing an image attachment", ELL_ERROR);
			break;

		case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			os::Printer::log("FBO format unsupported", ELL_ERROR);
			break;

		default:
			break;
	}
	os::Printer::log("FBO error", ELL_ERROR);
#endif
	return false;
}

//! constructor
COpenGLTexture::COpenGLTexture(IImage* image, bool generateMipLevels, const char* name, COpenGLDriver* driver)
 : ITexture(name), Pitch(0), SurfaceHasSameSize(false), ImageData(0),
  ColorFormat(ECF_A8R8G8B8), Driver(driver), TextureName(0),
  InternalFormat(GL_RGBA), PixelFormat(GL_BGRA_EXT),
  PixelType(GL_UNSIGNED_BYTE), HasMipMaps(generateMipLevels),
  ColorFrameBuffer(0), DepthRenderBuffer(0), StencilRenderBuffer(0)
{
	#ifdef _DEBUG
	setDebugName("COpenGLTexture");
	#endif

	getImageData(image);

	if (ImageData)
	{
		glGenTextures(1, &TextureName);
		copyTexture();
	}
}

//! ColorFrameBuffer constructor
COpenGLTexture::COpenGLTexture(const core::dimension2d<s32>& size,
                                bool extPackedDepthStencilSupported,
                                const char* name,
                                COpenGLDriver* driver)
 : ITexture(name), ImageSize(size), Pitch(0), SurfaceHasSameSize(false),
  ImageData(0), ColorFormat(ECF_A8R8G8B8), Driver(driver), TextureName(0),
  InternalFormat(GL_RGBA), PixelFormat(GL_BGRA_EXT),
  PixelType(GL_UNSIGNED_BYTE), HasMipMaps(false),
  ColorFrameBuffer(0), DepthRenderBuffer(0), StencilRenderBuffer(0)
{
	#ifdef _DEBUG
	setDebugName("COpenGLTexture_FBO");
	#endif

	// generate color texture
	glGenTextures(1, &TextureName);
	glBindTexture(GL_TEXTURE_2D, TextureName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, ImageSize.Width,
		ImageSize.Height, 0, GL_RGBA, GL_INT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

#ifdef GL_EXT_packed_depth_stencil
	if (extPackedDepthStencilSupported)
	{
		// generate packed depth stencil texture
		glGenTextures(1, &DepthRenderBuffer);
		glBindTexture(GL_TEXTURE_2D, DepthRenderBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_STENCIL_EXT, ImageSize.Width,
			ImageSize.Height, 0, GL_DEPTH_STENCIL_EXT, GL_UNSIGNED_INT_24_8_EXT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		StencilRenderBuffer = DepthRenderBuffer; // stencil is packed with depth
	}
	else // generate separate stencil and depth textures
#endif
	{
		// generate depth texture
		glGenTextures(1, &DepthRenderBuffer);
		glBindTexture(GL_TEXTURE_2D, DepthRenderBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, ImageSize.Width,
			ImageSize.Height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // we 're in trouble! the code below does not complete the FBO currently...
        // stencil buffer is only supported with EXT_packed_depth_stencil extension (above)

//        // generate stencil texture
//        glGenTextures(1, &StencilRenderBuffer);
//        glBindTexture(GL_TEXTURE_2D, StencilRenderBuffer);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX, ImageSize.Width,
//                        ImageSize.Height, 0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, 0);
//        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

#ifdef GL_EXT_framebuffer_object
    // generate frame buffer
    Driver->extGlGenFramebuffersEXT(1, &ColorFrameBuffer);
    Driver->extGlBindFramebufferEXT(GL_FRAMEBUFFER_EXT, ColorFrameBuffer);

    // attach color texture to frame buffer
    Driver->extGlFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                         GL_COLOR_ATTACHMENT0_EXT,
                                         GL_TEXTURE_2D,
                                         TextureName,
                                         0);
    // attach depth texture to depth buffer
    Driver->extGlFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                         GL_DEPTH_ATTACHMENT_EXT,
                                         GL_TEXTURE_2D,
                                         DepthRenderBuffer,
                                         0);
    // attach stencil texture to stencil buffer
    Driver->extGlFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                         GL_STENCIL_ATTACHMENT_EXT,
                                         GL_TEXTURE_2D,
                                         StencilRenderBuffer,
                                         0);
    glGetError();

    // check the status
    if (!checkFBOStatus(Driver))
    {
        printf("FBO=%d, Color=%d, Depth=%d, Stencil=%d\n",
                ColorFrameBuffer, TextureName, DepthRenderBuffer, StencilRenderBuffer);
        if (ColorFrameBuffer)
            Driver->extGlDeleteFramebuffersEXT(1, &ColorFrameBuffer);
        if (DepthRenderBuffer)
            glDeleteTextures(1, &DepthRenderBuffer);
        if (StencilRenderBuffer && StencilRenderBuffer != DepthRenderBuffer)
            glDeleteTextures(1, &StencilRenderBuffer);
        ColorFrameBuffer = 0;
        DepthRenderBuffer = 0;
        StencilRenderBuffer = 0;
    }
    Driver->extGlBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
#endif
}

//! destructor
COpenGLTexture::~COpenGLTexture()
{
    if (ColorFrameBuffer)
        Driver->extGlDeleteFramebuffersEXT(1, &ColorFrameBuffer);
    if (DepthRenderBuffer)
        glDeleteTextures(1, &DepthRenderBuffer);
    if (StencilRenderBuffer && StencilRenderBuffer != DepthRenderBuffer)
        glDeleteTextures(1, &StencilRenderBuffer);
    ColorFrameBuffer = 0;
    DepthRenderBuffer = 0;
    StencilRenderBuffer = 0;

    glDeleteTextures(1, &TextureName);
	if (ImageData)
	{
		delete [] ImageData;
		ImageData=0;
	}
}


void COpenGLTexture::getImageData(IImage* image)
{
	if (!image)
	{
		os::Printer::log("No image for OpenGL texture.", ELL_ERROR);
		return;
	}

	ImageSize = image->getDimension();
	OriginalSize = ImageSize;

	if ( !ImageSize.Width || !ImageSize.Height)
	{
		os::Printer::log("Invalid size of image for OpenGL Texture.", ELL_ERROR);
		return;
	}

	core::dimension2d<s32> nImageSize;
	if (Driver && Driver->queryFeature(EVDF_TEXTURE_NPOT))
		nImageSize=ImageSize;
	else
	{
		nImageSize.Width = getTextureSizeFromSurfaceSize(ImageSize.Width);
		nImageSize.Height = getTextureSizeFromSurfaceSize(ImageSize.Height);
	}
	SurfaceHasSameSize=ImageSize==nImageSize;

	s32 bpp=0;
	if (image->getColorFormat()==ECF_R8G8B8)
	{
		bpp=4;
		ColorFormat = ECF_A8R8G8B8;
	}
	else
	{
		bpp=image->getBytesPerPixel();
		ColorFormat = image->getColorFormat();
	}

	Pitch = nImageSize.Width*bpp;
	ImageData = new u8[Pitch * nImageSize.Height];

	if (nImageSize == ImageSize)
	{
		void* source = image->lock();
		if (image->getColorFormat()==ECF_R8G8B8)
			CColorConverter::convert_R8G8B8toA8R8G8B8(source,ImageSize.Width*ImageSize.Height,ImageData);
		else
			memcpy(ImageData,source,Pitch*nImageSize.Height);
	}
	else
	{
		u8* source = (u8*)image->lock();
		// scale texture

		f32 sourceXStep = (f32)ImageSize.Width / (f32)nImageSize.Width;
		f32 sourceYStep = (f32)ImageSize.Height / (f32)nImageSize.Height;
		f32 sx,sy;

		// copy texture scaling
		sy = 0.0f;
		for (s32 y=0; y<nImageSize.Height; ++y)
		{
			sx = 0.0f;
			for (s32 x=0; x<nImageSize.Width; ++x)
			{
				s32 i=((s32)(((s32)sy)*ImageSize.Width + sx));
				if (image->getColorFormat()==ECF_R8G8B8)
				{
					i*=3;
					((s32*)ImageData)[y*nImageSize.Width + x]=SColor(255,source[i],source[i+1],source[i+2]).color;
				}
				else
					memcpy(&ImageData[(y*nImageSize.Width + x)*bpp],&source[i*bpp],bpp);
				sx+=sourceXStep;
			}
			sy+=sourceYStep;
		}
	}
	image->unlock();
	ImageSize = nImageSize;
}



//! copies the the texture into an open gl texture.
void COpenGLTexture::copyTexture(bool newTexture)
{
	glBindTexture(GL_TEXTURE_2D, TextureName);
	if (Driver->testGLError())
		os::Printer::log("Could not bind Texture", ELL_ERROR);

	switch (ColorFormat)
	{
		case ECF_A1R5G5B5:
			InternalFormat=GL_RGBA;
			PixelFormat=GL_BGRA_EXT;
			PixelType=GL_UNSIGNED_SHORT_1_5_5_5_REV;
			break;
		case ECF_R5G6B5:
			InternalFormat=GL_RGB;
			PixelFormat=GL_RGB;
			PixelType=GL_UNSIGNED_SHORT_5_6_5;
			break;
		case ECF_R8G8B8:
			InternalFormat=GL_RGB8;
			PixelFormat=GL_RGB;
			PixelType=GL_UNSIGNED_BYTE;
			break;
		case ECF_A8R8G8B8:
			InternalFormat=GL_RGBA;
			PixelFormat=GL_BGRA_EXT;
			PixelType=GL_UNSIGNED_INT_8_8_8_8_REV;
			break;
		default:
			os::Printer::log("Unsupported texture format", ELL_ERROR);
			break;
	}

	#ifndef DISABLE_MIPMAPPING
	if (HasMipMaps && Driver && Driver->queryFeature(EVDF_MIP_MAP_AUTO_UPDATE))
	{
		// automatically generate and update mipmaps
		glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
		AutomaticMipmapUpdate=true;
	}
	else
	{
		AutomaticMipmapUpdate=false;
		regenerateMipMapLevels();
	}
	if (HasMipMaps) // might have changed in regenerateMipMapLevels
	{
		// enable bilinear mipmap filter
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	#else
		HasMipMaps=false;
		os::Printer::log("Did not create OpenGL texture mip maps.", ELL_ERROR);
	#endif
	{
		// enable bilinear filter without mipmaps
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	if (newTexture)
		glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, ImageSize.Width,
			ImageSize.Height, 0, PixelFormat, PixelType, ImageData);
	else
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, ImageSize.Width,
			ImageSize.Height, PixelFormat, PixelType, ImageData);

	if (Driver->testGLError())
		os::Printer::log("Could not glTexImage2D", ELL_ERROR);
}



//! returns the size of a texture which would be the optimal size for rendering it
inline s32 COpenGLTexture::getTextureSizeFromSurfaceSize(s32 size)
{
	s32 ts = 0x01;
	while(ts < size)
		ts <<= 1;

	return ts;
}


//! lock function
void* COpenGLTexture::lock()
{
	return ImageData;
}



//! unlock function
void COpenGLTexture::unlock()
{
	copyTexture(false);
}



//! Returns original size of the texture.
const core::dimension2d<s32>& COpenGLTexture::getOriginalSize()
{
	return OriginalSize;
}



//! Returns (=size) of the texture.
const core::dimension2d<s32>& COpenGLTexture::getSize()
{
	return ImageSize;
}



//! returns driver type of texture (=the driver, who created the texture)
E_DRIVER_TYPE COpenGLTexture::getDriverType()
{
	return EDT_OPENGL;
}



//! returns color format of texture
ECOLOR_FORMAT COpenGLTexture::getColorFormat() const
{
	return ColorFormat;
}



//! returns pitch of texture (in bytes)
u32 COpenGLTexture::getPitch() const
{
	return Pitch;
}



//! return open gl texture name
GLuint COpenGLTexture::getOpenGLTextureName()
{
	return TextureName;
}



//! Returns whether this texture has mipmaps
//! return true if texture has mipmaps
bool COpenGLTexture::hasMipMaps()
{
	return HasMipMaps;
}



//! Regenerates the mip map levels of the texture. Useful after locking and
//! modifying the texture
//! MipMap updates are automatically performed by OpenGL.
void COpenGLTexture::regenerateMipMapLevels()
{
	if (AutomaticMipmapUpdate || !HasMipMaps)
		return;
		HasMipMaps=false;
	return;
	if (gluBuild2DMipmaps(GL_TEXTURE_2D, InternalFormat,
			ImageSize.Width, ImageSize.Height,
			PixelFormat, PixelType, ImageData))
		return;
	else
		HasMipMaps=false;
	return;

	// This code is wrong as it does not take into account the image scaling
	// Therefore it is currently disabled
	u32 width=ImageSize.Width>>1;
	u32 height=ImageSize.Height>>1;
	u32 i=1;
	while (width>1 || height>1)
	{
		//TODO: Add image scaling
		glTexImage2D(GL_TEXTURE_2D, i, InternalFormat, ImageSize.Width,
			ImageSize.Height, 0, PixelFormat, PixelType, ImageData);
		if (width>1)
			width>>=1;
		if (height>1)
			height>>=1;
		++i;
	}
}

bool COpenGLTexture::isFrameBufferObject()
{
    return ColorFrameBuffer != 0;
}

//! Bind ColorFrameBuffer (valid only if isFrameBufferObject() returns true).
void COpenGLTexture::bindFrameBufferObject()
{
#ifdef GL_EXT_framebuffer_object
    if (ColorFrameBuffer != 0)
        Driver->extGlBindFramebufferEXT(GL_FRAMEBUFFER_EXT, ColorFrameBuffer);
#endif
}

//! Unbind ColorFrameBuffer (valid only if isFrameBufferObject() returns true).
void COpenGLTexture::unbindFrameBufferObject()
{
#ifdef GL_EXT_framebuffer_object
    if (ColorFrameBuffer != 0)
        Driver->extGlBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
#endif
}


} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_OPENGL_
