/*
 * String <-> Octet —ñ‚Ì•ÏŠ·ŠÖ”
 *
 * Written by Kouhei Yanagita
 */

/*

Octet —ñ -> •¶Žš—ñ
 String decodeSJIS(Octet data)
 String decodeUTF8(Octet data)

•¶Žš—ñ -> Octet —ñ
 Octet  encodeUTF8(String str)

 */

//---------------------------------------------------------------------------
#include <windows.h>
#include "tp_stub.h"
#include <vector>
#include <algorithm>
//---------------------------------------------------------------------------



class tDecodeSJISFunction : public tTJSDispatch
{
    tjs_error TJS_INTF_METHOD FuncCall(
        tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
        tTJSVariant *result,
        tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);

} * DecodeSJISFunction;

tjs_error TJS_INTF_METHOD tDecodeSJISFunction::FuncCall(
    tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
    tTJSVariant *result,
    tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
{
    if (membername) {
        return TJS_E_MEMBERNOTFOUND;
    }

    if (numparams == 0) {
        return TJS_E_BADPARAMCOUNT;
    }

    if (param[0]->Type() != tvtOctet) {
        return TJS_E_INVALIDPARAM;
    }

    if (result) {
        tTJSVariantOctet *oct = param[0]->AsOctetNoAddRef();
        if (oct) {
            const tjs_uint8 *data = oct->GetData();
            std::vector<char> v;
            v.reserve(oct->GetLength() + 1);
            std::copy(data, data + oct->GetLength(), std::back_inserter(v));
            v.push_back(0);
            *result = &v[0];
        }
        else {
            *result = TJS_W("");
        }
    }


    return TJS_S_OK;
}




//----------------------------------------------------------------------------
class tDecodeUTF8Function : public tTJSDispatch
{
    tjs_error TJS_INTF_METHOD FuncCall(
        tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
        tTJSVariant *result,
        tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);
} * DecodeUTF8Function;

tjs_error TJS_INTF_METHOD tDecodeUTF8Function::FuncCall(
    tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
    tTJSVariant *result,
    tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
{
    if (membername) {
        return TJS_E_MEMBERNOTFOUND;
    }

    if (numparams == 0) {
        return TJS_E_BADPARAMCOUNT;
    }

    if (param[0]->Type() != tvtOctet) {
        return TJS_E_INVALIDPARAM;
    }

    if (result) {
        tTJSVariantOctet *oct = param[0]->AsOctetNoAddRef();
        if (oct) {
            const tjs_uint8 *data = oct->GetData();
            std::vector<tjs_char> v;
            int len = oct->GetLength();
            for (int i = 0; i < len; ++i) {
                if (data[i] <= 0x7f) {
                    v.push_back(data[i]);
                }
                else if (data[i] <= 0xdf) {
                    if (len <= i + 1) {
                        TVPThrowExceptionMessage(TJS_W("Illegal UTF-8 sequence"));
                    }
                    v.push_back(((data[i] & 0x1f) << 6) | (data[i+1] & 0x3f));
                    i += 1;
                }
                else if (data[i] <= 0xef) {
                    if (len <= i + 2) {
                        TVPThrowExceptionMessage(TJS_W("Illegal UTF-8 sequence"));
                    }
                    v.push_back(((data[i] & 0x0f) << 12) | ((data[i+1] & 0x3f) << 6) | (data[i+2] & 0x3f));
                    i += 2;
                }
            }
            v.push_back(0);
            *result = &v[0];
        }
        else {
            *result = TJS_W("");
        }
    }

    return TJS_S_OK;
}
//----------------------------------------------------------------------------
class tEncodeUTF8Function : public tTJSDispatch
{
    tjs_error TJS_INTF_METHOD FuncCall(
        tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
        tTJSVariant *result,
        tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);
} * EncodeUTF8Function;

tjs_error TJS_INTF_METHOD tEncodeUTF8Function::FuncCall(
    tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint,
    tTJSVariant *result,
    tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
{
    if (membername) {
        return TJS_E_MEMBERNOTFOUND;
    }

    if (numparams == 0) {
        return TJS_E_BADPARAMCOUNT;
    }

    if (param[0]->Type() != tvtString) {
        return TJS_E_INVALIDPARAM;
    }

    if (result) {
        ttstr str(*param[0]);
        std::vector<tjs_uint8> v;

        tjs_int len = str.GetLen();
        for (tjs_int i = 0; i < len; ++i) {
            tjs_char c = str[i];

            if (c < (1 << 7)) {
                v.push_back(c);
            }
            else if (c < (1 << 11)) {
                v.push_back(0xc0 | ((c >> 6) & 0x1f));
                v.push_back(0x80 | (c & 0x3f));
            }
            else {
                v.push_back(0xe0 | ((c >> 12) & 0x0f));
                v.push_back(0x80 | ((c >> 6) & 0x3f));
                v.push_back(0x80 | (c & 0x3f));
            }
        }

        *result = TJSAllocVariantOctet(&v[0], v.size());
    }

    return TJS_S_OK;
}



//---------------------------------------------------------------------------
#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason,
    void* lpReserved)
{
    return 1;
}
//---------------------------------------------------------------------------
static tjs_int GlobalRefCountAtInit = 0;
extern "C" HRESULT __declspec(dllexport) __stdcall V2Link(iTVPFunctionExporter *exporter)
{
    TVPInitImportStub(exporter);

    tTJSVariant val;

    iTJSDispatch2 * global = TVPGetScriptDispatch();


    DecodeSJISFunction = new tDecodeSJISFunction();
    val = tTJSVariant(DecodeSJISFunction);
    DecodeSJISFunction->Release();
    global->PropSet(
        TJS_MEMBERENSURE,
        TJS_W("decodeSJIS"),
        NULL,
        &val,
        global
    );

    DecodeUTF8Function = new tDecodeUTF8Function();
    val = tTJSVariant(DecodeUTF8Function);
    DecodeUTF8Function->Release();
    global->PropSet(
        TJS_MEMBERENSURE,
        TJS_W("decodeUTF8"),
        NULL,
        &val,
        global
    );

    EncodeUTF8Function = new tEncodeUTF8Function();
    val = tTJSVariant(EncodeUTF8Function);
    EncodeUTF8Function->Release();
    global->PropSet(
        TJS_MEMBERENSURE,
        TJS_W("encodeUTF8"),
        NULL,
        &val,
        global
    );

    global->Release();

    val.Clear();

    GlobalRefCountAtInit = TVPPluginGlobalRefCount;

    return S_OK;
}
//---------------------------------------------------------------------------
extern "C" HRESULT __declspec(dllexport) __stdcall V2Unlink()
{
    if(TVPPluginGlobalRefCount > GlobalRefCountAtInit) return E_FAIL;

    iTJSDispatch2 * global = TVPGetScriptDispatch();

    if(global)
    {
        global->DeleteMember(
            0,
            TJS_W("decodeSJIS"),
            NULL,
            global
        );
        global->DeleteMember(
            0,
            TJS_W("decodeUTF8"),
            NULL,
            global
        );
        global->DeleteMember(
            0,
            TJS_W("encodeUTF8"),
            NULL,
            global
        );
    }

    if(global) global->Release();

    TVPUninitImportStub();

    return S_OK;
}

