// サンプルコード

// TJSのクラスをプロトタイプの形で取得
var Window = createTJSClass("Window");
var Layer  = createTJSClass("Layer");

// 独自レイヤクラス
function MyLayer(win, parent, width, height, color)
{
	// 親コンストラクタ呼び出し
	Layer.call(this, win, parent);
	this.setSize(width,height);
	this.fillRect(0,0,width,height,color);
}

// 独自レイヤのプロトタイプ
MyLayer.prototype = {
  __proto__: Layer.prototype
};

// 独自ウインドウクラス
function MyWindow(width, height)
{
	// 親コンストラクタ呼び出し
	Window.call(this);
	this.setInnerSize(width, height);
	this.add(new MyLayer(this, null, this.innerWidth, this.innerHeight, 0xff00ff));
	// イベント登録
	this.tjsOverride("onKeyDown");
}

// 独自ウインドウのプロトタイプ
MyWindow.prototype = {
  __proto__: Window.prototype,
  onKeyDown : function (key, shift) {
	  if (key == krkr.VK_ESCAPE) {
		  this.close();
	  }
  }
};

// 生成
var win = new MyWindow(400,200);
win.visible = true;
