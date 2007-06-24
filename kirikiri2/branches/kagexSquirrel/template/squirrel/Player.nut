/**
 * 再生処理の基底クラス
 * ・タイムアウトつき待ち機能
 * ・アクション機能
 * ・外部ファンクション機能
 * を提供する
 */
class Player {

	// ---------------------------------------------------
	// 共通処理
	// ---------------------------------------------------
	
	function dm(msg) {
		print(msg);
	}

	// ---------------------------------------------------
	// 動作情報
	// ---------------------------------------------------
	
	STOP = 0; // 停止
	RUN  = 1; // 動作中
	WAIT = 2; // 待ち
	status = 0; // シナリオ状態

	// 待ち情報
	waitUntil = {};

	// 時間情報
	currentTick = 0;  // 現在の実行時間
	timeoutTick = null;  // 時間待ち用
	
	// ----------------------------------------------------
	// システム本体
	// ----------------------------------------------------
	
	/**
	 * コンストラクタ
	 */
	constructor() {
	}

	/**
	 * システムがスキップ状態にあるか
	 */
	function isSkip() {
		return false;
	}
	
	/**
	 * 待ち情報の廃棄
	 */
	function clearWait() {
		foreach (idx,val in waitUntil) {
			delete waitUntil[idx];
		}
	}

	/**
	 * 状態のクリア
	 */
	function clear() {
		clearWait();
		killFunc();
		status = STOP;
	}

	/**
	 * 待ち
	 * param util シグナル名とコールバック関数の辞書配列
	 */
	function wait(until) {
		status = WAIT;
		foreach (name,value in until) {
			waitUntil[name] <- value;
		}
	}

	/**
	 * タイムアウトつきの待ち
	 * タイムアウト時には 'timeout' がトリガされる
	 * @param until 待ち情報
	 * @param timeout タイムアウト時間
	 */
	function waitWithTimeout(until, timeout) {
		wait(until);
		if (timeout == 0) timeout = 1; // timeout が 0 の場合は 1 に
		timeoutTick = currentTick + timeout;
	}

	/**
	 * タイムアウト情報の更新
	 * @param diff 経過時間
	 */
	function updateTimeout(diff) {
		// タイムアウト処理
		if (timeoutTick != null) {
			timeoutTick -= diff;
			if (timeoutTick < 0) {
				trigger("timeout");
				timeoutTick = null;
			}
		}
	}
	
	/**
	 * 待ち解除トリガ
	 *
	 * waitUntil 内にシグナル名 name が存在すれば、実行再開、
	 * 同時に waitUntil に登録されたメソッド(リスタートハンドラ)を呼ぶ
	 * シグナル名に _arg がついたものが waitUntil 内にあれば、
	 * それを引数としてハンドラに渡す
	 * waitUntil はクリアされる
	 * @param name トリガ名
	 * @return トリガ成功したら true
	 */
	function trigger(name) {
		if (status != WAIT) return false;
		if (name in waitUntil) {
			local func = waitUntil[name];
			local argname = name + "_arg";
			if (argname in waitUntil) {
				func(waitUntil[arg]);
			} else {
				func();
			}
			clearWait();
			status = RUN;
			return true;
		} else {
			return false;
		}
	}

	/**
	 * トリガ待ち
	 * @param name トリガ名
	 * @param waittime タイムアウト時間
	 * @param canskip スキップ可能か
	 * @param onskip スキップ時に実行する処理
	 * @return 待ち状態なら -2
	 */
	function waitTrigger(name, waittime, canskip, onskip) {
		// elm.name で示されたトリガを待つ
		if (canskip)	{
			// スキップできる場合
			if (isSkip()) {
				// スキップ動作中
				if (onskip != null) eval(onskip);
				return 0; // すぐに返る
			}
			waitWithTimeout({
				click = function(arg) {	if (arg != null) eval(arg);	} 
				click_arg = onskip,
				[elm.name] = function() {},
				timeout = function() {},
			}, waittime);
		} else {
			waitWithTimeout({
				[elm.name] = function(){},
				timeout = function() {},
			});
		}
		return -2;
	}
	
	/**
	 * 時間待ち
	 * @param waittime 待ち時間
	 * @param canskip  スキップ可能か
	 * @return 待ち状態なら -2
	 */
	function waitTime(waittime, canskip) {
		if (waittime <= 0) {
			return 0;
		}
		if (canskip) {
			if (isSkip()) {
				return 0;
			}
			waitWithTimeout({
				click = function() {},
				timeout = function() {},
			}, waittime);
		} else {
			waitWithTimeout({
				timeout = function() {},
			}, waittime);
		}
		return -2;
	}

