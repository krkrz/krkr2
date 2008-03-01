#include "ncbind/ncbind.hpp"
#include <vector>
using namespace std;
#include <process.h>

#define WM_SAVE_TLG_PROGRESS (WM_APP+4)
#define WM_SAVE_TLG_DONE     (WM_APP+5)

#include <tlg5/slide.h>
#define BLOCK_HEIGHT 4
//---------------------------------------------------------------------------

typedef bool ProgressFunc(int percent, void *userdata);

// 圧縮処理用

class Compress {

protected:
	vector<unsigned char> data; //< 格納データ
	ULONG cur;                  //< 格納位置
	ULONG size;                 //< 格納サイズ
	ULONG dataSize;             //< データ領域確保サイズ

public:
	/**
	 * コンストラクタ
	 */
	Compress() {
		cur = 0;
		size = 0;
		dataSize = 1024 * 100;
		data.resize(dataSize);
	}

	/**
	 * サイズ変更
	 * 指定位置がはいるだけのサイズを確保する。
	 * 指定した最大サイズを保持する。
	 * @param サイズ
	 */
	void resize(size_t s) {
		if (s > size) {
			size = s;
			if (size > dataSize) {
				dataSize = size * 2;
				data.resize(dataSize);
			}
		}
	}

	/**
	 * 8bit数値の書き出し
	 * @param num 数値
	 */
	void writeInt8(int num) {
		resize(cur + 1);
		data[cur++] = num & 0xff;
	}
	
	/**
	 * 32bit数値の書き出し
	 * @param num 数値
	 */
	void writeInt32(int num) {
		resize(cur + 4);
		data[cur++] = num & 0xff;
		data[cur++] = (num >> 8) & 0xff;
		data[cur++] = (num >> 16) & 0xff;
		data[cur++] = (num >> 24) & 0xff;
	}

	/**
	 * 32bit数値の書き出し
	 * @param num 数値
	 */
	void writeInt32(int num, int cur) {
		resize(cur + 4);
		data[cur++] = num & 0xff;
		data[cur++] = (num >> 8) & 0xff;
		data[cur++] = (num >> 16) & 0xff;
		data[cur++] = (num >> 24) & 0xff;
	}
	
	/**
	 * バッファの書き出し
	 * @param buf バッファ
	 * @param size 出力バイト数
	 */
	void writeBuffer(const char *buf, int size) {
		resize(cur + size);
		memcpy((void*)&data[cur], buf, size);
		cur += size;
	}

