/**
 * 背景用レイヤ
 * EnvLevelLayer の定義を上書きして消してしまう
 */
class EnvBackLayer extends EnvLayer {

	// 表示レベル
	absolute = null;
	
	// 描画対象レイヤ
	targetLayer = null;
	
    /**
     * コンストラクタ
     */
	constructor(env, name, absolute) {
		::EnvLayer.constructor(env, name);
		this.absolute = absolute;
    }

	/**
	 * レイヤのコピー処理
	 */
	function onCopyLayer(toback, backlay) {
		if (backlay) {
			if (toback) {
				if (targetLayer != null) {
					if (player.isForeLayer(targetLayer) && targetLayer.visible) {
						targetLayer = player.getNewLayer(this, false, targetLayer);
						targetLayer.absolute = absolute;
					} else {
						targetLayer = player.trashLayer(targetLayer);
					}
				}
			} else {
				if (targetLayer != null) {
					if (!player.isForeLayer(targetLayer) && targetLayer.visible) {
						player.toForeLayer(targetLayer);
						targetLayer.absolute = absolute;
					} else {
						targetLayer = player.trashLayer(targetLayer);
					}
				}
			}
		}
	}
	
    /**
     * レイヤ情報の取得
     * @param isfore fore / back      
     * 同時にレベルに応じた front/back の処理も実行する
     */
	function getLayer(isfore) {
		if (targetLayer == null || player.isForeLayer(targetLayer) != isfore) {
			targetLayer = player.getNewLayer(this, isfore, targetLayer);
		}
		targetLayer.absolute = absolute;
		return targetLayer;
    }

}
