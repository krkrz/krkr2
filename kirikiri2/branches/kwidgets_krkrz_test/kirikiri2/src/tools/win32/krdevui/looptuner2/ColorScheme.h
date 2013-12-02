//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
#ifndef __COLORSCHEME_H__
#define __COLORSCHEME_H__

//---------------------------------------------------------------------------
#define BYTEOF(x, b)  (((x) >> ((b)*8)) & 0xff)
#define COLOR_BLEND_8(x, y, ratio)  (((x) * (ratio) + (y) * (100-(ratio))) / 100)
#define BLEND_ARGB(x, y, ratio) (\
			(COLOR_BLEND_8(BYTEOF(x, 0), BYTEOF(y, 0), (ratio))    )+ \
			(COLOR_BLEND_8(BYTEOF(x, 1), BYTEOF(y, 1), (ratio))<<8 )+ \
			(COLOR_BLEND_8(BYTEOF(x, 2), BYTEOF(y, 2), (ratio))<<16)+ \
			(COLOR_BLEND_8(BYTEOF(x, 3), BYTEOF(y, 3), (ratio))<<24)  )

//---------------------------------------------------------------------------
// Color schemes - currently fixed colors only
//---------------------------------------------------------------------------
static  const TColor C_DISABLEED_CLIENT		= clBtnFace;
static  const TColor C_CLIENT				= ((TColor)0xffffff);
static  const TColor C_WAVE					= ((TColor)0x000000);
static  const TColor C_INF_LINE				= ((TColor)0x808080);
static  const TColor C_DISABLED_TIME_CLIENT	= ((TColor)0xf0f0f0);

static  const TColor C_WAVE_B_FG_BASE		= ((TColor)0x800000);
static  const TColor C_WAVE_B_BG			= ((TColor)0xd0d0e0);
static  const TColor C_WAVE_B_FG			= ((TColor)BLEND_ARGB(C_WAVE_B_FG_BASE, 0xffffff, 60));
static  const TColor C_WAVE_B_FG_BG			= ((TColor)BLEND_ARGB(C_WAVE_B_FG_BASE, 0xe0d0d0, 80));
static  const TColor C_WAVE_A_FG_BASE		= ((TColor)0x000080);
static  const TColor C_WAVE_A_BG			= ((TColor)0xe0d0d0);
static  const TColor C_WAVE_A_FG			= ((TColor)BLEND_ARGB(C_WAVE_A_FG_BASE, 0xffffff, 60));
static  const TColor C_WAVE_A_FG_BG			= ((TColor)BLEND_ARGB(C_WAVE_A_FG_BASE, 0xd0d0e0, 80));

static  const TColor C_WAVE_CENTER_SPLIT	= ((TColor)0xc0c0c0);

static  const TColor C_TIME_CLIENT			= clBtnFace;
static  const TColor C_TIME_COLOR			= ((TColor)0x303030);
static  const TColor C_LINK_WAVE_MARK		= ((TColor)0x0000f0);
static  const TColor C_LINK_CLIENT			= clWhite;
static  const TColor C_LINK_SEPARETOR  		= ((TColor)0xe0e0e0);
static  const TColor C_LINK_LINE			= clBlack;
static  const TColor C_LINK_HOVER			= ((TColor)0x40d090);
static  const TColor C_LINK_FOCUS			= clRed;
static  const TColor C_LINK_WEAK_LINE		= clGray;

static  const TColor C_LABEL_WAVE_MARK		= ((TColor)0x00f000);
static  const TColor C_LABEL_MARK_LINE		= clBlack;
static  const TColor C_LABEL_MARK_HOVER		= ((TColor)0x40d090);
static  const TColor C_LABEL_MARK_FOCUS		= clRed;
static  const TColor C_LABEL_MARK			= clYellow;
static  const TColor C_LABEL_TEXT_BG		= clWhite;
static  const TColor C_LABEL_TEXT			= ((TColor)0x000080);
static  const TColor C_LABEL_TEXT_HOVER		= ((TColor)0x008080);
static  const TColor C_LABEL_TEXT_FOCUS		= ((TColor)0x0000ff);

static  const TColor C_WAVETHUMB_LINKS		= ((TColor)0x8080ff);
static  const TColor C_WAVETHUMB_LABELS		= ((TColor)0x00a000);

//---------------------------------------------------------------------------

#endif

