//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Text read/write stream
//---------------------------------------------------------------------------
#ifndef TextStreamH
#define TextStreamH


#include "StorageIntf.h"

//---------------------------------------------------------------------------
// TextStream Functions
//---------------------------------------------------------------------------
extern tTJSTextReadStream * TVPCreateTextStreamForRead(const ttstr &name,
	const ttstr &modestr);
extern tTJSTextWriteStream * TVPCreateTextStreamForWrite(const ttstr &name,
	const ttstr &modestr);
//---------------------------------------------------------------------------


#endif
