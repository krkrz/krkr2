//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------

#ifndef DSoundUnitH
#define DSoundUnitH

//---------------------------------------------------------------------------
#include "WaveLoopManager.h"
//---------------------------------------------------------------------------
// プロトタイプ
typedef DWORD __fastcall (__closure *TGetPCMDataEvent)
	(__int16 * Buffer, DWORD Offset, DWORD Samples);
void __fastcall InitDirectSound(HWND wnd);
void __fastcall FreeDirectSound(void);
void __fastcall CreateSoundBuffer(const WAVEFORMATEXTENSIBLE * wfx);
void __fastcall DestroySoundBuffer(void);
void __fastcall StartPlay(const WAVEFORMATEXTENSIBLE * wfx, tTVPWaveLoopManager *manager);
void __fastcall StopPlay(void);
bool __fastcall GetPlaying(void);
//---------------------------------------------------------------------------
DWORD __fastcall GetCurrentPlayingPos(void);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------


#endif
