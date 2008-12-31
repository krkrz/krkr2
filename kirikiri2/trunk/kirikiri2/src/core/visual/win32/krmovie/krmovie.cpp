//---------------------------------------------------------------------------
// krmovie.cpp ( part of KRMOVIE.DLL )
// (c)2001-2009, W.Dee <dee@kikyou.info> and contributors
//---------------------------------------------------------------------------

/*
	We must separate this module because sucking MS library has a lack of
	compiler portability.

	This requires DirectX7 or later or Windows Media Player 6.4 or later for
	playbacking MPEG streams.

	Modified by T.Imoto <http://www.kaede-software.com>
*/

//---------------------------------------------------------------------------


#include "dsoverlay.h"
#include "..\krmovie.h"

#include "asyncio.h"
#include "asyncrdr.h"

#include "tp_stub.h"

#include "OptionInfo.h"

//---------------------------------------------------------------------------
// DllMain
//---------------------------------------------------------------------------
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
//---------------------------------------------------------------------------
// GetVideoOverlayObject
//---------------------------------------------------------------------------
const void __stdcall GetVideoOverlayObject(
	HWND callbackwin, IStream *stream, const wchar_t * streamname,
	const wchar_t *type, unsigned __int64 size, iTVPVideoOverlay **out)
{
	*out = new tTVPDSVideoOverlay;

	if( *out )
		static_cast<tTVPDSVideoOverlay*>(*out)->BuildGraph( callbackwin, stream, streamname, type, size );
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// GetAPIVersion
//---------------------------------------------------------------------------
void __stdcall GetAPIVersion(DWORD *ver)
{
	*ver = TVP_KRMOVIE_VER;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// V2Link : Initialize TVP plugin interface
//---------------------------------------------------------------------------
HRESULT __stdcall V2Link(iTVPFunctionExporter *exporter)
{
// �������m�ۈʒu�Ńu���[�N��\��ɂ͈ȉ��̃��\�b�h�Ŋm�۔ԍ����w�肷��B
// �u���[�N������������́A�Ăяo������(�R�[���X�^�b�N)�����āA�ǂ��Ŋm�ۂ��ꂽ�����������[�N���Ă��邩�T��B
// _CrtDumpMemoryLeaks �Ńf�o�b�O�o�͂Ƀ��[�N�����������̊m�۔ԍ����o��̂ŁA���������OK
// �m�ۏ����s�m��ȏꍇ�͐h�����A�X�N���v�g���Œ肷��΂قړ������Ŋm�ۂ����͂��B
//	_CrtSetBreakAlloc(53);	// �w�肳�ꂽ�񐔖ڂ̃������m�ێ��Ƀu���[�N��\��

	TVPInitImportStub(exporter);

	return S_OK;
}
//---------------------------------------------------------------------------
// V2Unlink : Uninitialize TVP plugin interface
//---------------------------------------------------------------------------
HRESULT __stdcall V2Unlink()
{
	TVPUninitImportStub();

#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif

	return S_OK;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// GetOptionDesc : Give option information to kirikiri configurator
//---------------------------------------------------------------------------
extern "C" const wchar_t * _stdcall GetOptionDesc()
{
	return GetOptionInfoString();
}
//---------------------------------------------------------------------------

