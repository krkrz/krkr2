//
// 汎用アクション機能実装用クラス群
//

// アクション時間制御用
// グローバルに指定
drawspeed <- 1;

/**
 * 単独プロパティに対するアクション情報
 * 絶対アクション：ターゲットプロパティの絶対値を指定する　　　　　１つしか使えない
 * 相対アクション：ターゲットプロパティに対する相対値を指定する　　複数指定できる
 */
class PropActionInfo {

	propName = null;      // プロパティ名
    absolute = null;      // 絶対アクションハンドラ
    relative = []; // 相対アクションハンドラ
	first = true;     // 初回実行
	startTime = null; // 開始時刻
    
    /**
     * コンストラクタ
     */
	constructor(propName) {
        this.propName = propName;
	}

	/**
	 * 複製の生成
	 */
	function cloneObj() {
		local ret = ::PropActionInfo(propName);
		ret.absolute = absolute;
		for (local i=0;i<relative.len();i++) {
			ret.relative.append(relative[i]);
		}
		ret.first = this.first;
		ret.startTime = this.startTime;
		return ret;
	}

    /**
     * アクションを追加する
     * @param target ターゲットオブジェクト
     * @param handler アクションハンドラクラス
     * @param elm 初期化パラメータ
     */
    function addAction(target, handler, elm) {
		//dm("アクション追加:" + propName);
		if (typeof andler == "class") {
			if (inherited(handler, AbsoluteActionHandler)) {
				// 絶対アクションは１つだけ設定可能
				absolute = handler(target, propName, elm);
			} else if (inherited(handler, RelativeActionHandler)) {
				// デフォルトアクション登録
				if (absolute == null) {
					//dm("デフォルトアクション登録");
					absolute = DefaultAction(target, propName, {});
				}
				// 相対アクションは複数設定可能
				relative.append(handler(target, propName, elm));
			} else {
				dm("ハンドラ異常:" + handler);
			}
		} else {
			dm("ハンドラ異常:" + handler);
		}
    }

	/**
	 * アクションの実行結果を返す
	 */
	function getResultValue() {
		if (absolute != null) {
			return absolute.getResultValue();
		} else {
			return null;
		}
	}

	
    /**
     * アクション処理実行
     * @param target ターゲットオブジェクト
     * @param now 時間
     * @param stopFlag 強制終了指定
     * @return 終了したら true
     */
    function doAction(target, now, stopFlag) {
		if (first) {
			//dm("アクション初回");
			startTime = now;
			if (absolute != null) {
				absolute.init(target, propName);
			}
			for (local i=0;i<relative.len();i++) {
				relative[i].init(target, propName);
			}
			first = false;
		}
		now -= startTime;
		//dm("アクション状態:"+ now + " stop:" + stopFlag);
        local done = true;
		if (absolute != null) {
			if (!absolute.action(target, propName, now, stopFlag)) {
				done = false;
            }
        }
        if (!stopFlag) {
			for (local i=0;i<relative.len();i++) {
				if (!relative[i].action(target, propName, now)) {
					done = false;
                }
            }
        }
        return done;
    }
};

/**
 * 複数プロパティに対するアクション
 */
class ActionInfo {

	actionDict = {}; // アクション情報
	actionList = [];

    /**
     * コンストラクタ
     */
	constructor() {
    }

	/**
	 * 複製の生成
	 */
	function cloneObj() {
		local ret = ::ActionInfo();
		for (local i=0;i<actionList.len();i++) {
			local newact = actionList[i].cloneObj();
			ret.actionList[i] <- newact;
			ret.actionDict[newact.propName] <- newact;
		}
		return ret;
	}

	
    function getPropActionInfo(propName) {
		local ret;
		if (propName in actionDict) {
			ret = actionDict[propName];
		} else {
			ret = PropActionInfo(propName);
			actionDict[propName] <- ret;
			actionList.append(ret);
		}
		return ret;
    }

	
    /**
     * 指定されたプロパティのアクションを追加する
     * @param target ターゲット
     * @param propName プロパティ名
     * @param handler アクションハンドラ
     * @param elm 初期化パラメータ
     */
    function addAction(target, propName, handler, elm) {
		local info = getPropActionInfo(propName);
        info.addAction(target, handler, elm);
    }

    /**
     * 指定されたプロパティのアクションを消去する
     * @param target ターゲット
     * @param propName プロパティ名
     */
	function delAction(target, propName) {
		if (propName in actionDict) {
			local ret = actionDict[propName];
			ret.doAction(target, 0, true);
			removeFromArray(actionList, ret);
			delete actionDict[propName];
		}
	}

