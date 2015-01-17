Title: Steam 用プラグイン
Author: 合資会社ワムソフト 渡邊剛

●これは何？

Steam 対応用のプラグインです

●ファイルとコンパイルについて

事前に環境変数 STEAMWORKS_SDK に steamwork sdk のパスを設定しておく必要があります

readme.txt			このファイル
manual.tjs			プラグイン追加メソッド類の説明

premake4.lua			プロジェクト生成用定義ファイル


コンパイルにはpremake4が必要です。( http://industriousone.com/premake )
premake4.exeにパスを通したコマンドライン等から

VS2012の場合：
premake4 vs2012

とすると，vs2012 フォルダにプロジェクトファイルが生成されるので
それをコンパイルしてください。

●使い方

manual.tjs 参照

※Steam のオーバレイの更新描画を適切に行わせるためには D3D で
毎フレーム描画する必要があります。
そういう形の drawDevice を別途作成するか、あるいは標準の drawDevice でも、D3D 固定に
した上で continuous handler の類いで primary layer を update() すれば
同様の効果が得られると思われますが未テストです

●ライセンス

このプラグインのライセンスは吉里吉里本体に準拠してください。
動作に必要な Steam 用のランタイムDLLについては Steam の規約に従って扱って下さい

