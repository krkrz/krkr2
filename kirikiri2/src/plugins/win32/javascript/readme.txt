Title: Javascript Plugin
Author: わたなべごう

●これはなに？

V8 JavaScript Engine (http://code.google.com/p/v8/) の吉里吉里バインドです。

●システム概要

◇名前空間

・Javascript のグローバル空間は吉里吉里全体に対して１つだけ存在します。
　
　Javascript 用のスクリプトの実行はこのグローバル空間上でおこなわれ、
　定義されたファンクションやクラスもこのグローバル空間に登録されていきます。

・TJS2 のグローバル空間を Javascript 側から "krkr" で参照できます。

・Javascript のグローバル空間を TJS2 側から "jsglobal" で参照できます。

・整数、実数、文字列などのプリミティブ値は値渡しになります。
・TJS2 の void は javascript の undefined と対応します
・TJS2 の null は javascript の null と対応します
・オブジェクトは相互に呼び出し可能です

プリミティブ値が吉里吉里のそれとして登録されることになるので注意が必要です。
完全に Javascript な空間が欲しい場合は適宜辞書などを作成してそこに値を
登録してください。

●使用方法

◇Scripts 拡張

Javascript の実行用メソッドが Scripts クラスに拡張されます。
これにより外部の Javascript ファイルを読み込んでクラスや
メソッドとして登録可能になります。

◇吉里吉里クラスの javascriptクラス化

吉里吉里のクラスを javascript のクラスとして継承可能な状態で
扱うことができます。

・createTJSClass()で、TJSのクラスを Javascriptクラスとして扱うことができます

　登録されたクラスでは、Javascript側で生成されたインスタンスの場合は、
　Javascript に返す際にもそのまま元の Javascriptオブジェクトが帰ります。
　TJS側で生成されたインスタンスの場合は、新しいJavascriptインスタンスに
　ラッピングされて帰ります

  tjsOverride() でTJSインスタンスに直接メソッドを登録できます

  TJSインスタンス側に callSQ() として javascript インスタンスの
　メソッドを明示的に呼び出す命令が拡張されます。

  TJSインスタンス側では missing 機能が設定され、存在しないメンバが
　参照された場合は javascript インスタンスの同名メンバが参照されます。
  TJSインスタンス内部からのイベント呼び出しにもこれが適用されるため、
　TJSインスタンス中に定義がなければ自動的に javascript インスタンスの
　それが呼び出されます

操作の詳細は manual.tjs / manual.js を参照してください。

●ライセンス

Javascript V8 は 新BSDライセンスです

このプラグイン自体のライセンスは吉里吉里本体に準拠してください。
