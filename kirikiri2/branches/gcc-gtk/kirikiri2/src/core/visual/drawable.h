//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info>

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
	ltBinder = 0,
	ltCoverRect = 1,
	ltOpaque = 1, // the same as ltCoverRect
	ltTransparent = 2, // alpha blend
	ltAlpha = 2, // the same as ltTransparent
	ltAdditive = 3,
	ltSubtractive = 4,
	ltMultiplicative = 5,
	ltEffect = 6,
	ltFilter = 7,
	ltDodge = 8,
	ltDarken = 9,
	ltLighten = 10,
	ltScreen = 11,
	ltAddAlpha = 12 // additive alpha blend
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

	virtual tTVPLayerType GetTargetLayerType() = 0;
		// returns target layer type

	virtual void DrawCompleted(const tTVPRect &destrect,
		tTVPBaseBitmap *bmp, const tTVPRect &cliprect,
		tTVPLayerType type, tjs_int opacity) = 0;
		// call this when the drawing is completed, passing
		// the bitmap containing the image, and its clip region.
};
//---------------------------------------------------------------------------



#endif
