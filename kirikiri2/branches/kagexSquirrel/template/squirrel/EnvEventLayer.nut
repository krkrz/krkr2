/**
 * イベント画像を処理するレイヤ
 */
class EnvEventLayer extends EnvBackLayer {

    /**
	 * コンストラクタ
     * @param env 環境
	 * @param name 環境のレイヤ名
	 */
	constructor(env) {
		::EnvBackLayer.constructor(env, "event", 6* 100000 - 100);
    }

	/**
	 * イベント用にファイル名のかけかえ
	 */
    function setImageFile(file, elm) {
		local eventInfo;
		if ("events" in env && file in env.events) {
			eventInfo = env.events[file];
			file = eventInfo.image;
			if (!("xpos" in elm) && !("left" in elm) && "left" in eventInfo) {
				props.left <- eventInfo.left;
			}
			if (!("ypos" in elm) && !("top" in elm) && "top" in eventInfo) {
				props.top <- eventInfo.top;
			}
		}
		::EnvBackLayer.setImageFile(file, elm);
		if (eventInfo != null && "trans" in eventInfo) {
			setAutoTrans(eventTrans);
		} else if ("eventTrans" in env.envinfo) {
			setAutoTrans(env.envinfo.eventTrans);
		} else if ("envTrans" in env.envinfo) {
			setAutoTrans(env.envinfo.envTrans);
		}
        return true;
    }

	// ------------------------------------------------------
	
    /**
	 * コマンドの実行
	 * @param cmd コマンド
	 * @param param パラメータ
	 * @param elm 他のコマンドも含む全パラメータ
     * @return 実行が行われた場合 true
     */
    function doCommand(cmd, param, elm) {

		// 共通コマンド
		if (::EnvBackLayer.doCommand(cmd, param, elm)) {
			return true;
        }
		
		// 画像のロード
		return setImageFile(cmd, elm);
	}
	// ------------------------------------------------------

	/**
	 * 表示処理用の自動トランジションの指定
	 */
	function setDispAutoTrans() {
		if ("eventTrans" in env.envinfo) {
			setAutoTrans(env.envinfo.eventTrans);
		} else if ("envTrans" in env.envinfo) {
			setAutoTrans(env.eventinfo.envTrans);
		}
	}
}
