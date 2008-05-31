#include "ncbind/ncbind.hpp"
#include "../json/Writer.hpp"
#include <vector>
#include <algorithm>


//-----------------------------------------------------------------------
// TJS式を文字列に変換するコード(saveStruct.dllより引用)
//----------------------------------------------------------------------
static void
quoteString(const tjs_char *str, IWriter *writer)
{
	if (str) {
		writer->write((tjs_char)'"');
		const tjs_char *p = str;
		int ch;
		while ((ch = *p++)) {
			if (ch == '"') {
				writer->write(L"\\\"");
			} else if (ch == '\\') {
				writer->write(L"\\\\");
			} else {
				writer->write((tjs_char)ch);
			}
		}
		writer->write((tjs_char)'"');
	} else {
		writer->write(L"null");
	}
}

static void getVariantString(tTJSVariant &var, IWriter *writer);

/**
 * 辞書の内容表示用の呼び出しロジック
 */
class DictMemberDispCaller : public tTJSDispatch /** EnumMembers 用 */
{
protected:
	IWriter *writer;
	bool first;
public:
	DictMemberDispCaller(IWriter *writer) : writer(writer) { first = true; };
	virtual tjs_error TJS_INTF_METHOD FuncCall( // function invocation
												tjs_uint32 flag,			// calling flag
												const tjs_char * membername,// member name ( NULL for a default member )
												tjs_uint32 *hint,			// hint for the member name (in/out)
												tTJSVariant *result,		// result
												tjs_int numparams,			// number of parameters
												tTJSVariant **param,		// parameters
												iTJSDispatch2 *objthis		// object as "this"
												) {
		if (numparams > 1) {
			if ((int)param[1] != TJS_HIDDENMEMBER) {
				if (first) {
					first = false;
				} else {
					writer->write((tjs_char)',');
					writer->newline();
				}
				const tjs_char *name = param[0]->GetString();
				quoteString(name, writer);
				writer->write(L"=>");
				getVariantString(*param[2], writer);
			}
		}
		if (result) {
			*result = true;
		}
		return TJS_S_OK;
	}
};

static void getDictString(iTJSDispatch2 *dict, IWriter *writer)
{
	writer->write(L"%[");
	//writer->addIndent();
	DictMemberDispCaller caller(writer);
	tTJSVariantClosure closure(&caller);
	dict->EnumMembers(TJS_IGNOREPROP, &closure, dict);
	//writer->delIndent();
	writer->write((tjs_char)']');
}

// Array クラスメンバ
static iTJSDispatch2 *ArrayCountProp   = NULL;   // Array.count

static void getArrayString(iTJSDispatch2 *array, IWriter *writer)
{
	writer->write((tjs_char)'[');
	//writer->addIndent();
	tjs_int count = 0;
	{
		tTJSVariant result;
		if (TJS_SUCCEEDED(ArrayCountProp->PropGet(0, NULL, NULL, &result, array))) {
			count = result;
		}
	}
	for (tjs_int i=0; i<count; i++) {
		if (i != 0) {
			writer->write((tjs_char)',');
			//writer->newline();
		}
		tTJSVariant result;
		if (array->PropGetByNum(TJS_IGNOREPROP, i, &result, array) == TJS_S_OK) {
			getVariantString(result, writer);
		}
	}
	//writer->delIndent();
	writer->write((tjs_char)']');
}

static void
getVariantString(tTJSVariant &var, IWriter *writer)
{
	switch(var.Type()) {

	case tvtVoid:
		writer->write(L"void");
		break;
		
	case tvtObject:
		{
			iTJSDispatch2 *obj = var.AsObjectNoAddRef();
			if (obj == NULL) {
				writer->write(L"null");
			} else if (obj->IsInstanceOf(TJS_IGNOREPROP,NULL,NULL,L"Array",obj) == TJS_S_TRUE) {
				getArrayString(obj, writer);
			} else {
				getDictString(obj, writer);
			}
		}
		break;
		
	case tvtString:
		quoteString(var.GetString(), writer);
		break;

	case tvtInteger:
		writer->write(L"int ");
		writer->write((tTVInteger)var);
		break;

	case tvtReal:
		writer->write(L"real ");
		writer->write((tTVReal)var);
		break;

	default:
		writer->write(L"void");
		break;
	};
}



