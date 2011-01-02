Title: xpressive - yet another RegExp プラグイン
Author: miahmie

●これは何か？

※ experimental（実験的）なプラグインです！　実践投入はご遠慮ください。

RegExp を純正の boost::regexp から boost::xpressive に置き換えるプラグインです。
boost::xpressive は，テンプレートのみの実装で完結しており，余分なライブラリの
リンクが不要なため，比較的扱いやすい正規表現ライブラリとなっています。


●tjsRegExp.* について

このフォルダの tjsRegExp.* は，吉里吉里本体の core/tjs2/tjsRegExp.* を
改造したコードになっており，このままのコードで tjs2 コアを生成することも
できるようになっています。

その場合，あらかじめ TJS_USE_XPRESSIVE マクロを定義しておいてから
コンパイルしてください。定義しない場合は従来の boost::regexp を使用しますが，
最新の boost ではうまくコンパイルできなかったので，そちらも一部修正してあります。


●既知の不具合

プラグインでの static tTJSVariant のコンストラクタ問題が未修正です。

xpressiveプラグインをリンクすると，Array.split(/～/, ...) が
正しく動かなくなります。（Array側の実装による問題）
＞Array.splitメソッドを上書きして対応することで回避する予定

改造版 tjsRegExp.* を使用して tjs2コアを生成した場合はこの限りではありません。

その他，まだ十分なテストがなされていません。


●ライセンス

このプラグインのライセンスは吉里吉里本体に準拠してください。

