//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000-2009 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
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

#include "DSound.h"
#include "WaveLoopManager.h"

//---------------------------------------------------------------------------
static void __fastcall ResetSoundBuffer();
static void __fastcall FillSecondaryBuffer(void);
//---------------------------------------------------------------------------
static TCriticalSection *WriteLock = NULL;
class TBufferWriteLock // ���b�N�p�N���X
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
			Sleep(50);
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
static bool FirstWrite;
static bool Playing = false;
static tTVPWaveLoopManager * Manager = NULL;
static TPlayerThread *PlayerThread = NULL;
static std::vector<tTVPWaveLoopSegment> Segments[2];
static std::vector<tTVPWaveLabel> Labels[2];
//---------------------------------------------------------------------------
void __fastcall InitDirectSound(HWND wnd)
{
	if(!Sound)
	{
		// DLL �̃��[�h

		if(DirectSoundDLLHandle==NULL)
		{
			DirectSoundDLLHandle = LoadLibrary("dsound.dll");
			if(!DirectSoundDLLHandle) throw Exception("dsound.dll �����[�h�ł��܂���");
		}

		if(procDirectSoundCreate==NULL)
		{
			procDirectSoundCreate = (HRESULT (WINAPI*)(_GUID *,IDirectSound **,IUnknown*))
				GetProcAddress(DirectSoundDLLHandle,"DirectSoundCreate");
			if(!procDirectSoundCreate)
			{
				FreeLibrary(DirectSoundDLLHandle);
				DirectSoundDLLHandle=NULL;
				throw Exception("dsound.dll ���ُ�ł�");
			}
		}
		// �I�u�W�F�N�g�쐬
		HRESULT hr;
		hr=procDirectSoundCreate(NULL,&Sound,NULL);
		if(FAILED(hr))
		{
			throw Exception(
				AnsiString("DirectSound ������ł��܂���(����Wave���Đ���?)/HRESULT:")+
					IntToHex((int)hr,8));
				// �쐬�Ɏ��s�@���� WAVE output ���g���Ă����ꍇ�ȂǁA
				// �����Ȃ�
		}

		// �������x���̐ݒ�
		hr=Sound->SetCooperativeLevel(wnd, DSSCL_PRIORITY);
		if(FAILED(hr))
		{
			Sound->Release(); // �T�E���h�I�u�W�F�N�g�������[�X
			Sound=NULL;
			throw Exception(AnsiString("DirectSound �̋������x���̐ݒ�Ɏ��s���܂���(phase1)/HRESULT:")+
				IntToHex((int)hr,8));
		}

//--
		// �v���C�}���o�b�t�@�̍쐬

		SoundPrimaryBuffer= NULL; // �ʂɍ쐬�ł��Ȃ��Ⴛ��ł�����

		ZeroMemory(&SoundBufferDesc,sizeof(DSBUFFERDESC));
		SoundBufferDesc.dwSize=sizeof(DSBUFFERDESC);
		SoundBufferDesc.dwFlags=DSBCAPS_PRIMARYBUFFER;
		hr=Sound->CreateSoundBuffer(&SoundBufferDesc,
			&SoundPrimaryBuffer,NULL);

		if(FAILED(hr) || SoundPrimaryBuffer == NULL)
		{
			// DbgMsgAdd("�x��:DirectSound �v���C�}���o�b�t�@���쐬�ł��܂���");
			hr=Sound->SetCooperativeLevel(wnd, DSSCL_NORMAL);
			if(FAILED(hr))
			{
				Sound->Release(); // �T�E���h�I�u�W�F�N�g�������[�X
				Sound = NULL;
				throw Exception(AnsiString("DirectSound �̋������x���̐ݒ�Ɏ��s���܂���(phase2)/HRESULT:")+
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
	// �T�E���h�o�b�t�@�̔j��
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

		// DLL �̉��
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
	// 0.25 �b�Ԃ̃Z�J���_���o�b�t�@���쐬����

	/* wfx.Format �� nAvgBytesPerSec �� nBlockAlign �͕ςȒl�ɂȂ��Ă��Ȃ����ǂ���
	   �`�F�b�N���K�v */

	if(SoundBuffer)
	{
		SoundBuffer->Release();
		SoundBuffer = NULL;
	}

	// �v���C�}���o�b�t�@�̃t�H�[�}�b�g���Đݒ�
	if(SoundPrimaryBuffer)
	{
		SoundPrimaryBuffer->SetFormat((const WAVEFORMATEX *)wfx);
	}

	// �Z�J���_���o�b�t�@�̍쐬
	memset(&SoundBufferDesc,0,sizeof(DSBUFFERDESC));
	SoundBufferDesc.dwSize=sizeof(DSBUFFERDESC);
	SoundBufferDesc.dwFlags=
		DSBCAPS_GETCURRENTPOSITION2/* | DSBCAPS_CTRLPAN */| DSBCAPS_CTRLVOLUME
		 |DSBCAPS_GLOBALFOCUS ;
	SoundBufferDesc.dwBufferBytes = wfx->Format.nAvgBytesPerSec /4;
	SoundBufferDesc.dwBufferBytes /=  wfx->Format.nBlockAlign * 2;
	SoundBufferDesc.dwBufferBytes *=  wfx->Format.nBlockAlign * 2;
	SoundBufferBytes = SoundBufferDesc.dwBufferBytes;
	SoundBufferBytesHalf = SoundBufferBytes / 2;

	Frequency = wfx->Format.nSamplesPerSec;
	Channels = wfx->Format.nChannels;
	SampleSize = wfx->Format.nBlockAlign;

	SoundBufferDesc.lpwfxFormat = (WAVEFORMATEX*)wfx;


	// �Z�J���_���o�b�t�@���쐬����
	HRESULT hr = Sound->CreateSoundBuffer(&SoundBufferDesc,&SoundBuffer,NULL);
	if(FAILED(hr))
	{
		SoundBuffer=NULL;
		throw Exception("�Z�J���_���o�b�t�@�̍쐬�Ɏ��s���܂���"
			"/��g��:"+AnsiString(wfx->Format.nSamplesPerSec)+
			"/�`���l����:"+AnsiString(wfx->Format.nChannels)+
			"/�r�b�g��:"+AnsiString(16)+
			"/HRESULT:"+IntToHex((int)hr,8));
	}

	ResetSoundBuffer();
}
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
	BufferRemainBlock = 0;
	Playing = false;


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
void __fastcall StartPlay(const WAVEFORMATEXTENSIBLE * wfx, tTVPWaveLoopManager * manager)
{
	TBufferWriteLock lock;
	Manager = manager;

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
	if(!Manager) return;

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

		wp/=SoundBufferBytesHalf;
		pp/=SoundBufferBytesHalf;

		DWORD d = wp-SoundBufferWritePos;
		if(d<0) d+=2;

		wp-=pp;
		if(wp<0) wp+=2;
			// ���̎��_�� wp =pp �� wp �̍�

		if(d <= wp)
		{
			// pp �� wp �̊Ԃ͌��݉��t���̂��߁A�����ɏ������ނ��Ƃ͂ł��Ȃ�
			return;
		}

		writepos = SoundBufferWritePos?SoundBufferBytesHalf:0;
		SoundBufferWritePos ^= 1;

		// pp �� wp �̋�Ԃ͉��t���̂��߁A�����ɏ�������ł͂Ȃ�Ȃ�
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
		tjs_uint written;
		Manager->Decode(p1, SoundBufferBytesHalf/SampleSize,
			written, Segments[writepos ? 1: 0],
				Labels[writepos ? 1: 0]);

		written *= SampleSize;
		if(written < SoundBufferBytesHalf)
		{
			// ����Ȃ����� 0 �Ŗ�����
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
	// ���݂̉��t�ʒu�𓾂�
    if(!Playing) return -1;

	TBufferWriteLock lock;

	DWORD pp=0,wp=0;
	SoundBuffer->GetCurrentPosition(&pp,&wp);

	int idx;
	if(pp >= SoundBufferBytesHalf)
	{
		idx = 1;
		pp -= SoundBufferBytesHalf;
	}
	else
	{
		idx = 0;
	}

	pp /= SampleSize;

	tjs_uint pos = 0;
	for(unsigned int i = 0 ; i < Segments[idx].size(); i++)
	{
		tjs_uint limit = pos + (tjs_uint)Segments[idx][i].Length;
		if(pp >= pos && pp < limit)
			return (DWORD)(Segments[idx][i].Start + (pp - pos));
		pos = limit;
	}

	return 0; // will no be reached
}
//---------------------------------------------------------------------------
bool __fastcall GetPlaying(void)
{
	return Playing;
}
//---------------------------------------------------------------------------


#pragma package(smart_init)