//----------------------------------------------------------------------
// クリップボード拡張
//----------------------------------------------------------------------
// TJS式のクリップボードフォーマット
static const wchar_t *TJS_EXPRESSION_FORMAT = L"application/x-kirikiri-tjsexpression";
// TJS式のクリップボードID
tjs_uint CF_TJS_EXPRESSION;


// 拡張クラス
class ClipboardEx
{
public:
  static const tjs_uint cbfText = 1;
  static const tjs_uint cbfBitmap = 2;
  static const tjs_uint cbfTJSExpression = 3;

  // 特定のフォーマットがクリップボードに存在するか調べる
  static bool hasFormat(tjs_uint format) 
  {
    switch(format)
      {
      case cbfText:
        return IsClipboardFormatAvailable(CF_TEXT) == TRUE ||
          IsClipboardFormatAvailable(CF_UNICODETEXT) == TRUE; // ANSI text or UNICODE text
      case cbfBitmap:
        return IsClipboardFormatAvailable(CF_DIB) == TRUE;
      case cbfTJSExpression:
        return IsClipboardFormatAvailable(CF_TJS_EXPRESSION) == TRUE;
      default:
        return false;
      }
    return true;
  }

  // クリップボードにTJS式を設定する
  static void setTJSExpression(tTJSVariant data) {
    HGLOBAL unicodehandle = NULL;
    
    if (! OpenClipboard(NULL))
      TVPThrowExceptionMessage(L"copying to clipboard failed.");

    try {
      IStringWriter writer(0);
      getVariantString(data, &writer);
      ttstr &unicode = writer.buf;

      // store UNICODE string
      unicodehandle = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE,
                                  (unicode.GetLen() + 1) * sizeof(tjs_char));
      if(!unicodehandle)
        TVPThrowExceptionMessage(L"copying to clipboard failed.");
      
      tjs_char *unimem = (tjs_char*)GlobalLock(unicodehandle);
      if(unimem) TJS_strcpy(unimem, unicode.c_str());
      GlobalUnlock(unicodehandle);

      EmptyClipboard();
      SetClipboardData(CF_TJS_EXPRESSION, unicodehandle);
      unicodehandle = NULL;
    } 
    catch (...) {
      if(unicodehandle) 
        GlobalFree(unicodehandle);
      CloseClipboard();
      throw;
    }
    CloseClipboard();
  }

  // クリップボードからTJS式を取得する
  static tTJSVariant getTJSExpression(void) {
    tTJSVariant result;

    if (! OpenClipboard(NULL))
      return result;
    try {
      if (IsClipboardFormatAvailable(CF_TJS_EXPRESSION)) {
        HGLOBAL hglb = (HGLOBAL)GetClipboardData(CF_TJS_EXPRESSION);
        if (hglb != NULL) {
          const tjs_char *p = (const tjs_char *)GlobalLock(hglb);
          if(p)
            {
              try
                {
                  TVPExecuteExpression(p, &result);
                }
              catch(...)
                {
                  GlobalUnlock(hglb);
                  throw;
                }
              GlobalUnlock(hglb);
            }
        }
      }
    }
    catch(...) {
      CloseClipboard();
      throw;
    }
    CloseClipboard();
    return result;
  }

  // クリップボードにレイヤの内容を設定する
  static void setAsBitmap(tTJSVariant layer) {
    if (! OpenClipboard(NULL))
      TVPThrowExceptionMessage(L"copying to clipboard failed.");

    HGLOBAL dibhandle = NULL;

    try {
      ncbPropAccessor obj(layer);
      tjs_int width, height;
      width = obj.GetValue(L"imageWidth",  ncbTypedefs::Tag<tjs_int>());
      height = obj.GetValue(L"imageHeight",  ncbTypedefs::Tag<tjs_int>());
      const tjs_uint8 *imageBuffer = (tjs_uint8*)obj.GetValue(L"mainImageBuffer", ncbTypedefs::Tag<tjs_int>());
      tjs_int imagePitch = obj.GetValue(L"mainImageBufferPitch", ncbTypedefs::Tag<tjs_int>());
      tjs_int pixelWidth = (width * 3 + 4 - 1) / 4 * 4;
      tjs_int pixelSize = pixelWidth * height;

      // store UNICODE string
      dibhandle = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE | GMEM_ZEROINIT,
                              sizeof(BITMAPINFOHEADER) + pixelSize);
      if(! dibhandle)
        TVPThrowExceptionMessage(L"copying to clipboard failed.");
      
      BITMAPINFOHEADER *bmpinfo = (BITMAPINFOHEADER*)GlobalLock(dibhandle);
      if (bmpinfo) {
        bmpinfo->biSize = sizeof(BITMAPINFOHEADER);
        bmpinfo->biWidth = width;
        bmpinfo->biHeight = height;
        bmpinfo->biPlanes = 1;
        bmpinfo->biBitCount = 24;
        for (tjs_int y = 0; y < height; y++) {
          tjs_uint8 *dst = (tjs_uint8*)(bmpinfo) + sizeof(BITMAPINFOHEADER) + pixelWidth * (height - y - 1);
          const tjs_uint8 *src = imageBuffer + imagePitch * y;
          for (tjs_int x = 0; x < width; x++, src += 4, dst += 3) 
            CopyMemory(dst, src, 3);
        }
      }
      GlobalUnlock(dibhandle);

      EmptyClipboard();
      SetClipboardData(CF_DIB, dibhandle);
      dibhandle = NULL;
    } 
    catch (...) {
      if(dibhandle)
        GlobalFree(dibhandle);
      CloseClipboard();
      throw;
    }
    CloseClipboard();
  }
    
  // クリップボードからレイヤの内容を取得する
  static bool getAsBitmap(tTJSVariant layer) {
    if (! OpenClipboard(NULL))
      return false;
    HDC dstDC = NULL;
    HBITMAP dstBitmap = NULL;
    HGLOBAL hglb = NULL;
    bool result = false;
    try {
      if (IsClipboardFormatAvailable(CF_DIB)) {
        HGLOBAL hglb = (HGLOBAL)GetClipboardData(CF_DIB);
        if (hglb != NULL) {
          BITMAPINFOHEADER *srcbmpinfo = (BITMAPINFOHEADER*)GlobalLock(hglb);
          tjs_int width = srcbmpinfo->biWidth;
          tjs_int height = srcbmpinfo->biHeight;
          tjs_int pixelWidth = (width * 3 + 4 - 1) / 4 * 4;
          const tjs_uint8 *srcPixels = (const tjs_uint8*)(srcbmpinfo) + sizeof(BITMAPINFOHEADER);
          if (srcbmpinfo->biBitCount <= 8)
            srcPixels += (1 << srcbmpinfo->biBitCount) * sizeof(RGBQUAD);

          ncbPropAccessor obj(layer);
          obj.SetValue(L"imageWidth", width);
          obj.SetValue(L"imageHeight", height);
          obj.SetValue(L"width", width);
          obj.SetValue(L"height", height);
          unsigned char *imageBuffer = (unsigned char*)obj.GetValue(L"mainImageBufferForWrite", ncbTypedefs::Tag<tjs_int>());
          tjs_int imagePitch = obj.GetValue(L"mainImageBufferPitch", ncbTypedefs::Tag<tjs_int>());

          BITMAPINFO dstbmpinfo;
          FillMemory(&dstbmpinfo, sizeof(dstbmpinfo), 0);
          dstbmpinfo.bmiHeader.biSize = sizeof(BITMAPINFO);
          dstbmpinfo.bmiHeader.biWidth = srcbmpinfo->biWidth;
          dstbmpinfo.bmiHeader.biHeight = srcbmpinfo->biHeight;
          dstbmpinfo.bmiHeader.biPlanes = 1;
          dstbmpinfo.bmiHeader.biBitCount = 24;
          if (srcbmpinfo) {
            dstDC = CreateCompatibleDC(NULL);
            if (dstDC) {
              void *pixels;
              dstBitmap = CreateDIBSection(dstDC, &dstbmpinfo, DIB_RGB_COLORS, &pixels, NULL, 0);
              if (dstBitmap) {
                if (SetDIBits(dstDC, dstBitmap, 
                              0, height,
                              srcPixels,
                              (const BITMAPINFO*)srcbmpinfo,
                              DIB_RGB_COLORS)) {
                  for (tjs_int y = 0; y < height; y++) {
                    const tjs_uint8 *src = (tjs_uint8*)(pixels) + pixelWidth * (height - y - 1);
                    tjs_uint8 *dst = imageBuffer + imagePitch * y;
                    for (tjs_int x = 0; x < width; x++, src += 3, dst += 4) {
                      CopyMemory(dst, src, 3);
                      dst[3] = 255;
                    }
                  }
                  result = true;
                }
              }
            }
          }
        }
      }
    }
    catch(...) {
      if (hglb)
        GlobalUnlock(hglb);
      if (dstBitmap)
        DeleteObject(dstBitmap);
      if (dstDC)
        ReleaseDC(NULL, dstDC);
      CloseClipboard();
      throw;
    }
        if (hglb)
          GlobalUnlock(hglb);
        if (dstBitmap)
        DeleteObject(dstBitmap);
      if (dstDC)
        ReleaseDC(NULL, dstDC);
    CloseClipboard();
    return result;
  }
};

