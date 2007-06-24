/**
 * 舞台用レイヤ
 */
class StageGraphicLayer extends EnvGraphicLayer {

	/**
	 * コンストラクタ
	 * @param owner オブジェクト情報をもってる親
	 * @param isfore 表画面か裏画面か
	 */
	function constructor(owner, isfore) {
		::EnvGraphicLayer.constructor(owner, isfore);
	}

	function recalcPosition() {
		setRealPosition(owner.env.xmax + _left - getWidth() / 2,
						owner.env.ymax + _top  - getHeight() / 2);
	}
};


/**
 * 背景を処理するレイヤ
 * グローバルに以下のメソッドがあった場合はフックとして呼び出します
 */
class EnvStageLayer extends EnvBackLayer {

	/// 時間
	timeName = null;
    /// 舞台
	stageName = null;

	function initImage() {
		timeName = null;
		stageName = null;
	}

	stageLayerCommands = null;
	
    /**
	 * コンストラクタ
     * @param env 環境
	 * @param name 環境のレイヤ名
	 */
	function constructor(env) {
		::EnvBackLayer.constructor(env, "stage", 100);
		stageLayerCommands = {
			stage = setStage,
			stime = setTime,
		};
    }

	/**
	 * 背景画像の更新処理
	 */
	function updateStageImage(elm) {

		if (stageName == null) {
			return;
		}

		local info;
		if (stageName in env.stages) {
			info = env.stages[stageName];
		} else {
			throw "ステージ情報がありません:" + stageName;
		}
		
		local image = info.image;
		if (timeName == null) {
			timeName = env.defaultTime;
		}
		if (timeName in env.times) {
			image = replace(image, "TIME", env.times[timeName].prefix);
		}

		if (!isExistImageName(image)) {
			// 画像がない場合は標準画像の補正で対応
			image = info.image;
			image = replace(image, "TIME", env.times[env.defaultTime].prefix);
			// 色補正処理
			local timeInfo;
			if ((timeInfo = env.getCurrentTime()) != null) {
				if ("lightColor" in timeInfo) {
					elm.lightcolor = timeInfo.lightColor;
				}
				if ("lightType" in timeInfo) {
					elm.lighttype  = timeInfo.lightType;
				}
				if ("brigtness" in timeInfo) {
					elm.brightness = timeInfo.brightness;
				}
				if ("contrast" in timeInfo) {
					elm.contrast   = timeInfo.contrast;
				}
			}
		}
		
		if (!("xpos" in elm) && !("left" in elm) && "left" in info) {
			props.left <- info.left;
		}
		if (!("ypos" in elm) && !("top" in elm) && "top" in info) {
			props.top <- info.top;
		}
		setImageFile(image, elm);
		if ("trans" in info) {
			setAutoTrans(info.trans);
		} else if ("stageTrans" in env.envinfo) {
			setAutoTrans(env.envinfo.stageTrans);
		} else if ("envTrans" in env.envinfo) {
			setAutoTrans(env.envinfo.envTrans);
		}
	}
	
	/**
     * 舞台を設定する
     * @param stageName 舞台名
     * @param elm コマンドのほかの要素
     */
    function setStage(stageName, elm) {

		if (this.stageName != stageName || isClear() || env.event.isShow()) {
			this.stageName = stageName;
			updateStageImage(elm);
        }
		// イベント絵は消去
		if (env.event.isShow()) {
			env.event.disp = CLEAR;
		}
	}

    /**
     * 時間を設定する
     * @param timeName 時間名
     * @param elm コマンドのほかの要素
     */
    function setTime(timeName, elm) {

		if (this.timeName != timeName || isClear() || env.event.isShow()) {
			
			this.timeName = timeName;
			updateStageImage(elm);
			
			// 時間変更はキャラの立ち絵も再描画の必要がある
			foreach (name, value in env.characters) {
				value.setRedraw();
            };
        }
        // イベント絵は消去
		if (env.event.isShow()) {
			env.event.disp = CLEAR;
        }
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

		// レイヤ共通コマンド
		if (cmd in stageLayerCommands) {
			local func = stageLayerCommands[cmd];
			if (func != null) {
				func(param, elm);
			}
			return true;
		}

		// 時間と舞台
		if (cmd in env.times) {
			setTime(cmd, elm);
			return true;
		} else if (cmd in env.stages) {
			setStage(cmd, elm);
			return true;
		}

		return false;
	}
	
	// ------------------------------------------------------

	/**
	 * 更新処理
	 * キャラクタも更新する必要がある
	 */
	function update(isfore) {
		foreach (name, value in env.characters) {
			value.update(isfore);
		};
		::EnvBackLayer.update(isfore);
	}
	
	// ------------------------------------------------------

	/**
	 * 表示処理用の自動トランジションの指定
	 */
	function setDispAutoTrans() {
		if ("stageTrans" in env.envinfo) {
			setAutoTrans(env.envinfo.stageTrans);
		} else if ("envTrans" in env.envinfo) {
			setAutoTrans(env.envinfo.envTrans);
		}
	}

	/**
	 * 新レイヤ生成
	 */
	function createLayer(isfore) {
		return StageGraphicLayer(this, isfore);
	}
}
