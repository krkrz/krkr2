/****************************************************************************/
/*! @file
@brief part of KRLMOVIE.DLL

-----------------------------------------------------------------------------
	Copyright (C) 2004 T.Imoto <http://www.kaede-software.com>
-----------------------------------------------------------------------------
@author		T.Imoto
@date		2004/09/22
@note
			2004/09/22	T.Imoto		
*****************************************************************************/

#include "dslayerd.h"
#include "..\..\krmovie.h"

#include "asyncio.h"
#include "asyncrdr.h"

//----------------------------------------------------------------------------
//! @brief	  	DLL のエントリーポイント
//! @param		hModule : 
//! @param		ul_reason_for_call : 
//! @param		lpReserved : 
//! @return		Always TRUE
//----------------------------------------------------------------------------
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
	return TRUE;
}
//----------------------------------------------------------------------------
//! @brief	  	VideoOverlay Objectを取得する
//! @param		callbackwin : 
//! @param		stream : 
//! @param		streamname : 
//! @param		type : 
//! @param		size : 
//! @param		out : VideoOverlay Object
//! @return		エラー文字列
//----------------------------------------------------------------------------
const wchar_t* __stdcall GetVideoOverlayObject(
	HWND callbackwin, IStream *stream, const wchar_t * streamname,
	const wchar_t *type, unsigned __int64 size, iTVPVideoOverlay **out)
{
	*out = new tTVPDSLayerVideo;

	if( *out )
		return static_cast<tTVPDSLayerVideo*>(*out)->BuildGraph( callbackwin, stream, streamname, type, size );
	else
		return NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	API のバージョンを取得
//! @param		ver : バージョン
//----------------------------------------------------------------------------
void __stdcall GetAPIVersion(DWORD *ver)
{
	*ver = TVP_KRMOVIE_VER;
}