	/**
	 * 画像情報の書き出し
	 * @param width 画像横幅
	 * @param height 画像縦幅
	 * @param buffer 画像バッファ
	 * @param pitch 画像データのピッチ
	 */
	bool compress(int width, int height, const unsigned char *buffer, int pitch, ProgressFunc *progress=NULL, void *progressData = NULL) {

		bool canceled = false;
		
		// ARGB 固定
		int colors = 4;
	
		// header
		{
			writeBuffer("TLG5.0\x00raw\x1a\x00", 11);
			writeInt8(colors);
			writeInt32(width);
			writeInt32(height);
			writeInt32(BLOCK_HEIGHT);
		}

		int blockcount = (int)((height - 1) / BLOCK_HEIGHT) + 1;

		// buffers/compressors
		SlideCompressor * compressor = NULL;
		unsigned char *cmpinbuf[4];
		unsigned char *cmpoutbuf[4];
		for (int i = 0; i < colors; i++) {
			cmpinbuf[i] = cmpoutbuf[i] = NULL;
		}
		long written[4];
		int *blocksizes;
		
		// allocate buffers/compressors
		try	{
			compressor = new SlideCompressor();
			for(int i = 0; i < colors; i++)	{
				cmpinbuf[i] = new unsigned char [width * BLOCK_HEIGHT];
				cmpoutbuf[i] = new unsigned char [width * BLOCK_HEIGHT * 9 / 4];
				written[i] = 0;
			}
			blocksizes = new int[blockcount];

			// ブロックサイズの位置を記録
			ULONG blocksizepos = cur;

			cur += blockcount * 4;

			//
			int block = 0;
			for(int blk_y = 0; blk_y < height; blk_y += BLOCK_HEIGHT, block++) {
				if (progress) {
					if (progress(blk_y * 100 / height, progressData)) {
						canceled = true;
						break;
					}
				}
				int ylim = blk_y + BLOCK_HEIGHT;
				if(ylim > height) ylim = height;
				
				int inp = 0;
				
				for(int y = blk_y; y < ylim; y++) {
					// retrieve scan lines
					const unsigned char * upper;
					if (y != 0) {
						upper = (const unsigned char *)buffer;
						buffer += pitch;
					} else { 
						upper = NULL;
					}
					const unsigned char * current;
					current = (const unsigned char *)buffer;
					
					// prepare buffer
					int prevcl[4];
					int val[4];
					
					for(int c = 0; c < colors; c++) prevcl[c] = 0;
					
					for(int x = 0; x < width; x++) {
						for(int c = 0; c < colors; c++) {
							int cl;
							if(upper)
								cl = 0[current++] - 0[upper++];
							else
								cl = 0[current++];
							val[c] = cl - prevcl[c];
							prevcl[c] = cl;
						}
						// composite colors
						switch(colors){
						case 1:
							cmpinbuf[0][inp] = val[0];
							break;
						case 3:
							cmpinbuf[0][inp] = val[0] - val[1];
							cmpinbuf[1][inp] = val[1];
							cmpinbuf[2][inp] = val[2] - val[1];
							break;
						case 4:
							cmpinbuf[0][inp] = val[0] - val[1];
							cmpinbuf[1][inp] = val[1];
							cmpinbuf[2][inp] = val[2] - val[1];
							cmpinbuf[3][inp] = val[3];
							break;
						}
						inp++;
					}
				}
				
				// compress buffer and write to the file
				
				// LZSS
				int blocksize = 0;
				for(int c = 0; c < colors; c++) {
					long wrote = 0;
					compressor->Store();
					compressor->Encode(cmpinbuf[c], inp,
									   cmpoutbuf[c], wrote);
					if(wrote < inp)	{
						writeInt8(0x00);
						writeInt32(wrote);
						writeBuffer((const char *)cmpoutbuf[c], wrote);
						blocksize += wrote + 4 + 1;
					} else {
						compressor->Restore();
						writeInt8(0x01);
						writeInt32(inp);
						writeBuffer((const char *)cmpinbuf[c], inp);
						blocksize += inp + 4 + 1;
					}
					written[c] += wrote;
				}
			
				blocksizes[block] = blocksize;
			}

			if (!canceled) {
				// ブロックサイズ格納
				for (int i = 0; i < blockcount; i++) {
					writeInt32(blocksizes[i], blocksizepos);
					blocksizepos += 4;
				}
			}

		} catch(...) {
			for(int i = 0; i < colors; i++) {
				if(cmpinbuf[i]) delete [] cmpinbuf[i];
				if(cmpoutbuf[i]) delete [] cmpoutbuf[i];
			}
			if(compressor) delete compressor;
			if(blocksizes) delete [] blocksizes;
			throw;
		}
		for(int i = 0; i < colors; i++) {
			if(cmpinbuf[i]) delete [] cmpinbuf[i];
			if(cmpoutbuf[i]) delete [] cmpoutbuf[i];
		}
		if(compressor) delete compressor;
		if(blocksizes) delete [] blocksizes;

		if (progress) {
			progress(100, progressData);
		}
		
		return canceled;
	}

