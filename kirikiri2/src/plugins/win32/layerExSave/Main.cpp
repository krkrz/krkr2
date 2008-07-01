#include "ncbind/ncbind.hpp"
#include <vector>
using namespace std;
#include <process.h>

#define WM_SAVE_TLG_PROGRESS (WM_APP+4)
#define WM_SAVE_TLG_DONE     (WM_APP+5)

// Layer クラスメンバ
static iTJSDispatch2 *layerClass = NULL;         // Layer のクラスオブジェクト
static iTJSDispatch2 *layerAssignImages = NULL;  // Layer.setTime メソッド


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
		: handler(handler), notify(notify), layer(layer), filename(filename), info(info), canceled(false) {}
	
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

		// 保存用にレイヤを複製する
		tTJSVariant newLayer;
		{
			// 新しいレイヤを生成
			tTJSVariant window(objthis, objthis);
			tTJSVariant primaryLayer;
			objthis->PropGet(0, L"primaryLayer", NULL, &primaryLayer, objthis);
			tTJSVariant *vars[] = {&window, &primaryLayer};
			iTJSDispatch2 *obj;
			if (TJS_SUCCEEDED(layerClass->CreateNew(0, NULL, NULL, &obj, 2, vars, objthis))) {

				// 名前づけ
				tTJSVariant name = "saveLayer:";
				name +=filename;
				obj->PropSet(0, L"name", NULL, &name, obj);

				// 元レイヤの画像を複製
				tTJSVariant *param[] = {&layer};
				if (TJS_SUCCEEDED(layerAssignImages->FuncCall(0, NULL, NULL, NULL, 1, param, obj))) {
					newLayer = tTJSVariant(obj, obj);
					obj->Release();
				} else {
					obj->Release();
					TVPThrowExceptionMessage(L"保存処理用レイヤへの画像の複製に失敗しました");
				}
			} else {
				TVPThrowExceptionMessage(L"保存処理用レイヤの生成に失敗しました");
			}
		}
		SaveInfo *saveInfo = new SaveInfo(handler, this, newLayer, filename, info);
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
 * TLG5 形式での画像の保存。注意点:データの保存が終わるまで処理が帰りません。
 * @param filename ファイル名
 * @param tags タグ情報
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
}

NCB_ATTACH_FUNCTION(saveLayerImageTlg5, Layer, saveLayerImageTlg5Func);


//----------------------------------------------
// レイヤイメージ操作ユーティリティ

// バッファ参照用の型
typedef unsigned char const *BufRefT;
typedef unsigned char       *WrtRefT;

/**
 * レイヤのサイズとバッファを取得する
 */
static bool
GetLayerSize(iTJSDispatch2 *lay, long &w, long &h, long &pitch)
{
	// レイヤインスタンス以外ではエラー
	if (!lay || TJS_FAILED(lay->IsInstanceOf(0, 0, 0, TJS_W("Layer"), lay))) return false;

	// レイヤイメージは在るか？
	tTJSVariant val;
	if (TJS_FAILED(lay->PropGet(0, TJS_W("hasImage"), 0, &val, lay)) || (val.AsInteger() == 0)) return false;

	// レイヤサイズを取得
	val.Clear();
	if (TJS_FAILED(lay->PropGet(0, TJS_W("imageWidth"), 0, &val, lay))) return false;
	w = (long)val.AsInteger();

	val.Clear();
	if (TJS_FAILED(lay->PropGet(0, TJS_W("imageHeight"), 0, &val, lay))) return false;
	h = (long)val.AsInteger();

	// ピッチ取得
	val.Clear();
	if (TJS_FAILED(lay->PropGet(0, TJS_W("mainImageBufferPitch"), 0, &val, lay))) return false;
	pitch = (long)val.AsInteger();

	// 正常な値かどうか
	return (w > 0 && h > 0 && pitch != 0);
}

// 読み込み用
static bool
GetLayerBufferAndSize(iTJSDispatch2 *lay, long &w, long &h, BufRefT &ptr, long &pitch)
{
	if (!GetLayerSize(lay, w, h, pitch)) return false;

	// バッファ取得
	tTJSVariant val;
	if (TJS_FAILED(lay->PropGet(0, TJS_W("mainImageBuffer"), 0, &val, lay))) return false;
	ptr = reinterpret_cast<BufRefT>(val.AsInteger());
	return  (ptr != 0);
}

// 書き込み用
static bool
GetLayerBufferAndSize(iTJSDispatch2 *lay, long &w, long &h, WrtRefT &ptr, long &pitch)
{
	if (!GetLayerSize(lay, w, h, pitch)) return false;

	// バッファ取得
	tTJSVariant val;
	if (TJS_FAILED(lay->PropGet(0, TJS_W("mainImageBufferForWrite"), 0, &val, lay))) return false;
	ptr = reinterpret_cast<WrtRefT>(val.AsInteger());
	return  (ptr != 0);
}

