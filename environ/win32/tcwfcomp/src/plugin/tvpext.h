#ifndef TVPEXTH
#define TVPEXTH
#include <windows.h>
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

#pragma pack(push,4)
struct TTVPVersion
{
	char Version[20];
	char VersionInfo[128];
};
#pragma pack(pop)

//---------------------------------------------------------------------------
#ifndef TJS_ITJSDISPATCH_DEFINED
#define TJS_ITJSDISPATCH_DEFINED

//#pragma option push -b  -a4
#pragma pack(push,4)
enum TTJSVariantType
{
	tvtVoid,  // empty
	tvtObject,
	tvtString,
	tvtInteger,
	tvtReal
};
class ITJSDispatch;
struct TTJSVariant_S
{
	typedef __int64 TTVInteger;
	typedef long double TTVReal;

	union
	{
		ITJSDispatch *Object;
		wchar_t *String;  // SysAllocString 等で確保する
		TTVInteger Integer;
		TTVReal Real;
	};
	TTJSVariantType vt;
};
#pragma pack(pop)
//#pragma option pop


class ITJSDispatch : public IUnknown
{
public:
// IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(
			REFIID riid,
			void **ppvObject) = 0;
	virtual ULONG STDMETHODCALLTYPE AddRef(void) = 0;
	virtual ULONG STDMETHODCALLTYPE Release(void) = 0;

// ITJSDispatch
	virtual HRESULT STDMETHODCALLTYPE Invoke(wchar_t* name,DWORD flag,
		TTJSVariant_S &Result,TTJSVariant_S **ValArray,long NumVal,
		ITJSDispatch *ObjThis) =0;

	virtual HRESULT STDMETHODCALLTYPE Reserved1(WORD,long *,
		TTJSVariant_S *,TTJSVariant_S *)=0;
	virtual HRESULT STDMETHODCALLTYPE Reserved2(WORD,ITJSDispatch *)=0;
};
#else
typedef TTJSVariant TTJSVariant_S;
#endif
//---------------------------------------------------------------------------

class ITVP
{
public:
	// IUnkown 派生クラスではないので注意

// ITVP
	virtual HRESULT STDMETHODCALLTYPE
		GetVersion(TTVPVersion *version)=0;
		/*
			TTVPVersion 構造体にバージョンを表す文字列を設定します。
		*/


	virtual HRESULT STDMETHODCALLTYPE
		GetAppWindow(HWND *appwin)=0;
		/*
			アプリケーションウィンドウを appwin に返します。
			アプリケーションウィンドウは、吉里吉里のすべてのウィンドウの親
			ウィンドウで、画面上ではいつも非表示ですが、タスクバーに表示さ
			れているのはこのアプリケーションウィンドウです。
		*/

	virtual HRESULT STDMETHODCALLTYPE
		GetScriptDispatch(ITJSDispatch **out)=0;
		/*
			スクリプト言語 TJS の global オブジェクトを out に返します。
			global オブジェクトから、吉里吉里上の system をはじめとした
			オブジェクト・プロパティにアクセスできます。
		*/

	virtual HRESULT STDMETHODCALLTYPE
		GetStream(char *filename,IStream **out);
		/*
			filename で指定されたファイルに対するストリームを out に
			返します。
		*/
};


//---------------------------------------------------------------------------

class IWaveUnpacker
{
public:
// IUnknown 派生クラスではないので注意
	virtual ULONG STDMETHODCALLTYPE AddRef(void) = 0;
	virtual ULONG STDMETHODCALLTYPE Release(void) = 0;

// IWaveUnpacker
	virtual HRESULT STDMETHODCALLTYPE GetTypeName(char *buf,long buflen)=0;
		/*
			buf に、この Wave 形式を表す文字列を *buf に設定してください。
			buflen は、buf に確保された文字列で、null terminater も含むので
			注意。
		*/

	virtual HRESULT STDMETHODCALLTYPE GetWaveFormat(long *samplepersec,
		long *channels,long *bitspersample)=0;
		/*
			出力する Wave の形式を *samplepersec, *channels, *bitspersample に
			返してください。
		*/

	virtual HRESULT STDMETHODCALLTYPE Render(void *buffer,long bufsize,
		long *numwrite) =0;
		/*
			デコードしてください。
			bufsize には buffer のサイズがバイト単位で指定されます。
			numwrite には、バッファに書かれたデータの数をバイト単位で返します。
			ただし、WaveUnpacker は、numwrite<bufsize の場合は、残りを
			0 で埋めてください。
		*/
	
	virtual HRESULT STDMETHODCALLTYPE GetLength(long *length)=0;
		/*
			データ長を ms 単位で *length に返してください。
			対応できない場合は E_NOTIMPL を返してください。その場合は
			WaveSoundBuffer の totalTime プロパティは 0 を表すようになります。
		*/

	virtual HRESULT STDMETHODCALLTYPE GetCurrentPosition(long *pos)=0;
		/*
			現在のデコード位置を *pos に返してください。
			対応できない場合は E_NOTIMPL を返してください。その場合は
			WaveSoundBuffer の position プロパティは意味のない数値を
			示すようになります。
		*/

	virtual HRESULT STDMETHODCALLTYPE SetCurrentPosition(long pos)=0;
		/*
			現在のデコード位置を設定してください。pos は ms 単位での
			位置です。
			最低でも pos=0 として呼ばれたときに、先頭への巻き戻しが
			出来ようにしてください。

			そのほかの場合、対応できない場合は E_NOTIMPL を返してください。
			その場合はWaveSoundBuffer の position プロパティへの代入は無視されます。
		*/

	virtual HRESULT STDMETHODCALLTYPE Invoke(wchar_t* name,DWORD flag,
		TTJSVariant_S &Result,TTJSVariant_S **ValArray,long NumVal,
		ITJSDispatch *ObjThis) =0;
		/*
			WaveSoundBuffer オブジェクトのうち、処理されなかったプロパティ/
			メソッドがこの Invoke を通して通知されますので、必要に応じて
			処理してください。プロパティやメソッドを追加するのに使います。
			処理しない場合は E_NOTIMPL を返してください。
		*/
};

//---------------------------------------------------------------------------
#endif


