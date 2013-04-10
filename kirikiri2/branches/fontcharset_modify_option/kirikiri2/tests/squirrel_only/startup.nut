// 全例外を表示する
notifyAllExceptions(1);
printf("squirrel test start\n");

local cnt=0;
while (cnt < vargc) {
	print(format("arg%d:%s",cnt,vargv[cnt++]));
}

// クラスを参照
Window <- ::createTJSClass("TestWindow", "Window"); // 継承クラスは要列挙
Layer  <- ::createTJSClass("Layer");
System <- ::createTJSClass("System");

local WIDTH=800;
local HEIGHT=600;

/**
 * ウインドウクラス
 */
class MyWindow extends Window
{
	base = null;  //< プライマリレイヤ
	layer = null; //< 表示レイヤ

	/**
	 * コンストラクタ
	 */
	constructor(w, h) {
		::Window.constructor();
		setInnerSize(w, h);

		// primary layer
		base = Layer(this, ::tjsNull);
		base.setSize(w, h);
		add(base);

		// 表示用レイヤ
		layer = Layer(this, base);
		layer.setSize(100,100);
		layer.fillRect(0,0,100,100,0xffff0000);
		layer.setPos(400,100);
		layer.visible = true;

		// イベントを受理できるようにTJSインスタンスを上書きする

		//tjsOverride("onMouseDown", onMouseDown);   // これだとコンテキストが global
		//tjsOverride("onMouseDown", onMouseDown.bindenv(this));   // これが妥当
		tjsOverride("onMouseDown"); // この記述は自動的に bindenv(this) した自分の onMouseDown を取得

		//コメントアウトするとクローズ拒否
		//tjsOverride("onCloseQuery");

		// onKeyDown については TJS側に callSQ による呼び出しブリッジ記述がある
	}

	/**
	 * 実行処理
	 */
	function main() {
		// メインループを構築
		while (tjsIsValid()) {
			if (System.getKeyState(13)) { // VK_RETURN
				break;
			}
			local tick = ::getCurrentTick();
			layer.left = 400 + 200*sin(2 * PI * tick / 1000);
			// これにより描画処理がいったん吉里吉里に戻る
			::suspend();
		}
	};
	
	/**
	 * マウス押し下げイベント
	 */
	function onMouseDown(x, y, button, shift) {
		printf("%s:onMouseDown:%d,%d,%d,%d\n", this, x, y, button, shift);
	}

	/**
	 * キー押し下げイベント
	 */
	function onKeyDown(key,shift) {
		printf("%s:onKeyDown:%02x,%02x\n", this, key, shift);
	}

	/**
	 * 終了確認イベント
	 */
	function onCloseQuery(onclose) {
		// 終了させない
		::Window.onCloseQuery(false);
	}

};

// window
local win = MyWindow(WIDTH, HEIGHT);
win.visible = true;
win.main();
