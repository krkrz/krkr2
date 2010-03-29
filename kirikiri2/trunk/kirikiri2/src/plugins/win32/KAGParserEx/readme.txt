Title: KAGParserEx プラグイン
Author: miahmie

●これは何か？

KAGParserを置き換えて拡張するプラグインです。
安定したら本体側にcontribする予定ですが，
状況次第ではこのままで行くかもしれません。

現在のところ，拡張されるのは下記１点のみです。
なお，仕様は予告無く変更される場合があります。


・タグの複数行記述が可能になる multiLineTagEnabled プロパティの追加

　行末に "\" を記述し，次の行の先頭に ";" を入れることで，
　複数行にわたるタグを記述できます。
　※行末に "\" のみで次の行頭に ";" がないとエラーになります。

	@tag hoge=123 fuga=test \
	;    someoption=true \
	;    andmore=false

　行頭に ";" を入れるのは，旧仕様のパーサーに通しても ch タグとして
　認識されないようにコメントとする無理やりな hack 仕様です。

　"\" の前には必ず１つ以上のスペースを空けてください。
　空けないと前の要素とくっついて認識されてしまいます。
　また，タグ名は必ず１行目に記述します。

	; ダメな例：
	@tag hoge=123\			← hoge="123\"と認識
	@tag hoge\			← hoge\=trueと認識
	@ \				← \ というタグ名で認識

　現仕様では，[～]表記で複数行のタグを記述する場合，そのタグを閉じた以降に
　さらにタグを続けても無視されるようになっています。
　（改行タグも自動で挿入されません）

	[tag hoge=123 fuga=test \
	;    someoption=true \
	;    andmore=false][このタグは無視される]


●ソースについて

KAGParserのソースをそのまま流用＆改造して組み込んでいます。
ソースはsvn copyしているので，ログから本体側からの変更点を追うことができます。

	KAGParser.cpp   <- src/core/utils/KAGParser.cpp
	KAGParser.h     <- src/core/utils/KAGParser.h
	tjsHashSearch.h <- src/core/tjs/tjsHashSearch.h


●使い方

KAGParserEx.dll をリンクすると，KAGParserクラスが置き換わります。
アンリンクすると，元に戻ります。


●ライセンス

このプラグインのライセンスは吉里吉里本体に準拠してください。

