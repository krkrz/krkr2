#pragma once

//----------------------------------------------------------------------------
// C言語の標準定義ではないらしいマクロ
// http://www2s.biglobe.ne.jp/~nuts/labo/inti/inti09.html
// M_PIはwxWidgetsに含まれている。M_PI_2以降は含まれていない
#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif
#ifndef M_PI_2
/* Constants rounded for 21 decimals. */
#define M_E         2.71828182845904523536
#define M_LOG2E     1.44269504088896340736
#define M_LOG10E    0.434294481903251827651
#define M_LN2       0.693147180559945309417
#define M_LN10      2.30258509299404568402
#define M_PI_2      1.57079632679489661923
#define M_PI_4      0.785398163397448309616
#define M_1_PI      0.318309886183790671538
#define M_2_PI      0.636619772367581343076
#define M_1_SQRTPI  0.564189583547756286948
#define M_2_SQRTPI  1.12837916709551257390
#define M_SQRT2     1.41421356237309504880
#define M_SQRT_2    0.707106781186547524401
#endif

//----------------------------------------------------------------------------
// 定数値定義
enum { mbYes=0, mbNo=1, mbOk=2, mbOK=2, mbCancel=3, mbAbort=4, mbRetry=5, mbIgnore=6, mbALL=7, mbNoToAll=8, mbYesToAll=9, mbHelp=10 };
enum { mrYes, mrNo, mrOk, mrCancel, mrAbort, mrRetry, mrIgnore, mrALL, mrHelp };
enum { fmOpenRead=0, fmOpenWrite=1, fmOpenReadWrite=2, fmShareDenyWrite=32, fmCreate=0xFFFF };
enum TMsgDlgType { mtWarning, mtError, mtInformation, mtConfirmation, mtCustom };

enum TPixelFormat {pfDevice=0, pf1bit=1, pf4bit=2, pf8bit=3, pf15bit=4, pf16bit=5, pf24bit=6, pf32bit=7, pfCustom=8 };

enum TCursor { crDefault=0, crNone=-1, crIBeam=-4, crHourGlass=-11 };
enum TCloseAction{ caNone, caHide, caFree };
enum TBrushStyle { bsSolid=0, bsClear=1 };
enum TDuplicates { dupIgnore=0, dupError=1, dupAccept=2 };
enum TAnchors { akLeft=0, akTop=1, akRight=2, akBottom=3 };


namespace Forms
{
	enum TFormBorderStyle { bsNone, bsSingle, bsSizeable, bsDialog, bsToolWindow, bsSizeToolWin };
//	enum TBorderStyle { bsNone, bsSingle };
	// 同じ識別子のenumは変すぎるので、typedefで重複させとく
	typedef TFormBorderStyle TBorderStyle;
};

enum {
	faReadOnly  = 0x00000001,// 読み出し専用 
	faHidden    = 0x00000002,   // 隠しファイル 
	faSysFile   = 0x00000004,  // システム 
	faVolumeID  = 0x00000008, // ボリューム 
	faDirectory = 0x00000010,// ディレクトリ 
	faArchive   = 0x00000020,  // アーカイブファイル 
	faAnyFile   = 0x0000003F   // すべての属性を持つファイル 
};

enum TAlign { alNone, alClient };
enum TWindowState { wsNormal=0, wsMinimized=1, wsMaximized=2 };
enum TPenStyle { psSolid }; // pen style
enum TPenMode { pmCopy=4, pmMerge=10, pmNotMask=13 }; // pen mode
namespace Controls {
	enum TMouseButton { mbLeft, mbRight, mbMiddle };
	enum {imDisable, imClose, imOpen, imDontCare, imSAlpha, imAlpha, imHira,
		imSKata, imKata, imChinese, imSHanguel, imHanguel };
};

enum TScrollStyle { ssNone, ssHorizontal, ssVertical, ssBoth };
enum{maManual};
enum{lbOwnerDrawFixed};
enum TBorderIcons{ biMaximize };

enum { ssShift, ssAlt, ssCtrl, ssLeft, ssRight, ssMiddle, ssDouble };

enum TFormStyle { fsStayOnTop, fsNormal};
typedef int TOwnerDrawState;
typedef unsigned int TColor;
enum { clBlack=0x000000, clWhite=0xFFFFFF, clWindow=0x8000005, clWindowText=0x8000008, clNone=0x1fffffff };
enum TFontStyle { fsBold, fsItalic, fsUnderline, fsStrikeOut };

using namespace Controls;
using namespace Forms;
