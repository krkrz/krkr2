print("オブジェクト参照のテスト");

// 吉里吉里クラスのバインド
Layer <- createTJSClass("Layer");
layer <- Layer(::krkr.win, ::krkr.win.base);
layer.setSize(100,100);
layer.setPos(150,150);
layer.fillRect(0,0,100,100,0xffff00ff);
print("layer visible:" + layer.visible);
layer.visible = true;
print("layer visible:" + layer.visible);

System <- createTJSClass("System");

print("type:" + typeof System);
print("tick値:" + System.getTickCount.bindenv(System)());
print("実行ファイルのパス:" + System.getExePath.bindenv(System)());
