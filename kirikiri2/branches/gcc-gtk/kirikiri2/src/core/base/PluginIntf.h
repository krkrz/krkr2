//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info>

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// "Plugins" class interface
//---------------------------------------------------------------------------
#ifndef PluginIntfH
#define PluginIntfH

#include "tjsNative.h"

//---------------------------------------------------------------------------
// tTJSNC_Plugins : TJS Plugins class
//---------------------------------------------------------------------------
class tTJSNC_Plugins : public tTJSNativeClass
{
	typedef tTJSNativeClass inherited;

public:
	tTJSNC_Plugins();
	static tjs_uint32 ClassID;

protected:
	tTJSNativeInstance *CreateNativeInstance();
};
//---------------------------------------------------------------------------
extern tTJSNativeClass * TVPCreateNativeClass_Plugins();
//---------------------------------------------------------------------------


#endif