// アタッチ
NCB_ATTACH_CLASS(ClipboardEx, Clipboard)
{
  NCB_METHOD(hasFormat);
  NCB_PROPERTY(asTJSExpression, getTJSExpression, setTJSExpression);
  NCB_METHOD(setAsBitmap);
  NCB_METHOD(getAsBitmap);
}

//----------------------------------------------------------------------
// Window追加関数
//----------------------------------------------------------------------
class WindowClipboardEx
{
private:
  iTJSDispatch2 *objthis; //< オブジェクト情報の参照
  bool clipboardWatchEnabled;
  HWND curHWND, nextHWND;

public:
  // コンストラクタ
  WindowClipboardEx(iTJSDispatch2 *objthis)
    : objthis(objthis)
    , clipboardWatchEnabled(false)
    , curHWND(NULL)
    , nextHWND(NULL) {
  }

  // デストラクタ
  virtual ~WindowClipboardEx(void) {
    if (clipboardWatchEnabled)
      disjoinClipboardViewerChain();
  }

  // クリップボード監視をオンオフする
  void setClipboardWatchEnabled(bool state) {
    if (clipboardWatchEnabled == state)
      return;
    clipboardWatchEnabled = state;
    if (clipboardWatchEnabled) 
      joinClipboardViewerChain();
    else
      disjoinClipboardViewerChain();
  }

