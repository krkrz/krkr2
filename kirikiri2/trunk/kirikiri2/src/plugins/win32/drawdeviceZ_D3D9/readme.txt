●これはなに？

Direct3D9 ベースで動作する吉里吉里 drawdevice プラグインです。
吉里吉里Z由来のコードを使用しています。


●使い方

1. Window の drawDevice に対して指定可能です

-------------------------------------------
Plugins.link("drawdeviceZ.dll");
class MyWindow extends Window {
  function MyWindow() {
    super.Window();
    // drawdevice を差し替え
    drawDevice = new DrawDeviceZ();

    //...
  }
};
-------------------------------------------

機能については manual.tjs を参照してください


●コンパイル

premake4にてプロジェクトを作成してください。(vs20xxフォルダ作成済み）
コンパイルには
../tp_stub.*
../drawdevice/BasicDrawDevice.*
../00_simplebinder/*
のフォルダ・ファイルが必要です。


●既知の問題

吉里吉里由来の例外のプラグイン内でのキャッチ（再送なし）は
例外発生時にメモリリークが起こる可能性があります。
（../exceptiontest/Main.cpp ソースコメント参照）

try/catchの例外吸収を使用している下記実装を再検討のこと：
	tTVPBasicDrawDevice::EnsureDevice()
	tTVPBasicDrawDevice::SetDestRectangle()
	TVPEnsureDirect3DObject()

	＞TVPDoTryBlockによる実装に変更（ifdef __TP_STUB_H__による場合わけ）


●ライセンス

このプラグインのライセンスは吉里吉里本体および吉里吉里Zに準拠してください。

DrawDeviceZ/* のファイルは吉里吉里Zのソースコードを一部改変して使用しています。
Main.cpp 内の一部のコード（Direct3D9の初期化部分など）は
吉里吉里ZのWindowImpl.cppから流用・改変しています。
errmsg.cpp のテキストは吉里吉里Zのstring_table_en.rcから流用した
メッセージテキストを使用しています。


----------------------------------------------------------------------------
吉里吉里Zライセンス:

Copyright (c), W.Dee and contributors All rights reserved.
Contributors
 Go Watanabe, Kenjo, Kiyobee, Kouhei Yanagita, mey, MIK, Takenori Imoto, yun
Kirikiri Z Project Contributors
W.Dee, casper, 有限会社MCF, Biscrat, 青猫, nagai, ルー, 高際 雅之, 永劫,
ゆんゆん探偵, りょうご（今は無きあの星）, AZ-UME, 京 秋人, 
Katsumasa Tsuneyoshi, 小池潤, miahmie, サークル獏, アザナシ, はっしぃ, 
棚中製作所, わっふる/waffle, ワムソフト, TYPE-MOON, 有限会社エムツー
----------------------------------------------------------------------------
ソースコード形式かバイナリ形式か、変更するかしないかを問わず、以下の条件を満
たす場合に限り、再頒布および使用が許可されます。

・ソースコードを再頒布する場合、上記の著作権表示、本条件一覧、および下記免責
  条項を含めること。
・バイナリ形式で再頒布する場合、頒布物に付属のドキュメント等の資料に、上記の
  著作権表示、本条件一覧、および下記免責条項を含めること。
・書面による特別の許可なしに、本ソフトウェアから派生した製品の宣伝または販売
  促進に、組織の名前またはコントリビューターの名前を使用してはならない。

本ソフトウェアは、著作権者およびコントリビューターによって「現状のまま」提供
されており、明示黙示を問わず、商業的な使用可能性、および特定の目的に対する適
合性に関する暗黙の保証も含め、またそれに限定されない、いかなる保証もありませ
ん。著作権者もコントリビューターも、事由のいかんを問わず、損害発生の原因いか
んを問わず、かつ責任の根拠が契約であるか厳格責任であるか（過失その他の）不法
行為であるかを問わず、仮にそのような損害が発生する可能性を知らされていたとし
ても、本ソフトウェアの使用によって発生した（代替品または代用サービスの調達、
使用の喪失、データの喪失、利益の喪失、業務の中断も含め、またそれに限定されな
い）直接損害、間接損害、偶発的な損害、特別損害、懲罰的損害、または結果損害に
ついて、一切責任を負わないものとします。