/**
 * 矩形領域の辞書を生成
 */
static void
MakeResult(tTJSVariant *result, long x, long y, long w, long h)
{
	ncbDictionaryAccessor dict;
	dict.SetValue(TJS_W("x"), x);
	dict.SetValue(TJS_W("y"), y);
	dict.SetValue(TJS_W("w"), w);
	dict.SetValue(TJS_W("h"), h);
	*result = dict;
}

/**
 * 不透明チェック関数
 */
static bool
CheckTransp(BufRefT p, long next, long count)
{
	for (; count > 0; count--, p+=next) if (p[3] != 0) return true;
	return false;
}

/**
 * レイヤイメージをクロップ（上下左右の余白透明部分を切り取る）したときのサイズを取得する
 *
 * Layer.getCropRect = function();
 * @return %[ x, y, w, h] 形式の辞書，またはvoid（全部透明のとき）
 */
static tjs_error TJS_INTF_METHOD
GetCropRect(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *lay)
{
	// レイヤバッファの取得
	BufRefT p, r = 0;
	long w, h, nl, nc = 4;
	if (!GetLayerBufferAndSize(lay, w, h, r, nl))
		TVPThrowExceptionMessage(TJS_W("Invalid layer image."));

	// 結果領域
	long x1=0, y1=0, x2=w-1, y2=h-1;
	result->Clear();

	for (p=r;             x1 <  w; x1++,p+=nc) if (CheckTransp(p, nl,  h)) break; // 左から透明領域を調べる
	/*                                      */ if (x1 >= w) return TJS_S_OK;      // 全部透明なら void を返す
	for (p=r+x2*nc;       x2 >= 0; x2--,p-=nc) if (CheckTransp(p, nl,  h)) break; // 右から透明領域を調べる
	/*                                      */ long rw = x2 - x1 + 1;             // 左右に挟まれた残りの幅
	for (p=r+x1*nc;       y1 <  h; y1++,p+=nl) if (CheckTransp(p, nc, rw)) break; // 上から透明領域を調べる
	for (p=r+x1*nc+y2*nl; y2 >= 0; y2--,p-=nl) if (CheckTransp(p, nc, rw)) break; // 下から透明領域を調べる

	// 結果を辞書に返す
	MakeResult(result, x1, y1, rw, y2 - y1 + 1);

	return TJS_S_OK;
}

NCB_ATTACH_FUNCTION(getCropRect, Layer, GetCropRect);

/**
 * 色比較関数
 */
#define IS_SAME_COLOR(A1,R1,G1,B1, A2,R2,G2,B2) \
	(((A1)==(A2)) && ((A1)==0 || ((R1)==(R2) && (G1)==(G2) && (B1)==(B2))))
//	!((p1[3] != p2[3]) || (p1[3] != 0 && (p1[0] != p2[0] || p1[1] != p2[1] || p1[2] != p2[2])))
//  ((A1 == A2) && (A1 == 0 || (R1==R2 && G1==G2 && B1==B2)))


static bool
CheckDiff(BufRefT p1, long p1n, BufRefT p2, long p2n, long count)
{
	for (; count > 0; count--, p1+=p1n, p2+=p2n)
		if (!IS_SAME_COLOR( p1[3],p1[2],p1[1],p1[0],  p2[3],p2[2],p2[1],p2[0] )) return true;
	return false;
}

/**
 * レイヤの差分領域を取得する
 * 
 * Layer.getDiffRegion = function(base);
 * @param base 差分元となるベース用の画像（インスタンス自身と同じ画像サイズであること）
 * @return %[ x, y, w, h ] 形式の辞書，またはvoid（完全に同じ画像のとき）
 */

