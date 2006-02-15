/****************************************************************************/
/*! @file
@brief VMR9のレンダーレスモード用アロケータとプレゼンタ

-----------------------------------------------------------------------------
	Copyright (C) 2005 T.Imoto ( http://www.kaede-software.com/ )
-----------------------------------------------------------------------------
@author		T.Imoto
@date		2005/12/24
@note
*****************************************************************************/

#include "CVMRCustomAllocatorPresenter9.h"
#include "dsmixer.h"
#include "DShowException.h"
#include "OptionInfo.h"

#define EC_UPDATE		(EC_USER+1)

ATOM CVMRCustomAllocatorPresenter9::m_ChildAtom = 0;

//----------------------------------------------------------------------------
//! @brief	  	CVMRCustomAllocatorPresenter9 constructor
//! @param		owner : このクラスを保持しているクラス
//! @param		lock : ロックオブジェクト
//----------------------------------------------------------------------------
CVMRCustomAllocatorPresenter9::CVMRCustomAllocatorPresenter9( tTVPDSMixerVideoOverlay* owner, CCritSec &lock )
 : CUnknown(NAME("VMR Custom Allocator Presenter"),NULL), m_ChildWnd(NULL), m_Visible(false)
 , m_Surfaces(NULL), m_dwUserID(NULL), m_Owner(owner), m_Lock(&lock), m_RebuildingWindow(false)
 {
	m_Rect.left = 0;
	m_Rect.top = 0;
	m_Rect.right = 0;
	m_Rect.bottom = 0;
	m_VideoSize.cx = 0;
	m_VideoSize.cy = 0;
}
//----------------------------------------------------------------------------
//! @brief	  	CDemuxSource destructor
//----------------------------------------------------------------------------
CVMRCustomAllocatorPresenter9::~CVMRCustomAllocatorPresenter9()
{
	ReleaseAll();
}
//----------------------------------------------------------------------------
//! @brief	  	初期化処理
//----------------------------------------------------------------------------
void CVMRCustomAllocatorPresenter9::Initialize()
{	
	CAutoLock	autoLock(m_Lock);
	HRESULT	hr;
	if( FAILED(hr = CreateChildWindow()) )
		ThrowDShowException(L"Failed to create window.", hr );

	if( FAILED(hr = CreateD3D() ) )
		ThrowDShowException(L"Failed to create device.", hr );
}
//----------------------------------------------------------------------------
//! @brief	  	要求されたインターフェイスを返す
//! @param		riid : インターフェイスのIID
//! @param		ppv : インターフェイスを返すポインターへのポインタ
//! @return		エラーコード
//----------------------------------------------------------------------------
STDMETHODIMP CVMRCustomAllocatorPresenter9::NonDelegatingQueryInterface( REFIID riid, void ** ppv )
{
	if( riid == IID_IVMRSurfaceAllocator9 ) {
		return GetInterface(static_cast<IVMRSurfaceAllocator9*>(this), ppv);
	} else if( riid == IID_IVMRImagePresenter9 ) {
		return GetInterface(static_cast<IVMRImagePresenter9*>(this), ppv);
	} else {
		return CUnknown::NonDelegatingQueryInterface(riid, ppv);
	}
}
//----------------------------------------------------------------------------
//! @brief	  	インターフェイスを解放する
//----------------------------------------------------------------------------
void CVMRCustomAllocatorPresenter9::ReleaseAll()
{
	CAutoLock Lock(m_Lock);
	ReleaseSurfaces();
	ReleaseD3D();
	DestroyChildWindow();
}
//----------------------------------------------------------------------------
//! @brief	  	Direct3D デバイスを初期化する ( 実際はサーフェイスを確保する)
//! @param		dwUserID : VMR のこのインスタンスを指定する
//! @param		lpAllocInfo : 初期化引数
//! @param		lpNumBuffers : 入力では、作成するバッファの数を指定する。メソッドが返ると、この引数には実際に割り当てられたバッファの数が格納されている
//! @return		エラーコード
//----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CVMRCustomAllocatorPresenter9::InitializeDevice( DWORD_PTR dwUserID, VMR9AllocationInfo *lpAllocInfo, DWORD *lpNumBuffers )
{
	if( lpAllocInfo == NULL || lpNumBuffers == NULL )
		return E_INVALIDARG;

	m_dwUserID = dwUserID;
	HRESULT hr;
	CAutoLock Lock(m_Lock);
	m_VideoSize.cx = lpAllocInfo->dwWidth;
	m_VideoSize.cy = lpAllocInfo->dwHeight;
	lpAllocInfo->Pool = D3DPOOL_MANAGED;
	ReleaseSurfaces();
    m_Surfaces.resize(*lpNumBuffers);
	if( FAILED(hr = AllocatorNotify()->AllocateSurfaceHelper( lpAllocInfo, lpNumBuffers, &m_Surfaces.at(0) )) )
		return hr;

	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	Direct3D デバイスを解放する ( 実際はサーフェイスを解放する)
//! @param		dwID : VMR のこのインスタンスを示す ID を指定する
//! @return		エラーコード
//----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CVMRCustomAllocatorPresenter9::TerminateDevice( DWORD_PTR dwID )
{
	HRESULT hr = S_OK;

//	CAutoLock Lock(m_Lock);
	if( m_dwUserID == dwID ) {
		if( FAILED(hr = ReleaseSurfaces()) )
			return hr;
		return S_OK;
	} else {
		return S_OK;
	}
}
//----------------------------------------------------------------------------
//! @brief	  	Direct3D サーフェイスを取得する
//! @param		dwUserID : VMR のこのインスタンスを示す ID を指定する
//! @param		SurfaceIndex : 取得するサーフェイスのインデックスを指定する
//! @param		SurfaceFlags : サーフェイス フラグを指定する ( 何に使うの？ )
//! @param		lplpSurface : IDirect3DSurface9 インターフェイス ポインタを受け取る変数のアドレス。呼び出し元はインターフェイスを必ず解放すること
//! @return		エラーコード
//----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CVMRCustomAllocatorPresenter9::GetSurface( DWORD_PTR dwUserID, DWORD SurfaceIndex, DWORD SurfaceFlags, IDirect3DSurface9 **lplpSurface )
{
    if( lplpSurface == NULL )
        return E_POINTER;

	if( m_dwUserID == dwUserID ) {
		CAutoLock Lock(m_Lock);
		if( SurfaceIndex < m_Surfaces.size() )
		{
			CAutoLock Lock(m_Lock);
			m_Surfaces[SurfaceIndex].CopyTo( lplpSurface );
			if( *lplpSurface == NULL )
				return E_FAIL;
			(*lplpSurface)->AddRef();
			return S_OK;
		}
		else
		{
			return E_FAIL;
		}
	}
	return E_INVALIDARG;
}
//----------------------------------------------------------------------------
//! @brief	  	VMR から呼び出され、アロケータプレゼンタに通知コールバックのインターフェイス ポインタを提供する
//! @param		lpIVMRSurfAllocNotify : アロケータプレゼンタが通知コールバックを VMR に渡すために使う、IVMRSurfaceAllocatorNotify9 インターフェイスを指定する
//! @return		エラーコード
//----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CVMRCustomAllocatorPresenter9::AdviseNotify( IVMRSurfaceAllocatorNotify9 *lpIVMRSurfAllocNotify )
{
	CAutoLock Lock(m_Lock);
	m_VMR9SurfAllocNotify = lpIVMRSurfAllocNotify;
    HMONITOR hMonitor = D3D()->GetAdapterMonitor( D3DADAPTER_DEFAULT );
	HRESULT	hr;
    if( FAILED(hr = AllocatorNotify()->SetD3DDevice( D3DDevice(), hMonitor ) ) )
		return hr;
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	Direct3D関連のものを開放する
//! @return		エラーコード
//----------------------------------------------------------------------------
HRESULT CVMRCustomAllocatorPresenter9::ReleaseD3D()
{
	CAutoLock Lock(m_Lock);
	if( m_D3DDevice.p )
		m_D3DDevice.Release();

	if( m_D3D.p )
		m_D3D.Release();
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	サーフェイスを開放する
//! @return		エラーコード
//----------------------------------------------------------------------------
HRESULT CVMRCustomAllocatorPresenter9::ReleaseSurfaces()
{
	CAutoLock Lock(m_Lock);
	for( DWORD i = 0; i < m_Surfaces.size(); ++i )
	{
		m_Surfaces[i] = NULL;
	}
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオが再生を開始する直前に呼び出す。アロケータプレゼンタは、このメソッドの必要な構成を実行する必要がある
//! @param		dwUserID : アロケータプレゼンタの 1 つのインスタンスが複数の VMR インスタンスで使われる場合に使う、VMR のこのインスタンスを一意に識別するアプリケーションが定義した DWORD_PTR クッキー
//! @return		エラーコード
//----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CVMRCustomAllocatorPresenter9::StartPresenting( DWORD_PTR dwUserID )
{
	CAutoLock Lock(m_Lock);
	if( m_D3DDevice.p == NULL )
		return E_FAIL;
    return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	このビデオ フレームを表示しなければならないときに呼び出される
//! @param		dwUserID : アロケータプレゼンタの 1 つのインスタンスが複数の VMR インスタンスで使われる場合に使う、VMR のこのインスタンスを一意に識別するアプリケーションが定義した DWORD_PTR クッキー
//! @param		lpPresInfo : ビデオ フレームに関する情報を格納する VMR9PresentationInfo 構造体を指定する
//! @return		エラーコード
//----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CVMRCustomAllocatorPresenter9::PresentImage( DWORD_PTR dwUserID, VMR9PresentationInfo *lpPresInfo )
{
	if( (lpPresInfo == NULL) || (lpPresInfo->lpSurf == NULL) )
		return E_POINTER;

	HRESULT hr = S_OK;
	if( m_dwUserID == dwUserID ) {
		CAutoLock Lock(m_Lock);
		AllocatorNotify()->NotifyEvent(EC_UPDATE,0,0);
		if( m_RebuildingWindow ) return S_OK;	// フルスクリーン切り替え中は描画しない
		hr = PresentHelper( lpPresInfo );
#if 1
		if( hr == D3DERR_DEVICELOST)
		{
			hr = D3DDevice()->TestCooperativeLevel();
			if( hr == D3DERR_DEVICENOTRESET )
			{	// リセット可能
				ReleaseSurfaces();
				RebuildD3DDevice();
//				if( FAILED(hr = RebuildD3DDevice()) )
//					AllocatorNotify()->NotifyEvent(EC_ERRORABORT,hr,0);
				// 失敗しても通知しない。
				// どうやらフルスクリーン切り替え時などに何度か失敗することがあるようだ
			}
		}
#endif
	}
	return hr;
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオ フレームを描画する
//! @param		lpPresInfo : ビデオ フレームに関する情報を格納する VMR9PresentationInfo 構造体を指定する
//! @return		エラーコード
//----------------------------------------------------------------------------
HRESULT CVMRCustomAllocatorPresenter9::PresentHelper( VMR9PresentationInfo *lpPresInfo )
{
	HRESULT hr;

	CAutoLock Lock(m_Lock);
	CComPtr<IDirect3DDevice9> device;
	if( FAILED(hr = lpPresInfo->lpSurf->GetDevice(&device.p )) )
		return hr;

	IDirect3DSurface9	*pBackBuffer;
	if( SUCCEEDED(hr = device->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer )) )
	{
		if( FAILED(hr = device->StretchRect( lpPresInfo->lpSurf, NULL, pBackBuffer, NULL, D3DTEXF_NONE )) )
			AllocatorNotify()->NotifyEvent(EC_ERRORABORT,hr,0);
		pBackBuffer->Release();
	}
//	hr = device->Present( NULL, NULL, m_ChildWnd, NULL );
//	hr = device->Present( NULL, NULL, m_ChildWnd, NULL );
//		hr = device->Present( NULL, NULL, NULL, NULL );
//	if( FAILED(hr = device->Present( NULL, NULL, m_ChildWnd, NULL )) )
//		AllocatorNotify()->NotifyEvent(EC_ERRORABORT,hr,0);
	return hr;
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオ フレームを画面に反映する
//----------------------------------------------------------------------------
void CVMRCustomAllocatorPresenter9::PresentVideoImage()
{
	CAutoLock Lock(m_Lock);
	if( m_D3DDevice.p ) {
		HRESULT hr;
		hr = m_D3DDevice->Present( NULL, NULL, m_ChildWnd, NULL );
		if( hr == D3DERR_DEVICELOST)
		{
			hr = D3DDevice()->TestCooperativeLevel();
			if( hr == D3DERR_DEVICENOTRESET )
			{	// リセット可能
				ReleaseSurfaces();
				RebuildD3DDevice();
			}
		}
	}
}
//----------------------------------------------------------------------------
//! @brief	  	ウィンドウのあるモニタの序数を取得する
//! @return		ウィンドウのあるモニタの序数
//----------------------------------------------------------------------------
UINT CVMRCustomAllocatorPresenter9::GetMonitorNumber()
{
	CAutoLock Lock(m_Lock);
	if( m_D3D.p == NULL || m_ChildWnd == NULL ) return 0;
	HMONITOR windowMonitor = MonitorFromWindow( m_ChildWnd, MONITOR_DEFAULTTOPRIMARY );
	UINT iCurrentMonitor = 0;
	UINT numOfMonitor = D3D()->GetAdapterCount();
	for( ; iCurrentMonitor < numOfMonitor; ++numOfMonitor )
	{
		if( D3D()->GetAdapterMonitor(iCurrentMonitor) == windowMonitor )
			break;
	}
	if( iCurrentMonitor == numOfMonitor )
		iCurrentMonitor = D3DADAPTER_DEFAULT;
	return iCurrentMonitor;
}
//----------------------------------------------------------------------------
//! @brief	  	Direct3Dを初期化する
//! @return		エラーコード
//----------------------------------------------------------------------------
HRESULT CVMRCustomAllocatorPresenter9::CreateD3D()
{
	HRESULT	hr;

	CAutoLock Lock(m_Lock);
	ReleaseSurfaces();
	ReleaseD3D();
	
	if( m_D3DDll.IsLoaded() == false )
		m_D3DDll.Load("d3d9.dll");
	if( m_D3DDll.IsLoaded() == false )
		return m_D3DDll.GetLastError();

	typedef IDirect3D9 *(WINAPI *FuncDirect3DCreate9)( UINT SDKVersion );
	FuncDirect3DCreate9 pDirect3DCreate9 = (FuncDirect3DCreate9)m_D3DDll.GetProcAddress("Direct3DCreate9");

	if( pDirect3DCreate9 == NULL )
		return E_FAIL;

	if( NULL == ( m_D3D = pDirect3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;

	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.BackBufferHeight = m_VideoSize.cy;
	d3dpp.BackBufferWidth = m_VideoSize.cx;
	d3dpp.hDeviceWindow = m_ChildWnd;
	UINT iCurrentMonitor = GetMonitorNumber();

	DWORD	BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
	if( D3D_OK != ( hr = D3D()->CreateDevice( iCurrentMonitor, D3DDEVTYPE_HAL, NULL, BehaviorFlags, &d3dpp, &m_D3DDevice ) ) )
		return hr;
//	m_CurPresentParam = d3dpp;
	m_ThreadID = GetCurrentThreadId();

	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	Direct3Dデバイスの変更を通知する
//! @return		エラーコード
//----------------------------------------------------------------------------
HRESULT CVMRCustomAllocatorPresenter9::ChangeD3DDevice()
{
	HRESULT	hr;

	CAutoLock Lock(m_Lock);
	D3DDEVICE_CREATION_PARAMETERS	Parameters;
	if( FAILED( hr = D3DDevice()->GetCreationParameters(  &Parameters ) ) )
		return hr;

	CComPtr<IDirect3D9>			pD3D;
	if( FAILED( hr = D3DDevice()->GetDirect3D(&pD3D) ) )
		return hr;

	HMONITOR hMonitor = pD3D->GetAdapterMonitor(Parameters.AdapterOrdinal);
	if( FAILED( hr = AllocatorNotify()->ChangeD3DDevice( D3DDevice(), hMonitor ) ) )
		return hr;
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	Direct3Dデバイスを再構築する
//! @return		エラーコード
//----------------------------------------------------------------------------
HRESULT CVMRCustomAllocatorPresenter9::RebuildD3DDevice()
{
	HRESULT	hr;
	CAutoLock Lock(m_Lock);
	if( FAILED(hr = CreateD3D()) )
		return hr;
	if( FAILED(hr = ChangeD3DDevice() ) )
		return hr;
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	バックバッファのサイズを変更する
//! @return		エラーコード
//----------------------------------------------------------------------------
HRESULT CVMRCustomAllocatorPresenter9::ResizeBackbuffer()
{
	HRESULT	hr = S_OK;
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	CAutoLock Lock(m_Lock);

	if( m_ThreadID != GetCurrentThreadId() )
		OutputDebugString("may be cannot reset.");

	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.BackBufferHeight = m_VideoSize.cy;
	d3dpp.BackBufferWidth = m_VideoSize.cx;
	d3dpp.hDeviceWindow = m_ChildWnd;
	hr = D3DDevice()->Reset(&d3dpp);
	if( hr == D3DERR_DEVICELOST )
		OutputDebugString("Cannot reset device.");
	else if( hr == D3DERR_DRIVERINTERNALERROR )
	{
		OutputDebugString("Device internal fatal error.");
		AllocatorNotify()->NotifyEvent(EC_ERRORABORT,hr,0);
	}
	else if( hr == D3DERR_INVALIDCALL )
		OutputDebugString("Invalid call.");
	else if( hr  == D3DERR_OUTOFVIDEOMEMORY )
		OutputDebugString("Cannot allocate video memory.");
	else if( hr == E_OUTOFMEMORY  )
		OutputDebugString("Cannot allocate memory.");
	return hr;
}
//----------------------------------------------------------------------------
//! @brief	  	子ウィンドウを生成する
//! @return		エラーコード
//----------------------------------------------------------------------------
HRESULT CVMRCustomAllocatorPresenter9::CreateChildWindow()
{
	if( m_ChildWnd != NULL ) return S_OK;
	if( Owner()->OwnerWindow == NULL ) return E_FAIL;

	CAutoLock Lock(m_Lock);
	if( m_ChildAtom == 0 )
	{
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_PARENTDC | CS_VREDRAW | CS_HREDRAW, (WNDPROC)CVMRCustomAllocatorPresenter9::WndProc, 0L, 0L, Owner()->m_OwnerInst, NULL, NULL, NULL, NULL, _T("krmovie VMR9 Child Window Class"), NULL };
		m_ChildAtom = RegisterClassEx(&wcex);
		if( m_ChildAtom == 0 )
			return HRESULT_FROM_WIN32(GetLastError());
	}

	DWORD	atom = (DWORD)(0xFFFF & m_ChildAtom);
	if( (m_Rect.right - m_Rect.left) != 0 && (m_Rect.bottom - m_Rect.top) != 0 )
//		m_ChildWnd = CreateWindow( (LPCTSTR)atom, "VMR9 child", WS_CHILDWINDOW, 0, 0, m_Rect.right - m_Rect.left, m_Rect.bottom - m_Rect.top, Owner()->OwnerWindow, NULL, Owner()->m_OwnerInst, NULL );
		m_ChildWnd = CreateWindow( _T("krmovie VMR9 Child Window Class"), "VMR9 child", WS_CHILDWINDOW, 0, 0, m_Rect.right - m_Rect.left, m_Rect.bottom - m_Rect.top, Owner()->OwnerWindow, NULL, Owner()->m_OwnerInst, NULL );
	else
//		m_ChildWnd = CreateWindow( (LPCTSTR)atom, "VMR9 child", WS_CHILDWINDOW, 0, 0, 320, 240, Owner()->OwnerWindow, NULL, Owner()->m_OwnerInst, NULL );
		m_ChildWnd = CreateWindow( _T("krmovie VMR9 Child Window Class"), "VMR9 child", WS_CHILDWINDOW, 0, 0, 320, 240, Owner()->OwnerWindow, NULL, Owner()->m_OwnerInst, NULL );
	if( m_ChildWnd == NULL )
		return HRESULT_FROM_WIN32(GetLastError());

	SetWindowLong(m_ChildWnd,GWL_USERDATA,(LONG)this);
	ShowWindow(m_ChildWnd,SW_SHOWDEFAULT);
	if( UpdateWindow(m_ChildWnd) == 0 )
		return HRESULT_FROM_WIN32(GetLastError());
	return S_OK;
}
//----------------------------------------------------------------------------
//! @brief	  	子ウィンドウを破棄する
//----------------------------------------------------------------------------
void CVMRCustomAllocatorPresenter9::DestroyChildWindow()
{
	CAutoLock Lock(m_Lock);
	if( m_ChildWnd != NULL )
	{
		SetWindowLong(m_ChildWnd,GWL_USERDATA,0);
		DestroyWindow(m_ChildWnd);
	}
	m_ChildWnd = NULL;
}
//----------------------------------------------------------------------------
//! @brief	  	ウィンドウプロシージャ
//! @param		hWnd : ウィンドウハンドル
//! @param		msg : メッセージID
//! @param		wParam : パラメタ
//! @param		lParam : パラメタ
//! @return		エラーコード
//----------------------------------------------------------------------------
LRESULT WINAPI CVMRCustomAllocatorPresenter9::WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	CVMRCustomAllocatorPresenter9	*win = reinterpret_cast<CVMRCustomAllocatorPresenter9*>(GetWindowLong(hWnd,GWL_USERDATA));
	if( win != NULL )
	{
		return win->Proc( hWnd, msg, wParam, lParam );
	}
	return DefWindowProc(hWnd,msg,wParam,lParam);
}
//----------------------------------------------------------------------------
//! @brief	  	ウィンドウプロシージャ
//! @param		hWnd : ウィンドウハンドル
//! @param		msg : メッセージID
//! @param		wParam : パラメタ
//! @param		lParam : パラメタ
//! @return		エラーコード
//----------------------------------------------------------------------------
LRESULT WINAPI CVMRCustomAllocatorPresenter9::Proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	if( msg == WM_PAINT ) {
		PAINTSTRUCT ps;
		HDC			hDC;
		hDC = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0;
	}

	return DefWindowProc(hWnd,msg,wParam,lParam);
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオのサイズを設定する
//! @param		rect : 要求するサイズ
//----------------------------------------------------------------------------
void CVMRCustomAllocatorPresenter9::SetRect( RECT *rect )
{
	m_Rect = *rect;
	if( m_ChildWnd != NULL )
	{
		if( MoveWindow( m_ChildWnd, m_Rect.left, m_Rect.top, m_Rect.right - m_Rect.left, m_Rect.bottom - m_Rect.top, TRUE ) == 0 )
			ThrowDShowException(L"Failed to call MoveWindow.", HRESULT_FROM_WIN32(GetLastError()));
	}
}
//----------------------------------------------------------------------------
//! @brief	  	ビデオの表示/非表示を設定する
//! @param		b : 表示/非表示
//----------------------------------------------------------------------------
void CVMRCustomAllocatorPresenter9::SetVisible( bool b )
{
	m_Visible = b;
	if( m_ChildWnd != NULL )
	{
		if( b == false )
			ShowWindow( m_ChildWnd, SW_HIDE );
		else
		{
			ShowWindow( m_ChildWnd, SW_SHOW );
			if( UpdateWindow( m_ChildWnd ) == 0 )
				ThrowDShowException(L"Failed to call ShowWindow.", HRESULT_FROM_WIN32(GetLastError()));
		}

		if( MoveWindow( m_ChildWnd, m_Rect.left, m_Rect.top, m_Rect.right - m_Rect.left, m_Rect.bottom - m_Rect.top, TRUE ) == 0 )
			ThrowDShowException(L"Failed to call MoveWindow.", HRESULT_FROM_WIN32(GetLastError()));
	}
}
//----------------------------------------------------------------------------
//! @brief	  	Direct3D Deviceをリセットする
//----------------------------------------------------------------------------
void CVMRCustomAllocatorPresenter9::Reset()
{
	HRESULT	hr;
	if( Owner()->OwnerWindow != NULL )
	{
		CAutoLock Lock(m_Lock);
		DestroyChildWindow();
		if( FAILED(hr = CreateChildWindow() ) )
			ThrowDShowException(L"Failed to create window.", hr );

		if( FAILED(hr = ResizeBackbuffer()) )
		{
			if( FAILED(hr = RebuildD3DDevice()) )
				ThrowDShowException(L"Failed to rebuild Device.", hr );
		}
		m_RebuildingWindow = false;
	} else {
		m_RebuildingWindow = true;
	}
}
