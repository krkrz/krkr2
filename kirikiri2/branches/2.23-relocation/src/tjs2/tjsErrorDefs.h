//---------------------------------------------------------------------------
/*
	TJS2 Script Engine
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// return values as tjs_error
//---------------------------------------------------------------------------

#ifndef tjsErrorDefsH
#define tjsErrorDefsH
/*[*/
//---------------------------------------------------------------------------
// return values as tjs_error
//---------------------------------------------------------------------------
#define TJS_E_MEMBERNOTFOUND		(-1001)
#define TJS_E_NOTIMPL				(-1002)
#define TJS_E_INVALIDPARAM			(-1003)
#define TJS_E_BADPARAMCOUNT			(-1004)
#define TJS_E_INVALIDTYPE			(-1005)
#define TJS_E_INVALIDOBJECT			(-1006)
#define TJS_E_ACCESSDENYED			(-1007)
#define TJS_E_NATIVECLASSCRASH		(-1008)

#define TJS_S_TRUE					(1)
#define TJS_S_FALSE					(2)

#define TJS_S_OK                    (0)
#define TJS_E_FAIL					(-1)

#define TJS_FAILED(x)				((x)<0)
#define TJS_SUCCEEDED(x)			((x)>=0)




/*]*/

#endif
