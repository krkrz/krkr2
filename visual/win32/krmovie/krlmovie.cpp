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

#include "tp_stub.h"

#include "OptionInfo.h"


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
void __stdcall GetVideoOverlayObject(
	HWND callbackwin, IStream *stream, const wchar_t * streamname,
	const wchar_t *type, unsigned __int64 size, iTVPVideoOverlay **out)
{
	*out = new tTVPDSLayerVideo;

	if( *out )
		static_cast<tTVPDSLayerVideo*>(*out)->BuildGraph( callbackwin, stream, streamname, type, size );
}
//----------------------------------------------------------------------------
//! @brief	  	API のバージョンを取得
//! @param		ver : バージョン
//----------------------------------------------------------------------------
void __stdcall GetAPIVersion(DWORD *ver)
{
	*ver = TVP_KRMOVIE_VER;
}


//---------------------------------------------------------------------------
// V2Link : Initialize TVP plugin interface
//---------------------------------------------------------------------------
HRESULT __stdcall V2Link(iTVPFunctionExporter *exporter)
{
	TVPInitImportStub(exporter);

	return S_OK;
}
//---------------------------------------------------------------------------
// V2Unlink : Uninitialize TVP plugin interface
//---------------------------------------------------------------------------
HRESULT __stdcall V2Unlink()
{
	TVPUninitImportStub();

	return S_OK;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// GetOptionDesc : Give option information to kirikiri configurator
//---------------------------------------------------------------------------
extern "C" const wchar_t * _stdcall GetOptionDesc()
{
	return GetOptionInfoString();
}
//---------------------------------------------------------------------------