	// ------------------------------------------------
	// アクション処理
	// ------------------------------------------------

    // アクション対象全部
	allActions = {};
	actionCount = 0;

	/**
	 * アクション実行中か
	 */
	function isInAction(target) {
		return target in allActions;
	}

	/**
	 * アクション情報をコピーする
	 * @param src コピー元
	 * @param dest コピー先
	 * @param completed 終了時処理(関数)
	 */
	function copyAction(src, dest, completed) {
		//dm("アクション情報のコピー");
		stopAction(dest);
		if (src in allActions) {
			local srcAction  = allActions[src];
			local destAction = srcAction.cloneObj();
			destAction.target = dest;
			if (completed != null) {
				destAction.onActionCompleted = completed;
			}
			allActions[dest] = destAction.weakref();
			actionCount++;
		}
	}

	/**
	 * アクション実行結果を返す
	 * @param target ターゲットオブジェクト
	 */
	function getActionResult(target) {
		if (target in allActions) {
			local info = allActions[target];
			return info.getResultValue();
		}
	}
	
    /**
	 * アクションを登録
	 * @param target ターゲットオブジェクト
     * @param action アクション情報
	 * @param completed 終了ハンドラ
	 * @param nowait 処理を待たない指定
	 * @return そのアクションの実行結果
     */
	function beginAction(target, action, completed, nowait) {
		//dm("アクション開始:" + nowait + ":" + action.nowait);
		if (!(target in allActions)) {
			info = ActionSequense(target);
			allActions[target] = info.weakref();
			actionCount++;
		}
		if (completed != null) {
			// 終了ハンドラを登録
			info.onActionCompleted = completed;
		}
        
        // 文字列の場合は評価してしまう
		if (typeof action == "string") {
			action = eval(action);
        }
        
		if (typeof action == "table") {
			//dm("辞書アクション:" + action);
			//showKeys("アクション", action);
			// 辞書の場合
			if ("module" in action) {
				if (typeof action.module != "string") {
					throw "アクションモジュール名は文字列指定してください";
				}
				local module = eval(action.module);
				if (module != null && typeof module == "function") {
					info.addActions(module(action));
				}
			} else {
				// 複数アクションの同時指定
				info.addActions(action);
			}
			if (getint(action, "next")) {
				info.next();
			}
			if (getint(action, "nowait")) {
				info.nowait = action.nowait;
			}
		} else if (typeof action == "array") {
			//dm("シーケンスアクション");
			for (local i=0;i<action.len();i++) {
				info.addActions(action[i]);
				info.next();
			}
		}
		if (nowait != null) {
			info.nowait = nowait;
		}
		return info.getResultValue();
	}
    
    /**
     * アクションを停止
     * @param target ターゲットオブジェクト
     */
	function stopAction(target) {
		if (target in allActions) {
			//dm("指定アクション停止:" + target.name);
			actionCount--;
			info.stopAction();
			if ("onActionCompleted" in info &&
				typeof info.onActionCompleted == "function") {
				info.onActionCompleted(info.target);
			}
			delete allActions[target];
        }
    }

    /**
	 * 全アクションの停止
	 * @param all true なら本当に全部停止 false の場合 nowait 指定のあるものはとめない
     */
	function stopAllActions(all) {
		//dm("全アクション停止:" + all);
		foreach (name, info in allActions) {
			if (all || !getint(info, "nowait")) {
				//dm("停止対象アクション:" + name + ": " + info.nowait);
				actionCount--;
				info.stopAction();
				if ("onActionCompleted" in info &&
					typeof info.onActionCompleted == "function") {
					info.onActionCompleted(info.target);
				}
				delete allActions[name];
			}
		}
	}
    
    /**
     * アクション状態更新
     * システム全体から呼び出す
     * @param now 現在時刻
     */
    function updateAction(now) {
		foreach (name, info in allActions) {
			if (info.doAction(now, false)) {
				//dm("アクション終了:" + info.target);
				actionCount--;
				if ("onActionCompleted" in info &&
					typeof info.onActionCompleted == "function") {
					info.onActionCompleted(info.target);
				}
				delete allActions[name];
			}
        }
    }
    