  // クリップボード監視の状態を取得
  bool getClipboardWatchEnabled(void) const {
    return clipboardWatchEnabled;
  }

  void registerReceiver(bool enable) {
    // レシーバ更新
    tTJSVariant mode    = enable ? (tTVInteger)(tjs_int)wrmRegister : (tTVInteger)(tjs_int)wrmUnregister;
    tTJSVariant proc     = (tTVInteger)(tjs_int)MyReceiverHook;
    tTJSVariant userdata = (tTVInteger)(tjs_int)objthis;
    tTJSVariant *p[3] = {&mode, &proc, &userdata};
    objthis->FuncCall(0, L"registerMessageReceiver", NULL, NULL, 3, p, objthis);
  }

  // クリップボードビューアーチェーンに参加
  void joinClipboardViewerChain(void) {
    // メッセージチェインに参加
    tTJSVariant hwndValue;
    objthis->PropGet(0, TJS_W("HWND"), NULL, &hwndValue, objthis);
    curHWND = reinterpret_cast<HWND>(tjs_int(hwndValue));
    nextHWND = SetClipboardViewer(curHWND);
    // レシーバーをオンにする
    registerReceiver(true);
  }

  // クリップボードビューアーチェーンから外れる
  void disjoinClipboardViewerChain(void) {
    ChangeClipboardChain(curHWND, nextHWND);
    curHWND = nextHWND = NULL;
    // レシーバーをオフにする
    registerReceiver(false);
  }

  // コールバックを呼び出す
  void exexDrawClipboardCallback(void) {
    objthis->FuncCall(0, L"onDrawClipboard", NULL, NULL, 0, NULL, objthis);
  }

