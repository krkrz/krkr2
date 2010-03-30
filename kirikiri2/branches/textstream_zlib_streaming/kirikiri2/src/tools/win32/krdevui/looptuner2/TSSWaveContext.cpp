//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "tvpsnd.h"
#include "TSSWaveContext.h"

//---------------------------------------------------------------------------
TStorageProviderImpl StorageProvider;
//---------------------------------------------------------------------------
HRESULT _stdcall TStorageProviderImpl::GetStreamForRead(LPWSTR url, IUnknown ** stream )
{
	TStream *tvpstream;

	try
	{
		tvpstream = new TFileStream(AnsiString(url), fmOpenRead|fmShareDenyWrite);
	}
	catch(...)
	{
		return E_FAIL;
	}

	TStreamAdapter * adapter;
	adapter = new TStreamAdapter(tvpstream, soOwned);

	IUnknown *istream = (IUnknown*)(IStream*)(*adapter);
	*stream = istream;
	istream->AddRef();

	return S_OK;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
__fastcall TTSSWaveContext::TTSSWaveContext(AnsiString dllname)
{
	FModule = NULL;
	FHandle = NULL;
	FDecoder = NULL;

	FHandle = LoadLibrary(dllname.c_str());
	if(FHandle == NULL)
	{
		throw Exception(dllname + "を読み込むことができません");
	}

	FGetModuleInstance =(GetModuleInstanceProc)
		GetProcAddress(FHandle, "GetModuleInstance");
	FGetModuleThreadModel =(GetModuleThreadModelProc)
		GetProcAddress(FHandle, "GetModuleThreadModel");
	FShowConfigWindow =(ShowConfigWindowProc)
		GetProcAddress(FHandle, "ShowConfigWindow");
	FCanUnloadNow = (CanUnloadNowProc)
		GetProcAddress(FHandle, "CanUnloadNow");

	if(!FGetModuleInstance || FAILED(FGetModuleInstance(&FModule, &StorageProvider,
		NULL, Application->Handle)) )
	{
		FreeLibrary(FHandle);
		FHandle = NULL;
		throw Exception("外部 DLL "+dllname+" は使用できません");
	}
}
//---------------------------------------------------------------------------
__fastcall TTSSWaveContext::~TTSSWaveContext()
{
	if(FDecoder) FDecoder->Release();
	if(FModule) FModule->Release();
	if(FHandle) FreeLibrary(FHandle);
}
//---------------------------------------------------------------------------
bool __fastcall TTSSWaveContext::Start(AnsiString filename)
{
	if(FDecoder) FDecoder->Release(), FDecoder = NULL;


	HRESULT hr;
	IUnknown * decoder;

	hr = FModule->GetMediaInstance(WideString(filename).c_bstr(), &decoder);
	if(FAILED(hr))
	{
		throw Exception(filename + " は開くことができません");
	}

	hr = decoder->QueryInterface(IID_ITSSWaveDecoder, (void**)&FDecoder);
	if(FAILED(hr))
	{
		decoder->Release();
		FDecoder = NULL;
		throw Exception(filename + " のメディア・タイプは扱うことができません");
	}

	decoder->Release();

	TSSWaveFormat format;
	FDecoder->GetFormat(&format);
	if(format.dwBitsPerSample != 16)
	{
		throw Exception(filename + " は 16bit PCM に変換できないため扱うことができません");
	}
	if(format.dwChannels > 8)
	{
		throw Exception(filename + " は 9チャネル以上あるため扱うことができません");
	}
	if(format.ui64TotalSamples == 0)
	{
		throw Exception(filename + " は 総サンプル数が不明か、ゼロのため扱うことができません");
	}
	if(format.ui64TotalSamples >= 0x10000000ui64)
	{
		throw Exception(filename + " は 大きすぎるため、扱うことができません");
	}
	FChannels = format.dwChannels;
	FFrequency = format.dwSamplesPerSec;
	FBitsPerSample = 16;
	FSpeakerConfig = 0; // 現時点では常に 0 (なにか規格をつくるかも)
	FTotalSamples = (int)format.ui64TotalSamples;

	FGranuleSize = FChannels * sizeof(__int16);

//	FDecoder->SetPosition(0); /// test

	return true;
}
//---------------------------------------------------------------------------
int __fastcall TTSSWaveContext::Read(__int16 * dest, int destgranules)
{
	unsigned long written;
	unsigned long status ;

	int pos = 0;
	int remain = destgranules;
	do
	{
		FDecoder->Render(dest + FChannels * pos, remain, &written, &status);
		remain -= written;
		pos += written;
	} while(remain >0 && status == 1);

	return pos;
}
//---------------------------------------------------------------------------
