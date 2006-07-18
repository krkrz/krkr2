これはなに？

Squirrel の吉里吉里バインドです。
Squirrel は zlib ライセンスです。
このプラグインのライセンスは吉里吉里本体に準拠してください。

Scripts.execSQ("script");					Squirrel スクリプトの実行
Scripts.execStorageSQ("filename.nut");		Squirrel スクリプトファイルの実行

・Squirrel の名前空間はプラグインがロードされている限り有効です
・TJS2 の名前空間を ::krkr で参照できます。インスタンスの生成も可能です。
・現在の版では TJS2 側から Squirrel の名前空間を直接参照することはできません。

このフォルダのファイルのコンパイルには以下のライブラリが必要になります

・sqplus 

　http://wiki.squirrel-lang.org/default.aspx/SquirrelWiki/SqPlus.html

  開発時は SQUIRREL2_1_0_sqplus20.zip
  を使って作業しています。

 Author: わたなべごう
