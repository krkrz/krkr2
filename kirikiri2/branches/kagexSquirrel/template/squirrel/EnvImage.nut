BOTH      <- 1;
BU        <- 2;
FACE      <- 3;
SHOW      <- 4;
CLEAR     <- 5;
INVISIBLE <- 6;

/**
 * 環境用レイヤ
 * ・座標配置調整機能を持つ
 */
class EnvGraphicLayer extends GraphicLayer {

	owner = null;

	/**
	 * コンストラクタ
	 * @param owner オブジェクト情報をもってる親
	 * @param isfore 表画面か裏画面か
	 */
	function EnvGraphicLayer(owner, isfore) {
		this.owner = owner;
		::GraphicLayer(owner.player, isfore);
	}

	_left = null;
	function getLeft() {
		return _left;
	}
	function setLeft(v) {
		_left = v;
		recalcPosition();
	}
	_top = null;
	function getTop() {
		return _top;
	}
	function setTop(v) {
		_top = v;
		recalcPosition();
	}

	function setPos(left, top, ...) {
		//dm("座標設定:" + left + "," + top);
		_left = left;
		_top  = top;
		recalcPosition();
	}
	
	/**
	 * 配置位置調整
	 */
	function recalcPosition() {
		setRealPosition(_left, _top);
	}

	function setRealPosition(left, top) {
		_setPos(left, top);
	}
};

/**
 * 環境画像の基底クラス
 */
class EnvImage extends EnvBase {

	// デフォルト情報
	init = null;

	layerOn = null; //< レイヤが有効になっている
	doShow = null;  //< 表示処理実行
	
	/// 表示状態変更
    // BOTH      バストアップ＋フェイス (標準）
    // BU        バストアップ
    // FACE      フェイス
	// SHOW      表示状態（自動選択）
	// CLEAR     消去状態 (標準)
    // INVISIBLE 非表示
	_disp = CLEAR;
	function getDisp() {
		return _disp;
	}
	function setDisp(v) {
		if (v != _disp) {
			if (v == SHOW) {
				if (isClear()) {
					if (init != null && "noPose" in init) {
						_disp = init.noPose ? FACE : BOTH;
					} else {
						_disp = BOTH;
					}
				}
			} else {
				_disp = v;
			}
			doShow = true;
			if (isShowBU()) {
				layerOn = true;
				redraw = true;
			}
			updateFlag = true;
		}
	}

	// バストアップ表示中
	function isShowBU() {
		return _disp <= BU;
    }

	// フェイス表示中
    function isShowFace() {
		return (_disp == BOTH && env.bothFace) || _disp == FACE;
	}

	// 表示されているか
	function isShow() {
        return _disp <= FACE;
	}

	// 消去状態か
	function isClear() {
		return _disp == CLEAR;
	}

	// 更新フラグ
	updateFlag = false;
	// 再描画指示フラグ
	redraw = false;

    /**
     * 時間用最初期化処理
     */
    function setRedraw() {
		if (isShowBU()) {
			redraw = true;
		}
    }
	
	// -----------------------------------------------------------------------
	// レイヤ制御
	// -----------------------------------------------------------------------

    // 種別
	_type = null;
	function getType() {
		return _type;
	}
	function setType(v) {
		_type = v;
		updateFlag = true;
    }

	// サイズ指定
	_width = null;
	function getWidth() {
		return _width;
	}
	function setWidth(v) {
		_width = v;
		updateFlag = true;
	}
	
	_height = null;
	function getHeight() {
		return _height;
	}
	function setHeight(v) {
		_height = v;
		updateFlag = true;
	}
	
	props = { zoom=100, rotate=0, opacity=255 };
	
    // アクション処理
	doStopAction = false;
	actionList = [];

	imageCommands = null;
	
    /**
     * コンストラクタ
     * @param env 環境
     */
	constructor(env, name) {
		::EnvBase.constructor(env, name);

		imageCommands = {
			type = function(param, elm) { type =  global[param]; },
			afx = setAfx,
			afy = setAfy,
			xpos = setLeft,
			ypos = setTop,
			left = setLeft,
			top = setTop,
			opacity = setOpacity,
			rotate = setRotate,
			zoom = setZoom,
			reset = setReset,
			action = setAction,
			stopaction = stopAction,
			sync = function(param) { if (param) { syncMode = true; } },
			show = function(param) { disp = SHOW; },
			hide = function(param) { disp = CLEAR; },
			accel = null,  // 移動系処理の加速指定用
			nosync = null, // シンクロしない指定
			delay = null,  // アクション delay 指定用
			nowait = null, // アクション nowait 指定用
		};
	}

	function initImage() {
		setType(null);
		props = { zoom=100, rotate=0, opacity=255 };
		setDisp(CLEAR);
		actionList.resize(0);
	}