	/**
	 * アクションの結果を取得する
	 */
	function getResultValue(ret) {
		for (local i=0;i<actionList.len();i++) {
			local info = actionList[i];
			local result = info.getResultValue();
			if (result != null) {
				ret[info.propName] <- result;
			}
		}
		return ret;
	}
	
    /**
     * アクション処理実行
     * @param target ターゲットオブジェクト
     * @param now 時間
     * @param stopFlag 強制終了指定
     * @return 終了したら true
     */
    function doAction(target, now, stopFlag) {
        local done = true;
        for (local i=0; i<actionList.len(); i++) {
            if (!actionList[i].doAction(target, now, stopFlag)) {
                done = false;
            }
        }
        return done;
    }
};

/**
 * 一連のアクション
 */
class ActionSequense {

	actions = [];

    target = null; // 対象オブジェクト
	create = true; // アクション追加フラグ
	nowait = false; // 待ちなしフラグ
	
    /**
     * コンストラクタ
     */
	constructor(target) {
		this.target = target;
	}

	/**
	 * 複製の生成
	 */
	function cloneObj() {
		local ret = ::ActionSequense(target);
		ret.create = create;
		ret.nowait = nowait;
		for (local i=0;i<actions.len();i++) {
			ret.actions.append(actions[i].cloneObj());
		}
		return ret;
	}
	
    /**
     * アクションシーケンスを進める
     */
    function next() {
        create = true;
    }
    
    /**
     * アクションを追加する
     * @param propName プロパティ名
     * @param handler アクションハンドラ
     * @param elm 初期化パラメータ
     */
    function addAction(propName, handler, elm) {
		if (target.hasProp(propName)) {
			if (create) {
				actions.append(ActionInfo());
				create = false;
            }
			actions[actions.len() - 1].addAction(target, propName, handler, elm);
		} else {
			dm("指定されたプロパティは存在しません:" + target + ":" + propName);
        }
    }

    /**
     * アクションを消去する
     * @param propName プロパティ名
     */
    function delAction(propName) {
        if (actions.len() > 0) {
            actions[actions.len() - 1].delAction(target, propName);
        }
    }
    
    /**
     * 複数のアクションを同時登録する
     * @param dict アクション情報の入った辞書
     */
    function addActions(dict) {
		foreach (name, elm in dict) {
			if (elm == null) {
				delAction(name);
			} else if (typeof elm == "table") {
				if ("handler" in elm) {
					if (typeof elm.handler == "string") {
						addAction(name, eval(elm.handler), elm);
					} else if (typeof elm.handler == "class") {
						addAction(name, elm.handler, elm);
					} else {
						dm("無効なアクションハンドラ指定:" + elm.handler);
					}
				} else {
					dm("ハンドラ指定がありません");
				}
			}
        }
    }

    // -----------------------------------------------


    /**
	 * アクションの結果を取得する
     */
	function getResultValue() {
		local ret = {};
		for (local i=0;i<actions.len();i++) {
			actions[i].getResultValue(ret);
		}
		return ret;
	}
	
    /**
     * アクション実行
     * @param now 現在時刻
     * @param stopFlag 強制停止指定
     * @param 終了したら true
     */
    function doAction(now, stopFlag) {
		if (actions.len() > 0) {
			if (actions[0].doAction(target, now, stopFlag)) {
				actions.remove(0);
			}
		}
		return actions.len() <= 0;
    }

    /**
     * 強制停止
     */
	function stopAction() {
		while (actions.len() > 0) {
			//dm("アクション停止:" + actions.len());
			actions[0].doAction(target, 0, true);
			actions.remove(0);
        }
    }
};

// ----------------------------------------------------------------
// ハンドラクラス
// ----------------------------------------------------------------

/**
 * アクションハンドラ基底クラス
 */
class ActionHandler {

	time  = null; // 駆動時間
	delay = null; // 開始遅延時間

    /**
     * コンストラクタ
     * @param target 対象オブジェクト
     * @param propName 対象プロパティ
     * @param elm 初期化パラメータ
     */
    function ActionHandler(target, propName, elm) {
		time  = getint(elm, "time");
		delay = getint(elm, "delay");
		if (time != null) {
			time *= ::drawspeed;
		}
		//dm("ActionHandler:" + propName + " time:" + time + " delay:" + delay);
    }

