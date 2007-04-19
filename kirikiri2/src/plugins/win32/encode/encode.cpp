/*
 * String <-> Octet —ñ‚Ì•ÏŠ·ŠÖ”
 *
 * Written by Kouhei Yanagita
 */

/*

Octet —ñ -> •¶Žš—ñ
 String Encode.decode(Octet data, String encoding)

 encoding = [ 'Shift_JIS' | 'UTF-8' ]

•¶Žš—ñ -> Octet —ñ
 Octet  Encode.encode(String str, String encoding)

 encoding = [ 'UTF-8' ]

 */

//---------------------------------------------------------------------------
#include <windows.h>
#include "tp_stub.h"
#include <vector>
#include <algorithm>
//---------------------------------------------------------------------------


class NI_Encode : public tTJSNativeInstance
{
public:
    NI_Encode() { }

    tjs_error TJS_INTF_METHOD Construct(tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *tjs_obj)
    {
        return S_OK;
    }

    void TJS_INTF_METHOD Invalidate()
    {

    }

    static ttstr Decode(tTJSVariant *param, const ttstr &encoding)
    {
        if (encoding == TJS_W("Shift_JIS")) {
            return DecodeShiftJIS(param);
        }
        else if (encoding == TJS_W("UTF-8")) {
            return DecodeUTF8(param);
        }
        else {
            TVPThrowExceptionMessage(TJS_W("Unknown encoding"));
        }
    }

    static tTJSVariantOctet *Encode(const ttstr &str, const ttstr &encoding)
    {
        if (encoding == TJS_W("UTF-8")) {
            return EncodeUTF8(str);
        }
        else {
            TVPThrowExceptionMessage(TJS_W("Unknown encoding"));
        }        
    }

private:
    static ttstr DecodeUTF8(tTJSVariant *param)
    {
        tTJSVariantOctet *oct = param->AsOctetNoAddRef();
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
            return ttstr(&v[0]);
        }
        else {
            return TJS_W("");
        }
    }

    static ttstr DecodeShiftJIS(tTJSVariant *param)
    {
        tTJSVariantOctet *oct = param->AsOctetNoAddRef();
        if (oct) {
            const tjs_uint8 *data = oct->GetData();
            std::vector<char> v;
            v.reserve(oct->GetLength() + 1);
            std::copy(data, data + oct->GetLength(), std::back_inserter(v));
            v.push_back(0);
            return ttstr(&v[0]);
        }
        else {
            return TJS_W("");
        }
    }


    static tTJSVariantOctet *EncodeUTF8(const ttstr &str)
    {
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

        return TJSAllocVariantOctet(&v[0], v.size());
    }
};


static iTJSNativeInstance * TJS_INTF_METHOD Create_NI_Encode()
{
    TVPThrowExceptionMessage(
        TJSGetMessageMapMessage(TJS_W("TVPCannotCreateInstance")).c_str()
    );
    return NULL;
}

#define TJS_NATIVE_CLASSID_NAME ClassID_Encode
static tjs_int32 TJS_NATIVE_CLASSID_NAME = -1;


static iTJSDispatch2 *Create_NC_Encode()
{
    tTJSNativeClassForPlugin *classobj =
        TJSCreateNativeClassForPlugin(TJS_W("Encode"), Create_NI_Encode);

    TJS_BEGIN_NATIVE_MEMBERS(Encode)
        TJS_DECL_EMPTY_FINALIZE_METHOD

        TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL_NO_INSTANCE(Encode)
        {
            return TJS_S_OK;
        }
        TJS_END_NATIVE_CONSTRUCTOR_DECL(Encode)


        TJS_BEGIN_NATIVE_METHOD_DECL(decode)
        {
            if (numparams < 2) {
                return TJS_E_BADPARAMCOUNT;
            }

            if (param[0]->Type() != tvtOctet || param[1]->Type() != tvtString) {
                return TJS_E_INVALIDPARAM;
            }

            if (result) {
                *result = NI_Encode::Decode(param[0], ttstr(*param[1]));
            }

            return TJS_S_OK;
        }
        TJS_END_NATIVE_CONSTRUCTOR_DECL(decode)


        TJS_BEGIN_NATIVE_METHOD_DECL(encode)
        {
            if (numparams < 2) {
                return TJS_E_BADPARAMCOUNT;
            }

            if (param[0]->Type() != tvtString || param[1]->Type() != tvtString) {
                return TJS_E_INVALIDPARAM;
            }

            if (result) {
                *result = NI_Encode::Encode(ttstr(*param[0]), ttstr(*param[1]));
            }

            return TJS_S_OK;
        }
        TJS_END_NATIVE_METHOD_DECL(encode)

    TJS_END_NATIVE_MEMBERS

    return classobj;
}

#undef TJS_NATIVE_CLASSID_NAME


#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void *lpReserved)
{
    return 1;
}

static tjs_int GlobalRefCountAtInit = 0;
extern "C" HRESULT _stdcall __declspec(dllexport) V2Link(iTVPFunctionExporter *exporter)
{
    TVPInitImportStub(exporter);

    tTJSVariant val;
    iTJSDispatch2 *global = TVPGetScriptDispatch();

    iTJSDispatch2 *tjsclass = Create_NC_Encode();
    val = tTJSVariant(tjsclass);
    tjsclass->Release();

    global->PropSet(TJS_MEMBERENSURE, TJS_W("Encode"), NULL, &val, global);
    global->Release();
    val.Clear();
    GlobalRefCountAtInit = TVPPluginGlobalRefCount;

    return S_OK;
}

extern "C" HRESULT _stdcall __declspec(dllexport) V2Unlink()
{
    if (TVPPluginGlobalRefCount > GlobalRefCountAtInit) {
        return E_FAIL;
    }

    iTJSDispatch2 *global = TVPGetScriptDispatch();

    if (global) {
        global->DeleteMember(0, TJS_W("Encode"), NULL, global);
    }

    if (global) {
        global->Release();
    }

    TVPUninitImportStub();

    return S_OK;
}