	// -------------------------------------------------------
	// 初期化処理
	// -------------------------------------------------------

	doReset = false;

	function setReset(param, elm) {
		setType(null);
		props = { zoom=100, rotate=0, opacity=255 };
		doReset = true;
		updateFlag = true;
	}

	// -------------------------------------------------------

	/*
	 * 分割パラメータの前側を取得
	 * @param value パラメータ
	 */
	function getTo(value) {
		if (typeof value == "string") {
			local p;
			if ((p = value.find(":")) != null) {
				return value.slice(0, p);
			} else  if (p == 0) {
				return null;
			} else  {
				return value;
			}
		} else {
			return value == null ? value : value.tointeger();
		}
	}
	
	/*
	 * 分割パラメータの後側を取得
	 * @param value パラメータ
	 */
	function getFrom(value) {
		if (typeof value == "string") {
			local p;
			if ((p = value.indexOf(":")) != null) {
				return value.slice(p+1);
			} else {
				return null;
			}
		}
		return null;
	}

	/**
	 * 場所指定 相対位置指定の判定。
	 * 指定値が "%" で終わっていたら最大値への相対での指定とみなす
	 * 指定値が "@" ではじまっていたら現在値への相対の指定とみなす
	 * @param base  現在値
	 * @param value 指定値
	 * @param valueBase 指定の最大値
	 */
	function calcRelative(base, value, valueBase) {
		if (value == null) {
			return value;
		} else {
			if (typeof value == "string") {
				// 相対指定の場合
				if (value.slice(0,1) == "@") {
					value = value.slice(1);
					// %指定の場合
					if (valueBase != null && value.slice(-1) == "%") {
						value = valueBase * (value.slice(0,-1)).tointeger() / 100;
					}
					return base.tointeger() + value.tointeger();
				} else {
					// %指定の場合
					if (valueBase != null && value.slice(-1) == "%") {
						value = valueBase * (value.slice(0,-1)).tointeger() / 100;
					}
				}
			}
			return value.tointeger();
		}
	}
	
	function setPropFromTo(name, max, from, to, elm) {
		if (to != null) {
			// 初期値不定の場合は同じ値のまま処理
			if (!(name in props) && from == null) {
				from = to;
			}
			addAction({
				name = {
					handler = "MoveAction",
					start = calcRelative(props[name], from, max),
					value = calcRelative(props[name], to, max),
					time  = getval(elm, "time", 0),
					accel = getval(elm, "accel",0),
				}
			});
		}
	}

	function setProp(name, max, param, elm) {
		setPropFromTo(name, max, getFrom(param), getTo(param), elm);
	}

	function setAfx(param, elm) {
		setProp("afx", width, param, elm);
	} 

	function setAfy(param, elm) {
		setProp("afy", height, param, elm);
	} 
	
	function setLeft(param, elm) {
		setProp("left", env.xmax, param, elm);
	} 

	function setTop(param, elm) {
		setProp("top", env.ymax, param, elm);
    }

	function setOpacity(param, elm) {
		setProp("opacity", 255, param, elm);
	}

	function setRotate(param, elm) {
		setProp("rotate", 360, param, elm);
	}

	function setZoom(param, elm) {
		setProp("zoom", 100, param, elm);
	}

	/**
	 * 相対値計算 Action.tjs からのコピー
	 * @param value 値指定
	 * @param orig オリジナルの値
	 */
	function getRelative(value, orig) {
		if (typeof value == "string" && (value.indexOf("@") != null)) {
			return player.eval(replace(value, "@", orig.tostring()));
		} else {
			return value.tointeger();
		}
	}

	/**
	 * アクションの適用
	 */
	function applyMoveAction(action) {
		foreach (name, info in action) {
			if (info != null && typeof info == "table" && "handler" in info && info.handler == "MoveAction" && "value" in info) {
				//dm("アクション適用 " + name + ":" + info.value);
				props[name] <- getRelative(info.value, name in props ? props[name] : null);
			}
		}
	}

    /**
	 * アクション情報の追加
	 * @param action アクション情報
     */
	function addAction(action) {
		if (typeof action == "table") {
			actionList.append(action);
			applyMoveAction(action);
		} else if (typeof action == "array" && action.len() > 0) {
			actionList.append(action);
			for (local j=0;j<action.len();j++) {
				applyMoveAction(action[j]);
			}
		}
		updateFlag = true;
	}

