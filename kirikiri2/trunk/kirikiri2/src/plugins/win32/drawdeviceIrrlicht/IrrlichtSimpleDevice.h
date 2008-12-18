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
	
public:
	/**
	 * コンストラクタ
	 * @param window 親になる窓
	 * @param width 横幅
	 * @param height 縦幅
	 */
	IrrlichtSimpleDevice(iTJSDispatch2 *window, int width, int height);
		
	/**
	 * デストラクタ
	 */
	virtual ~IrrlichtSimpleDevice();

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
	 */
	void updateToLayer(iTJSDispatch2 *layer);
};

#endif
