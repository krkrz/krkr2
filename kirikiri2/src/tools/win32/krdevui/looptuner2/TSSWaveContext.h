//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
#ifndef TSSWaveContextH
#define TSSWaveContextH

#include <mmsystem.h>
#include <mmreg.h>
#include <ks.h>
#include <ksmedia.h>
#include "WaveContext.h"
#include "tvpsnd.h"


//---------------------------------------------------------------------------
class ITSSModule;
class ITSSStorageProvider;
class ITSSWaveDecoder;
class TTSSWaveContext : public TCustomWaveContext
{
	typedef HRESULT _stdcall (*GetModuleInstanceProc)(ITSSModule **out,
		ITSSStorageProvider *provider, IStream * config, HWND mainwin);
	typedef ULONG _stdcall (*GetModuleThreadModelProc)(void);
	typedef HRESULT _stdcall (*ShowConfigWindowProc)(HWND parentwin, IStream * storage );
	typedef ULONG _stdcall (*CanUnloadNowProc)(void);

	GetModuleInstanceProc FGetModuleInstance;
	GetModuleThreadModelProc FGetModuleThreadModel;
	ShowConfigWindowProc FShowConfigWindow;
	CanUnloadNowProc FCanUnloadNow;

	ITSSModule *FModule;
	HMODULE FHandle;
	ITSSWaveDecoder *FDecoder;


	int FGranuleSize;
	int FChannels;
	int FFrequency;
	int FBitsPerSample;
	int FTotalSamples;
	DWORD FSpeakerConfig;

protected:
	int __fastcall GetChannels() { return FChannels; }
	int __fastcall GetFrequency() { return FFrequency; }
	int __fastcall GetBitsPerSample() { return FBitsPerSample; }
	int __fastcall GetTotalSamples() { return FTotalSamples; }
	DWORD __fastcall GetSpeakerConfig() { return FSpeakerConfig; }

public:
	__fastcall TTSSWaveContext(AnsiString dllname);
	__fastcall ~TTSSWaveContext();

	bool __fastcall Start(AnsiString filename);
	int __fastcall Read(__int16 * dest, int destgranules);
};
//---------------------------------------------------------------------------
class TStorageProviderImpl : public ITSSStorageProvider
{
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid,void **ppvObjOut)
	{
		if(!ppvObjOut) return E_INVALIDARG;

		*ppvObjOut=NULL;
		if(!memcmp(&iid,&IID_IUnknown,16))
			*ppvObjOut=(IUnknown*)this;
		else if(!memcmp(&iid,&IID_ITSSStorageProvider,16))
			*ppvObjOut=(ITSSStorageProvider*)this;

		if(*ppvObjOut)
		{
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}

	ULONG STDMETHODCALLTYPE AddRef(void) { return 1; }
	ULONG STDMETHODCALLTYPE Release(void) { return 1; }

	HRESULT _stdcall GetStreamForRead(LPWSTR url, IUnknown ** stream );
	HRESULT _stdcall GetStreamForWrite(LPWSTR url, IUnknown ** stream )
		{ return E_NOTIMPL; }
	HRESULT _stdcall GetStreamForUpdate(LPWSTR url, IUnknown ** stream )
		{ return E_NOTIMPL; }
};
//---------------------------------------------------------------------------
extern TStorageProviderImpl StorageProvider;
//---------------------------------------------------------------------------

#endif

