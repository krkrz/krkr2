//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Base Layer Bitmap implementation
//---------------------------------------------------------------------------
#include "tjsCommHead.h"
#pragma hdrstop

#include "LayerBitmapIntf.h"
#include "MsgIntf.h"
#include "Resampler.h"
#include "tvpgl.h"



//---------------------------------------------------------------------------
// intact ( does not affect ) gamma adjustment data
tTVPGLGammaAdjustData TVPIntactGammaAdjustData =
{ 1.0, 0, 255, 1.0, 0, 255, 1.0, 0, 255 };
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
#define RET_VOID
#define BOUND_CHECK(x) \
{ \
	tjs_int i; \
	if(rect.left < 0) rect.left = 0; \
	if(rect.top < 0) rect.top = 0; \
	if(rect.right > (i=GetWidth())) rect.right = i; \
	if(rect.bottom > (i=GetHeight())) rect.bottom = i; \
	if(rect.right - rect.left <= 0 || rect.bottom - rect.top <= 0) \
		return x; \
}

//---------------------------------------------------------------------------
// tTVPBaseBitmap
//---------------------------------------------------------------------------
tTVPBaseBitmap::tTVPBaseBitmap(tjs_uint w, tjs_uint h, tjs_uint bpp) :
		tTVPNativeBaseBitmap(w, h, bpp)
{
}
//---------------------------------------------------------------------------
tTVPBaseBitmap::~tTVPBaseBitmap()
{
}
//---------------------------------------------------------------------------
void tTVPBaseBitmap::SetSizeWithFill(tjs_uint w, tjs_uint h, tjs_uint32 fillvalue)
{
	// resize, and fill the expanded region with specified value.

	tjs_uint orgw = GetWidth();
	tjs_uint orgh = GetHeight();

	SetSize(w, h);

	if(w > orgw && h > orgh)
	{
		// both width and height were expanded
		tTVPRect rect;
		rect.left = orgw;
		rect.top = 0;
		rect.right = w;
		rect.bottom = h;
		Fill(rect, fillvalue);

		rect.left = 0;
		rect.top = orgh;
		rect.right = orgw;
		rect.bottom = h;
		Fill(rect, fillvalue);
	}
	else if(w > orgw)
	{
		// width was expanded
		tTVPRect rect;
		rect.left = orgw;
		rect.top = 0;
		rect.right = w;
		rect.bottom = h;
		Fill(rect, fillvalue);
	}
	else if(h > orgh)
	{
		// height was expanded
		tTVPRect rect;
		rect.left = 0;
		rect.top = orgh;
		rect.right = w;
		rect.bottom = h;
		Fill(rect, fillvalue);
	}
}
//---------------------------------------------------------------------------
tjs_uint32 tTVPBaseBitmap::GetPoint(tjs_int x, tjs_int y) const
{
	// get specified point's color or color index
	if(x < 0 || y < 0 || x >= (tjs_int)GetWidth() || y >= (tjs_int)GetHeight())
		TVPThrowExceptionMessage(TVPOutOfRectangle);

	if(Is32BPP())
		return  *( (const tjs_uint32*)GetScanLine(y) + x); // 32bpp
	else
		return  *( (const tjs_uint8*)GetScanLine(y) + x); // 8bpp
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::SetPoint(tjs_int x, tjs_int y, tjs_uint32 value)
{
	// set specified point's color(and opacity) or color index
	if(x < 0 || y < 0 || x >= (tjs_int)GetWidth() || y >= (tjs_int)GetHeight())
		TVPThrowExceptionMessage(TVPOutOfRectangle);

	if(Is32BPP())
		*( (tjs_uint32*)GetScanLineForWrite(y) + x) = value; // 32bpp
	else
		*( (tjs_uint8*)GetScanLine(y) + x) = value; // 8bpp

	return true;
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::SetPointMain(tjs_int x, tjs_int y, tjs_uint32 color)
{
	// set specified point's color (mask is not touched)
	// for 32bpp
	if(!Is32BPP()) TVPThrowExceptionMessage(TVPInvalidOperationFor8BPP);

	if(x < 0 || y < 0 || x >= (tjs_int)GetWidth() || y >= (tjs_int)GetHeight())
		TVPThrowExceptionMessage(TVPOutOfRectangle);

	tjs_uint32 *addr = (tjs_uint32*)GetScanLineForWrite(y) + x;
	*addr &= 0xff000000;
	*addr += color & 0xffffff;

	return true;
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::SetPointMask(tjs_int x, tjs_int y, tjs_int mask)
{
	// set specified point's mask (color is not touched)
	// for 32bpp
	if(!Is32BPP()) TVPThrowExceptionMessage(TVPInvalidOperationFor8BPP);

	if(x < 0 || y < 0 || x >= (tjs_int)GetWidth() || y >= (tjs_int)GetHeight())
		TVPThrowExceptionMessage(TVPOutOfRectangle);

	tjs_uint32 *addr = (tjs_uint32*)GetScanLineForWrite(y) + x;
	*addr &= 0x00ffffff;
	*addr += (mask & 0xff) << 24;

	return true;
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::Fill(tTVPRect rect, tjs_uint32 value)
{
	// fill target rectangle represented as "rect", with color ( and opacity )
	// passed by "value".
	// value must be : 0xAARRGGBB (for 32bpp) or 0xII ( for 8bpp )
	BOUND_CHECK(false);

	if(!IsIndependent())
	{
		if(rect.left == 0 && rect.top == 0 &&
			rect.right == (tjs_int)GetWidth() && rect.bottom == (tjs_int)GetHeight())
		{
			// cover overall
			IndependNoCopy(); // indepent with no-copy
		}
	}

	if(Is32BPP())
	{
		// 32bpp
		tjs_int pitch = GetPitchBytes();
		tjs_uint8 *sc = (tjs_uint8*)GetScanLineForWrite(rect.top);
		tjs_int height = rect.bottom - rect.top;
		tjs_int width = rect.right - rect.left;

		if(height * width >= 64*1024/4)
		{
			for(;rect.top < rect.bottom; rect.top++)
			{
				tjs_uint32 * p = (tjs_uint32*)sc + rect.left;
				TVPFillARGB_NC(p, width, value);
				sc += pitch;
			}
		}
		else
		{
			for(;rect.top < rect.bottom; rect.top++)
			{
				tjs_uint32 * p = (tjs_uint32*)sc + rect.left;
				TVPFillARGB(p, width, value);
				sc += pitch;
			}
		}
	}
	else
	{
		// 8bpp
		tjs_int pitch = GetPitchBytes();
		tjs_uint8 *sc = (tjs_uint8*)GetScanLineForWrite(rect.top);

		for(;rect.top < rect.bottom; rect.top++)
		{
			tjs_uint8 * p = (tjs_uint8*)sc + rect.left;
			memset(p, value, rect.right - rect.left);
			sc += pitch;
		}
	}

	return true;
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::FillColor(tTVPRect rect, tjs_uint32 color, tjs_int opa)
{
	// fill rectangle with specified color.
	// this ignores destination alpha (destination alpha will not change)
	// opa is fill opacity if opa is positive value.
	// negative value of opa is not allowed.
	BOUND_CHECK(false);

	if(!Is32BPP()) TVPThrowExceptionMessage(TVPInvalidOperationFor8BPP);

	if(opa == 0) return false; // no action

	if(opa < 0) opa = 0;
	if(opa > 255) opa = 255;

	tjs_int pitch = GetPitchBytes();
	tjs_uint8 *sc = (tjs_uint8*)GetScanLineForWrite(rect.top);
	tjs_int width = rect.right - rect.left;

	if(opa == 255)
	{
		// complete opaque fill
		for(;rect.top < rect.bottom; rect.top++)
		{
			tjs_uint32 * p = (tjs_uint32*)sc + rect.left;
			TVPFillColor(p, width, color);
			sc += pitch;
		}
	}
	else
	{
		// alpha fill
		for(;rect.top < rect.bottom; rect.top++)
		{
			tjs_uint32 * p = (tjs_uint32*)sc + rect.left;
			TVPConstColorAlphaBlend(p, width, color, opa);
			sc += pitch;
		}
	}

	return true;
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::BlendColor(tTVPRect rect, tjs_uint32 color, tjs_int opa,
	bool additive)
{
	// fill rectangle with specified color.
	// this considers destination alpha (additive or simple)

	BOUND_CHECK(false);

	if(!Is32BPP()) TVPThrowExceptionMessage(TVPInvalidOperationFor8BPP);

	if(opa == 0) return false; // no action
	if(opa < 0) opa = 0;
	if(opa > 255) opa = 255;

	if(opa == 255)
	{
		// complete opaque fill
		color |= 0xff000000;

		if(!IsIndependent())
		{
			if(rect.left == 0 && rect.top == 0 &&
				rect.right == (tjs_int)GetWidth() && rect.bottom == (tjs_int)GetHeight())
			{
				// cover overall
				IndependNoCopy(); // indepent with no-copy
			}
		}

		tjs_int pitch = GetPitchBytes();
		tjs_uint8 *sc = (tjs_uint8*)GetScanLineForWrite(rect.top);
		tjs_int width = rect.right - rect.left;

		for(;rect.top < rect.bottom; rect.top++)
		{
			tjs_uint32 * p = (tjs_uint32*)sc + rect.left;
			TVPFillARGB(p, width, color);
			sc += pitch;
		}
	}
	else
	{
		// alpha fill
		tjs_int pitch = GetPitchBytes();
		tjs_uint8 *sc = (tjs_uint8*)GetScanLineForWrite(rect.top);
		tjs_int width = rect.right - rect.left;

		if(!additive)
		{
			for(;rect.top < rect.bottom; rect.top++)
			{
				tjs_uint32 * p = (tjs_uint32*)sc + rect.left;
				TVPConstColorAlphaBlend_d(p, width, color, opa);
				sc += pitch;
			}
		}
		else
		{
			for(;rect.top < rect.bottom; rect.top++)
			{
				tjs_uint32 * p = (tjs_uint32*)sc + rect.left;
				TVPConstColorAlphaBlend_a(p, width, color, opa);
				sc += pitch;
			}
		}
	}

	return true;
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::RemoveConstOpacity(tTVPRect rect, tjs_int level)
{
	// remove constant opacity from bitmap. ( similar to PhotoShop's eraser tool )
	// level is a strength of removing ( 0 thru 255 )
	// this cannot work with additive alpha mode.

	BOUND_CHECK(false);

	if(!Is32BPP()) TVPThrowExceptionMessage(TVPInvalidOperationFor8BPP);

	if(level == 0) return false; // no action
	if(level < 0) level = 0;
	if(level > 255) level = 255;

	tjs_int pitch = GetPitchBytes();
	tjs_uint8 *sc = (tjs_uint8*)GetScanLineForWrite(rect.top);
	tjs_int width = rect.right - rect.left;

	if(level == 255)
	{
		// completely remove opacity
		for(;rect.top < rect.bottom; rect.top++)
		{
			tjs_uint32 * p = (tjs_uint32*)sc + rect.left;
			TVPFillMask(p, width, 0);
			sc += pitch;
		}
	}
	else
	{
		for(;rect.top < rect.bottom; rect.top++)
		{
			tjs_uint32 * p = (tjs_uint32*)sc + rect.left;
			TVPRemoveConstOpacity(p, width, level);
			sc += pitch;
		}

	}

	return true;
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::FillMask(tTVPRect rect, tjs_int value)
{
	// fill mask with specified value.
	BOUND_CHECK(false);

	if(!Is32BPP()) TVPThrowExceptionMessage(TVPInvalidOperationFor8BPP);

	tjs_int pitch = GetPitchBytes();
	tjs_uint8 *sc = (tjs_uint8*)GetScanLineForWrite(rect.top);
	tjs_int width = rect.right - rect.left;


	for(;rect.top < rect.bottom; rect.top++)
	{
		tjs_uint32 * p = (tjs_uint32*)sc + rect.left;
		TVPFillMask(p, width, value);
		sc += pitch;
	}

	return true;
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::CopyRect(tjs_int x, tjs_int y, const tTVPBaseBitmap *ref,
		tTVPRect refrect, tjs_int plane)
{
	// copy bitmap rectangle.
	// TVP_BB_COPY_MAIN in "plane" : main image is copied
	// TVP_BB_COPY_MASK in "plane" : mask image is copied
	// "plane" is ignored if the bitmap is 8bpp
	// the source rectangle is ( "refrect" ) and the destination upper-left corner
	// is (x, y).
	if(!Is32BPP()) plane = (TVP_BB_COPY_MASK|TVP_BB_COPY_MAIN);
	if(x == 0 && y == 0 && refrect.left == 0 && refrect.top == 0 &&
		refrect.right == (tjs_int)ref->GetWidth() &&
		refrect.bottom == (tjs_int)ref->GetHeight() &&
		(tjs_int)GetWidth() == refrect.right &&
		(tjs_int)GetHeight() == refrect.bottom &&
		plane == (TVP_BB_COPY_MASK|TVP_BB_COPY_MAIN) &&
		(bool)!Is32BPP() == (bool)!ref->Is32BPP())
	{
		// entire area of both bitmaps
		AssignBitmap(*ref);
		return true;
	}

	// bound check
	tjs_int bmpw, bmph;

	bmpw = ref->GetWidth();
	bmph = ref->GetHeight();

	if(refrect.left < 0)
		x -= refrect.left, refrect.left = 0;
	if(refrect.right > bmpw)
		refrect.right = bmpw;

	if(refrect.left >= refrect.right) return false;

	if(refrect.top < 0)
		y -= refrect.top, refrect.top = 0;
	if(refrect.bottom > bmph)
		refrect.bottom = bmph;

	if(refrect.top >= refrect.bottom) return false;

	bmpw = GetWidth();
	bmph = GetHeight();

	tTVPRect rect;
	rect.left = x;
	rect.top = y;
	rect.right = rect.left + refrect.get_width();
	rect.bottom = rect.top + refrect.get_height();

	if(rect.left < 0)
	{
		refrect.left += -rect.left;
		rect.left = 0;
	}

	if(rect.right > bmpw)
	{
		refrect.right -= (rect.right - bmpw);
		rect.right = bmpw;
	}

	if(refrect.left >= refrect.right) return false; // not drawable

	if(rect.top < 0)
	{
		refrect.top += -rect.top;
		rect.top = 0;
	}

	if(rect.bottom > bmph)
	{
		refrect.bottom -= (rect.bottom - bmph);
		rect.bottom = bmph;
	}

	if(refrect.top >= refrect.bottom) return false; // not drawable


	// transfer
	tjs_int pixelsize = (Is32BPP()?sizeof(tjs_uint32):sizeof(tjs_uint8));
	tjs_int dpitch = GetPitchBytes();
	tjs_int spitch = ref->GetPitchBytes();
	tjs_int w;
	tjs_int wbytes = (w = refrect.get_width()) * pixelsize;
	tjs_int h = refrect.get_height();

	if(ref == this && rect.top > refrect.top)
	{
		// backward copy
		tjs_uint8 * dest = (tjs_uint8*)GetScanLineForWrite(rect.bottom-1) +
			rect.left*pixelsize;
		const tjs_uint8 * src = (const tjs_uint8*)ref->GetScanLine(refrect.bottom-1) +
			refrect.left*pixelsize;

		switch(plane)
		{
		case TVP_BB_COPY_MAIN:
			while(h--)
			{
				TVPCopyColor((tjs_uint32*)dest, (const tjs_uint32*)src, w);
				dest -= dpitch;
				src -= spitch;
			}
			break;
		case TVP_BB_COPY_MASK:
			while(h--)
			{
				TVPCopyMask((tjs_uint32*)dest, (const tjs_uint32*)src, w);
				dest -= dpitch;
				src -= spitch;
			}
			break;
		case TVP_BB_COPY_MAIN|TVP_BB_COPY_MASK:
			while(h--)
			{
				memmove(dest, src, wbytes);
				dest -= dpitch;
				src -= spitch;
			}
			break;
		}
	}
	else
	{
		// forward copy
		tjs_uint8 * dest = (tjs_uint8*)GetScanLineForWrite(rect.top) +
			rect.left*pixelsize;
		const tjs_uint8 * src = (const tjs_uint8*)ref->GetScanLine(refrect.top) +
			refrect.left*pixelsize;

		switch(plane)
		{
		case TVP_BB_COPY_MAIN:
			while(h--)
			{
				TVPCopyColor((tjs_uint32*)dest, (const tjs_uint32*)src, w);
				dest += dpitch;
				src += spitch;
			}
			break;
		case TVP_BB_COPY_MASK:
			while(h--)
			{
				TVPCopyMask((tjs_uint32*)dest, (const tjs_uint32*)src, w);
				dest += dpitch;
				src += spitch;
			}
			break;
		case TVP_BB_COPY_MAIN|TVP_BB_COPY_MASK:
			while(h--)
			{
				memmove(dest, src, wbytes);
				dest += dpitch;
				src += spitch;
			}
			break;
		}
	}

	return true;
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::Blt(tjs_int x, tjs_int y, const tTVPBaseBitmap *ref,
		tTVPRect refrect, tTVPBBBltMethod method, tjs_int opa, bool hda)
{
	// blt src bitmap with various methods.

	// hda option ( hold destination alpha ) holds distination alpha,
	// but will select more complex function ( and takes more time ) for it ( if
	// can do )

	// this function does not matter whether source and destination bitmap is
	// overlapped.

	if(opa == 255 && method == bmCopy && !hda)
	{
		return CopyRect(x, y, ref, refrect);
	}

	if(!Is32BPP()) TVPThrowExceptionMessage(TVPInvalidOperationFor8BPP);

	if(opa == 0) return false; // opacity==0 has no action

	// bound check
	tjs_int bmpw, bmph;

	bmpw = ref->GetWidth();
	bmph = ref->GetHeight();

	if(refrect.left < 0)
		x -= refrect.left, refrect.left = 0;
	if(refrect.right > bmpw)
		refrect.right = bmpw;

	if(refrect.left >= refrect.right) return false;

	if(refrect.top < 0)
		y -= refrect.top, refrect.top = 0;
	if(refrect.bottom > bmph)
		refrect.bottom = bmph;

	if(refrect.top >= refrect.bottom) return false;

	bmpw = GetWidth();
	bmph = GetHeight();


	tTVPRect rect;
	rect.left = x;
	rect.top = y;
	rect.right = rect.left + refrect.get_width();
	rect.bottom = rect.top + refrect.get_height();

	if(rect.left < 0)
	{
		refrect.left += -rect.left;
		rect.left = 0;
	}

	if(rect.right > bmpw)
	{
		refrect.right -= (rect.right - bmpw);
		rect.right = bmpw;
	}

	if(refrect.left >= refrect.right) return false; // not drawable

	if(rect.top < 0)
	{
		refrect.top += -rect.top;
		rect.top = 0;
	}

	if(rect.bottom > bmph)
	{
		refrect.bottom -= (rect.bottom - bmph);
		rect.bottom = bmph;
	}

	if(refrect.top >= refrect.bottom) return false; // not drawable


	// blt
 	tjs_int dpitch = GetPitchBytes();
	tjs_int spitch = ref->GetPitchBytes();
	tjs_int w = refrect.get_width();
	tjs_int h = refrect.get_height();

	// Blt performs always forward transfer;
	// this does not consider the duplicated area of the same bitmap.

	tjs_uint8 * dest = (tjs_uint8*)GetScanLineForWrite(rect.top) +
		rect.left*sizeof(tjs_uint32);
	const tjs_uint8 * src = (const tjs_uint8*)ref->GetScanLine(refrect.top) +
		refrect.left*sizeof(tjs_uint32);

	switch(method)
	{
	case bmCopy:
		// constant ratio alpha blendng
		if(opa == 255 && hda)
		{
			while(h--)
				TVPCopyColor((tjs_uint32*)dest, (tjs_uint32*)src, w),
					dest+=dpitch, src+=spitch;
		}
		else if(!hda)
		{
			while(h--)
				TVPConstAlphaBlend((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
					dest+=dpitch, src+=spitch;
		}
		else
		{
			while(h--)
				TVPConstAlphaBlend_HDA((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
					dest+=dpitch, src+=spitch;
		}
		break;

	case bmCopyOnAlpha:
		// constant ratio alpha blending (assuming source is opaque)
		// with consideration of destination alpha
		if(opa == 255)
			while(h--)
				TVPCopyOpaqueImage((tjs_uint32*)dest, (tjs_uint32*)src, w),
				dest+=dpitch, src+=spitch;
		else
			while(h--)
				TVPConstAlphaBlend_d((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
				dest+=dpitch, src+=spitch;
		break;


	case bmAlpha:
		// alpha blending, ignoring destination alpha
		if(opa == 255)
		{
			if(!hda)
			{
				while(h--)
					TVPAlphaBlend((tjs_uint32*)dest, (tjs_uint32*)src, w),
					dest+=dpitch, src+=spitch;

			}
			else
			{
				while(h--)
					TVPAlphaBlend_HDA((tjs_uint32*)dest, (tjs_uint32*)src, w),
					dest+=dpitch, src+=spitch;
			}
		}
		else
		{
			if(!hda)
			{
				while(h--)
					TVPAlphaBlend_o((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
					dest+=dpitch, src+=spitch;
			}
			else
			{
				while(h--)
					TVPAlphaBlend_HDA_o((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
					dest+=dpitch, src+=spitch;
			}
		}
		break;

	case bmAlphaOnAlpha:
		// alpha blending, with consideration of destination alpha
		if(opa == 255)
			while(h--)
				TVPAlphaBlend_d((tjs_uint32*)dest, (tjs_uint32*)src, w),
				dest+=dpitch, src+=spitch;
		else
			while(h--)
				TVPAlphaBlend_do((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
				dest+=dpitch, src+=spitch;
		break;

	case bmAdd:
		// additive blending ( this does not consider distination alpha )
		if(opa == 255)
		{
			if(!hda)
			{
				while(h--)
					TVPAddBlend((tjs_uint32*)dest, (tjs_uint32*)src, w),
					dest+=dpitch, src+=spitch;
			}
			else
			{
				while(h--)
					TVPAddBlend_HDA((tjs_uint32*)dest, (tjs_uint32*)src, w),
					dest+=dpitch, src+=spitch;
			}
		}
		else
		{
			if(!hda)
			{
				while(h--)
					TVPAddBlend_o((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
					dest+=dpitch, src+=spitch;
			}
			else
			{
				while(h--)
					TVPAddBlend_HDA_o((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
					dest+=dpitch, src+=spitch;
			}
		}
		break;

	case bmSub:
		// subtractive blending ( this does not consider distination alpha )
		if(opa == 255)
		{
			if(!hda)
			{
				while(h--)
					TVPSubBlend((tjs_uint32*)dest, (tjs_uint32*)src, w),
					dest+=dpitch, src+=spitch;
			}
			else
			{
				while(h--)
					TVPSubBlend_HDA((tjs_uint32*)dest, (tjs_uint32*)src, w),
					dest+=dpitch, src+=spitch;
			}
		}
		else
		{
			if(!hda)
			{
				while(h--)
					TVPSubBlend_o((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
					dest+=dpitch, src+=spitch;
			}
			else
			{
				while(h--)
					TVPSubBlend_HDA_o((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
					dest+=dpitch, src+=spitch;
			}
		}
		break;

	case bmMul:
		// multiplicative blending ( this does not consider distination alpha )
		if(opa == 255)
		{
			if(!hda)
			{
				while(h--)
					TVPMulBlend((tjs_uint32*)dest, (tjs_uint32*)src, w),
					dest+=dpitch, src+=spitch;
			}
			else
			{
				while(h--)
					TVPMulBlend_HDA((tjs_uint32*)dest, (tjs_uint32*)src, w),
					dest+=dpitch, src+=spitch;
			}
		}
		else
		{
			if(!hda)
			{
				while(h--)
					TVPMulBlend_o((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
					dest+=dpitch, src+=spitch;
			}
			else
			{
				while(h--)
					TVPMulBlend_HDA_o((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
					dest+=dpitch, src+=spitch;
			}
		}
		break;


	case bmDodge:
		// color dodge mode ( this does not consider distination alpha )
		if(opa == 255)
		{
			if(!hda)
			{
				while(h--)
					TVPColorDodgeBlend((tjs_uint32*)dest, (tjs_uint32*)src, w),
					dest+=dpitch, src+=spitch;
			}
			else
			{
				while(h--)
					TVPColorDodgeBlend_HDA((tjs_uint32*)dest, (tjs_uint32*)src, w),
					dest+=dpitch, src+=spitch;
			}
		}
		else
		{
			if(!hda)
			{
				while(h--)
					TVPColorDodgeBlend_o((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
					dest+=dpitch, src+=spitch;
			}
			else
			{
				while(h--)
					TVPColorDodgeBlend_HDA_o((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
					dest+=dpitch, src+=spitch;
			}
		}
		break;


	case bmDarken:
		// darken mode ( this does not consider distination alpha )
		if(opa == 255)
		{
			if(!hda)
			{
				while(h--)
					TVPDarkenBlend((tjs_uint32*)dest, (tjs_uint32*)src, w),
					dest+=dpitch, src+=spitch;
			}
			else
			{
				while(h--)
					TVPDarkenBlend_HDA((tjs_uint32*)dest, (tjs_uint32*)src, w),
					dest+=dpitch, src+=spitch;
			}
		}
		else
		{
			if(!hda)
			{
				while(h--)
					TVPDarkenBlend_o((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
					dest+=dpitch, src+=spitch;
			}
			else
			{
				while(h--)
					TVPDarkenBlend_HDA_o((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
					dest+=dpitch, src+=spitch;
			}
		}
		break;


	case bmLighten:
		// lighten mode ( this does not consider distination alpha )
		if(opa == 255)
		{
			if(!hda)
			{
				while(h--)
					TVPLightenBlend((tjs_uint32*)dest, (tjs_uint32*)src, w),
					dest+=dpitch, src+=spitch;
			}
			else
			{
				while(h--)
					TVPLightenBlend_HDA((tjs_uint32*)dest, (tjs_uint32*)src, w),
					dest+=dpitch, src+=spitch;
			}
		}
		else
		{
			if(!hda)
			{
				while(h--)
					TVPLightenBlend_o((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
					dest+=dpitch, src+=spitch;
			}
			else
			{
				while(h--)
					TVPLightenBlend_HDA_o((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
					dest+=dpitch, src+=spitch;
			}
		}
		break;


	case bmScreen:
		// screen multiplicative mode ( this does not consider distination alpha )
		if(opa == 255)
		{
			if(!hda)
			{
				while(h--)
					TVPScreenBlend((tjs_uint32*)dest, (tjs_uint32*)src, w),
					dest+=dpitch, src+=spitch;
			}
			else
			{
				while(h--)
					TVPScreenBlend_HDA((tjs_uint32*)dest, (tjs_uint32*)src, w),
					dest+=dpitch, src+=spitch;
			}
		}
		else
		{
			if(!hda)
			{
				while(h--)
					TVPScreenBlend_o((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
					dest+=dpitch, src+=spitch;
			}
			else
			{
				while(h--)
					TVPScreenBlend_HDA_o((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
					dest+=dpitch, src+=spitch;
			}
		}
		break;


	case bmAddAlpha:
		// Additive Alpha
		if(opa == 255)
		{
			if(!hda)
			{
				while(h--)
					TVPAdditiveAlphaBlend((tjs_uint32*)dest, (tjs_uint32*)src, w),
					dest+=dpitch, src+=spitch;
			}
			else
			{
				while(h--)
					TVPAdditiveAlphaBlend_HDA((tjs_uint32*)dest, (tjs_uint32*)src, w),
					dest+=dpitch, src+=spitch;
			}
		}
		else
		{
			if(!hda)
			{
				while(h--)
					TVPAdditiveAlphaBlend_o((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
					dest+=dpitch, src+=spitch;
			}
			else
			{
				while(h--)
					TVPAdditiveAlphaBlend_HDA_o((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
					dest+=dpitch, src+=spitch;
			}
		}
		break;


	case bmAddAlphaOnAddAlpha:
		// Additive Alpha on Additive Alpha
		if(opa == 255)
		{
			while(h--)
				TVPAdditiveAlphaBlend_a((tjs_uint32*)dest, (tjs_uint32*)src, w),
				dest+=dpitch, src+=spitch;
		}
		else
		{
			while(h--)
				TVPAdditiveAlphaBlend_ao((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
				dest+=dpitch, src+=spitch;
		}
		break;


	case bmAddAlphaOnAlpha:
		// additive alpha on simple alpha
		// Not yet implemented
		break;

	case bmAlphaOnAddAlpha:
		// simple alpha on additive alpha
		if(opa == 255)
		{
			while(h--)
				TVPAlphaBlend_a((tjs_uint32*)dest, (tjs_uint32*)src, w),
				dest+=dpitch, src+=spitch;
		}
		else
		{
			while(h--)
				TVPAlphaBlend_ao((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
				dest+=dpitch, src+=spitch;
		}
		break;

	case bmCopyOnAddAlpha:
		// constant ratio alpha blending (assuming source is opaque)
		// with consideration of destination additive alpha
		if(opa == 255)
			while(h--)
				TVPCopyOpaqueImage((tjs_uint32*)dest, (tjs_uint32*)src, w),
				dest+=dpitch, src+=spitch;
		else
			while(h--)
				TVPConstAlphaBlend_a((tjs_uint32*)dest, (tjs_uint32*)src, w, opa),
				dest+=dpitch, src+=spitch;
		break;





	default:
				 ;
	}

	return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// template function for strech loop
//---------------------------------------------------------------------------

// define function pointer types for stretching line
typedef TVP_GL_FUNC_PTR_DECL(void, tTVPStretchFunction,
	(tjs_uint32 *dest, tjs_int len, const tjs_uint32 *src,
	tjs_int srcstart, tjs_int srcstep));
typedef TVP_GL_FUNC_PTR_DECL(void, tTVPStretchWithOpacityFunction,
	(tjs_uint32 *dest, tjs_int len, const tjs_uint32 *src,
	tjs_int srcstart, tjs_int srcstep, tjs_int opa));
typedef TVP_GL_FUNC_PTR_DECL(void, tTVPBilinearStretchFunction,
	(tjs_uint32 *dest, tjs_int destlen, const tjs_uint32 *src1, const tjs_uint32 *src2,
	tjs_int blend_y, tjs_int srcstart, tjs_int srcstep));
typedef TVP_GL_FUNC_PTR_DECL(void, tTVPBilinearStretchWithOpacityFunction,
	(tjs_uint32 *dest, tjs_int destlen, const tjs_uint32 *src1, const tjs_uint32 *src2,
	tjs_int blend_y, tjs_int srcstart, tjs_int srcstep, tjs_int opa));


//---------------------------------------------------------------------------

// declare stretching function object class
class tTVPStretchFunctionObject
{
	tTVPStretchFunction Func;
public:
	tTVPStretchFunctionObject(tTVPStretchFunction func) : Func(func) {;}
	void operator () (tjs_uint32 *dest, tjs_int len, const tjs_uint32 *src,
	tjs_int srcstart, tjs_int srcstep)
	{
		Func(dest, len, src, srcstart, srcstep);
	}
};

class tTVPStretchWithOpacityFunctionObject
{
	tTVPStretchWithOpacityFunction Func;
	tjs_int Opacity;
public:
	tTVPStretchWithOpacityFunctionObject(tTVPStretchWithOpacityFunction func, tjs_int opa) :
		Func(func), Opacity(opa) {;}
	void operator () (tjs_uint32 *dest, tjs_int len, const tjs_uint32 *src,
	tjs_int srcstart, tjs_int srcstep)
	{
		Func(dest, len, src, srcstart, srcstep, Opacity);
	}
};

class tTVPBilinearStretchFunctionObject
{
protected:
	tTVPBilinearStretchFunction Func;
public:
	tTVPBilinearStretchFunctionObject(tTVPBilinearStretchFunction func) : Func(func) {;}
	void operator () (tjs_uint32 *dest, tjs_int destlen, const tjs_uint32 *src1, const tjs_uint32 *src2,
	tjs_int blend_y, tjs_int srcstart, tjs_int srcstep)
	{
		Func(dest, destlen, src1, src2, blend_y, srcstart, srcstep);
	}
};

#define TVP_DEFINE_BILINEAR_STRETCH_FUNCTION(func, one) class \
t##func##FunctionObject : \
	public tTVPBilinearStretchFunctionObject \
{ \
public: \
	t##func##FunctionObject() : \
		tTVPBilinearStretchFunctionObject(func) {;} \
	void DoOnePixel(tjs_uint32 *dest, tjs_uint32 color) \
	{ one; } \
};


class tTVPBilinearStretchWithOpacityFunctionObject
{
protected:
	tTVPBilinearStretchWithOpacityFunction Func;
	tjs_int Opacity;
public:
	tTVPBilinearStretchWithOpacityFunctionObject(tTVPBilinearStretchWithOpacityFunction func, tjs_int opa) :
		Func(func), Opacity(opa) {;}
	void operator () (tjs_uint32 *dest, tjs_int destlen, const tjs_uint32 *src1, const tjs_uint32 *src2,
	tjs_int blend_y, tjs_int srcstart, tjs_int srcstep)
	{
		Func(dest, destlen, src1, src2, blend_y, srcstart, srcstep, Opacity);
	}
};

#define TVP_DEFINE_BILINEAR_STRETCH_WITH_OPACITY_FUNCTION(func, one) class \
t##func##FunctionObject : \
	public tTVPBilinearStretchWithOpacityFunctionObject \
{ \
public: \
	t##func##FunctionObject(tjs_int opa) : \
		tTVPBilinearStretchWithOpacityFunctionObject(func, opa) {;} \
	void DoOnePixel(tjs_uint32 *dest, tjs_uint32 color) \
	{ one; } \
};

//---------------------------------------------------------------------------

// declare streting function object for bilinear interpolation
TVP_DEFINE_BILINEAR_STRETCH_FUNCTION(
	TVPInterpStretchCopy,
	*dest = color);

TVP_DEFINE_BILINEAR_STRETCH_WITH_OPACITY_FUNCTION(
	TVPInterpStretchConstAlphaBlend,
	*dest = TVPBlendARGB(*dest, color, Opacity));

//---------------------------------------------------------------------------

// declare stretching loop function
#define TVP_DoStretchLoop_ARGS  x_ref_start, y_ref_start, x_len, y_len, \
						destp, destpitch, x_step, \
						y_step, refp, refpitch
template <typename tFunc>
void TVPDoStretchLoop(
		tFunc func,
		tjs_int x_ref_start,
		tjs_int y_ref_start,
		tjs_int x_len, tjs_int y_len,
		tjs_uint8 * destp, tjs_int destpitch,
		tjs_int x_step, tjs_int y_step,
		const tjs_uint8 * refp, tjs_int refpitch)
{
	tjs_int y_ref = y_ref_start;
	while(y_len--)
	{
		func(
			(tjs_uint32*)destp,
			x_len,
			(const tjs_uint32*)(refp + (y_ref>>16)*refpitch),
			x_ref_start,
			x_step);
		y_ref += y_step;
		destp += destpitch;
	}
}
//---------------------------------------------------------------------------


// declare stretching loop function for bilinear interpolation

#define TVP_DoBilinearStretchLoop_ARGS  rw, rh, dw, dh, \
						refw, refh, x_ref_start, y_ref_start, x_len, y_len, \
						destp, destpitch, x_step, \
						y_step, refp, refpitch
template <typename tStretchFunc>
void TVPDoBiLinearStretchLoop(
		tStretchFunc stretch,
		tjs_int rw, tjs_int rh,
		tjs_int dw, tjs_int dh,
		tjs_int refw, tjs_int refh,
		tjs_int x_ref_start,
		tjs_int y_ref_start,
		tjs_int x_len, tjs_int y_len,
		tjs_uint8 * destp, tjs_int destpitch,
		tjs_int x_step, tjs_int y_step,
		const tjs_uint8 * refp, tjs_int refpitch)
{
/*
	memo
          0         1         2         3         4
          .         .         .         .                  center = 1.5 = (4-1) / 2
          ------------------------------                 reference area
     ----------++++++++++----------++++++++++
                         ^                                 4 / 1  step 4   ofs =  1.5   = ((4-1) - (1-1)*4) / 2
               ^                   ^                       4 / 2  step 2   ofs =  0.5   = ((4-1) - (2-1)*2) / 2
          ^         ^         ^         *                  4 / 4  step 1   ofs =  0     = ((4-1) - (4-1)*1) / 2
         *       ^       ^       ^       *                 4 / 5  steo 0.8 ofs = -0.1   = ((4-1) - (5-1)*0.8) / 2
        *    ^    ^    ^    ^    ^    ^    *               4 / 8  step 0.5 ofs = -0.25

*/



	// adjust start point
	if(x_step >= 0)
		x_ref_start += (((rw-1)<<16) - (dw-1)*x_step)/2;
	else
		x_ref_start -= (((rw-1)<<16) + (dw-1)*x_step)/2 - x_step;
	if(y_step >= 0)
		y_ref_start += (((rh-1)<<16) - (dh-1)*y_step)/2;
	else
		y_ref_start -= (((rh-1)<<16) + (dh-1)*y_step)/2 - y_step;

	// horizontal destination line is splitted into three parts;
	// 1. left fraction (x_ref < 0               (lf)
	//                or x_ref >= refw - 1)
	// 2. center                                 (c)
	// 3. right fraction (x_ref >= refw - 1      (rf)
	//                or x_ref < 0)

	tjs_int ref_right_limit = (refw-1)<<16;

	tjs_int y_ref = y_ref_start;
	while(y_len--)
	{
		tjs_int y1 = y_ref >> 16;
		tjs_int y2 = y1+1;
		tjs_int y_blend = (y_ref & 0xffff) >> 8;
		if(y1 < 0) y1 = 0; else if(y1 >= refh) y1 = refh-1;
		if(y2 < 0) y2 = 0; else if(y2 >= refh) y2 = refh-1;

		const tjs_uint32 * l1 =
			(const tjs_uint32*)(refp + refpitch * y1);
		const tjs_uint32 * l2 =
			(const tjs_uint32*)(refp + refpitch * y2);


		// perform left and right fractions
		tjs_int x_remain = x_len;
		tjs_uint32 * dp;
		tjs_int x_ref;

		// from last point
		if(x_remain)
		{
			dp = (tjs_uint32*)destp + (x_len - 1);
			x_ref = x_ref_start + (x_len - 1) * x_step;
			if(x_ref < 0 && x_remain)
			{
				tjs_uint color =
					TVPBlendARGB(*l1, *l2, y_blend);
				do
					stretch.DoOnePixel(dp, color), dp-- , x_ref -= x_step, x_remain --;
				while(x_ref < 0 && x_remain);
			}
			else if(x_ref >= ref_right_limit)
			{
				tjs_uint color =
					TVPBlendARGB(
						*(l1 + refw-1),
						*(l2 + refw-2), y_blend);
				do
					stretch.DoOnePixel(dp, color), dp-- , x_ref -= x_step, x_remain --;
				while(x_ref >= ref_right_limit && x_remain);
			}
		}

		// from first point
		if(x_remain)
		{
			dp = (tjs_uint32*)destp;
			x_ref = x_ref_start;
			if(x_ref < 0)
			{
				tjs_uint color =
					TVPBlendARGB(*l1, *l2, y_blend);
				do
					stretch.DoOnePixel(dp, color), dp++ , x_ref += x_step, x_remain --;
				while(x_ref < 0 && x_remain);
			}
			else if(x_ref >= ref_right_limit)
			{
				tjs_uint color =
					TVPBlendARGB(
						*(l1 + refw-1),
						*(l2 + refw-2), y_blend);
				do
					stretch.DoOnePixel(dp, color), dp++ , x_ref += x_step, x_remain --;
				while(x_ref >= ref_right_limit && x_remain);
			}
		}

		// perform center part
		// (this may take most time of this function)
		if(x_remain)
		{
			stretch(
				dp,
				x_remain,
				l1, l2, y_blend,
				x_ref,
				x_step);
		}

		// step to the next line
		y_ref += y_step;
		destp += destpitch;
	}
}

//---------------------------------------------------------------------------
bool tTVPBaseBitmap::StretchBlt(tTVPRect cliprect,
		tTVPRect destrect, const tTVPBaseBitmap *ref,
		tTVPRect refrect, tTVPBBBltMethod method, tjs_int opa,
			bool hda, tTVPBBStretchType mode)
{
	// do stretch blt
	// stFastLinear is enabled only in following condition:
	// -------TODO: write corresponding condition--------

	// stLinear and stCubic mode are enabled only in following condition:
	// any magnification, opa:255, method:bmCopy, hda:false
	// no reverse, destination rectangle is within the image.

	tjs_int dw = destrect.get_width(), dh = destrect.get_height();
	tjs_int rw = refrect.get_width(), rh = refrect.get_height();

	if(!dw || !rw || !dh || !rh) return false; // nothing to do

	if(dw == rw && dh == rh)
	{
		return Blt(destrect.left, destrect.top, ref, refrect, method, opa, hda);
			// no stretch; do normal blt
	}

	if(!Is32BPP()) TVPThrowExceptionMessage(TVPInvalidOperationFor8BPP);

	// compute pitch, step, etc. needed for stretching copy/blt

	tjs_int w = GetWidth();
	tjs_int h = GetHeight();
	tjs_int refw = ref->GetWidth();
	tjs_int refh = ref->GetHeight();


	// check clipping rect
	if(cliprect.left < 0) cliprect.left = 0;
	if(cliprect.top < 0) cliprect.top = 0;
	if(cliprect.right > w) cliprect.right = w;
	if(cliprect.bottom > h) cliprect.bottom = h;

	// check mode
	if((mode == stLinear || mode == stCubic) && !hda && opa==255 && method==bmCopy
		&& dw > 0 && dh > 0 && rw > 0 && rh > 0 &&
		destrect.left >= cliprect.left && destrect.top >= cliprect.top &&
		destrect.right <= cliprect.right && destrect.bottom <= cliprect.bottom)
	{
		// takes another routine
		TVPResampleImage(this, destrect, ref, refrect, mode==stLinear?1:2);
		return true;
	}


	// check transfer direction
	bool x_dir = true, y_dir = true; // direction;  true:forward, false:backward

	if(dw < 0)
		x_dir = !x_dir, std::swap(destrect.left, destrect.right), dw = -dw;
	if(dh < 0)
		y_dir = !y_dir, std::swap(destrect.top, destrect.bottom), dh = -dh;

	if(rw < 0)
		x_dir = !x_dir, std::swap(refrect.left, refrect.right), rw = -rw;
	if(rh < 0)
		y_dir = !y_dir, std::swap(refrect.top, refrect.bottom), rh = -rh;

	// ref bound check
	if(refrect.left >= refrect.right ||
		refrect.top >= refrect.bottom) return false;
	if(refrect.left < 0 || refrect.right > refw ||
		refrect.top < 0 || refrect.bottom > refh)
		TVPThrowExceptionMessage(TVPSrcRectOutOfBitmap);

	// compute step
	tjs_int x_step = (rw << 16) / dw;
	tjs_int y_step = (rh << 16) / dh;

//	bool x_2x = x_step == 32768;
//	bool y_2x = y_step == 32768;

	tjs_int x_ref_start, y_ref_start;
	tjs_int x_dest_start, y_dest_start;
	tjs_int x_len, y_len;

	if(x_dir)
	{
		// x; forward
		if(destrect.left < cliprect.left)
			x_dest_start = cliprect.left,
			x_ref_start = (refrect.left << 16) + x_step * (cliprect.left - destrect.left);
		else
			x_dest_start = destrect.left,
			x_ref_start = (refrect.left << 16);

		if(destrect.right > cliprect.right)
			x_len = cliprect.right - x_dest_start;
		else
			x_len = destrect.right - x_dest_start;
	}
	else
	{
		// x; backward
		x_step = -x_step;

		if(destrect.left < cliprect.left)
			x_ref_start = ((refrect.right << 16) - 1) + x_step * (cliprect.left - destrect.left),
			x_dest_start = cliprect.left;
		else
			x_ref_start = ((refrect.right << 16) - 1),
			x_dest_start = destrect.left;

		if(destrect.right > cliprect.right)
			x_len = cliprect.right - x_dest_start;
		else
			x_len = destrect.right - x_dest_start;
	}
	if(x_len <= 0) return false;

	if(y_dir)
	{
		// y; forward
		if(destrect.top < cliprect.top)
			y_dest_start = cliprect.top,
			y_ref_start = (refrect.top << 16) + y_step * (cliprect.top - destrect.top);
		else
			y_dest_start = destrect.top,
			y_ref_start = (refrect.top << 16);

		if(destrect.bottom > cliprect.bottom)
			y_len = cliprect.bottom - y_dest_start;
		else
			y_len = destrect.bottom - y_dest_start;
	}
	else
	{
		// y; backward
		y_step = -y_step;

		if(destrect.top < cliprect.top)
			y_ref_start = ((refrect.bottom << 16) - 1) + y_step * (cliprect.top - destrect.top),
			y_dest_start = cliprect.top;
		else
			y_ref_start = ((refrect.bottom << 16) - 1),
			y_dest_start = destrect.top;

		if(destrect.bottom > cliprect.bottom)
			y_len = cliprect.bottom - y_dest_start;
		else
			y_len = destrect.bottom - y_dest_start;
	}

	if(y_len <= 0) return false;

	// independent check
	if(method == bmCopy && opa == 255 && !hda && !IsIndependent())
	{
		if(destrect.left == 0 && destrect.top == 0 &&
			destrect.right == (tjs_int)GetWidth() && destrect.bottom == (tjs_int)GetHeight())
		{
			// cover overall
			IndependNoCopy(); // indepent with no-copy
		}
	}

	const tjs_uint8 * refp = (const tjs_uint8*)ref->GetScanLine(0);
	tjs_uint8 * destp = (tjs_uint8*)GetScanLineForWrite(y_dest_start);
	destp += x_dest_start * sizeof(tjs_uint32);
	tjs_int refpitch = ref->GetPitchBytes();
	tjs_int destpitch = GetPitchBytes();


	// transfer
	switch(method)
	{
	case bmCopy:
		if(opa == 255)
		{
			// stretching copy
			if(mode >= stFastLinear)
			{
				if(!hda)
				{
					TVPDoBiLinearStretchLoop( // bilinear interpolation
						tTVPInterpStretchCopyFunctionObject(),
						TVP_DoBilinearStretchLoop_ARGS);
				}
				else
				{
					TVPDoStretchLoop(tTVPStretchFunctionObject(TVPStretchColorCopy),
						TVP_DoStretchLoop_ARGS);
				}
			}
			else
			{
				if(!hda)
					TVPDoStretchLoop(tTVPStretchFunctionObject(TVPStretchCopy),
						TVP_DoStretchLoop_ARGS);
				else
					TVPDoStretchLoop(tTVPStretchFunctionObject(TVPStretchColorCopy),
						TVP_DoStretchLoop_ARGS);
			}
		}
		else
		{
			// stretching constant ratio alpha blendng
			if(mode >= stFastLinear)
			{
				// bilinear interpolation
				if(!hda)
				{
					// adjust start point
					TVPDoBiLinearStretchLoop( // bilinear interpolation
						tTVPInterpStretchConstAlphaBlendFunctionObject(opa),
						TVP_DoBilinearStretchLoop_ARGS);
				}
				else
				{
					TVPDoStretchLoop(
						tTVPStretchWithOpacityFunctionObject(TVPStretchConstAlphaBlend_HDA, opa),
						TVP_DoStretchLoop_ARGS);
				}
			}
			else
			{
				if(!hda)
					TVPDoStretchLoop(
						tTVPStretchWithOpacityFunctionObject(TVPStretchConstAlphaBlend, opa),
						TVP_DoStretchLoop_ARGS);
				else
					TVPDoStretchLoop(
						tTVPStretchWithOpacityFunctionObject(TVPStretchConstAlphaBlend_HDA, opa),
						TVP_DoStretchLoop_ARGS);
			}
		}
		break;

	case bmCopyOnAlpha:
		// constant ratio alpha blending, with consideration of destination alpha
		if(opa == 255)
		{
			// full opaque stretching copy
			TVPDoStretchLoop(
				tTVPStretchFunctionObject(TVPStretchCopyOpaqueImage),
				TVP_DoStretchLoop_ARGS);
		}
		else
		{
			// stretching constant ratio alpha blending
			TVPDoStretchLoop(
				tTVPStretchWithOpacityFunctionObject(TVPStretchConstAlphaBlend_d, opa),
				TVP_DoStretchLoop_ARGS);
		}
		break;

	case bmAlpha:
		// alpha blending, ignoring destination alpha
		if(opa == 255)
		{
			if(!hda)
				TVPDoStretchLoop(
					tTVPStretchFunctionObject(TVPStretchAlphaBlend),
					TVP_DoStretchLoop_ARGS);
			else
				TVPDoStretchLoop(
					tTVPStretchFunctionObject(TVPStretchAlphaBlend_HDA),
					TVP_DoStretchLoop_ARGS);
		}
		else
		{
			if(!hda)
				TVPDoStretchLoop(
					tTVPStretchWithOpacityFunctionObject(TVPStretchAlphaBlend_o, opa),
					TVP_DoStretchLoop_ARGS);
			else
				TVPDoStretchLoop(
					tTVPStretchWithOpacityFunctionObject(TVPStretchAlphaBlend_HDA_o, opa),
					TVP_DoStretchLoop_ARGS);
		}
		break;

	case bmAlphaOnAlpha:
		// stretching alpha blending, with consideration of destination alpha
		if(opa == 255)
			TVPDoStretchLoop(
				tTVPStretchFunctionObject(TVPStretchAlphaBlend_d),
				TVP_DoStretchLoop_ARGS);
		else
			TVPDoStretchLoop(
				tTVPStretchWithOpacityFunctionObject(TVPStretchAlphaBlend_do, opa),
				TVP_DoStretchLoop_ARGS);
		break;

	case bmAddAlpha:
		// additive alpha blending
		if(opa == 255)
		{
			if(!hda)
				TVPDoStretchLoop(
					tTVPStretchFunctionObject(TVPStretchAdditiveAlphaBlend),
					TVP_DoStretchLoop_ARGS);
			else
				TVPDoStretchLoop(
					tTVPStretchFunctionObject(TVPStretchAdditiveAlphaBlend_HDA),
					TVP_DoStretchLoop_ARGS);
		}
		else
		{
			if(!hda)
				TVPDoStretchLoop(
					tTVPStretchWithOpacityFunctionObject(TVPStretchAdditiveAlphaBlend_o, opa),
					TVP_DoStretchLoop_ARGS);
			else
				TVPDoStretchLoop(
					tTVPStretchWithOpacityFunctionObject(TVPStretchAdditiveAlphaBlend_HDA_o, opa),
					TVP_DoStretchLoop_ARGS);
		}
		break;

	case bmAddAlphaOnAddAlpha:
		// additive alpha on additive alpha
		if(opa == 255)
			TVPDoStretchLoop(
				tTVPStretchFunctionObject(TVPStretchAdditiveAlphaBlend_a),
				TVP_DoStretchLoop_ARGS);
		else
			TVPDoStretchLoop(
				tTVPStretchWithOpacityFunctionObject(TVPStretchAdditiveAlphaBlend_ao, opa),
				TVP_DoStretchLoop_ARGS);
		break;

	case bmAddAlphaOnAlpha:
		// additive alpha on simple alpha
		; // yet not implemented
		break;

	case bmAlphaOnAddAlpha:
		// simple alpha on additive alpha
		if(opa == 255)
			TVPDoStretchLoop(
				tTVPStretchFunctionObject(TVPStretchAlphaBlend_a),
				TVP_DoStretchLoop_ARGS);
		else
			TVPDoStretchLoop(
				tTVPStretchWithOpacityFunctionObject(TVPStretchAlphaBlend_ao, opa),
				TVP_DoStretchLoop_ARGS);
		break;

	case bmCopyOnAddAlpha:
		// constant ratio alpha blending (assuming source is opaque)
		// with consideration of destination additive alpha
		if(opa == 255)
			TVPDoStretchLoop(
				tTVPStretchFunctionObject(TVPStretchCopyOpaqueImage),
				TVP_DoStretchLoop_ARGS);
		else
			TVPDoStretchLoop(
				tTVPStretchWithOpacityFunctionObject(TVPStretchConstAlphaBlend_a, opa),
				TVP_DoStretchLoop_ARGS);
		break;


	default:
		; // yet not implemented
	}

	return true;
}
//---------------------------------------------------------------------------
static inline tjs_int round_to_int(double x)
{
	// round x to an integer
	return x; // this works better than following;
//	if(x < 0) return (tjs_int)(x - 0.5f); else return (tjs_int)(x + 0.5f);
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::AffineBlt(tTVPRect destrect, const tTVPBaseBitmap *ref,
		tTVPRect refrect, const tTVPPoint * points,
			tTVPBBBltMethod method, tjs_int opa,
			tTVPRect * updaterect,
			bool hda, tTVPBBStretchType type)
{
	// unlike other drawing methods, 'destrect' is the clip rectangle of the
	// destination bitmap.
	// affine transformation is to be applied on zero-offset source rectangle;
	// (0, 0) - (refreft.right - refrect.left, refrect.bottom - refrect.top)

	// points are given as destination points, corresponding to three source
	// points; upper-left, upper-right, bottom-left.

	// returns false if the updating rect is not updated

	// check source rectangle
	if(refrect.left >= refrect.right ||
		refrect.top >= refrect.bottom) return false;
	if(refrect.left < 0 || refrect.top < 0 ||
		refrect.right > (tjs_int)ref->GetWidth() ||
		refrect.bottom > (tjs_int)ref->GetHeight())
		TVPThrowExceptionMessage(TVPOutOfRectangle);

	// check destination rectangle
	if(destrect.left < 0) destrect.left = 0;
	if(destrect.top < 0) destrect.top = 0;
	if(destrect.right > (tjs_int)GetWidth()) destrect.right = GetWidth();
	if(destrect.bottom > (tjs_int)GetHeight()) destrect.bottom = GetHeight();

	if(destrect.left >= destrect.right ||
		destrect.top >= destrect.bottom) return false; // not drawable

	// vertex points
	tjs_int points_x[4];
	tjs_int points_y[4];

	// check each vertex and find most-top/most-bottom/most-left/most-right points
	tjs_int scanlinestart, scanlineend; // most-top/most-bottom
	tjs_int leftlimit, rightlimit; // most-left/most-right
	tjs_int toppoint, bottompoint;

	// - upper-left
	points_x[0] = points[0].x;
	points_y[0] = points[0].y;
	leftlimit = points_x[0];
	rightlimit = points_x[0];
	scanlinestart = points_y[0], toppoint = 0;
	scanlineend = points_y[0], bottompoint = 0;

	// - upper-right
	points_x[1] = points[1].x;
	points_y[1] = points[1].y;
	if(leftlimit > points_x[1]) leftlimit = points_x[1];
	if(rightlimit < points_x[1]) rightlimit = points_x[1];
	if(scanlinestart > points_y[1]) scanlinestart = points_y[1], toppoint = 1;
	if(scanlineend < points_y[1]) scanlineend = points_y[1], bottompoint = 1;

	// - bottom-right
	points_x[2] = points[1].x - points[0].x + points[2].x;
	points_y[2] = points[1].y - points[0].y + points[2].y;
	if(leftlimit > points_x[2]) leftlimit = points_x[2];
	if(rightlimit < points_x[2]) rightlimit = points_x[2];
	if(scanlinestart > points_y[2]) scanlinestart = points_y[2], toppoint = 2;
	if(scanlineend < points_y[2]) scanlineend = points_y[2], bottompoint = 2;

	// - bottom-left
	points_x[3] = points[2].x;
	points_y[3] = points[2].y;
	if(leftlimit > points_x[3]) leftlimit = points_x[3];
	if(rightlimit < points_x[3]) rightlimit = points_x[3];
	if(scanlinestart > points_y[3]) scanlinestart = points_y[3], toppoint = 3;
	if(scanlineend < points_y[3]) scanlineend = points_y[3], bottompoint = 3;

	// check destrect intersections
	if(leftlimit >= destrect.right) return false;
	if(rightlimit < destrect.left) return false;
	if(scanlinestart >= destrect.bottom) return false;
	if(scanlineend < destrect.top) return false;

	// prepare to transform...
	tjs_int pd, pa, pdnext, panext;
	tjs_int pdstepx, pastepx;
	tjs_int pdx, pax;
	tjs_int sdstep, sastep;
	tjs_int sd, sa;
	tjs_int y = destrect.top < scanlinestart ? scanlinestart : destrect.top;
	tjs_int ylim = (destrect.bottom-1) < scanlineend ? (destrect.bottom-1) : scanlineend;
	tjs_uint8 * dest = (tjs_uint8*)GetScanLineForWrite(y);
	tjs_int destpitch = GetPitchBytes();
	const tjs_uint8 * src = (const tjs_uint8 *)ref->GetScanLine(0);
	tjs_int srcpitch = ref->GetPitchBytes();


	// skip to the first scanline

	// - for descent
	pd = pdnext = toppoint;
	pdnext --;
	pdnext &= 3;

	while(pdnext != bottompoint && points_y[pdnext] < y)
	{
		pdnext--;
		pdnext &= 3; // because pd, pdnext, pa and panext take ring of 0..3
		pd--;
		pd &= 3;
	}

	while(pdnext != bottompoint && points_y[pdnext] == y)
	{
		pd--;
		pdnext--;
		pd &= 3;
		pdnext &= 3;
	}

	// - for ascent
	pa = panext = toppoint;
	panext ++;
	panext &= 3;

	while(panext != bottompoint && points_y[panext] < y)
	{
		panext++;
		panext &= 3;
		pa++;
		pa &= 3;
	}

	while(panext != bottompoint && points_y[panext] == y)
	{
		pa++;
		panext++;
		pa &= 3;
		panext &= 3;
	}

	// compute initial horizontal step per a line

	// - for descent
	if(points_y[pdnext] - points_y[pd] + 1)
		pdstepx = 65536 * (points_x[pdnext] - points_x[pd]) /
			(points_y[pdnext] - points_y[pd] + 1);
	else
		pdstepx = 65536;

	// - for ascent
	if(points_y[panext] - points_y[pa] + 1)
		pastepx = 65536 * (points_x[panext] - points_x[pa]) /
			(points_y[panext] - points_y[pa] + 1);
	else
		pastepx = 65536;


	// compute initial source step

	// - for descent
	if(points_y[pdnext] - points_y[pd] + 1)
	{
		switch(pd)
		{
		case 0: sdstep = 65536 * (refrect.bottom - refrect.top) /
				(points_y[pdnext] - points_y[pd] + 1); break;
		case 1: sdstep = 65536 * (refrect.left - refrect.right) /
				(points_y[pdnext] - points_y[pd] + 1); break;
		case 2: sdstep = 65536 * (refrect.top - refrect.bottom) /
				(points_y[pdnext] - points_y[pd] + 1); break;
		case 3: sdstep = 65536 * (refrect.right - refrect.left) /
				(points_y[pdnext] - points_y[pd] + 1); break;
		}
	}
	else
	{
		sdstep = 65536;
	}

	// - for ascent
	if(points_y[panext] - points_y[pa] + 1)
	{
		switch(pa)
		{
		case 0: sastep = 65536 * (refrect.right - refrect.left) /
				(points_y[panext] - points_y[pa] + 1); break;
		case 1: sastep = 65536 * (refrect.bottom - refrect.top) /
				(points_y[panext] - points_y[pa] + 1); break;
		case 2: sastep = 65536 * (refrect.left - refrect.right) /
				(points_y[panext] - points_y[pa] + 1); break;
		case 3: sastep = 65536 * (refrect.top - refrect.bottom) /
				(points_y[panext] - points_y[pa] + 1); break;
		}
	}
	else
	{
		sastep = 65536;
	}

	// compute initial horizontal position

	// - for descent
	pdx = points_x[pd] * 65536;
	if(points_y[pd] < y) pdx += pdstepx * (y - points_y[pd]);

	// - for ascent
	pax = points_x[pa] * 65536;
	if(points_y[pa] < y) pax += pastepx * (y - points_y[pa]);

	// compute initial source position

	// - for descent
	switch(pd)
	{
	case 0: sd = refrect.top * 65536; break;
	case 1: sd = refrect.right * 65536 - 1; break;
	case 2: sd = refrect.bottom * 65536 - 1; break;
	case 3: sd = refrect.left * 65536; break;
	}
	if(points_y[pd] < y) sd += sdstep * (y - points_y[pd]);

	// - for ascent
	switch(pa)
	{
	case 0: sa = refrect.left * 65536; break;
	case 1: sa = refrect.top * 65536; break;
	case 2: sa = refrect.right * 65536 - 1; break;
	case 3: sa = refrect.bottom * 65536 - 1; break;
	}
	if(points_y[pa] < y) sa += sastep * (y - points_y[pa]);

	// process per a line
	tjs_int mostupper;
	tjs_int mostbottom;
	bool firstline = true;
	for(; y <= ylim; y++)
	{
		// transfer a line

		// - compute descent x and ascent x
		tjs_int ddx = pdx >> 16;
		tjs_int adx = pax >> 16;

		// - compute descent source position
		tjs_int dx, dy, ax, ay;
		switch(pd)
		{
		case 0: dx = refrect.left * 65536; dy = sd; break;
		case 1: dx = sd; dy = refrect.top * 65536; break;
		case 2: dx = refrect.right * 65536 - 1; dy = sd; break;
		case 3: dx = sd; dy = refrect.bottom * 65536 - 1; break;
		}

		// - compute ascent source position
		switch(pa)
		{
		case 0: ax = sa; ay = refrect.top * 65536; break;
		case 1: ax = refrect.right * 65536 - 1; ay = sa; break;
		case 2: ax = sa; ay = refrect.bottom * 65536 - 1; break;
		case 3: ax = refrect.left *65536; ay = sa; break;
		}

		// - swap dx/dy ax/ay dax/ddx if adx < ddx
		if(adx < ddx) std::swap(dx, ax), std::swap(dy, ay), std::swap(adx, ddx);

		// - compute source step
		tjs_int sxstep, systep;
		if(adx != ddx)
		{
			sxstep = (ax - dx + 1) / (adx - ddx + 1);
			systep = (ay - dy + 1) / (adx - ddx + 1);
		}
		else
		{
			sxstep = systep = 65536;
		}

		// - clip widh destrect.left and destrect.right
		tjs_int l = ddx, r = adx;
		if(l < destrect.left) l = destrect.left;
		if(r >= destrect.right) r = destrect.right - 1;
		dx += (l - ddx) * sxstep;
		dy += (l - ddx) * systep;
		tjs_int len = r - l + 1;

#define LINTRANS_LINE(func, func2) { \
	if(systep == 0) \
		func2((tjs_uint32*)dest + l, len, \
			(tjs_uint32*)(src + (dy>>16) * srcpitch), dx, sxstep); \
	else \
		func((tjs_uint32*)dest + l, len, \
			(tjs_uint32*)src, dx, dy, sxstep, systep, srcpitch); \
}

#define LINTRANS_LINE_OPA(func, func2) { \
	if(systep == 0) \
		func2((tjs_uint32*)dest + l, len, \
			(tjs_uint32*)(src + (dy>>16) * srcpitch), dx, sxstep, opa); \
	else \
		func((tjs_uint32*)dest + l, len, \
			(tjs_uint32*)src, dx, dy, sxstep, systep, srcpitch, opa); \
}



		// - transfer
		if(len > 0)
		{
			// update updaterect
			if(firstline)
			{
				leftlimit = l;
				rightlimit = r;
				firstline = false;
				mostupper = mostbottom = y;
			}
			else
			{
				if(l < leftlimit) leftlimit = l;
				if(r > rightlimit) rightlimit = r;
				mostbottom = y;
			}

			// transfer using each blend function
			switch(method)
			{
			case bmCopy:
				if(opa == 255 && !hda)
				{
					if(sxstep == 65536 && systep == 0)
					{
						// intact copy
						memcpy((tjs_uint32*)dest + l,
							(tjs_uint32*)(src + (dy>>16) * srcpitch) + (dx>>16),
							len * sizeof(tjs_int32));
					}
					else
					{
						LINTRANS_LINE(TVPLinTransCopy, TVPStretchCopy)
					}
				}
				else if(!hda)
					LINTRANS_LINE_OPA(TVPLinTransConstAlphaBlend, TVPStretchConstAlphaBlend)
				else
					LINTRANS_LINE_OPA(TVPLinTransConstAlphaBlend_HDA, TVPStretchConstAlphaBlend_HDA)
				break;

			case bmCopyOnAlpha:
				// constant ratio alpha blending, with consideration of destination alpha
				if(opa == 255)
					LINTRANS_LINE(TVPLinTransCopyOpaqueImage, TVPStretchCopyOpaqueImage)
				else
					LINTRANS_LINE_OPA(TVPLinTransConstAlphaBlend_d, TVPStretchConstAlphaBlend_d)
				break;

			case bmAlpha:
				// alpha blending, ignoring destination alpha
				if(opa == 255)
				{
					if(!hda)
						LINTRANS_LINE(TVPLinTransAlphaBlend, TVPStretchAlphaBlend)
					else
						LINTRANS_LINE(TVPLinTransAlphaBlend_HDA, TVPStretchAlphaBlend_HDA)
				}
				else
				{
					if(!hda)
						LINTRANS_LINE_OPA(TVPLinTransAlphaBlend_o, TVPStretchAlphaBlend_o)
					else
						LINTRANS_LINE_OPA(TVPLinTransAlphaBlend_HDA_o, TVPStretchAlphaBlend_HDA_o)
				}
				break;

			case bmAlphaOnAlpha:
				// alpha blending, with consideration of destination alpha
				if(opa == 255)
					LINTRANS_LINE(TVPLinTransAlphaBlend_d, TVPStretchAlphaBlend_d)
				else
					LINTRANS_LINE_OPA(TVPLinTransAlphaBlend_do, TVPStretchAlphaBlend_do)
				break;

			case bmAddAlpha:
				// additive alpha blending, ignoring destination alpha
				if(opa == 255)
				{
					if(!hda)
						LINTRANS_LINE(TVPLinTransAdditiveAlphaBlend, TVPStretchAdditiveAlphaBlend)
					else
						LINTRANS_LINE(TVPLinTransAdditiveAlphaBlend_HDA, TVPStretchAdditiveAlphaBlend_HDA)
				}
				else
				{
					if(!hda)
						LINTRANS_LINE_OPA(TVPLinTransAdditiveAlphaBlend_o, TVPStretchAlphaBlend_o)
					else
						LINTRANS_LINE_OPA(TVPLinTransAdditiveAlphaBlend_HDA_o, TVPStretchAdditiveAlphaBlend_HDA_o)
				}
				break;

			case bmAddAlphaOnAddAlpha:
				// additive alpha blending, with consideration of destination additive alpha
				if(opa == 255)
					LINTRANS_LINE(TVPLinTransAdditiveAlphaBlend_a, TVPStretchAlphaBlend_a)
				else
					LINTRANS_LINE_OPA(TVPLinTransAdditiveAlphaBlend_ao, TVPStretchAlphaBlend_ao)
				break;

			case bmAddAlphaOnAlpha:
				// additive alpha on simple alpha
				; // yet not implemented
				break;

			case bmAlphaOnAddAlpha:
				// simple alpha on additive alpha
				if(opa == 255)
					LINTRANS_LINE(TVPLinTransAlphaBlend_a, TVPStretchAlphaBlend_a)
				else
					LINTRANS_LINE_OPA(TVPLinTransAlphaBlend_ao, TVPStretchAlphaBlend_ao)
				break;

			case bmCopyOnAddAlpha:
				// constant ratio alpha blending (assuming source is opaque)
				// with consideration of destination additive alpha
				if(opa == 255)
					LINTRANS_LINE(TVPLinTransCopyOpaqueImage, TVPStretchCopyOpaqueImage)
				else
					LINTRANS_LINE_OPA(TVPLinTransConstAlphaBlend_a, TVPStretchConstAlphaBlend_a)
				break;

			default:
				; // yet not implemented
			}
		}

		// check descent point
		if(points_y[pdnext] == y)
		{
			do
			{
				pd--;
				pdnext--;
				pd &= 3;
				pdnext &= 3;
			} while(pdnext != bottompoint && points_y[pdnext] == y);


			if(points_y[pdnext] - points_y[pd] + 1)
				pdstepx = 65536 * (points_x[pdnext] - points_x[pd]) /
					(points_y[pdnext] - points_y[pd] + 1);
			else
				pdstepx = 65536;

			if(points_y[pdnext] - points_y[pd] + 1)
			{
				switch(pd)
				{
				case 0: sdstep = 65536 * (refrect.bottom - refrect.top) /
						(points_y[pdnext] - points_y[pd] + 1); break;
				case 1: sdstep = 65536 * (refrect.left - refrect.right) /
						(points_y[pdnext] - points_y[pd] + 1); break;
				case 2: sdstep = 65536 * (refrect.top - refrect.bottom) /
						(points_y[pdnext] - points_y[pd] + 1); break;
				case 3: sdstep = 65536 * (refrect.right - refrect.left) /
						(points_y[pdnext] - points_y[pd] + 1); break;
				}
			}
			else
			{
				sdstep = 65536;
			}

			switch(pd)
			{
			case 0: sd = refrect.top * 65536; break;
			case 1: sd = refrect.right * 65536 - 1; break;
			case 2: sd = refrect.bottom * 65536 - 1; break;
			case 3: sd = refrect.left * 65536; break;
			}

			pdx = points_x[pd] * 65536;
		}

		// check ascent point
		if(points_y[panext] == y)
		{
			do
			{
				pa++;
				panext++;
				pa &= 3;
				panext &= 3;
			} while(panext != bottompoint && points_y[panext] == y);

			if(points_y[panext] - points_y[pa] + 1)
				pastepx = 65536 * (points_x[panext] - points_x[pa]) /
					(points_y[panext] - points_y[pa] + 1);
			else
				pastepx = 65536;

			if(points_y[panext] - points_y[pa] + 1)
			{
				switch(pa)
				{
				case 0: sastep = 65536 * (refrect.right - refrect.left) /
						(points_y[panext] - points_y[pa] + 1); break;
				case 1: sastep = 65536 * (refrect.bottom - refrect.top) /
						(points_y[panext] - points_y[pa] + 1); break;
				case 2: sastep = 65536 * (refrect.left - refrect.right) /
						(points_y[panext] - points_y[pa] + 1); break;
				case 3: sastep = 65536 * (refrect.top - refrect.bottom) /
						(points_y[panext] - points_y[pa] + 1); break;
				}
			}
			else
			{
				sastep = 65536;
			}

			switch(pa)
			{
			case 0: sa = refrect.left * 65536; break;
			case 1: sa = refrect.top * 65536; break;
			case 2: sa = refrect.right * 65536 - 1; break;
			case 3: sa = refrect.bottom * 65536 - 1; break;
			}

			pax = points_x[pa] * 65536;
		}

		// to next ...
		pdx += pdstepx;
		pax += pastepx;
		sd += sdstep;
		sa += sastep;
		dest += destpitch;
	}

	// fill members of updaterect
	if(!firstline && updaterect)
	{
		updaterect->left = leftlimit;
		updaterect->right = rightlimit + 1;
		updaterect->top = mostupper;
		updaterect->bottom = mostbottom + 1;
	}

	return !firstline;
}
//---------------------------------------------------------------------------
bool tTVPBaseBitmap::AffineBlt(tTVPRect destrect, const tTVPBaseBitmap *ref,
		tTVPRect refrect, const t2DAffineMatrix & matrix,
			tTVPBBBltMethod method, tjs_int opa,
			tTVPRect * updaterect,
			bool hda, tTVPBBStretchType type)
{
	// do transformation using 2D affine matrix.
	//  x' =  ax + cy + tx
	//  y' =  bx + dy + ty
	tTVPPoint points[3];
	int rp = refrect.get_width() - 1;
	int bp = refrect.get_height() - 1;

	// - upper-left
	points[0].x = round_to_int(matrix.tx);
	points[0].y = round_to_int(matrix.ty);

	// - upper-right
	points[1].x = round_to_int(matrix.a * rp + matrix.tx);
	points[1].y = round_to_int(matrix.b * rp + matrix.ty);

/*	// - bottom-right
	points[2].x = round_to_int(matrix.a * rp + matrix.c * bp + matrix.tx);
	points[2].y = round_to_int(matrix.b * rp + matrix.d * bp + matrix.ty);
*/

	// - bottom-left
	points[2].x = round_to_int(matrix.c * bp + matrix.tx);
	points[2].y = round_to_int(matrix.d * bp + matrix.ty);

	return AffineBlt(destrect, ref, refrect, points, method, opa, updaterect, hda, type);
}
//---------------------------------------------------------------------------
void tTVPBaseBitmap::UDFlip(const tTVPRect &rect)
{
	// up-down flip for given rectangle

	if(rect.left < 0 || rect.top < 0 || rect.right > (tjs_int)GetWidth() ||
		rect.bottom > (tjs_int)GetHeight())
				TVPThrowExceptionMessage(TVPSrcRectOutOfBitmap);

	tjs_int h = (rect.bottom - rect.top) /2;
	tjs_int w = rect.right - rect.left;
	tjs_int pitch = GetPitchBytes();
	tjs_uint8 * l1 = (tjs_uint8*)GetScanLineForWrite(rect.top);
	tjs_uint8 * l2 = (tjs_uint8*)GetScanLineForWrite(rect.bottom - 1);


	if(Is32BPP())
	{
		// 32bpp
		l1 += rect.left * sizeof(tjs_uint32);
		l2 += rect.left * sizeof(tjs_uint32);
		while(h--)
		{
			TVPSwapLine32((tjs_uint32*)l1, (tjs_uint32*)l2, w);
			l1 += pitch;
			l2 -= pitch;
		}
	}
	else
	{
		// 8bpp
		l1 += rect.left;
		l2 += rect.left;
		while(h--)
		{
			TVPSwapLine8(l1, l2, w);
			l1 += pitch;
			l2 -= pitch;
		}
	}
}
//---------------------------------------------------------------------------
void tTVPBaseBitmap::LRFlip(const tTVPRect &rect)
{
	// left-right flip
	if(rect.left < 0 || rect.top < 0 || rect.right > (tjs_int)GetWidth() ||
		rect.bottom > (tjs_int)GetHeight())
				TVPThrowExceptionMessage(TVPSrcRectOutOfBitmap);

	tjs_int h = rect.bottom - rect.top;
	tjs_int w = rect.right - rect.left;

	tjs_int pitch = GetPitchBytes();
	tjs_uint8 * line = (tjs_uint8*)GetScanLineForWrite(rect.top);

	if(Is32BPP())
	{
		// 32bpp
		line += rect.left * sizeof(tjs_uint32);
		while(h--)
		{
			TVPReverse32((tjs_uint32*)line, w);
			line += pitch;
		}
	}
	else
	{
		// 8bpp
		line += rect.left;
		while(h--)
		{
			TVPReverse8(line, w);
			line += pitch;
		}
	}
}
//---------------------------------------------------------------------------
void tTVPBaseBitmap::DoGrayScale(tTVPRect rect)
{
	if(!Is32BPP()) return;  // 8bpp is always grayscaled bitmap

	BOUND_CHECK(RET_VOID);

	tjs_int h = rect.bottom - rect.top;
	tjs_int w = rect.right - rect.left;

	tjs_int pitch = GetPitchBytes();
	tjs_uint8 * line = (tjs_uint8*)GetScanLineForWrite(rect.top);


	line += rect.left * sizeof(tjs_uint32);
	while(h--)
	{
		TVPDoGrayScale((tjs_uint32*)line, w);
		line += pitch;
	}
}
//---------------------------------------------------------------------------
void tTVPBaseBitmap::AdjustGamma(tTVPRect rect, const tTVPGLGammaAdjustData & data)
{
	if(!Is32BPP()) TVPThrowExceptionMessage(TVPInvalidOperationFor8BPP);

	BOUND_CHECK(RET_VOID);

	if(!memcmp(&data, &TVPIntactGammaAdjustData, sizeof(tTVPGLGammaAdjustData)))
		return;

	tTVPGLGammaAdjustTempData temp;
	TVPInitGammaAdjustTempData(&temp, &data);

	try
	{
		tjs_int h = rect.bottom - rect.top;
		tjs_int w = rect.right - rect.left;

		tjs_int pitch = GetPitchBytes();
		tjs_uint8 * line = (tjs_uint8*)GetScanLineForWrite(rect.top);


		line += rect.left * sizeof(tjs_uint32);
		while(h--)
		{
			TVPAdjustGamma((tjs_uint32*)line, w, &temp);
			line += pitch;
		}

	}
	catch(...)
	{
		TVPUninitGammaAdjustTempData(&temp);
		throw;
	}

	TVPUninitGammaAdjustTempData(&temp);
}
//---------------------------------------------------------------------------
void tTVPBaseBitmap::AdjustGammaForAdditiveAlpha(tTVPRect rect, const tTVPGLGammaAdjustData & data)
{
	if(!Is32BPP()) TVPThrowExceptionMessage(TVPInvalidOperationFor8BPP);

	BOUND_CHECK(RET_VOID);

	if(!memcmp(&data, &TVPIntactGammaAdjustData, sizeof(tTVPGLGammaAdjustData)))
		return;

	tTVPGLGammaAdjustTempData temp;
	TVPInitGammaAdjustTempData(&temp, &data);

	try
	{
		tjs_int h = rect.bottom - rect.top;
		tjs_int w = rect.right - rect.left;

		tjs_int pitch = GetPitchBytes();
		tjs_uint8 * line = (tjs_uint8*)GetScanLineForWrite(rect.top);


		line += rect.left * sizeof(tjs_uint32);
		while(h--)
		{
			TVPAdjustGamma_a((tjs_uint32*)line, w, &temp);
			line += pitch;
		}

	}
	catch(...)
	{
		TVPUninitGammaAdjustTempData(&temp);
		throw;
	}

	TVPUninitGammaAdjustTempData(&temp);
}
//---------------------------------------------------------------------------
void tTVPBaseBitmap::ConvertAddAlphaToAlpha()
{
	// convert additive alpha representation to alpha representation

	if(!Is32BPP()) TVPThrowExceptionMessage(TVPInvalidOperationFor8BPP);

	tjs_int w = GetWidth();
	tjs_int h = GetHeight();
	tjs_int pitch = GetPitchBytes();
	tjs_uint8 * line = (tjs_uint8*)GetScanLineForWrite(0);

	while(h--)
	{
		TVPConvertAdditiveAlphaToAlpha((tjs_uint32*)line, w);
		line += pitch;
	}
}
//---------------------------------------------------------------------------
void tTVPBaseBitmap::ConvertAlphaToAddAlpha()
{
	// convert additive alpha representation to alpha representation

	if(!Is32BPP()) TVPThrowExceptionMessage(TVPInvalidOperationFor8BPP);

	tjs_int w = GetWidth();
	tjs_int h = GetHeight();
	tjs_int pitch = GetPitchBytes();
	tjs_uint8 * line = (tjs_uint8*)GetScanLineForWrite(0);

	while(h--)
	{
		TVPConvertAlphaToAdditiveAlpha((tjs_uint32*)line, w);
		line += pitch;
	}
}
//---------------------------------------------------------------------------