	/**
	 * 実行初回処理
	 */
	function init(target, propname) {
	}
	
    /**
     * 実行処理ベース
     * @param target 対象オブジェクト
     * @param propName 対象プロパティ
     * @param now 駆動時間
     * @param stopFlag 停止フラグ
     * @return 終了したら true
     */
    function action(target, propName, now, stopFlag) {
		if (!stopFlag && delay != null) {
			now -= delay;
			if (now < 0) {
				return false;
            }
		}
		return doAction(target, propName, now, stopFlag || (time != null && now > time));
    }
    
    /**
     * アクション実行
     * @param target 対象オブジェクト
     * @param propName 対象プロパティ
     * @param now 駆動時間
     * @param stopFlag 停止フラグ
     * @return 終了したら true
     */
    function doAction(target, propName, now, stopFlag) {
        // それぞれのアクションで実装
        return true;
    }

    /**
     * 相対値計算
     * @param value 値指定
     * @param orig オリジナルの値
     */
    function getRelative(value, orig) {
		if (typeof value == "string" && value.find("@") != null) {
			return eval(replace(value, "@", orig));
		} else {
			return value;
		}
    }
}

/**
 * 絶対動作アクション基底クラス
 * 値を維持するアクション
 */
class AbsoluteActionHandler extends ActionHandler {
	constructor(target, propName, elm) {
		::ActionHandler.constructor(target, propName, elm);
    }
};

/**
 * 相対アクション基底クラス
 */
class RelativeActionHandler extends ActionHandler {
	constructor(target, propName, elm) {
		::ActionHandler.constructor(target, propName, elm);
    }
};

/**
 * 値初期化アクション
 * 相対アクションのみの場合にベースの値を維持するために使う
 */
class DefaultAction extends AbsoluteActionHandler {

	initValue = null; // 初期値

	function constructor(target, propName, elm) {
		::AbsoluteActionHandler.constructor(target, propName, elm);
		initValue = target[propName];
    }
	/**
	 * 移動の結果
	 */
	function getResultValue() {
		return initValue;
	}

	function doAction(target, propName, now, stopFlag) {
		target[propName] = initValue;
		return true;
    }
}


/**
 * 基本移動アクション（絶対アクション）
 * @param 
 */
class MoveAction extends AbsoluteActionHandler {

	initValue = null; // 初期時の値
	start = null;  // 始点指定 
	value = null;  // 終点指定
    accel = null;  // 加減速指定
	diff  = null;  // 移動差分
    
	constructor(target, propName, elm) {
		::AbsoluteActionHandler.constructor(target, propName, elm);
		initValue = target[propName];
		start     = "start" in elm ? getRelative(getint(elm, "start"), initValue) : null;
		value     = getRelative(getint(elm, "value"), initValue);
		accel     = getint(elm.accel);
    }

	/**
	 * 移動の結果
	 */
	function getResultValue() {
		return value;
	}
	
	/**
	 * 実行初回処理
	 */
	function init(target, propName) {
		if (start == null) {
			start = target[propName];
		}
		diff  = value - start;
		//dm("MoveAction:" + propName + " start:" + start + " diff:" + diff);
	}

	
    function doAction(target, propName, now, stopFlag) {
		if (time == 0 || stopFlag) {
			target[propName] = start + diff;
			return true;
        } else {
			local n;
			if (accel == 0) {
				n = now / time;
			} else if (accel > 0) {
				local t = now / time;
				n = t*t;
            } else {
				local t = now/time;
				n = ( 1-(1-t)*(1-t) );
			}
			target[propName] = start + diff * n;
			return false;
        }
    }
}

/**
 * ループ移動アクション（絶対アクション）
 * @param start 開始位置（未指定時は現在値)
 * @param min ループ最少値
 * @param max ループ最大値
 * @param loop ループ時間
 */
class LoopMoveAction extends AbsoluteActionHandler {

	initValue = null; // 初期時の値
	start = null;     // 始点指定 
	min = null;       // ループ最少値
	max = null;       // ループ最大値
	loop = null;      // ループ時間

	/**
	 * コンストラクタ
	 */
	constructor(target, propName, elm) {
		::AbsoluteActionHandler.constructor(target, propName, elm);
		initValue = target[propName];
		start  = getval(elm, "start");
		min    = getint(elm, "min");
		max    = getint(elm, "max");
		loop   = getint(elm, "loop", time);
    }

	/**
	 * 最終値
	 */
	function getResultValue() {
		return initValue;
	}
	