    /**
     * アクションを設定
     * @param name アクション名
     * @param elm パラメータ
     */
	function setAction(name, elm) {
		// アクション情報から探す
		if (name in env.actions) {
			local info = env.actions[name];
			if (typeof info == "table") {
				local time = "time" in info ? info.time:null;
				local action = player.copyActionInfo(info, time, elm);
				addAction(action);
				return true;
			} else if (typeof info == "array") {
				//dm("配列アクション:" + name);
				local arrayAction = [];
				for (local i=0;i<info.len();i++) {
					local i = info[i];
					local time = "time" in i ? i.time:null;
					if (typeof i == "string") {
						// 文字列の場合再帰的に参照
						i = i in env.actions ? i = env.actions[i] : null;
					}
					if (i != null && typeof i == "table") {
						local action = player.copyActionInfo(i, time, elm);
						arrayAction.append(action);
					}
				}
				addAction(arrayAction);
				return true;
			}
			return false;
		}
		local action = getActionInfo(name, elm);
		if (action != null) {
			addAction(action);
			return true;
		}
		return false;
	}

    /**
	 * 全アクションを解除
     */
	function stopAction() {
		doStopAction = true;
		updateFlag = true;
	}

	// --------------------------------------------------------------

	dispMode = false;
	syncMode = false;
	prevShow = false;

	/**
	 * 状態更新処理
     */
	function updateLayer(layer) {

		//dm("updateLayer:" + name);
		if (doReset) {
			//dm("リセット処理");
			layer.reset();
		}
		if (doStopAction) {
			//dm("アクション停止");
			layer.stopAction();
		}
		if (doShow) {
			//dm("表示変更!");
			layer.visible = isShowBU();
		}
		// レイヤパラメータ反映
		if (_type != null && layer.type != type) { layer.type = type; }
		if (_width != null && _width != layer.width || _height != null && _height != layer.height()) {
			layer.setSize(_width, _height);
		}
		// アクション処理
		for (local i=0;i<actionList.len();i++) {
			layer.beginAction(actionList[i]);
		}
		// アクション終了まち
		if (syncMode) {
			ret = player.waitAction(layer, true);
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

		// 共通コマンド
		if (::EnvBase.doCommand(cmd, param, elm)) {
			return true;
        }
		
		// Image系共通コマンド
		if (cmd in imageCommands) {
			local func = imageCommands[cmd];
			if (func != null) {
				func(param, elm);
            }
			return true;
		}

		// アクション
		if (setAction(cmd, elm)) {
			return true;
		}

		// アクションパラメータの排除
		if (cmd in player.actionParam) {
			return true;
		}

		return false;
	}
	
    // このメソッドを実装する
    // function getLayer(isfore);
	// function drawLayer(layer)

	/**
	 * 新レイヤ生成
	 */
	function createLayer(isfore) {
		return EnvGraphicLayer(this, isfore);
	}

	// -------------------------------------------------------------------------------

	/**
	 * 単純更新
	 */
	function update(isfore) {
		//dm("更新処理:" + name + ":" + layerOn + ":" + updateFlag + ":" + redraw);
		if (layerOn && (updateFlag || redraw)) {
			local layer = getLayer(isfore);
			if (redraw) {
				//dm("再描画:" + name);
				layer.reset();
				if (type != null) {
					layer.type = type;
				}
				drawLayer(layer);
				foreach (name, value in props) {
					layer[name] = value;
				}
				layer.recalcPosition();
				redraw = false;
			}
			updateLayer(layer);
			updateFlag = false;
			return layer;
		}
		updateFlag = false;
	}

	/**
	 * 表示更新前のフラグ初期化
	 */
	function tagcommand(elm) {
		syncMode = false;
		layerOn = prevShow = isShowBU();
		dispMode = 0;
		doReset = false;
		doStopAction = false;
		actionList.clear();
		super.tagcommand(elm);
	}

	/**
	 * 表示処理用の自動トランジションの指定
	 */
	function setDispAutoTrans() {
	}
	
	function command(elm) {
		::EnvBase.command(elm);
		// 0:非表示 1:表示 2:消去 3:更新
		dispMode = (prevShow ? 2 : 0) + (isShowBU() ? 1 : 0);
		// 表示・消去指定に対応
		if (dispMode == 1 || dispMode == 2) {
			setDispAutoTrans();
		}
	}
	
	/**
	 * 表示更新処理特殊化
	 */
	function doUpdate(elm) {
		if ("method" in trans || dispMode == 3) {
			//dm("画像更新:" + trans.method + ":" + trans.showaction + ":" + trans.hideaction);
			player.backup();
			local layer = update(false);
			player.beginTransition(trans);
			if (layer != null) {
				player.beginTransActionUpdate(trans, layer);
			}
		} else if (dispMode == 2) {
			//dm("画像消去");
			doShow = false;
			local layer = update(true);
			if (layer != null) {
				player.beginTransAction(trans, layer, true, elm != null && getval(elm,"nosync"));
			}
		} else if (dispMode == 1) {
			//dm("画像表示");
			doShow = false;
			local layer = update(true);
			if (layer != null) {
				player.beginTransAction(trans, layer, false, elm != null && getval(elm,"nosync"));
			}
		} else {
			update(true);
		}
	}
}
