Title: Squirrel Plugin
Author: わたなべごう

●これはなに？

Squirrel (http://squirrel-lang.org/) の吉里吉里バインドです。

Squirrel 組み込み用オブジェクト指向言語です。
文法的には C 言語風で、TJS2 と構造も概念もよく似ています。

Squirrel は、協調スレッド（コルーチン）をサポートしており、
スクリプトの実行処理を任意のタイミングで中断できるため、
ゲーム用のロジックを組むのに非常に適しています。

●システム概要

・Squirrel のグローバル空間は吉里吉里全体に対して１つだけ存在します。
　
　Squirrel 用のスクリプトの実行はこのグローバル空間上でおこなわれ、
　定義されたファンクションやクラスもこのグローバル空間に登録されていきます。

・TJS2 ←→ Squirrel 間で変数を参照できます

  整数、実数、文字列などのプリミティブ値は値渡しになります。

  TJS2 オブジェクト(iTJSDispatch2*) は、Squirrel では UserData として
  参照可能で、メタメソッド get/set/call を通じて操作可能です。
  クラスも参照できますがこれを squirrel で継承することはできません。
　クラスを call した場合は、TJS2 側でインスタンスが作成され、
　それを UserData で参照したものが帰ります。

  squirrel オブジェクトは、TJS2 側では iTJSDispatch2 として参照可能で、
　PropGet/PropSet/FuncCall/CreateNew を通じて操作可能です。
  incontextof 指定は無視されます。

  Scripts.registSQ() で TJS2 の値を squirrel 側に登録できます。

  Scripts.registClassSQ() で、TJS2のクラスと対応した squirrel クラスを
  作ることができます。これで登録したクラスは Object（内部クラス) を
　継承した状態となりさらにこれを継承して新たなクラスを作ることが可能です。
  
・TJS2 のグローバル空間を Squirrel 側から "::krkr" で参照できます。

・Squirrel のグローバル空間を TJS2 側から "sqglobal" で参照できます。

・Squirrel 標準ライブラリのうち以下のものが利用可能です

  - I/O
  - blob
  - math
  - string

　I/O 関係は OS直接ではなく、TJS のストレージ空間が参照されます。
　また、ファイル名も TJS のストレージ名になります。
　stdin/stdout/stderr は利用できません

●使用方法

◇Scripts 拡張

Squirrel スクリプトの実行機能や、オブジェクトを Squirrel の書式で
文字列化したり、ファイルに保存したりするメソッドが Scripts クラス
に拡張されます。詳細は manual.tjs を参照してください

◇SQFunction 拡張

Squirrel のglobalファンクションを直接呼び出せるように保持するクラスです。
TJS2 ラッピングによる余分な負荷なしに呼び出し処理を行うことができます。
詳細は manual.tjs を参照してください。

◇SQContinous 拡張

Squirrel のglobalファンクションを直接呼び出す Continuous Handler を
保持するクラスです。
TJS2 ラッピングによる余分な負荷なしに呼び出し処理を行うことができます。
詳細は manual.tjs を参照してください。

◇スレッド拡張

squirrelによる複数のスレッドの並列実行処理が実装されています。
このために利用できる内部クラス Object / Thread が定義されています。
詳細は squirrel/sqobject/manual.nut を参照してください。

このスレッドの実行は、Continuous Handler によって実行されています。
また、処理の強制中断はないため、個別のスレッドで定期的に wait() を
行わないかぎりフリーズ状態となるので注意する必要があります。
※TJS同様、容易に busy loop を引き起こします

●ライセンス

Squirrel は いわゆる zlib/libpngスタイルライセンスです。

Copyright (c) 2003-2009 Alberto Demichelis

This software is provided 'as-is', without any 
express or implied warranty. In no event will the 
authors be held liable for any damages arising from 
the use of this software.

Permission is granted to anyone to use this software 
for any purpose, including commercial applications, 
and to alter it and redistribute it freely, subject 
to the following restrictions:

		1. The origin of this software must not be 
		misrepresented; you must not claim that 
		you wrote the original software. If you 
		use this software in a product, an 
		acknowledgment in the product 
		documentation would be appreciated but is 
		not required.

		2. Altered source versions must be plainly 
		marked as such, and must not be 
		misrepresented as being the original 
		software.

		3. This notice may not be removed or 
		altered from any source distribution.
-----------------------------------------------------
END OF COPYRIGHT

このプラグイン自体のライセンスは吉里吉里本体に準拠してください。
