#ifndef __LayerExImage__
#define __LayerExImage__

#include "LayerExBase.h"

/**
 * レイヤ拡張 イメージ操作用ネイティブインスタンス
 */
class NI_LayerExImage : public NI_LayerExBase
{
public:
	/**
	 * ルックアップテーブル反映
	 * @param pLut lookup table
	 */
	void lut(BYTE* pLut);
	
	/**
	 * 明度とコントラスト
	 * @param brightness 明度 -255 ～ 255, 負数の場合は暗くなる
	 * @param contrast コントラスト -100 ～100, 0 の場合変化しない
	 */
	void light(int brightness, int contrast);

	/**
	 * 色相と彩度
	 * @param hue 色相
	 * @param sat 彩度
	 * @param blend ブレンド 0 (効果なし) ～ 1 (full effect)
	 */
	void colorize(int hue, int sat, double blend);

	/**
	 * ノイズ追加
	 * @param level ノイズレベル 0 (no noise) ～ 255 (lot of noise).
	 */
	void noise(int level);
};

#endif