//---------------------------------------------------------------------------
// option information for kirikiri configurator
//---------------------------------------------------------------------------

#ifndef __OptionInfoH__
#define __OptionInfoH__

#include <windows.h>
#include "tp_stub.h"


//---------------------------------------------------------------------------
// GetOptionInfoString: returns option information string for kirikiri conf.
//---------------------------------------------------------------------------
static inline const wchar_t * GetOptionInfoString()
{
	if(GetACP() == 932) // 932 = Japan
	{
		return
L"デバッグ:ROTテーブルへの登録;ムービー再生時に"
L"ROT(Running Object Table)へ登録するかどうかの設定です。"
L"「する」を選択すると、DirectX SDK付属のGraphEditを用いて"
L"ムービー再生トラブルの解析を行うことができます。|"
L"movie_reg_rot|select,*no;いいえ,yes;はい\n";
	}
	else
	{
		return
		// for other languages; currently only English information is available.
L"Debug:ROT registration;Whether to register into ROT(Running Object Table) when "
L"playbacking movies. Choosing 'Yes' enables you to inspect the trouble related with movies, "
L"using GraphEdit(comes from DirectX SDK).|"
L"movie_reg_rot|select,*no;No,yes;Yes\n";
	}
}


//---------------------------------------------------------------------------
// GetShouldRegisterToROT: returns whether to register the process into ROT
//---------------------------------------------------------------------------
static inline bool GetShouldRegisterToROT()
{
	static bool cached = false;
	static bool state = false;
	if(!cached)
	{
		tTJSVariant val;
		if(TVPGetCommandLine(TJS_W("-movie_reg_rot"), &val))
		{
			if(ttstr(val) == TJS_W("yes")) state = true;
		}
		cached = true;
	}
	return state;
}

#endif
