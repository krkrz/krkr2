/**
 * 環境オブジェクトの基底クラスその2
 * ・画面更新機能対応
 */
class EnvBase extends EnvObject {

	baseCommands = null;
	
	/**
	 * コンストラクタ
	 * @param env 環境
	 */
	constructor(env, name) {
		::EnvObject.constructor(env, name);
		baseCommands = {
			trans = setTrans,
			tagname = null,     // タグ名は無視
			time = null,        // 各種時間指定用
			fade = null,
		};
	}

	// --------------------------------------------------------------------
	// トランジション処理
	// --------------------------------------------------------------------
	
    // 画面更新設定
	trans = null;
	
    /**
	 * トランジションを設定
	 * @param name トランジション名
	 * @param elm パラメータ
	 * @return トランジションが設定された
	 */
	function setTrans(name, elm) {
		//dm("トランジション設定:" + name);
		local tr = player.getTrans(name, elm);
		if (tr != null) {
			trans = tr;
			return true;
		}
		return false;
    }

	// 自動トランジション
	autoTrans = null;

	// 自動トランジションを登録
	function setAutoTrans(param) {
		if (param != null) {
			if (typeof param == "string") {
				local tr = player.getTrans(param);
				if (tr != null && "method" in tr) {
					autoTrans = tr;
					//dm("自動トランジション指定:" + tr.method + ":" + tr.time);
					return;
				}
			} else if (typeof param == "table") {
				autoTrans = param;
				return;
			}
		}
	}

	/**
	 * コマンドの実行
	 * @param cmd コマンド
	 * @param param パラメータ
	 * @param elm 他のコマンドも含む全パラメータ
	 * @return 実行が行われた場合 true
	 */
    function doCommand(cmd, param, elm) {
		
		//dm("コマンド処理:" + cmd + " パラメータ:" + param);
		
		if (cmd in baseCommands) {
			local func = baseCommands[cmd];
			if (func != null) {
				func(param, elm);
			}
			return true;
        }

		// トランジションパラメータを排除
		if (cmd in player.transitionParam) {
			return true;
		}

		// トランジション指定の判定
		if (setTrans(cmd, elm)) {
			return true;
		}
		
		return false;
	}
	
	_ret = null;
	
	function getRet() {
		return _ret;
	}
	
	function setRet(v) {
		if (v == null) {
			_ret = 0;
		} else {
			if (v < _ret) {
				_ret = v;
			}
		}
	}

	/**
	 * 個別コマンド処理
	 * @param elm 引数
	 */
	function command(elm) {
		foreach (name, value in elm) {
			if (!doCommand(name, value, elm)) {
				player.errorCmd(name + ":未知のコマンド:" + name);
			}
		}
	}

	/**
	 * 更新処理
	 */
	function doUpdate(elm) {
		player.backup();
		update(false);
		player.beginTransition(trans);
	}

	/**
	 * 更新処理
	 */
	function update(isfore) {
	}

	// フェード指定のデフォルト値
	function getFadeValue() {
		return env.fadeValue;
    }
	
	/**
	 * タグ処理
	 * @param elm 要素
	 */
	function tagcommand(elm) {
		
		setRet(null);
		trans = null;
		autoTrans = null;
		
		// コマンドを実行
		command(elm);

		// トランジション再処理
		if (trans == null) {
			if ("fade" in elm) {
				local fadeTime = elm.fade;
				trans = { time = fadeTime > 1 ? fadeTime : getFadeValue() };
			} else {
				trans = autoTrans;
			}
		}

		if (trans != null) {
			// キャラクタ強制消去
			if ("charoff" in trans) {
				foreach (name, value in env.characters) {
					value.disp = CLEAR;
				};
			}
			// レイヤ強制消去
			if ("layeroff" in trans) {
				foreach (name, value in env.layers) {
					value.disp = CLEAR;
				};
			}
			// メッセージ窓消去
			if ("msgoff" in trans) {
				player.addTag("msgoff");
			}
		}
	}

	function sync(elm) {
		//dm("更新処理開始" + name);
		if (player.transMode) {
			update(player.transMode != 1);
		} else if (trans == null) {
			update(true);
		} else {
			doUpdate(elm);
		}
		return getRet();
	}
};