	/**
	 * タグ展開用
	 */
	class TagsCaller : public tTJSDispatch /** EnumMembers 用 */ {
	protected:
		ttstr *store;
	public:
		TagsCaller(ttstr *store) : store(store) {};
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
					ttstr name  = *param[0];
					ttstr value = *param[2];
					*store += ttstr(name.GetNarrowStrLen()) + ":" + name + "=" +	ttstr(value.GetNarrowStrLen()) + ":" + value + ",";
				}
			}
			if (result) {
				*result = true;
			}
			return TJS_S_OK;
		}
	};

	
	/**
	 * 画像情報の書き出し
	 * @param width 画像横幅
	 * @param height 画像縦幅
	 * @param buffer 画像バッファ
	 * @param pitch 画像データのピッチ
	 * @param tagsDict タグ情報
	 * @return キャンセルされたら true
	 */
	bool compress(int width, int height, const unsigned char *buffer, int pitch, iTJSDispatch2 *tagsDict, ProgressFunc *progress=NULL, void *progressData=NULL) {

		bool canceled = false;
		
		// 取得
		ttstr tags;
		if (tagsDict) {
			TagsCaller caller(&tags);
			tTJSVariantClosure closure(&caller);
			tagsDict->EnumMembers(TJS_IGNOREPROP, &closure, tagsDict);
		}
		
		ULONG tagslen = tags.GetNarrowStrLen(); 
		if (tagslen > 0) {
			// write TLG0.0 Structured Data Stream header
			writeBuffer("TLG0.0\x00sds\x1a\x00", 11);
			ULONG rawlenpos = cur;
			cur += 4;
			// write raw TLG stream
			if (!(canceled = compress(width, height, buffer, pitch, progress, progressData))) {
				// write raw data size
				writeInt32(cur - rawlenpos - 4, rawlenpos);
				// write "tags" chunk name
				writeBuffer("tags", 4);
				// write chunk size
				writeInt32(tagslen);
				// write chunk data
				resize(cur + tagslen);
				tags.ToNarrowStr((tjs_nchar*)&data[cur], tagslen);
				cur += tagslen;
			}
		} else {
			// write raw TLG stream
			canceled = compress(width, height, buffer, pitch, progress, progressData);
		}
		return canceled;
	}

	/**
	 * データをファイルに書き出す
	 * @param out 出力先ストリーム
	 */
	void store(IStream *out) {
		ULONG s;
		out->Write(&data[0], size, &s);
	}
};


//---------------------------------------------------------------------------

/**
 * レイヤの内容を TLG5で保存する
 * @param layer レイヤ
 * @param filename ファイル名
 * @return キャンセルされたら true
 */
static bool
saveLayerImageTlg5(iTJSDispatch2 *layer, const tjs_char *filename, iTJSDispatch2 *info, ProgressFunc *progress=NULL, void *progressData=NULL)
{
	// レイヤ画像情報
	tjs_int width, height, pitch;
	unsigned char* buffer;
	{
		tTJSVariant var;
		layer->PropGet(0, L"imageWidth", NULL, &var, layer);
		width = (tjs_int)var;
		layer->PropGet(0, L"imageHeight", NULL, &var, layer);
		height = (tjs_int)var;
		layer->PropGet(0, L"mainImageBufferPitch", NULL, &var, layer);
		pitch = (tjs_int)var;
		layer->PropGet(0, L"mainImageBuffer", NULL, &var, layer);
		buffer = (unsigned char*)(tjs_int)var;
	}

	// 圧縮処理実行
	Compress compress;
	bool canceled = compress.compress(width, height, buffer, pitch, info, progress, progressData);

	// 圧縮がキャンセルされていなければファイル保存
	if (!canceled) {
		IStream *out = TVPCreateIStream(filename, TJS_BS_WRITE);
		if (!out) {
			ttstr msg = filename;
			msg += L":can't open";
			TVPThrowExceptionMessage(msg.c_str());
		}
		try {
			// 格納
			compress.store(out);
		} catch (...) {
			out->Release();
			throw;
		}
		out->Release();
	}
	
	return canceled;
}

//---------------------------------------------------------------------------
// ウインドウ拡張
//---------------------------------------------------------------------------

class WindowSaveImage;

/**
 * セーブ処理スレッド用情報
 */
class SaveInfo {

	friend class WindowSaveImage;
	
protected:

	// 初期化変数
	WindowSaveImage *notify; //< 情報通知先
	tTJSVariant layer; //< レイヤ
	tTJSVariant filename; //< ファイル名
	tTJSVariant info;  //< 保存用タグ情報
	bool canceled;        //< キャンセル指示
	tTJSVariant handler;  //< ハンドラ値
	tTJSVariant progressPercent; //< 進行度合い
	
protected:
	/**
	 * 現在の状態の通知
	 * @param percent パーセント
	 */
	bool progress(int percent);

	/**
	 * 呼び出し用
	 */
	static bool progressFunc(int percent, void *userData) {
		SaveInfo *self = (SaveInfo*)userData;
		return self->progress(percent);
	}
	
