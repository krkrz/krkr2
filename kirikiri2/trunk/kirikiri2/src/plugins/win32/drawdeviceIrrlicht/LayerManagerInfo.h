#ifndef LAYERMANAGERINFO_H
#define LAYERMANAGERINFO_H

/**
 * レイヤマネージャ用付随情報
 */
class LayerManagerInfo {
	
public:
	LayerManagerInfo(irr::video::ITexture *texture, int layerWidth, int layerHeight);
	virtual ~LayerManagerInfo();
	
private:
	// コピー処理用一時変数
	unsigned char *destBuffer;
	int destWidth;
	int destHeight;
	int destPitch;
	
public:
	// 割り当てテクスチャ
	irr::video::ITexture *texture;
	// 元レイヤサイズ
	int layerWidth;
	int layerHeight;

	// テクスチャ描画操作用
	void lock();
	void copy(tjs_int x, tjs_int y, const void * bits, const BITMAPINFO * bitmapinfo,
			  const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity);
	void unlock();
};

#endif
