//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// input related definition
//---------------------------------------------------------------------------


#ifndef __TVPINPUTDEFS_H__
#define __TVPINPUTDEFS_H__

/*[*/
//---------------------------------------------------------------------------
// mouse button
//---------------------------------------------------------------------------
enum tTVPMouseButton
{
	mbLeft,
	mbRight,
	mbMiddle
};



//---------------------------------------------------------------------------
// IME modes : comes from VCL's TImeMode
//---------------------------------------------------------------------------
enum tTVPImeMode
{
	imDisable,
	imClose,
	imOpen,
	imDontCare,
	imSAlpha,
	imAlpha,
	imHira,
	imSKata,
	imKata,
	imChinese,
	imSHanguel,
	imHanguel
};


//---------------------------------------------------------------------------
// shift state
//---------------------------------------------------------------------------
#define TVP_SS_SHIFT   0x01
#define TVP_SS_ALT     0x02
#define TVP_SS_CTRL    0x04
#define TVP_SS_LEFT    0x08
#define TVP_SS_RIGHT   0x10
#define TVP_SS_MIDDLE  0x20
#define TVP_SS_DOUBLE  0x40
#define TVP_SS_REPEAT  0x80


inline bool TVPIsAnyMouseButtonPressedInShiftStateFlags(tjs_uint32 state)
{ return (state & 
	(TVP_SS_LEFT | TVP_SS_RIGHT | TVP_SS_MIDDLE | TVP_SS_DOUBLE)) != 0; }



//---------------------------------------------------------------------------
// JoyPad virtual key codes
//---------------------------------------------------------------------------
// These VKs are KIRIKIRI specific. Not widely used.
#define VK_PAD_FIRST	0xB0   // first PAD related key code
#define VK_PADLEFT		0xB5
#define VK_PADUP		0xB6
#define VK_PADRIGHT		0xB7
#define VK_PADDOWN		0xB8
#define VK_PAD1			0xC0
#define VK_PAD2			0xC1
#define VK_PAD3			0xC2
#define VK_PAD4			0xC3
#define VK_PAD5			0xC4
#define VK_PAD6			0xC5
#define VK_PAD7			0xC6
#define VK_PAD8			0xC7
#define VK_PAD9			0xC8
#define VK_PAD10		0xC9
#define VK_PADANY		0xDF   // returns whether any one of pad buttons are pressed,
							   // in System.getKeyState
#define VK_PAD_LAST		0xDF   // last PAD related key code
//---------------------------------------------------------------------------
/*]*/
//---------------------------------------------------------------------------

#endif



