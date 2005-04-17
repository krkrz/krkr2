//---------------------------------------------------------------------------

#ifndef DSoundUnitH
#define DSoundUnitH


//---------------------------------------------------------------------------
// プロトタイプ
typedef DWORD __fastcall (__closure *TGetPCMDataEvent)
	(__int16 * Buffer, DWORD Offset, DWORD Samples);
void __fastcall InitDirectSound(HWND wnd);
void __fastcall FreeDirectSound(void);
void __fastcall CreateSoundBuffer(int freq, int channels);
void __fastcall DestroySoundBuffer(void);
void __fastcall StartPlay(const WAVEFORMATEXTENSIBLE * wfx, TGetPCMDataEvent event);
void __fastcall StopPlay(void);
bool __fastcall GetPlaying(void);
//---------------------------------------------------------------------------
DWORD __fastcall GetCurrentPlayingPos(void);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------


#endif
