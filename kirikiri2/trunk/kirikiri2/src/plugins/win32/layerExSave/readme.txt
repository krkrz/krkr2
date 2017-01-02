Title: layerExSave plugin
Author: わたなべごう

●これはなに？

Layer/Window クラスにTGL5/PNG 形式での保存メソッドを追加するプラグインです。

※吉里吉里Zでは標準でTLG5/TLG6/PNG/JPEGの保存機能があるので
　このプラグインではなく、そちらを利用することを推奨します。
＞詳細はLayer.saveLayerImage/Bitmap.saveのマニュアルを参照してください

●PNG保存の制限

PNG保存はlibpngを使用せず，下記メソッドごとに保存の実装が異なります。

	Window.startSaveLayerImage   : 独自実装による保存
	Layer.saveLayerImagePng      : LodePNGによる保存
	Layer.saveLayerImagePngOctet : 〃

独自実装はプログレス処理の都合による簡易処理のため、
フォーマットには、下記の制限があります。

・32bitRGBA固定（透明度あり）
・フィルタ/インターレース処理なし

フィルタ処理がないので、圧縮率は libpng/LodePNG の実装のものより劣ります。
※圧縮率が重要になるような画像は、LodePNGでの保存を推奨します。

LodePNG ( http://lodev.org/lodepng/ )はポータブルなPNGのロード/セーブの実装です。
./LodePNG/* の2ファイルが該当します。(version 20161127を使用)


タグ情報（offs_*, reso_*, vpag_*）もサポートされますが動作確認が不十分です。

タグ情報辞書に comp_lv を渡すと圧縮率を変更できます。
（0～9まで：この情報はPNGのチャンクとしては保存されません）
※LodePNG側でcomp_lvを指定するとzlibのdeflate処理を使用します。
　未指定の場合はLodePNG組み込みのdeflate処理を使用します。


●使い方

manual.tjs 参照


●コンパイル

premake4にてプロジェクトを作成してください。(vs20xxフォルダ作成済み）
コンパイルには
../tp_stub.*
../ncbind/*
../zlib/*
../../../tools/win32/krdevui/tpc/tlg5/slide.*
のフォルダ・ファイルが必要です。

LAYEREXSAVE_DISABLE_LODEPNG を指定してコンパイルするとLodePNGを使用せず
すべて独自実装による保存処理となります。（旧版と同じ仕様です）


●ライセンス

このプラグインのライセンスは吉里吉里本体に準拠してください。


LodePNG のライセンスは zlib license になります。

LodePNG version 20151208

Copyright (c) 2005-2015 Lode Vandevenne

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.


