// 全例外を表示する
notifyAllExceptions(1);
printf("squirrel test start\n");

local cnt=0;
while (cnt < vargc) {
	print(format("arg%d:%s",cnt,vargv[cnt++]));
}

// クラスを参照
Window <- ::createTJSClass("Window");
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
		//tjsOverride("onCloseQuery", onCloseQuery.bindenv(this));
	}

	/**
	 * 実行処理
	 */
	function main() {
		// メインループを構築
		while (tjsIsValid()) {
			if (System.getKeyState.bindenv(System)(13)) { // VK_RETURN
				break;
			}
			local tick = ::getCurrentTick();
			layer.left = 400 + 200*sin(2 * PI * tick / 1000);
			// これにより描画処理がいったん吉里吉里に戻る
			::suspend();
		}
	};

	/**
	 * 終了確認イベント
	 */
	function onCloseQuery(onclose) {
		::Window.onCloseQuery(false);
	}
};

// window
local win = MyWindow(WIDTH, HEIGHT);
win.visible = true;
win.main();
