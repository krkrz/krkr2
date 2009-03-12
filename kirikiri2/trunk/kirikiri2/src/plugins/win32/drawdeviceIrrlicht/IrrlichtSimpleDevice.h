#ifndef IrrlichtSimpleDevice_H
#define IrrlichtSimpleDevice_H

#include "IrrlichtBase.h"

/**
 * Irrlicht 描画デバイス
 */
class IrrlichtSimpleDevice :	public IrrlichtBase
{

protected:
	iTJSDispatch2 *window; //< オブジェクト情報の参照(親ウインドウ)
	tjs_int width;  //< Irrlicht 実画面の画面横幅
	tjs_int height; //< Irrlicht 実画面の画面縦幅
	bool useRender; //< レンダーターゲットを使う
	irr::video::ITexture *target; //< レンダーターゲット

	tjs_int dwidth;
	tjs_int dheight;
	HBITMAP hbmp; // 描画先DIB
	HBITMAP oldbmp;
	HDC destDC; // 描画先DC
	void *bmpbuffer;

	void clearDC();
	void updateDC(int dwidth, int dheight);
		
	
	// イベント処理
	static bool __stdcall messageHandler(void *userdata, tTVPWindowMessage *Message);
	
	// ユーザメッセージレシーバの登録/解除
	void setReceiver(tTVPWindowMessageReceiver receiver, bool enable);
	
	/**
	 * ウインドウを生成
	 * @param krkr 吉里吉里のウインドウ
	 */
	void createWindow(HWND krkr);

	/**
	 * ウインドウを破棄
	 */
	void destroyWindow();

	// デバイス割り当て
	virtual void attach(HWND hwnd, int width=0, int height=0);
	
	// デバイスの破棄
	virtual void detach();

public:
	/**
	 * コンストラクタ
	 * @param widow 親になるウインドウ
	 * @param width 横幅
	 * @param height 縦幅
	 * @param useRender レンダーターゲットを使う(αが有効)
	 */
	IrrlichtSimpleDevice(iTJSDispatch2 *window, int width, int height, bool useRender);
		
	/**
	 * デストラクタ
	 */
	virtual ~IrrlichtSimpleDevice();

	// -----------------------------------------------------------------------
	// 生成ファクトリ
	// -----------------------------------------------------------------------

	static tjs_error Factory(IrrlichtSimpleDevice **obj, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);

	// -----------------------------------------------------------------------
	// 共通メソッド呼び出し用
	// -----------------------------------------------------------------------

public:
	irr::video::IVideoDriver *getVideoDriver() {
		return IrrlichtBase::getVideoDriver();
	}

	irr::scene::ISceneManager *getSceneManager() {
		return IrrlichtBase::getSceneManager();
	}

	irr::gui::IGUIEnvironment *getGUIEnvironment() {
		return IrrlichtBase::getGUIEnvironment();
	}

	irr::ILogger *getLogger() {
		return IrrlichtBase::getLogger();
	}

	irr::io::IFileSystem *getFileSystem() {
		return IrrlichtBase::getFileSystem();
	}

	// -----------------------------------------------------------------------
	// 固有メソッド
	// -----------------------------------------------------------------------
protected:
	void _setSize();
	
public:
	void setWidth(int w) {
		if (width != w) {
			width = w;
			_setSize();
		}
	}

	int getWidth() {
		return width;
	}

	void setHeight(int h) {
		if (height != h) {
			height = h;
			_setSize();
		}
	}

	int getHeight() {
		return height;
	}
	
	/**
	 * 窓サイズ指定
	 */	
	void setSize(int w, int h) {
		if (width != w || height != h) {
			width = w;
			height = h;
			_setSize();
		}
	}

	/**
	 * レイヤに対して更新描画
	 * バックバッファからコピーします。
	 * @param layer レイヤ
	 * @param srcRect ソース領域
	 */
	void _updateToLayer(iTJSDispatch2 *layer, irr::core::rect<irr::s32> *srcRect = NULL);

	/**
	 * レイヤに対して更新描画(呼び出し用)
	 */
	static tjs_error updateToLayer(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis);
};

#endif
