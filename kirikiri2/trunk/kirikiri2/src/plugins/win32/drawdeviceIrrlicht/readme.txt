●これはなに？

Irrlicht ベースの drawdevice 実装のテストコードです

- Irrclicht のソースを吉里吉里レポジトリにとりこみ(1.4betaあわせ)
- gameswf は単独プラグインが安定化してからとりこみ予定でいったん削除

●コンパイル方法

コンパイルには DirectX9 SDK が必要です。

●動作上の特記事項

・DirectX9 専用でコンパイルされています。
  DirectX9 が使えない場合は drawdevice 設定時に例外がおこります

・Irrlicht からのファイルアクセスは吉里吉里のファイル空間に対して行われます
　＜まだ未実装

●使い方

  manual.tjs 参照

●今後の予定

・シーン処理系の実装

・ムービー再生処理の実装
　※できれば krmovie とうまく連携できるような方向で…

●ライセンス

Irrlicht は zlib/libpng スタイルのライセンスです。

  The Irrlicht Engine License
  ===========================

  Copyright (C) 2002-2007 Nikolaus Gebhardt

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgement in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be clearly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

このプラグイン自体のライセンスは吉里吉里本体に準拠してください。
