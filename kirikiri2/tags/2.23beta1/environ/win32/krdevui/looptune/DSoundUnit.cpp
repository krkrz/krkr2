//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <syncobjs.hpp>

#define DIRECTSOUND_VERSION 0x0300
#include <mmsystem.h>
#include <dsound.h>


#include <mmreg.h>
#include <ks.h>
#include <ksmedia.h>

#include "DSoundUnit.h"

//---------------------------------------------------------------------------
static void __fastcall ResetSoundBuffer();
static void __fastcall FillSecondaryBuffer(void);
//---------------------------------------------------------------------------
static TCriticalSection *WriteLock = NULL;
class TBufferWriteLock // ロック用クラス
{
public:
	__fastcall TBufferWriteLock(void) { if(WriteLock) WriteLock->Enter(); };
	__fastcall ~TBufferWriteLock(void) { if(WriteLock) WriteLock->Leave(); };
};
//---------------------------------------------------------------------------
class TPlayerThread : public TThread
{
public:
	__fastcall TPlayerThread(void) : TThread(true)
	{
		Priority = tpHigher;
		Suspended = false;
	}

	__fastcall ~TPlayerThread(void)
	{
		Suspended = false;
		Terminate();
		WaitFor();
	}

	void __fastcall Execute(void)
	{
		while(!Terminated)
		{
			FillSecondaryBuffer();
			Sleep(110);
		}
	}

} ;
//---------------------------------------------------------------------------
static HRESULT WINAPI (* procDirectSoundCreate)(LPGUID, LPDIRECTSOUND *, LPUNKNOWN)=NULL;
static HMODULE DirectSoundDLLHandle=NULL;
static IDirectSound * Sound = NULL;
static IDirectSoundBuffer * SoundPrimaryBuffer = NULL;
static IDirectSoundBuffer * SoundBuffer = NULL;
static DWORD SoundBufferBytes;
static DWORD SoundBufferBytesHalf;
static DSBUFFERDESC SoundBufferDesc;
static int Frequency;
static int Channels;
static int SampleSize;
static bool DecodeEnded;
static int BufferRemainBlock;
static DWORD SoundBufferWritePos;
static DWORD SoundBufferDecodeOfs;
static DWORD SoundBufferPrevPlayPos;
static int SoundBufferLoopCount;
static bool FirstWrite;
static bool Playing = false;
static TGetPCMDataEvent GetPCMData = NULL;
static TPlayerThread *PlayerThread = NULL;
//---------------------------------------------------------------------------
void __fastcall InitDirectSound(HWND wnd)
{
	if(!Sound)
	{
		// DLL のロード

		if(DirectSoundDLLHandle==NULL)
		{
			DirectSoundDLLHandle = LoadLibrary("dsound.dll");
			if(!DirectSoundDLLHandle) throw Exception("dsound.dll をロードできません");
		}

		if(procDirectSoundCreate==NULL)
		{
			procDirectSoundCreate = (HRESULT (WINAPI*)(_GUID *,IDirectSound **,IUnknown*))
				GetProcAddress(DirectSoundDLLHandle,"DirectSoundCreate");
			if(!procDirectSoundCreate)
			{
				FreeLibrary(DirectSoundDLLHandle);
				DirectSoundDLLHandle=NULL;
				throw Exception("dsound.dll が異常です");
			}
		}
		// オブジェクト作成
		HRESULT hr;
		hr=procDirectSoundCreate(NULL,&Sound,NULL);
		if(FAILED(hr))
		{
			throw Exception(
				AnsiString("DirectSound を初期化できません(他でWaveを再生中?)/HRESULT:")+
					IntToHex((int)hr,8));
				// 作成に失敗　他に WAVE output が使われていた場合など、
				// こうなる
		}

		// 協調レベルの設定
		hr=Sound->SetCooperativeLevel(wnd, DSSCL_PRIORITY);
		if(FAILED(hr))
		{
			Sound->Release(); // サウンドオブジェクトをリリース
			Sound=NULL;
			throw Exception(AnsiString("DirectSound の協調レベルの設定に失敗しました(phase1)/HRESULT:")+
				IntToHex((int)hr,8));
		}

//--
		// プライマリバッファの作成

		SoundPrimaryBuffer= NULL; // 別に作成できなきゃそれでもいい

		ZeroMemory(&SoundBufferDesc,sizeof(DSBUFFERDESC));
		SoundBufferDesc.dwSize=sizeof(DSBUFFERDESC);
		SoundBufferDesc.dwFlags=DSBCAPS_PRIMARYBUFFER;
		hr=Sound->CreateSoundBuffer(&SoundBufferDesc,
			&SoundPrimaryBuffer,NULL);

		if(FAILED(hr) || SoundPrimaryBuffer == NULL)
		{
			// DbgMsgAdd("警告:DirectSound プライマリバッファを作成できません");
			hr=Sound->SetCooperativeLevel(wnd, DSSCL_NORMAL);
			if(FAILED(hr))
			{
				Sound->Release(); // サウンドオブジェクトをリリース
				Sound = NULL;
				throw Exception(AnsiString("DirectSound の協調レベルの設定に失敗しました(phase2)/HRESULT:")+
					IntToHex((int)hr,8));
			}
		}

		
		if(SoundPrimaryBuffer)
		{
			WAVEFORMATEX wfx;
			wfx.cbSize=0;
			wfx.wFormatTag=WAVE_FORMAT_PCM;
			wfx.nChannels=2;
			wfx.nSamplesPerSec=44100;
			wfx.wBitsPerSample=16;
			wfx.nBlockAlign=(WORD)(wfx.wBitsPerSample/8*wfx.nChannels);
			wfx.nAvgBytesPerSec=wfx.nSamplesPerSec*wfx.nBlockAlign;
			SoundPrimaryBuffer->SetFormat(&wfx);
		}
		if(!WriteLock) WriteLock = new TCriticalSection;

		if(!PlayerThread)
			PlayerThread= new TPlayerThread();
	}
}
//---------------------------------------------------------------------------
void __fastcall FreeDirectSound(void)
{
	// サウンドバッファの破棄
	if(PlayerThread)
	{
		delete PlayerThread;
	}

	if(SoundBuffer)
	{
		SoundBuffer->Release();
		SoundBuffer = NULL;
	}

	if(Sound)
	{
		if(SoundPrimaryBuffer) SoundPrimaryBuffer->Release();
		Sound->Release();
		SoundPrimaryBuffer=NULL;
		Sound=NULL;

		// DLL の解放
		if(DirectSoundDLLHandle)
		{
			FreeLibrary(DirectSoundDLLHandle);
			DirectSoundDLLHandle=NULL;
			procDirectSoundCreate=NULL;
		}
	}


	if(WriteLock) delete WriteLock;
}
//---------------------------------------------------------------------------
void __fastcall SetPrimaryBufferFormat(WAVEFORMATEX *wfx)
{
	if(SoundPrimaryBuffer)
		SoundPrimaryBuffer->SetFormat(wfx);
}
//---------------------------------------------------------------------------
void __fastcall CreateSoundBuffer(const WAVEFORMATEXTENSIBLE * wfx)
{
	// 2 秒間のセカンダリバッファを作成する

	/* wfx.Format の nAvgBytesPerSec や nBlockAlign は変な値になっていないかどうか
	   チェックが必要 */

	if(SoundBuffer)
	{
		SoundBuffer->Release();
		SoundBuffer = NULL;
	}

	// プライマリバッファのフォーマットを再設定
	if(SoundPrimaryBuffer)
	{
		SoundPrimaryBuffer->SetFormat((const WAVEFORMATEX *)wfx);
	}

	// セカンダリバッファの作成
	memset(&SoundBufferDesc,0,sizeof(DSBUFFERDESC));
	SoundBufferDesc.dwSize=sizeof(DSBUFFERDESC);
	SoundBufferDesc.dwFlags=
		DSBCAPS_GETCURRENTPOSITION2/* | DSBCAPS_CTRLPAN */| DSBCAPS_CTRLVOLUME
		 |DSBCAPS_GLOBALFOCUS ;
	SoundBufferDesc.dwBufferBytes = wfx->Format.nAvgBytesPerSec * 2;
	SoundBufferBytes = SoundBufferDesc.dwBufferBytes;
	SoundBufferBytesHalf = SoundBufferBytes / 2;

	Frequency = wfx->Format.nSamplesPerSec;
	Channels = wfx->Format.nChannels;
	SampleSize = wfx->Format.nBlockAlign;

	SoundBufferDesc.lpwfxFormat = (WAVEFORMATEX*)wfx;


	// セカンダリバッファを作成する
	HRESULT hr = Sound->CreateSoundBuffer(&SoundBufferDesc,&SoundBuffer,NULL);
	if(FAILED(hr))
	{
		SoundBuffer=NULL;
		throw Exception("セカンダリバッファの作成に失敗しました"
			"/周波数:"+AnsiString(wfx->Format.nSamplesPerSec)+
			"/チャネル数:"+AnsiString(wfx->Format.nChannels)+
			"/ビット数:"+AnsiString(16)+
			"/HRESULT:"+IntToHex((int)hr,8));
	}

	ResetSoundBuffer();
}
//---------------------------------------------------------------------------
#if 0
void __fastcall CreateSoundBuffer(int freq, int channels)
{
	// 16bit/sample 2 秒間のセカンダリバッファを作成する
	if(SoundBuffer)
	{
		SoundBuffer->Release();
		SoundBuffer = NULL;
	}

	// セカンダリサウンドバッファの作成
	memset(&SoundBufferDesc,0,sizeof(DSBUFFERDESC));
	SoundBufferDesc.dwSize=sizeof(DSBUFFERDESC);
	SoundBufferDesc.dwFlags=
		DSBCAPS_GETCURRENTPOSITION2/* | DSBCAPS_CTRLPAN */| DSBCAPS_CTRLVOLUME
		 |DSBCAPS_GLOBALFOCUS ;
	SoundBufferDesc.dwBufferBytes= freq * sizeof(__int16) * channels*2;
	SoundBufferBytes = SoundBufferDesc.dwBufferBytes;
	SoundBufferBytesHalf = SoundBufferBytes / 2;

	WAVEFORMATEX wfx;
	wfx.wFormatTag=WAVE_FORMAT_PCM;
	wfx.nChannels=(WORD)(channels);
	wfx.nSamplesPerSec=freq;
	wfx.wBitsPerSample=(WORD)16;
	wfx.nBlockAlign=(WORD)(wfx.wBitsPerSample/8*wfx.nChannels);
	wfx.nAvgBytesPerSec=wfx.nSamplesPerSec*wfx.nBlockAlign;
	wfx.cbSize=0;

	Frequency = freq;
	Channels = channels;

	SoundBufferDesc.lpwfxFormat=&wfx;

	// セカンダリバッファを作成する
	HRESULT hr = Sound->CreateSoundBuffer(&SoundBufferDesc,&SoundBuffer,NULL);
	if(FAILED(hr))
	{
		SoundBuffer=NULL;
		throw Exception("セカンダリバッファの作成に失敗しました"
			"/周波数:"+AnsiString(freq)+
			"/チャネル数:"+AnsiString(channels)+
			"/ビット数:"+AnsiString(16)+
			"/HRESULT:"+IntToHex((int)hr,8));
	}

	ResetSoundBuffer();
}
#endif
//---------------------------------------------------------------------------
void __fastcall DestroySoundBuffer(void)
{
	if(SoundBuffer)
	{
		SoundBuffer->Release();
		SoundBuffer = NULL;
	}
}
//---------------------------------------------------------------------------
static void __fastcall ResetSoundBuffer()
{
	if(!SoundBuffer) return;

	TBufferWriteLock lock;

	SoundBufferWritePos = 0;
	DecodeEnded = false;
	SoundBufferDecodeOfs = 0;
	BufferRemainBlock = 0;
	Playing = false;
	SoundBufferPrevPlayPos = 0;
	SoundBufferLoopCount = 0;


	BYTE *p1,*p2;
	DWORD b1,b2;

	if(SUCCEEDED(SoundBuffer->Lock(0,SoundBufferBytes,(void**)&p1,
		&b1,(void**)&p2,&b2,0)))
	{
		ZeroMemory(p1,SoundBufferBytes);
		SoundBuffer->Unlock((void*)p1,b1,(void*)p2,b2);
	}

	SoundBuffer->SetCurrentPosition(0);

	FirstWrite = true;
}
//---------------------------------------------------------------------------
void __fastcall StartPlay(const WAVEFORMATEXTENSIBLE * wfx, TGetPCMDataEvent event)
{
	TBufferWriteLock lock;
	GetPCMData = event;

	CreateSoundBuffer(wfx);

	Playing = true;
	FillSecondaryBuffer();
	SoundBuffer->Play(0,0,DSBPLAY_LOOPING);
}
//---------------------------------------------------------------------------
void __fastcall StopPlay(void)
{
	if(!SoundBuffer) return;

	TBufferWriteLock lock;
	Playing = false;
	SoundBuffer->Stop();
	ResetSoundBuffer();
}
//---------------------------------------------------------------------------
static void __fastcall FillSecondaryBuffer(void)
{
	if(!Playing) return;
	if(!SoundBuffer) return;
	if(!GetPCMData) return;

	TBufferWriteLock lock;

	int writepos;

	if(FirstWrite)
	{
		FirstWrite = false;

		writepos = 0;
		SoundBufferWritePos = 1;
	}
	else
	{
		DWORD pp=0,wp=0;
		SoundBuffer->GetCurrentPosition(&pp,&wp);

		if(pp < SoundBufferPrevPlayPos)
		{
			SoundBufferLoopCount++;
		}
		SoundBufferPrevPlayPos = pp;

		wp/=SoundBufferBytesHalf;
		pp/=SoundBufferBytesHalf;

		DWORD d = wp-SoundBufferWritePos;
		if(d<0) d+=2;

		wp-=pp;
		if(wp<0) wp+=2;
			// この時点で wp =pp と wp の差

		if(d <= wp)
		{
			// pp と wp の間は現在演奏中のため、そこに書き込むことはできない
			return;
		}

		writepos = SoundBufferWritePos?SoundBufferBytesHalf:0;
		SoundBufferWritePos ^= 1;

		// pp と wp の区間は演奏中のため、そこに書き込んではならない
	}

	if(DecodeEnded)
	{
		BufferRemainBlock--;
		if(!BufferRemainBlock)
		{
			SoundBuffer->Stop();
			Playing = false;
			return;
		}
	}

	BYTE *p1,*p2;
	DWORD b1,b2;

	if(SUCCEEDED(SoundBuffer->Lock(writepos, SoundBufferBytesHalf,
		(void**)&p1, &b1, (void**)&p2, &b2, 0)))
	{
		DWORD written = GetPCMData((__int16*)p1, SoundBufferDecodeOfs,
			SoundBufferBytesHalf/SampleSize);
		SoundBufferDecodeOfs += written;
		written *= SampleSize;
		if(written < SoundBufferBytesHalf)
		{
			// 足りない分は 0 で満たす
			if(!DecodeEnded)
			{
				DecodeEnded = true;
				BufferRemainBlock = 2;
			}
			ZeroMemory(p1 + written, SoundBufferBytesHalf - written);
		}
		SoundBuffer->Unlock((void*)p1,b1,(void*)p2,b2);
	}
}
//---------------------------------------------------------------------------
DWORD __fastcall GetCurrentPlayingPos(void)
{
	// 現在の演奏位置を得る
    if(!Playing) return -1;

	TBufferWriteLock lock;

	DWORD pp=0,wp=0;
	SoundBuffer->GetCurrentPosition(&pp,&wp);

	if(pp < SoundBufferPrevPlayPos)
	{
		SoundBufferLoopCount++;
	}
	SoundBufferPrevPlayPos = pp;

	return (SoundBufferLoopCount * SoundBufferBytes +pp)  /(2*Channels);
}
//---------------------------------------------------------------------------
bool __fastcall GetPlaying(void)
{
	return Playing;
}
//---------------------------------------------------------------------------


#pragma package(smart_init)