  // メッセージ処理
  static bool __stdcall MyReceiverHook(void *userdata, tTVPWindowMessage *Message) {
    iTJSDispatch2 *obj = (iTJSDispatch2*)userdata; // Window のオブジェクト
    // 吉里吉里の内部処理の関係でイベント処理中は登録破棄後でも呼ばれることがあるので
    // Window の本体オブジェクトからネイティブオブジェクトを取り直す
    WindowClipboardEx *self = ncbInstanceAdaptor<WindowClipboardEx>::GetNativeInstance(obj);
    return self->MyReceiver(Message);
  }

  bool MyReceiver(tTVPWindowMessage *Message) {
    switch (Message->Msg) {
      // ウィンドウのDETACHに合わせて、一旦チェインから外れる
    case TVP_WM_DETACH:
      if (clipboardWatchEnabled)
	disjoinClipboardViewerChain();
      break;
      // ウィンドウのATTACHに合わせて、チェインに再び参加する
    case TVP_WM_ATTACH:
      if (clipboardWatchEnabled)
	joinClipboardViewerChain();
      break;
      // クリップボード更新メッセージの処理
    case WM_DRAWCLIPBOARD:
      if (nextHWND) SendMessage(nextHWND , Message->Msg , Message->WParam, Message->LParam);
      exexDrawClipboardCallback();
      return true;
      // クリップボードチェイン変更メッセージの処理
    case WM_CHANGECBCHAIN:
      if ((HWND)(Message->WParam) == nextHWND) nextHWND = (HWND)(Message->LParam);
      else if (nextHWND) SendMessage(nextHWND , Message->Msg , Message->WParam, Message->LParam);
      return true;
    }
    return false;
  }
};


// インスタンスゲッタ
NCB_GET_INSTANCE_HOOK(WindowClipboardEx)
{
  NCB_INSTANCE_GETTER(objthis) { // objthis を iTJSDispatch2* 型の引数とする
    ClassT* obj = GetNativeInstance(objthis);	// ネイティブインスタンスポインタ取得
    if (!obj) {
      obj = new ClassT(objthis);				// ない場合は生成する
      SetNativeInstance(objthis, obj);		// objthis に obj をネイティブインスタンスとして登録する
    }
    return obj;
  }
};


// フック付きアタッチ
NCB_ATTACH_CLASS_WITH_HOOK(WindowClipboardEx, Window) {
  Property(L"clipboardWatchEnabled", &WindowClipboardEx::getClipboardWatchEnabled, &WindowClipboardEx::setClipboardWatchEnabled);
}


//----------------------------------------------------------------------
// DLL登録時に呼び出すファンクション
//----------------------------------------------------------------------
static void RegistCallback(void)
{
  // 定数を登録
  TVPExecuteExpression(L"global.cbfBitmap = 2");
  TVPExecuteExpression(L"global.cbfTJSExpression = 3");
  // TJS式をクリップボードのフォーマットとして登録
  CF_TJS_EXPRESSION = RegisterClipboardFormat(TJS_EXPRESSION_FORMAT);

  // Array.count を取得
  {
    tTJSVariant varScripts;
    TVPExecuteExpression(TJS_W("Array"), &varScripts);
    iTJSDispatch2 *dispatch = varScripts.AsObjectNoAddRef();
    tTJSVariant val;
    if (TJS_FAILED(dispatch->PropGet(TJS_IGNOREPROP,
                                     TJS_W("count"),
                                     NULL,
                                     &val,
                                     dispatch))) {
      TVPThrowExceptionMessage(L"can't get Array.count");
    }
    ArrayCountProp = val.AsObject();
  }
}


// 登録
NCB_PRE_REGIST_CALLBACK(RegistCallback);


//----------------------------------------------------------------------
// DLL解放時に呼び出すファンクション
//----------------------------------------------------------------------
static void UnregistCallback(void)
{
  // 定数を削除
  TVPExecuteScript(L"delete global[\"cbfBitmap\"]");
  TVPExecuteScript(L"delete global[\"cbfTJSExpression\"]");

  // Array.countを解放
  if (ArrayCountProp) {
    ArrayCountProp->Release();
    ArrayCountProp = NULL;
  }
}


// 登録
NCB_POST_UNREGIST_CALLBACK(UnregistCallback);