	// 経過イベント送信
	void eventProgress(iTJSDispatch2 *objthis) {
		tTJSVariant *vars[] = {&handler, &progressPercent, &layer, &filename};
		objthis->FuncCall(0, L"onSaveLayerImageProgress", NULL, NULL, 4, vars, objthis);
	}

	// 終了イベント送信
	void eventDone(iTJSDispatch2 *objthis) {
		tTJSVariant result = canceled ? 1 : 0;
		tTJSVariant *vars[] = {&handler, &result, &layer, &filename};
		objthis->FuncCall(0, L"onSaveLayerImageDone", NULL, NULL, 4, vars, objthis);
	}
	
public:
	// コンストラクタ
	SaveInfo(int handler, WindowSaveImage *notify, tTJSVariant layer, const tjs_char *filename, tTJSVariant info)
		: handler(handler), notify(notify), layer(layer), filename(filename), canceled(false) {}
	
	// デストラクタ
	~SaveInfo() {}

	// ハンドラ取得
	int getHandler() {
		return (int)handler;
	}
	
 	// 処理開始
	void start();

	// 処理キャンセル
	void cancel() {
		canceled = true;
	}

	// 強制終了
	void stop() {
		canceled = true;
		notify = NULL;
	}
};

/**
 * ウインドウにレイヤセーブ機能を拡張
 */
class WindowSaveImage {

protected:
	iTJSDispatch2 *objthis; //< オブジェクト情報の参照

	vector<SaveInfo*> saveinfos; //< セーブ中情報保持用

	// 実行スレッド
	static void checkThread(void *data) {
		((SaveInfo*)data)->start();
	}

	// 経過通知
	void eventProgress(SaveInfo *sender) {
		int handler = sender->getHandler();
		if (saveinfos[handler] == sender) {
			sender->eventProgress(objthis);
		}
	}

	// 終了通知
	void eventDone(SaveInfo *sender) {
		int handler = sender->getHandler();
		if (saveinfos[handler] == sender) {
			saveinfos[handler] = NULL;
			sender->eventDone(objthis);
		}
		delete sender;
	}

	/*
	 * ウインドウイベント処理レシーバ
	 */
	static bool __stdcall receiver(void *userdata, tTVPWindowMessage *Message) {
		if (Message->Msg == WM_SAVE_TLG_PROGRESS) {
			iTJSDispatch2 *obj = (iTJSDispatch2*)userdata;
			WindowSaveImage *self = ncbInstanceAdaptor<WindowSaveImage>::GetNativeInstance(obj);
			if (self) {
				self->eventProgress((SaveInfo*)Message->WParam);
			}
			return true;
		} else if (Message->Msg == WM_SAVE_TLG_DONE) {
			iTJSDispatch2 *obj = (iTJSDispatch2*)userdata;
			WindowSaveImage *self = ncbInstanceAdaptor<WindowSaveImage>::GetNativeInstance(obj);
			if (self) {
				self->eventDone((SaveInfo*)Message->WParam);
			}
			return true;
		}
		return false;
	}

	// ユーザメッセージレシーバの登録/解除
	void setReceiver(tTVPWindowMessageReceiver receiver, bool enable) {
		tTJSVariant mode     = enable ? (tTVInteger)(tjs_int)wrmRegister : (tTVInteger)(tjs_int)wrmUnregister;
		tTJSVariant proc     = (tTVInteger)(tjs_int)receiver;
		tTJSVariant userdata = (tTVInteger)(tjs_int)objthis;
		tTJSVariant *p[] = {&mode, &proc, &userdata};
		if (objthis->FuncCall(0, L"registerMessageReceiver", NULL, NULL, 4, p, objthis) != TJS_S_OK) {
			TVPThrowExceptionMessage(L"can't regist user message receiver");
		}
	}

public:

	/**
	 * コンストラクタ
	 */
	WindowSaveImage(iTJSDispatch2 *objthis) : objthis(objthis) {
		setReceiver(receiver, true);
	}

