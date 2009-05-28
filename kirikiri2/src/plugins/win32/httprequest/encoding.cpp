#include <windows.h>
#include <comdef.h>
#include <mlang.h>

static bool coInitialized;
static IMultiLanguage *ml = NULL;

void
initCodePage()
{
	coInitialized = SUCCEEDED(CoInitialize(0));
	if (coInitialized) {
		::CoCreateInstance(CLSID_CMultiLanguage, NULL, CLSCTX_ALL, IID_IMultiLanguage, (LPVOID*)&ml);
	}
}

void
doneCodePage()
{
	if (coInitialized) {
		if (ml) {
			ml->Release();
			ml = NULL;
		}
		CoUninitialize();
	}
}

/**
 * IANA のエンコード名から codepage を取得
 * @param encoding エンコード
 * @return コードページ
 */
int
getCodePage(const wchar_t *encoding)
{
	if (encoding && *encoding && ml) {
		MIMECSETINFO csinfo;
		if (SUCCEEDED(ml->GetCharsetInfo(_bstr_t(encoding), &csinfo))) {
			return csinfo.uiInternetEncoding;
		}
	}
	return CP_UTF8;
}
