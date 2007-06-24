/**
 * 環境BGMオブジェクト
 */
class EnvBgm extends EnvObject {
	
	storage = null;   // 参照しているファイル名
	volume = null;    // 音量
	loop = null;      // ループ指定
	start = null;     // 開始指定
	pause = null;     // ポーズ指定

	transTime = null; // 変動時間
	waitFade = null;  // フェード待ち指定
	wait = null;      // 終了待ち指定

	replay = null;  // 再生指定
	refade = null;  // 音量指定
	repause = null;

	bgmcommands = null;
	
    /**
     * コンストラクタ
     */
	constructor(env) {
		::EnvObject.constructor(env, "bgm");
		bgmcommands = {
			tagname = null, 
			storage = setPlay,
			play = setPlay,
			stop = setStop,
			pause = setPause,
			["resume"] = setResume,
			fade = setFade,
			wait = setWait,
			waitfade = setWaitFade,
			loop = null,
			time = null,
			start = null,
		};
	}

	function init() {
		replay = true;
		storage = null;
		transTime = null;
	}
	
    /**
     * 再生処理
     * @param param 再生対象ファイル
     */
    function setPlay(param, elm) {
		//dm("BGM再生:" + param);
		if (param != null) {
			replay = true;
			storage = param;
			if ("volume" in elm) {
				volume    = elm.volume;
			} else {
				volume = 100;
			}
			if ("loop" in elm) {
				loop      = elm.loop;
			}
			if ("start" in elm) {
				start     = elm.start;
			}
			if ("time" in elm) {
				transTime = elm.time;
			}
			player.setBGMFlag(param);
        }
    }

    /**
     * 停止処理
     * @param param フェードアウト時間
     */
	function setStop(param, elm) {
		//dm("BGM停止");
		replay = true;
		storage = null;
		if ("time" in elm) {
			transTime = elm.time;
		} else {
			transTime = param;
		}
    }

    /**
     * ポーズ処理
     * @param param フェードアウト時間
     */
	function setPause(param, elm) {
		if (storage != null) {
			repause = true;
			pause = true;
		}
	}

    /**
	 * 再開
	 */
	function setResume(param, elm) {
		if (storage != null) {
			repause = true;
			pause = false;
		}
    }

    /**
     * 音量フェード
     * @param param フェード時間
     */
	function setFade(param, elm) {
		refade    = true;
		volume    = param;
		if ("time" in elm) {
			transTime = elm.time;
		} else {
			transTime = param;
		}
    }

    /**
     * 終了まち
     * @param param フェード時間
     */
    function setWait(param, elm) {
		wait = clone elm;
    }

    /**
     * 終了まち
     * @param param フェード時間
     */
    function setWaitFade(param, elm) {
		waitFade = clone elm;
	}

    /**
     * コマンドの実行
     * @param cmd コマンド
     * @param param パラメータ
     * @param elm 他のコマンドも含む全パラメータ
     * @return 実行が行われた場合 true
     */
    function doCommand(cmd, param, elm) {
		if (cmd in bgmcommands) {
			local func = bgmcommands[cmd];
			if (func != null) {
				func(param, elm);
				return true;
            }
			return false;
        }
        // 再生コマンドとみなす
		setPlay(cmd, elm);
        return true;
    }

    /**
     * タグ処理
     * @param elm 引数
     */
	function tagcommand(elm) {
		//dm("BGM 用ファンクション呼び出し!");
		wait = null;
		waitFade = null;
		transTime = null;
		local doflag = false;
		foreach (name, value in elm) {
			if (doCommand(name, value, elm)) {
				doflag = true;
			}
		}
		if (!doflag && elm.tagname != "bgm") {
			setPlay(elm.tagname, elm);
        }
	}

	/**
	 * コマンド同期
	 */
	function sync() {
		if (replay) {
			//dm("再生状態変更:" + storage);
			if (storage != null) {
				player.playBGM(transTime, {storage=storage, loop=loop, start=start, volume=volume});
			} else {
				player.stopBGM(transTime);
			}
			if (pause) {
				player.pauseBGM(0);
			}
			replay = false;
			refade = false;
			repause = false;
		} else if (refade) {
			player.fadeBGM(transTime, volume);
			refade = false;
		} else if (repause) {
			if (pause) {
				player.pauseBGM(transTime);
			} else {
				player.resumeBGM(transTime);
			}
			repause = false;
		}
		if (waitFade != null) {
			return player.waitBGMFade(getint(waitFade,"timeout"), getint(waitFade, "canskip", true));
		} else  if (wait != null) {
			return player.waitBGMStop(getint(wait, "timeout"), getint(wait, "canskip", true));
		}
        return 0;
    }
};