	/**
	 * 実行初回処理
	 */
	function init(target, propName) {
		if (start == null) {
			start = target[propName];
		} else {
			start = getRelative(start, initValue);
		}
		min = getRelative(min, initValue);
		max = getRelative(max, initValue);
		max   -= min;
		start -= min;
	}
	
	function doAction(target, propName, now, stopFlag) {
		if (stopFlag) {
			return true;
		} else {
			target[propName] = min + (start + max * (now%loop) / loop) % max;
			return false;
		}
	}
}


/**
 * 振動アクション（相対アクション）
 * vibration : 振動量
 * offset    : 補正値
 */
class VibrateAction extends RelativeActionHandler {

	vibration = null;
	offset = null;

	constructor(target, propName, elm) {
		::RelativeActionHandler.constructor(target, propName, elm);
		local initValue = target[propName];
		vibration  = getRelative("value" in elm ? getval(elm, "value") : getval(elm, "vibration"), initValue);
		offset     = getRelative(getval(elm, "offset"), init.value);
    }
};

/**
 * ランダム振動アクション（相対アクション）
 * vibration : 振動量
 * offset    : 補正値
 * waittime 振動間の待ち時間
 */
class RandomAction extends VibrateAction {

    waitTime = null;
    randomTime = null;
	randomValue = null;
    
	constructor(target, propName, elm) {
		::VibrateAction.constructor(target, propName, elm);
		waitTime    = getint(elm, "waittime");
		randomTime  = 0;
		randomValue = 0;
    }

    function doAction(target, propName, now, stopFlag) {
        if (stopFlag) {
            return true; 
        } else {
            if (now >= randomTime) {
				randomValue = (random(vibration * 2) - vibration) + offset;
				randomTime += waitTime;
			}
			target[propName] += randomValue;
			return false;
        }
    }
};

/**
 * 矩形波
 * vibration : 振動量
 * offset    : 補正値
 * ontime  : ONの時間   値:vibration
 * offtime : OFFの時間  値:-vibration
 */
class SquareAction extends VibrateAction {

    ontime = null;
    offtime = null;
	
	constructor(target, propName, elm) {
		::VibrateAction.constructor(target, propName, elm);
		ontime  = getint(elm, "ontime");
		offtime = getint(elm, "offtime");
    };

    function doAction(target, propName, now, stopFlag) {
        if (stopFlag) {
            return true;
        } else {
			target[propName] += ((now % (ontime + offtime) < ontime) ? vibration : -vibration) + offset;
			return false;
        }
    }
};

/**
 * 三角波
 * vibration : 振動量
 * offset    : 補正値
 * 0～vibration を繰り返す
 * ontime  : 上昇時間
 * offtime : 加工時間
 */
class TriangleAction extends VibrateAction {

    ontime = null;
	offtime = null;

	constructor(target, propName, elm) {
		::VibrateAction.constructor(target, propName, elm);
		ontime  = getint(elm, "ontime");
		offtime = getint(elm, "offtime");
    };

    function doAction(target, propName, now, stopFlag) {
        if (stopFlag) {
            return true;
        } else {
            now = now % (ontime + offtime);
            local v;
            if (now <= ontime) {
                v = vibration * now / ontime;
            } else {
                if (offtime > 0) {
                    v = vibration * (offtime - (now - ontime)) / offtime;
                } else {
                    v = 0;
                }
            }
			target[propName] += v + offset;
			return false;
        }
    }
};

/**
 * 三角関数振動アクション（相対アクション）
 * vibration : 振動量
 * offset    : 補正値
 * cycle     : 周期(ms)
 * angvel    : 角速度(度/sec) ※cycle指定時は無効
 */
class TrigonoAction extends VibrateAction {
    
    cycle = null;
	angvel = null;
    
    constructor(target, propName, elm) {
		::VibrateAction.constructor(target, propName, elm);
		cycle  = getint(elm, "cycle");
		angvel = getint(elm, "angvel");
    }

    function calcAngle(now) {
		return (cycle != null) ? (PI * 2 * now / cycle) : (PI * 2 * now * angvel / 360000.0);
    }
};

/**
 * sin振動アクション（相対アクション）
 * vibration : 振動量
 * offset    : 補正値
 * cycle     : 周期(ms)
 * angvel    : 角速度(度/sec) ※cycle指定時は無効
 */
class SinAction extends TrigonoAction {

