●これはなに？

OGRE ベースの drawdevice 実装のテストコード
まだ肝心の吉里吉里描画がはいってませんのであんま意味はありません。
とりあえず drawdevice の基本構造はこの形ということで。

●コンパイル方法

コンパイル時に設定しておく環境変数

 OGRE_HOME  OGRE の SDK が展開されてるフォルダ名

●使い方

とりあえず $(OGRESDKHOME)/bin/ に dll と krkr.exe と sample/startup.tjs を
data/ として配置して実行するのを想定。
エラー処理とかてきとーなので注意。あとおわるときになんか例外はいてる（てへ
