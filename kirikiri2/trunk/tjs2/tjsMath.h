//---------------------------------------------------------------------------
/*
	TJS2 Script Engine
	Copyright (C) 2000-2004  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// TJS Math class implementation
//---------------------------------------------------------------------------
#ifndef tjsMathH
#define tjsMathH

#include "tjsNative.h"


namespace TJS
{

//---------------------------------------------------------------------------
class tTJSNC_Math : public tTJSNativeClass
{
public:
	tTJSNC_Math();
private:
	static tjs_uint32 ClassID;

};
//---------------------------------------------------------------------------
} // namespace TJS


#endif
