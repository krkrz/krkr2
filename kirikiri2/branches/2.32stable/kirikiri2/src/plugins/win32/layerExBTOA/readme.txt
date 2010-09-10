Title: layerExBTOA
Author: わたなべごう

●これはなに？

レイヤのα領域や Province画像をいじるメソッドを集めたものです

●使い方

各メソッドについては manual.tjs 参照

α動画用に使う場合は、VideoOverlay クラスを使ってレイヤに動画(右半分にα画像)
を描画したあと、onFrameUpdate() で copyRightBlueToLeftAlpha() を
呼び出してください。処理は内部画像データ(imageWidthのサイズ)に対して行われます。

描画先レイヤの width は VideoOverlay クラスによって動画のサイズに
拡張されてるので、このタイミングで半分に再調整してください。

例
class AlphaVideo extends VideoOverlay
{
  function AlphaVideo(window) {
    super.VideoOverlay(window);
    mode = vomLayer;
  }

  function onFrameUpdate(frame) {
    if (layer1) {
      layer1.width = layer1.imageWidth / 2;
      layer1.copyRightBlueToLeftAlpha();
    }
  }
}

●ライセンス

このプラグインのライセンスは吉里吉里本体に準拠してください。