static tjs_error TJS_INTF_METHOD
GetDiffRect(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *lay)
{
	// 引数の数チェック
	if (numparams < 1) return TJS_E_BADPARAMCOUNT;

	iTJSDispatch2 *base = param[0]->AsObjectNoAddRef();

	// レイヤバッファの取得
	BufRefT fp, tp, fr = 0, tr = 0;
	long w, h, tnl, fw, fh, fnl, nc = 4;
	if (!GetLayerBufferAndSize(lay,   w,  h, tr, tnl) || 
		!GetLayerBufferAndSize(base, fw, fh, fr, fnl))
		TVPThrowExceptionMessage(TJS_W("Invalid layer image."));

	// レイヤのサイズは同じか
	if (w != fw || h != fh)
		TVPThrowExceptionMessage(TJS_W("Different layer size."));

	// 結果領域
	long x1=0, y1=0, x2=w-1, y2=h-1;
	result->Clear();

	for (fp=fr,             tp=tr;              x1 <  w; x1++,fp+=nc, tp+=nc ) if (CheckDiff(fp, fnl, tp, tnl,  h)) break; // 左から透明領域を調べる
	/*                                                                      */ if (x1 >= w) return TJS_S_OK;               // 全部透明なら void を返す
	for (fp=fr+x2*nc,       tp=tr+x2*nc;        x2 >= 0; x2--,fp-=nc, tp-=nc ) if (CheckDiff(fp, fnl, tp, tnl,  h)) break; // 右から透明領域を調べる
	/*                                                                      */ long rw = x2 - x1 + 1;                      // 左右に挟まれた残りの幅
	for (fp=fr+x1*nc,       tp=tr+x1*nc;        y1 <  h; y1++,fp+=fnl,tp+=tnl) if (CheckDiff(fp, nc,  tp, nc,  rw)) break; // 上から透明領域を調べる
	for (fp=fr+x1*nc+y2*fnl,tp=tr+x1*nc+y2*tnl; y2 >= 0; y2--,fp-=fnl,tp-=tnl) if (CheckDiff(fp, nc,  tp, nc,  rw)) break; // 下から透明領域を調べる

	// 結果を辞書に返す
	MakeResult(result, x1, y1, rw, y2 - y1 + 1);

	return TJS_S_OK;
}

NCB_ATTACH_FUNCTION(getDiffRect, Layer, GetDiffRect);

/**
 * レイヤのピクセル比較を行う
 * 
 * Layer.getDiffPixel = function(base, samecol, diffcol);
 * @param base 差分元となるベース用の画像（インスタンス自身と同じ画像サイズであること）
 * @param samecol 同じ場合に塗りつぶす色(0xAARRGGBB)（void・省略なら塗りつぶさない）
 * @param diffcol 違う場合に塗りつぶす色(0xAARRGGBB)（void・省略なら塗りつぶさない）
 */

static tjs_error TJS_INTF_METHOD
GetDiffPixel(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *lay)
{
	DWORD scol = 0, dcol = 0;
	bool sfill = false, dfill = false;

	// 引数の数チェック
	if (numparams < 1) return TJS_E_BADPARAMCOUNT;

	if (numparams >= 2 && param[1]->Type() != tvtVoid) {
		scol = (DWORD)(param[1]->AsInteger());
		sfill = true;
	}

	if (numparams >= 3 && param[2]->Type() != tvtVoid) {
		dcol = (DWORD)(param[2]->AsInteger());
		dfill = true;
	}

	iTJSDispatch2 *base = param[0]->AsObjectNoAddRef();

	// レイヤバッファの取得
	BufRefT fp, fr = 0;
	WrtRefT tp, tr = 0;
	long w, h, tnl, fw, fh, fnl, nc = 4;
	if (!GetLayerBufferAndSize(lay,   w,  h, tr, tnl) || 
		!GetLayerBufferAndSize(base, fw, fh, fr, fnl))
		TVPThrowExceptionMessage(TJS_W("Invalid layer image."));

	// レイヤのサイズは同じか
	if (w != fw || h != fh)
		TVPThrowExceptionMessage(TJS_W("Different layer size."));

	// 塗りつぶし
	for (long y = 0; (fp=fr, tp=tr, y < h); y++, fr+=fnl, tr+=tnl) {
		for (long x = 0; x < w; x++, fp+=nc, tp+=nc) {
			bool same = IS_SAME_COLOR(fp[3],fp[2],fp[1],fp[0], tp[3],tp[2],tp[1],tp[0]);
			if (      same && sfill) *(DWORD*)tp = scol;
			else if (!same && dfill) *(DWORD*)tp = dcol;
		}
	}

	return TJS_S_OK;
}

NCB_ATTACH_FUNCTION(getDiffPixel, Layer, GetDiffPixel);


// 色を加算
static inline void AddColor(DWORD &r, DWORD &g, DWORD &b, BufRefT p) {
	r += p[2], g += p[1], b += p[0];
}

/**
 * レイヤの淵の色を透明部分まで引き伸ばす（縮小時に偽色が出るのを防ぐ）
 * 
 * Layer.oozeColor = function(level);
 * @param level 処理を行う回数。大きいほど引き伸ばし領域が増える
 */