	/**
	 * アクション終了
	 * @param self アクションの実行対象
     */
	function onActionCompleted(self) {
		// 個別アクション停止
		trigger("action_" + self);
		if (actionCount == 0) {
			// 全アクション停止
			trigger("action");
		}
    }

	/**
	 * アクション待ち
	 * @param target アクション対象
	 * @param canskip スキップできるかどうか
	 */
	function waitAction(target, canskip) {
		if (!isInAction(target)) return 0; // アクションを待てない
		local name = "action_" + target.tostring();
		if (canskip) {
			// スキップできる場合
			if (isSkip()) {
				// スキップ動作中
				stopAction(target);
				return 0; // 自動移動を停止させてすぐに返る
			}
			wait({
				click = function (target) {	stopAction(target); },
				click_arg = target, // ハンドラへの引数
				[name] = function() {} 
			});
		} else {
			wait({[name] = function() {}});
		}
		return -2;
	}

	/**
	 * 全アクションを待つ
	 * @param canskip スキップできるかどうか
	 */
	function waitAllAction(canskip)	{
		// アクションを待つ
		if (actionCount == 0) return 0; // アクションを待てない
		if (calskip) {
			// スキップできる場合
			if (isSkip()) {
				// スキaップ動作中
				stopAllActions(false);
				return 0; // 自動移動を停止させてすぐに返る
			}
			wait({
				click = function() { stopAllActions(false); },
				action = function() {}
			});
		} else {
			wait({action = function(){}});
		}
		return -2;
	}

	// ------------------------------------------------
	// 機能ファンクション
	// ------------------------------------------------

	// 機能ファンクション
	funcs = [];

	// ファンクション系の強制終了
	function stopFunc()	{
		for (local i=funcs.len() - 1; i>=0; i--) {
			funcs[i].done();
		}
	}
	
	// ファンクション系のクリック処理
	function clickFunc() {
		for (local i=funcs.len() - 1; i>=0; i--) {
			funcs[i].click();
		}
	}
	
	// ファンクション系の強制破棄
	function killFunc() {
		if (funcs.len() > 0) {
			for (local i=funcs.len() - 1; i>=0; i--) {
				local func = funcs[i];
				funcs.remove(i);
			}
			trigger("func");
		}
	}

	/**
	 * 機能終了待ち
	 */
	function waitFunc(func, waittime, canskip) {
		if (canskip) {
			//print("Func待ちスキップ可能");
			if (isSkip()) {
				stopFunc();
				return 0;
			}
			funcs.append(func);
			waitWithTimeout({
				click = function() { stopFunc(); },
				func = function() {},
				timeout = function() { stopFunc(); },
			}, waittime);
		} else {
			//print("Func待ちスキップ不可");
			waitWithTimeout(func);
			addWait({
				func = function() {},
				timeout = function() { stopFunc();},
			}, waittime);
		}
	}

	/**
	 * 特殊機能状態更新
	 */
	function updateFunc(diff) {
		if (funcs.len() > 0) {
			for (local i=funcs.len() - 1; i>=0; i--) {
				local func = funcs[i];
				if (!func.update(diff)) {
					funcs.remove(i);
				}
			}
			if (funcs.len() == 0) {
				trigger("func");
			}
		}
	}

	// ------------------------------------------------------
	// メインロジック
	// ------------------------------------------------------

	/**
	 * 更新処理
	 */
	function update(diff) {
		currentTick += diff;
		updateFunc(diff);
		updateTimeout(diff);
		updateAction(currentTick);
	}

	/**
	 * 実行処理
	 */
	function run() {
	}

	/**
	 * メイン
	 */
	function main(diff)	{
		update(diff);
		if (status == RUN) {
			run();
		}
		return status;
	}

	// ------------------------------------------------------
	// 外部イベント
	// ------------------------------------------------------
	
	function onKeyPress(key) {
		print("キープレス:" + key + "\n");
	}

	function onKeyRelease(key) {
		print("キーリリース:" + key + "\n");
	}

	/**
	 * BGMのフェードが終了した
	 */
	function onBgmFadeCompleted() {
	}

	/**
	 * BGM が停止した
	 */
	function onBgmStop() {
	}

	/**
	 * SE のフェードが停止した
	 */
	function onSEFadeCompleted(id) {
	}

	/**
	 * SE が停止した
	 */
	function onSEStop(id) {
	}

	/**
	 * 全リセット時の処理
	 */
	function onResetAll() {
	}
}