	/**
	 * デストラクタ
	 */
	~WindowSaveImage() {
		setReceiver(receiver, false);
		for (int i=0;i<(int)saveinfos.size();i++) {
			SaveInfo *saveinfo = saveinfos[i];
			if (saveinfo) {
				saveinfo->stop();
				saveinfos[i] = NULL;
			}
		}
	}

	/**
	 * メッセージ送信
	 * @param msg メッセージ
	 * @param wparam WPARAM
	 * @param lparam LPARAM
	 */
	void postMessage(UINT msg, WPARAM wparam=NULL, LPARAM lparam=NULL) {
		// ウィンドウハンドルを取得して通知
		tTJSVariant val;
		objthis->PropGet(0, TJS_W("HWND"), NULL, &val, objthis);
		HWND hwnd = reinterpret_cast<HWND>((tjs_int)(val));
		::PostMessage(hwnd, msg, wparam, lparam);
	}

	/**
	 * レイヤセーブ開始
	 * @param layer レイヤ
	 * @param filename ファイル名
	 * @param info タグ情報
	 */
	int startSaveLayerImage(tTJSVariant layer, const tjs_char *filename, tTJSVariant info) {
		int handler = saveinfos.size();
		for (int i=0;i<(int)saveinfos.size();i++) {
			if (saveinfos[i] == NULL) {
				handler = i;
				break;
			}
		}
		if (handler >= (int)saveinfos.size()) {
			saveinfos.resize(handler + 1);
		}
		SaveInfo *saveInfo = new SaveInfo(handler, this, layer, filename, info);
		saveinfos[handler] = saveInfo;
		_beginthread(checkThread, 0, saveInfo);
		return handler;
	}
	
	/**
	 * レイヤセーブのキャンセル
	 */
	void cancelSaveLayerImage(int handler) {
		if (handler < (int)saveinfos.size() && saveinfos[handler] != NULL) {
			saveinfos[handler]->cancel();
		}
	}

	/**
	 * レイヤセーブの中止
	 */
	void stopSaveLayerImage(int handler) {
		if (handler < (int)saveinfos.size() && saveinfos[handler] != NULL) {
			saveinfos[handler]->stop();
			saveinfos[handler] = NULL;
		}
	}
};


/**
 * 現在の状態の通知
 * @param percent パーセント
 */
bool
SaveInfo::progress(int percent)
{
	if ((int)progressPercent != percent) {
		progressPercent = percent;
		if (notify) {
			notify->postMessage(WM_SAVE_TLG_PROGRESS, (WPARAM)this);
			Sleep(0);
		}
	}
	return canceled;
}

/*
 * 保存処理開始
 */
void
SaveInfo::start()
{
	// 画像をセーブ
	saveLayerImageTlg5(layer.AsObjectNoAddRef(), // layer
					   filename.GetString(),
					   info.Type() == tvtObject ? info.AsObjectNoAddRef() : NULL, // info
					   progressFunc, this);
	// 完了通知
	if (notify) {
		notify->postMessage(WM_SAVE_TLG_DONE, (WPARAM)this);
		Sleep(0);
	} else {
		delete this;
	}
}

//---------------------------------------------------------------------------

// インスタンスゲッタ
NCB_GET_INSTANCE_HOOK(WindowSaveImage)
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

NCB_ATTACH_CLASS_WITH_HOOK(WindowSaveImage, Window) {
	NCB_METHOD(startSaveLayerImage);
	NCB_METHOD(cancelSaveLayerImage);
	NCB_METHOD(stopSaveLayerImage);
};

//---------------------------------------------------------------------------
// レイヤ拡張
//---------------------------------------------------------------------------

/**
 * TLG画像保存
 */
static tjs_error TJS_INTF_METHOD saveLayerImageTlg5Func(tTJSVariant *result,
														tjs_int numparams,
														tTJSVariant **param,
														iTJSDispatch2 *objthis) {
	if (numparams < 1) return TJS_E_BADPARAMCOUNT;
	saveLayerImageTlg5(objthis, // layer
					   param[0]->GetString(),  // filename
					   numparams > 1 ? param[1]->AsObjectNoAddRef() : NULL // info
					   );
	return TJS_S_OK;
};

NCB_ATTACH_FUNCTION(saveLayerImageTlg5, Layer, saveLayerImageTlg5Func);
