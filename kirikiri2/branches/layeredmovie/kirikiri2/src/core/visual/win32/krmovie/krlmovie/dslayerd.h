/****************************************************************************/
/*! @file
@brief DirectShowを利用したムービーのレイヤー描画再生

-----------------------------------------------------------------------------
	Copyright (C) 2004 T.Imoto <http://www.kaede-software.com>
-----------------------------------------------------------------------------
@author		T.Imoto
@date		2004/08/25
@note
			2004/08/25	T.Imoto		
*****************************************************************************/

#ifndef __DSLAYERD_H__
#define __DSLAYERD_H__

#include "dsmovie.h"
#include "IRendererBufferAccess.h"

class tTVPBaseBitmap;
//----------------------------------------------------------------------------
//! @brief レイヤー描画ビデオクラス
//----------------------------------------------------------------------------
class tTVPDSLayerVideo : public tTVPDSMovie
{
private:
	CComPtr<IRendererBufferAccess>	m_BuffAccess;

//	tTVPBaseBitmap	*m_Bitmap[2];
	BYTE			*m_BmpBits[2];

	long			m_VideoWidth;
	long			m_VideoHeight;

	//----------------------------------------------------------------------------
	//! @brief	  	IBasicVideoを取得する
	//! @return		IBasicVideoインターフェイス
	//----------------------------------------------------------------------------
	IRendererBufferAccess *BufferAccess()
	{
		assert( m_BuffAccess.p );
		return m_BuffAccess;
	}
//	void AllocateVideoBuffer( long width, long height );
	HRESULT tTVPDSLayerVideo::ConnectFilters( IBaseFilter* pFilterUpstream, IBaseFilter* pFilterDownstream );

public:
	tTVPDSLayerVideo();
	virtual ~tTVPDSLayerVideo();

	virtual const wchar_t* __stdcall BuildGraph( HWND callbackwin, IStream *stream,
	const wchar_t * streamname, const wchar_t *type, unsigned __int64 size );

	virtual void __stdcall ReleaseAll();
//	virtual const wchar_t* __stdcall Update( class tTJSNI_BaseLayer *l1, class tTJSNI_BaseLayer *l2 );
//	virtual const wchar_t* __stdcall GetFrontBuffer( BYTE **buff );

	const wchar_t* __stdcall GetVideoSize( long *width, long *height );
	const wchar_t* __stdcall GetFrontBuffer( BYTE **buff );
	const wchar_t* __stdcall SetVideoBuffer( BYTE *buff1, BYTE *buff2, long size );

};

#endif	// __DSLAYERD_H__
