/**
 * 前景レイヤのレベル制御用
 */
class EnvLevelLayer extends EnvImage {

    /// 表示レベル
	level = null;
	/// 表示絶対レベル
	absolute = null;
	
	// 描画対象レイヤ
	targetLayer = null;
	
	/// 位置変更
    front = null;
	back = null;

    /**
     * コンストラクタ
     */
	function constructor(env, name) {
		::EnvImage.constructor(env, name);
		level = null;
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
					} else {
						targetLayer = player.trashLayer(targetLayer);
					}
				}
			} else {
				if (targetLayer != null) {
					if (!player.isForeLayer(targetLayer) && targetLayer.visible) {
						player.toForeLayer(targetLayer);
					} else {
						targetLayer = player.trashLayer(targetLayer);
					}
				}
			}
		}
	}
	
    /**
     * レイヤ情報の取得
     * @param base fore / back      
     * 同時にレベルに応じた front/back の処理も実行する
     */
	function getLayer(isfore) {
		if (targetLayer == null || player.isForeLayer(targetLayer) != isfore) {
			targetLayer = player.getNewLayer(this, isfore, targetLayer);
		}
		if (level == null) {
			level = env.defaultLevel;
		}
		player.toLevel(targetLayer, level);
		if (front) {
			player.toFront(targetLayer);
		} else if (back) {
			player.toBack(targetLayer);
		}
		front = back = null;
		absolute = targetLayer.absolute;
		return targetLayer;
    }

    /**
     * レベル内最前列
     */
    function setFront() {
        front = true;
		updateFlag = true;
    }

    /**
     * レベル内最後列
     */
    function setBack() {
        back = true;
		updateFlag = true;
    }

    /**
     * レベルの指定
	 */
	function setLevel(cmd, elm) {
		level = cmd;
		absolute = null;
	} 
}
