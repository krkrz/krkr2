#ifndef IRRLICHTWINDOW_H
#define IRRLICHTWINDOW_H

#include "IrrlichtBase.h"

extern void registerWindowClass();
extern void unregisterWindowClass();

/**
 * Irrlicht ベースの DrawDevice
 */
class IrrlichtWindow : public IrrlichtBase
{
protected:
	HWND parent; //< 親のハンドル
	HWND hwnd; //< 現在のハンドル
	iTJSDispatch2 *window; //< オブジェクト情報の参照
	
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
	 */
	IrrlichtWindow(int driverType, iTJSDispatch2 *win, int left, int top, int width, int height);
		
	/**
	 * デストラクタ
	 */
	virtual ~IrrlichtWindow();

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
	
	// -----------------------------------------------------------------------
	// 固有メソッド
	// -----------------------------------------------------------------------
	
protected:
	bool visible;
	int left;
	int top;
	int width;
	int height;

	void _setPos();
	
public:
	void setVisible(bool v);
	bool getVisible();

	void setLeft(int l);
	int getLeft();

	void setTop(int t);
	int getTop();
	
	void setWidth(int w);
	int getWidth();

	void setHeight(int h);
	int getHeight();
	
	/**
	 * 窓場所指定
	 */	
	void setPos(int l, int t);

	/**
	 * 窓サイズ指定
	 */	
	void setSize(int w, int h);
};

#endif