static tjs_error TJS_INTF_METHOD
OozeColor(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *lay)
{
	// 引数の数チェック
	if (numparams < 1) return TJS_E_BADPARAMCOUNT;
	int level = (int)(param[0]->AsInteger());
	if (level <= 0) 
		TVPThrowExceptionMessage(TJS_W("Invalid level count."));

	// レイヤバッファの取得
	WrtRefT p, r = 0;
	long x, y, w, h, nl, nc = 4, ow, oh;
	if (!GetLayerBufferAndSize(lay, w, h, r, nl))
		TVPThrowExceptionMessage(TJS_W("Invalid layer image."));

	ow = w+2, oh = h+2; // oozed map のサイズ
	char *o, *otop, *oozed = new char[ow*oh];
	otop = oozed + ow + 1; // oozed map 左上
	ZeroMemory(oozed, ow*oh); // クリア
	try {
		// アルファマップを調べる
		for (y = 0; y < h; y++) {
			o = otop + y*ow;
			p = r    + y*nl;
			for (x = 0; x < w; x++, o++, p+=nc) {
				if (p[3]) *o = -1;
				else p[2] = p[1] = p[0] = 0; // 完全透明部分の色は消す
			}
		}

		// 引き伸ばし処理
		for (int i = 0; i < level; i++) {
			bool L, R, U, D;
			for (y = 0; y < h; y++) {
				o = otop + y*ow;
				p = r    + y*nl;
				for (x = 0; x < w; x++, p+=nc, o++) {
					// 未処理領域をチェック
					if (!*o) {
						DWORD cr = 0, cg = 0, cb = 0;
						// 上下左右の領域をチェック
						U=o[-ow]<0, D=o[ow]<0, L=o[-1]<0, R=o[1]<0;
						if (U || D || L || R) {
							int cnt = 0;
							if (U) AddColor(cr, cg, cb, p-nl), cnt++;
							if (D) AddColor(cr, cg, cb, p+nl), cnt++;
							if (L) AddColor(cr, cg, cb, p-nc), cnt++;
							if (R) AddColor(cr, cg, cb, p+nc), cnt++;
							p[2] = (unsigned char)(cr / cnt);
							p[1] = (unsigned char)(cg / cnt);
							p[0] = (unsigned char)(cb / cnt);
							*o = 1;
						}
					}
				}
			}
			// 処理済マップの値を再設定
			for (y = 0; y < h; y++) {
				o = otop + y*ow;
				for (x = 0; x < w; x++, o++) if (*o>0) *o=-1;
			}
		}
	} catch (...) {
		delete[] oozed;
		throw;
	}
	delete[] oozed;

	return TJS_S_OK;
}

NCB_ATTACH_FUNCTION(oozeColor, Layer, OozeColor);

/**
 * Layer.copyAlpha = function(src);
 * src の B値を α領域にコピーする
 */
static tjs_error TJS_INTF_METHOD
CopyBlueToAlpha(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *lay)
{
	if (numparams < 0) {
		return TJS_E_BADPARAMCOUNT;
	}

	// 読み込みもと
	BufRefT sbuf = 0;
	long sw, sh, spitch;
	if (!GetLayerBufferAndSize(param[0]->AsObjectNoAddRef(), sw, sh, sbuf, spitch)) {
		TVPThrowExceptionMessage(TJS_W("src must be Layer."));
	}
	// 書き込み先
	WrtRefT dbuf = 0;
	long dw, dh, dpitch;
	if (!GetLayerBufferAndSize(lay, dw, dh, dbuf, dpitch)) {
		TVPThrowExceptionMessage(TJS_W("dest must be Layer."));
	}

	// 小さい領域分
	int w = (sw < dw ? sw : dw);
	int h = sh < dh ? sh : dh;
	// コピー
	for (int i=0;i<h;i++) {
		BufRefT p = sbuf;     // B領域
		WrtRefT q = dbuf+3;   // A領域
		for (int j=0;j<w;j++) {
			*q = *p;
			p += 4;
			q += 4;
		}
		sbuf += spitch;
		dbuf += dpitch;
	}
	return TJS_S_OK;
}

NCB_ATTACH_FUNCTION(copyBlueToAlpha, Layer, CopyBlueToAlpha);

/**
 * 登録処理後
 */
static void PostRegistCallback()
{
	tTJSVariant var;
	TVPExecuteExpression(TJS_W("Layer"), &var);
	layerClass = var.AsObject();
	TVPExecuteExpression(TJS_W("Layer.assignImages"), &var);
	layerAssignImages = var.AsObject();
}

#define RELEASE(name) name->Release();name= NULL

/**
 * 開放処理前
 */
static void PreUnregistCallback()
{
	RELEASE(layerClass);
	RELEASE(layerAssignImages);
}

NCB_POST_REGIST_CALLBACK(PostRegistCallback);
NCB_PRE_UNREGIST_CALLBACK(PreUnregistCallback);
