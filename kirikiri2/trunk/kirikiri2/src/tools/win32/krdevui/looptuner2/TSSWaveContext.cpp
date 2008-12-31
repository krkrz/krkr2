//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

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
		throw Exception(dllname + "��ǂݍ��ނ��Ƃ��ł��܂���");
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
		throw Exception("�O�� DLL "+dllname+" �͎g�p�ł��܂���");
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
		throw Exception(filename + " �͊J�����Ƃ��ł��܂���");
	}

	hr = decoder->QueryInterface(IID_ITSSWaveDecoder, (void**)&FDecoder);
	if(FAILED(hr))
	{
		decoder->Release();
		FDecoder = NULL;
		throw Exception(filename + " �̃��f�B�A�E�^�C�v�͈������Ƃ��ł��܂���");
	}

	decoder->Release();

	TSSWaveFormat format;
	FDecoder->GetFormat(&format);
	if(format.dwBitsPerSample != 16)
	{
		throw Exception(filename + " �� 16bit PCM �ɕϊ��ł��Ȃ����߈������Ƃ��ł��܂���");
	}
	if(format.dwChannels > 8)
	{
		throw Exception(filename + " �� 9�`���l���ȏ゠�邽�߈������Ƃ��ł��܂���");
	}
	if(format.ui64TotalSamples == 0)
	{
		throw Exception(filename + " �� ���T���v�������s�����A�[���̂��߈������Ƃ��ł��܂���");
	}
	if(format.ui64TotalSamples >= 0x10000000ui64)
	{
		throw Exception(filename + " �� �傫�����邽�߁A�������Ƃ��ł��܂���");
	}
	FChannels = format.dwChannels;
	FFrequency = format.dwSamplesPerSec;
	FBitsPerSample = 16;
	FSpeakerConfig = 0; // �����_�ł͏�� 0 (�Ȃɂ��K�i�����邩��)
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
