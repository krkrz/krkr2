Title: Squirrel Plugin
Author: わたなべごう (go@wam-soft.com)

●これはなに？

Squirrel (http://squirrel-lang.org/) の吉里吉里バインドです。

●使用方法

Squirrel の呼び出し機能が Scripts に拡張されます

------------------------------------------------------------------------------
Scripts.execSQ("local a=10; return a;");	// Squirrel スクリプトの実行
Scripts.execStorageSQ("filename.nut");		// Squirrel スクリプトファイルの実行
------------------------------------------------------------------------------

・TJS2 のグローバル空間を Squirrel 側から "::krkr" で参照できます。

  プリミティブ値の参照と、オブジェクトに対する
　get/set/call が実装されています。
  ※call はクラスからのインスタンスの生成も可能です。

・Squirrel のグローバル空間を TJS2 側から "sqglobal" で参照できます。

　プリミティブ値の参照と、
  オブジェクトに対する PropGet/PropSet/FuncCall/CreateNew
  が実装されています。なお incontextof は意味をもちません。

●コンパイル

プラグインのコンパイルには以下のライブラリが必要になります。
フォルダに展開してください。

・sqplus (Squirrel の C++ バインド)

　http://wiki.squirrel-lang.org/default.aspx/SquirrelWiki/SqPlus.html

　開発時は SQUIRREL2_1_0_sqplus20.zip を使って作業しています。

●ライセンス

Squirrel は いわゆる zlib/libpngスタイルライセンスです。

Copyright (c) 2003-2006 Alberto Demichelis

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

このプラグインのライセンス自体は吉里吉里本体に準拠してください。
