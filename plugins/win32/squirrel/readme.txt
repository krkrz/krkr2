Title: Squirrel Plugin
Author: わたなべごう

●これはなに？

Squirrel (http://squirrel-lang.org/) の吉里吉里バインドです。

Squirrel 組み込み用オブジェクト指向言語です。
文法的には C 言語風で、TJS2 と構造も概念もよく似ています。

Squirrel は、協調スレッド（コルーチン）をサポートしており、
処理を任意のタイミングで中断できるため、ゲーム用のロジック
を組むのに非常に適しています。

●使用方法

◇基本

・Squirrel のグローバル空間は吉里吉里全体に対して１つだけ存在します。
　
　Squirrel 用のスクリプトの実行はこのグローバル空間上でおこなわれ、
　定義されたファンクションやクラスもこのグローバル空間に登録されていきます。

・TJS2 のグローバル空間を Squirrel 側から "::krkr" で参照できます。

  プリミティブ値の参照と、オブジェクトに対する
　get/set/call が実装されています。
  ※call はクラスからのインスタンスの生成も可能です。

・Squirrel のグローバル空間を TJS2 側から "sqglobal" で参照できます。

　プリミティブ値の参照と、
  オブジェクトに対する PropGet/PropSet/FuncCall/CreateNew
  が実装されています。なお incontextof は意味をもちません。

・Squirrel 標準ライブラリのうち以下のものが利用可能です

  - I/O
  - blob
  - math
  - string

　I/O 関係は OS直接ではなく、TJS のストレージ空間が参照されます。
　また、ファイル名も TJS のストレージ名になります。
　stdin/stdout/stderr は利用できません

・スレッド処理の補助用にグローバル関数 isCurrentThread(th) が
  定義されています。

  パラメータ th スレッドオブジェクト
  返り値 thが現在実行中のスレッドなら true

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

●ライセンス

Squirrel は いわゆる zlib/libpngスタイルライセンスです。

Copyright (c) 2003-2008 Alberto Demichelis

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