	constructor(target, propName, elm) {
		::TrigonoAction.constructor(target, propName, elm);
	}
    function doAction(target, propName, now, stopFlag) {
        if (stopFlag) {
            return true;
        } else {
			target[propName] += vibration * sin(calcAngle(now)) + offset;
			return false;
        }
    }
}

/**
 * cos振動アクション（相対アクション）
 * vibration : 振動量
 * offset    : 補正値
 * cycle     : 周期(ms)
 * angvel    : 角速度(度/sec) ※cycle指定時は無効
 */
class CosAction extends TrigonoAction {

	constructor(target, propName, elm) {
		::TrigonoAction.constructor(target, propName, elm);
    }
    function doAction(target, propName, now, stopFlag) {
        if (stopFlag) {
            return true;
        } else {
			target[propName] += vibration * cos(calcAngle(now)) + offset;
            return false;
        } 
    }
}

/**
 * 落下アクション
 * distance 落下量
 * time 落下時間
 */
class FallAction extends RelativeActionHandler {

    distance = null;
    fallTime = null;
    
	constructor(target, propName, elm) {
		::RelativeActionHandler.consturctor(target, propName, elm);
		distance = getint(elm, "distance");
    }

    function doAction(target, propName, now, stopFlag) {
        if (stopFlag) {
            return true;
        } else {
			//dm("fall:" + now);
			target[propName] += distance * (now/time - 1.0);
            return false;
        }
    }
};

// ----------------------------------------------------------------
// 旧版互換用機能
// ----------------------------------------------------------------

function LayerNormalZoomModule(elm) {
	return {
		zoom = {
			handler = "MoveAction",
			start = getval(elm, "start"),
			value = getval(elm, "zoom"),
			time  = getval(elm, "time"),
			delay = getval(elm, "delay"),
			accel = getval(elm, "accel"),
		}
	};
}

function LayerToRotateModule(elm) {
	local rotate = getval(elm, "angle" in elm ? "angle" : "rotate");
	return {
		rotate = {
			handler = "MoveAction",
			start   = getval(elm, "start"),
			value   = rotate,
			time    = getval(elm, "time"),
			delay   = getval(elm, "delay"),
			accel   = getval(elm, "accel"),
		}
	};
}

function LayerFadeToModeModule(elm) {
	return {
		opacity= {
			handler = "MoveAction",
			start   = getval(elm, "start"),
			value   = getval(elm, "opacity"),
			time    = getval(elm, "time"),
			delay   = getval(elm, "delay"),
			accel   = getval(elm, "accel"),
		}
	};
}

function LayerMoveModule(elm, accel) {
	return {
		left = {
			handler = "MoveAction",
			start   = getval(elm, "startx"),
			value   = getval(elm, "x"),
			time    = getval(elm, "time"),
			delay   = getval(elm, "delay"),
			accel   = accel,
		},
		top = {
			handler = "MoveAction",
			start   = getval(elm, "starty"),
			value   = getval(elm, "y"),
			time    = getval(elm, "time"),
			delay   = getval(elm, "delay"),
			accel   = accel,
		}
	};
}

function LayerNormalMoveModule(elm) {
	return LayerMoveModule(elm, 0);
}

function LayerAccelMoveModule(elm) {
	return LayerMoveModule(elm, 1);
}

function LayerDecelMoveModule(elm) {
	return LayerMoveModule(elm, -1);
}

function LayerVibrateActionModule(elm) {
	return {
		left = {
			handler   = "RandomAction",
			vibration = getint(elm, "vibration")/2,
			waittime  = getval(elm, "waittime"),
			time      = getval(elm, "time"),
			delay     = getval(elm, "delay"),
		},
		top   = {
			handler   = "RandomAction",
			vibration = getint(elm, "vibration")/2,
			waittime  = getval(elm, "waittime"),
			time      = getval(elm, "time"),
			delay     = getval(elm, "delay"),
		}
	};
}

function LayerJumpActionModule(elm) {
	return {
		left =  null,
		top =  {
			handler   = "SinAction",
			vibration = getval(elm, "vibration"),
			cycle     = getval(elm, "cycle"),
			angvel    = getval(elm, "angvel"),
			time      = getval(elm, "time"),
			delay     = getval(elm, "delay"),
		}
	};
}

function LayerJumpOnceActionModule(elm) {
	return {
		left =  null,
		top =  {
			handler   = "SinAction",
			vibration = -getint(elm, "vibration"),
			cycle     = getval(elm, "cycle"),
			angvel    = getval(elm, "angvel"),
			time      = getint(elm, "cycle")/2,
			delay     = getval(elm, "delay"),
		}
	};
}

