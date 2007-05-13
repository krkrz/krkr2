/****************************************************************************/
/*! @file
@brief VMR9を使うオーバーレイクラス

実行にはDirectX9以降が必要
-----------------------------------------------------------------------------
	Copyright (C) 2005 T.Imoto
-----------------------------------------------------------------------------
@author		T.Imoto
@date		2005/09/25
@note
			2005/09/25	T.Imoto		作成
*****************************************************************************/


#ifndef __DSMIXER_H__
#define __DSMIXER_H__

#include "dsoverlay.h"
#include <d3d9.h>
#include <vmr9.h>
#include <assert.h>
#include <ctlutil.h>
#include "CVMRCustomAllocatorPresenter9.h"
//----------------------------------------------------------------------------
//! @brief オーバーレイ ビデオミキシング描画クラス
//----------------------------------------------------------------------------
class tTVPDSMixerVideoOverlay : public tTVPDSMovie
{
private:
	CComPtr<IVMRMixerControl9>		m_VMR9MixerCtrl;
	CComPtr<IVMRMixerBitmap9>		m_VMR9MixerBmp;
	CComPtr<IVMRSurfaceAllocatorNotify9>	m_VMR9SurfAllocNotify;
	CVMRCustomAllocatorPresenter9	*m_AllocatorPresenter;
	REFERENCE_TIME					m_AvgTimePerFrame;
	long							m_Width;
	long							m_Height;

	HINSTANCE	m_OwnerInst;
	CCritSec	m_VMRLock;	// for VMR


	IVMRMixerControl9 *MixerControl()
	{
		assert( m_VMR9MixerCtrl.p );
		return m_VMR9MixerCtrl;
	}
	IVMRMixerBitmap9 *MixerBmp()
	{
		assert(m_VMR9MixerBmp.p);
		return m_VMR9MixerBmp;
	}
	IVMRSurfaceAllocatorNotify9 *AllocatorNotify()
	{
		assert( m_VMR9SurfAllocNotify.p );
		return m_VMR9SurfAllocNotify;
	}
	IVMRSurfaceAllocator9 *Allocator()
	{
		return static_cast<IVMRSurfaceAllocator9*>(m_AllocatorPresenter);
	}
	IVMRImagePresenter9 *Presenter()
	{
		return static_cast<IVMRImagePresenter9*>(m_AllocatorPresenter);
	}
	CVMRCustomAllocatorPresenter9 *AllocatorPresenter()
	{
		return m_AllocatorPresenter;
	}

public:
	tTVPDSMixerVideoOverlay();
	virtual ~tTVPDSMixerVideoOverlay();

	virtual void __stdcall BuildGraph( HWND callbackwin, IStream *stream,
		const wchar_t * streamname, const wchar_t *type, unsigned __int64 size );

	virtual void __stdcall SetWindow(HWND window);
	virtual void __stdcall SetRect(RECT *rect);
	virtual void __stdcall SetVisible(bool b);

	virtual void __stdcall GetVideoSize( long *width, long *height );
	virtual HRESULT __stdcall GetAvgTimePerFrame( REFTIME *pAvgTimePerFrame );

	virtual void __stdcall ReleaseAll();
	virtual void __stdcall SetMixingBitmap( HDC hdc, RECT *dest, float alpha );
	virtual void __stdcall ResetMixingBitmap();

	virtual void __stdcall SetMixingMovieAlpha( float a );
	virtual void __stdcall GetMixingMovieAlpha( float *a );
	virtual void __stdcall SetMixingMovieBGColor( unsigned long col );
	virtual void __stdcall GetMixingMovieBGColor( unsigned long *col );
	virtual void __stdcall PresentVideoImage();

	void AddVMR9Filer( CComPtr<IBaseFilter> &pVMR9 );

	friend class CVMRCustomAllocatorPresenter9;
};

#endif	// __DSMIXER_H__
