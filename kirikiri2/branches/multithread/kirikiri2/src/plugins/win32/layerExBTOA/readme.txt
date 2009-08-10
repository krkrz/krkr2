Title: layerExBTOA
Author: わたなべごう

●これはなに？

α動画用にレイヤの右半分から左半分にコピーするメソッドを追加します。

●使い方

Layerに以下のメソッドが拡張されます。これを呼ぶだけです。

/**
 * レイヤ右半分の Blue CHANNEL を左半分の Alpha CHANNELに複製する
 */
function copyRightBlueToLeftAlpha();

/**
 * レイヤ下半分の Blue CHANNEL を上半分の Alpha CHANNELに複製する
 */
function copyBottomBlueToTopAlpha();

/**
 * レイヤの Alpha CHANNEL を0xffで埋める
 */
function fillAlpha();

/**
 * レイヤの Alpha CHANNEL を Province Image（領域画像）にコピーする
 * @param threshold 閾値。省略時またはvoidならそのままコピーする
 *                  数値を指定するとAlphaが閾値以上なら1，それ以外を0に埋める
 */
function copyAlphaToProvince(threshold);




VideoOverlay クラスを使ってレイヤに動画(右半分にα画像)
を描画したあと、onFrameUpdate() で呼び出してください。
処理は内部画像データ(imageWidthのサイズ)に対して行われます。

描画先レイヤの width は VideoOverlay クラスによって
動画のサイズに拡張されてるので、このタイミングで半分に
再調整してください。

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
