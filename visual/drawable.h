//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// "tTVPDrawable" definition
//---------------------------------------------------------------------------

#ifndef __DRAWABLE_H__
#define __DRAWABLE_H__


/*[*/
//---------------------------------------------------------------------------
// layer / blending types
//---------------------------------------------------------------------------
enum tTVPLayerType
{
	ltBinder,
	ltCoverRect,
	ltTransparent, // alpha blend
	ltAdditive,
	ltSubtractive,
	ltMultiplicative,
	ltEffect,
	ltFilter,
	ltDodge,
	ltDarken,
	ltLighten,
	ltScreen,
	ltAddAlpha // additive alpha blend
};
//---------------------------------------------------------------------------
/*]*/

/*[*/
//---------------------------------------------------------------------------
// alias to blending types
//---------------------------------------------------------------------------
enum tTVPBlendOperationMode
{
	omAdditive = ltAdditive,
	omSubtractive = ltSubtractive,
	omMultiplicative = ltMultiplicative,
	omDodge = ltDodge,
	omDarken = ltDarken,
	omLighten = ltLighten,
	omScreen = ltScreen,
	omAlpha = ltTransparent,
	omAddAlpha = ltAddAlpha,
	omOpaque = ltCoverRect
};
//---------------------------------------------------------------------------
/*]*/


//---------------------------------------------------------------------------
// tTVPDrawable definition
//---------------------------------------------------------------------------
class tTVPBaseBitmap;
class tTVPRect;
class tTVPDrawable
{
public:
	// base class of draw-able objects.
	virtual tTVPBaseBitmap * GetDrawTargetBitmap(const tTVPRect &rect,
		tTVPRect &cliprect) = 0;
		// returns target bitmap which has given size
		// (rect.get_width() * rect.get_height()).
		// put actually to be drawn rectangle to "cliprect"

	virtual bool GetDrawTargetHasAlpha() = 0;
		// returns whether the bitmap has alpha information

	virtual void DrawCompleted(const tTVPRect &destrect,
		tTVPBaseBitmap *bmp, const tTVPRect &cliprect,
		tTVPLayerType type, tjs_int opacity) = 0;
		// call this when the drawing is completed, passing
		// the bitmap containing the image, and its clip region.
};
//---------------------------------------------------------------------------



#endif
