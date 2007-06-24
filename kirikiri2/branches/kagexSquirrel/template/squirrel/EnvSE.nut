/**
 * 環境SEオブジェクト
 */
class EnvSE extends EnvObject {

	id = null;
	count = 0; // 参照されたカウント値

	storage = null;   // 参照しているファイル名
	volume = null;    // 音量
	loop = null;      // ループ指定
	start = null;     // 開始指定

	transTime = null; // 変動時間
	waitFade = null;  // フェード待ち指定
	wait = null;      // 終了待ち指定
	
	replay = null;  // 再生指定
	refade = null;  // 音量指定

    secommands = null;
	
    /**
     * コンストラクタ
     */
    function constructor(env, id) {
		::EnvObject.constructor(env, null);
		this.id = id;
		secommands = {
			tagname = null, 
			storage = setPlay,
			play = setPlay,
			stop = setStop,
			fade = setFade,
			volume = setFade,
			wait = setWait,
			waitfade = setWaitFade,
			loop = null,
			time = null,
			start = null,
			canskip = null,
			buf = null,
		};
	}

	function init() {
		name = null;
		count = 0;
		replay = true;
		storage = null;
		transTime = null;
	}
	
    /**
     * 再生処理
     * @param param 再生対象ファイル
     */
	function setPlay(param, elm) {
		if (param != null) {
			replay  = true;
			if (getval(elm, loop) || !(player.isSkip() && player.nosewhenskip)) {
				storage   = param;
				volume    = getval(elm, "volume", 100);
				loop      = getval(elm, "loop", false);
				start     = getval(elm, "start");
				transTime = getval(elm, "time");
			} else {
				storage = null;
			}
		}
	}

    /**
     * 停止処理
     * @param param フェードアウト時間
     */
	function setStop(param, elm) {
		replay    = true;
		storage   = null;
		transTime = getval(elm, "time", param);
    }

    /**
     * 音量フェード
     * @param param フェード時間
     */
	function setFade(param, elm) {
		refade    = true;
		volume    = param;
		transTime = getval(elm, "time", param);
    }

    /**
     * 終了待ち
     */
    function setWait(param, elm) {
		wait = clone elm;
    }

    /**
     * フェード終了待ち
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
		if (cmd in secommands) {
			local func = secommands[cmd];
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
		// dm("SE 用ファンクション呼び出し!");
		wait = null;
		waitFade = null;
		transTime = null;
		local doflag = false;
		foreach (name, value in elm) {
			if (doCommand(name, value, elm)) {
				doflag = true;
			}
		}
		if (!doflag && elm.tagname != "se") {
			setPlay(elm.tagname, elm);
        }
    }

	function sync() {
		if (replay) {
			if (storage != null) {
				player.playSE(id, transTime, {storage=storage, loop=loop, start=start, volume=volume});
			} else {
				player.stopSE(id, transTime);
			}
			replay = false;
			refade = false;
		} else if (refade) {
			player.fadeSE(id, transTime, volume);
			refade = false;
		}
		if (waitFade != null) {
			return player.waitSEFade(id, getint(waitFade, "timeout"), getint(waitFade, "canskip", true));
		} else if (wait != null) {
			return player.waitSEStop(id, getint(wait, "timeout"), getint(wiatFade, "canskip", true));
		}
		return 0;
	}

};