function LayerWaveActionModule(elm) {
	return {
		top =  null,
		left =  {
			handler = "SinAction",
			vibration = getval(elm, "vibration"),
			cycle     = getval(elm, "cycle"),
			angvel    = getval(elm, "angvel"),
			time      = getval(elm, "time"),
			delay     = getval(elm, "delay"),
		}
	};
}

function LayerWaveOnceActionModule(elm) {
	return {
		top =  null,
		left =  {
			handler = "SinAction",
			vibration = -getint(elm, "vibration"),
			cycle     = getval(elm, "cycle"),
			angvel    = getval(elm, "angvel"),
			time      = getval(elm, "cycle")/2,
			delay     = getval(elm, "delay"),
		},
	};
}

function LayerFallActionModule(elm) {
	local time  = "falltime" in elm ?
		getval(elm, "falltime") : getval(elm, "time");
	return {
		left =  null,
		top =  {
			handler  = "FallAction",
			distance = getval(elm, "distance"),
			delay    = getval(elm, "delay"),
			time     = time,
		}
	};
}

function LayerVRotateZoomModule(elm) {
	return {
		zoomy =  null,
		zoomx =  {
			handler   = "SinAction",
			vibration = "@",
			offset    = "-@",
			cycle     = getval(elm, "time"),
			angvel    = getval(elm, "angvel"),
			time      = getval(elm, "time"),
			delay     = getval(elm, "delay"),
		},
	};
}

function LayerHRotateZoomModule(elm) {
	return {
		zoomx =  null,
		zoomy =  {
			handler   = "SinAction",
			vibration = "@",
			offset    = "-@",
			cycle     = getval(elm, "time"),
			angvel    = getval(elm, "angvel"),
			time      = getval(elm, "time"),
			delay     = getval(elm, "delay"),
		},
	};
}

function LayerHeartBeatZoomModule(elm) {
	return {
		zoom =  {
			handler = "SinAction",
			vibration = getint(elm, "zoom")-100,
			cycle     = getint(elm, "time")*2,
			time      = getval(elm, "time"),
			delay     = getval(elm, "delay"),
		}
	};
}

function LayerVibrateZoomModule(elm) {
	return {
		zoomx =  {
			handler = "RandomAction",
			vibration = getint(elm, "vibration")/2,
			waittime  = getval(elm, "waittime"),
			time      = getval(elm, "time"),
			delay     = getval(elm, "delay"),
		},
		zoomy =  {
			handler = "RandomAction",
			vibration = getint(elm, "vibration")/2,
			waittime  = getval(elm, "waittime"),
			time      = getval(elm, "time"),
			delay     = getval(elm, "delay"),
		}
	};
}

function LayerNormalRotateModule(elm) {
	local cycle = "angvel" in elm ? 360/getval(elm, "angvel")*1000
		: getval(elm, "cycle");
	return {
		rotate =  {
			handler = "TriangleAction",
			vibration = 360,
			ontime = cycle,
			offtime = 0,
			time  = getval(elm, "time"),
			delay = getval(elm, "delay"),
		}
	};
}

function LayerVibrateRotateModule(elm) {
 	return {
		rotate =  {
			handler   = "RandomAction",
			vibration = getint(elm, "vibration")/2,
			waittime  = getval(elm, "waittime"),
			time      = getval(elm, "time"),
			delay     = getval(elm, "delay"),
		}
	};
}

function LayerFalldownRotateModule(elm) {
 	return {
		rotate =  {
			handler = "MoveAction",
			value   = "@+" + getval(elm, "angle"),
			accel   = 1,
			time    = getval(elm, "time"),
			delay   = getval(elm, "delay"),
		}
	};
}

function LayerBlinkModeModule(elm) {
 	return {
		visible =  {
			handler   = "SquareAction",
			vibration = 1,
			ontime    = getval(elm, "showtime"),
			offtime   = getval(elm, "hidetime"),
			time      = getval(elm, "time"),
			delay     = getval(elm, "delay"),
		}
	};
}

function LayerFadeModeModule(elm) {
 	return {
		opacity =  {
			handler   = "TriangleAction",
			vibration = "@",
			offset    = "-@",
			ontime    = getval(elm, "intime"),
			offtime   = getval(elm, "outtime"),
			time      = getval(elm, "time"),
			delay     = getval(elm, "delay"),
		},
	};
}
