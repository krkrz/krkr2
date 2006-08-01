@if exp="typeof(global.world_object) == 'undefined'"
@iscript

/**
 * ワールド拡張
 * ◇フック一覧
 * グローバルに以下のメソッドがあった場合はフックとして呼び出します
 * setTimeHook(time, elm)     時間変更時フック
 * setStageHook(stage, elm)   ステージ変更時フック
 */
var setTimeHook;
var setStageHook;

/**
 * 空の辞書
 */
var EMPTY = %[];

/**
 * 鍵の内容を表示する
 * @param name 名称
 * @param dict 辞書オブジェクト
 */
function showKeys(name, dict) {
    name += ":";
    if (dict) {
        var array = [];
        array.assign(dict);
        for (var i=0; i<array.count; i+= 2) {
            if (i != 0) {
                name += ",";
            }
            name += array[i];
        }
        dm(name);
    }
}

/*
 * 分割パラメータの前側を取得
 * @param value パラメータ
 */
function getTo(value) {
    var p;
    if ((p = value.indexOf(":")) > 0) {
        return value.substring(0, p);
    } else  if (p == 0) {
        return void;
    } else  {
        return value;
    }
}

/*
 * 分割パラメータの後側を取得
 * @param value パラメータ
 */
function getFrom(value) {
    var p;
    if ((p = value.indexOf(":")) >= 0) {
        return value.substring(p+1);
    } else {
        return void;
    }
}

var transitionName = %[
    "universal" => true,
    "crossfade" => true,
    "scroll" => true,
    "wave" => true,
    "mosaic" => true,
    "turn" => true,
    "rotatezoom" => true,
    "rotatevanish" => true,
    "rotateswap" => true,
    "ripple" => true,
    ];

var transitionParam = %[
    "time" => true,
    "rule" => true,
    "vague" => true,
    "from" => true,
    "stay" => true,
    "wavetype" => true,
    "maxh" => true,
    "maxomega" => true,
    "bgcolor1" => true,
    "bgcolor2" => true,
    "mosaic" => true,
    "maxsize" => true,
    "turn" => true,
    "bgcolor" => true,
    "factor" => true,
    "accel" => true,
    "twistaccel" => true,
    "twist" => true,
    "centerx" => true,
    "centery" => true,
    "rwidth" => true,
    "roundness" => true,
    "speed" => true,
    "maxdrift" => true,
    "msgoff" => true,
    "transwait" => true,
    ];

var actionParam = %[
    "delay" => true,
    "x" => true,
    "y" => true,
    "topTime" => true,
    "vibration" => true,
    "waitTime" => true,
    "cycle" => true,
    "distance" => true,
    "fallTime" => true,
    "angvel" => true,
    "angle" => true,
    "showTime" => true,
    "hideTime" => true,
    "intime" => true,
    "outtime" => true,
    ];

/**
 * 環境画像の基底クラス
 */
class KAGEnvImage {

    var _ret;
    property ret {
        getter() {
            return _ret;
        }
        setter(v) {
            if (v === void) {
                _ret = 0;
            } else {
                if (v < ret) {
                    _ret = v;
                }
            }
        }
    }
    
    function isSkip() {
        return env.kag.skipMode || env.kag.noeffect;
    }

    // 各種情報を保持する環境
    var env;

    /// 表示状態
    // BOTH      バストアップ＋フェイス (標準）
    // BU        バストアップ
    // FACE      フェイス
    // CLEAR      消去状態         (標準)
    // INVISIBLE 非表示
    var _disp;

    // バストアップ表示中
    function isShowBU() {
        return _disp <= BU;
    }

    function isShowFace() {
        return _disp == BOTH || _disp == FACE;
    }

    property disp {
        getter() {
            return _disp;
        }
        setter(v) {
            if (v !== void) {
                if (v < CLEAR && (_disp >= CLEAR)) {
                    if (opacity === void) {
                        // 非表示状態から表示に切り替わるときは不透明度を制御
                        opacity = 255;
                    }
                }
                _disp = v;
                redraw = true;
            }
        }
    }

    /// 再描画指示フラグ
    var redraw;
    
    // 初期化処理
    var resetFlag;
    // フェード処理フラグ
    var fadeTime;
    // 種別
    var type;
    // 透明度
    var opacityFrom;
    var opacity;
    var opacityTime;
    // 回転
    var rotateFrom;
    var rotate;
    var rotateTime;
    // ズーム処理
    var zoomFrom;
    var zoom;
    var zoomTime;

    // 回転原点指定
    var afx;
    var afy;

    // アクション処理
    var actionList;
    var syncMode;
    
    // 画面更新設定
    var trans;

    // フェード指定のデフォルト値
    property fadeValue {
        getter() {
            return env.fadeValue;
        }
    }
    
    /**
     * コンストラクタ
     * @param env 環境
     */
    function KAGEnvImage(env) {
        this.env = env;
        resetFlag = false;
        fadeTime = void;
        actionList = new Array();
        _disp = CLEAR;
    }

    function finalize() {
        actionList.clear();
        invalidate actionList;
    }

    /**
     * アクションを設定
     * @param name アクション名
     * @param elm パラメータ
     */
    function setAction(name, elm) {
        if (env.actions !== void) {
            var info;
            if ((info = env.actions[name]) !== void) {
                var action = %[];
                (Dictionary.assign incontextof action)(info, false); 
                // 元要素からパラメータを削除しつつコピー
                foreach(elm, function(name, value, elm, action) {
                    if (actionParam[name] !== void) {
                        action[name] = value;
                        //dm("追加パラメータ:" + name + ":" + value);
                        //delete elm[name];
                    }
                }, action);
                actionList.add(action);
                return true;
            }
            if (elm !== void && name.substring(0,5) == "Layer") {
                var moduleType;
                try {
                    moduleType = global[name];
                    if (moduleType !== void && typeof moduleType == "Object" && moduleType.LayerModule != void) {
                        // アクションモジュール名の場合
                        var action = %[];
                        // 元要素からパラメータを削除しつつコピー
                        foreach(elm, function(name, value, elm, action) {
                            if (actionParam[name] !== void) {
                                action[name] = value;
                                //delete elm[name];
                            }
                        }, action);
                        action.module = name;
                        action.time = elm.time if elm.time !== void;
						if (action.time !== void) {
							action.time *= kag.drawspeed;
						}
                        actionList.add(action);
                        return true;
                    }
                } catch() {
                    // 無視
                }
            }
        }
        return false;
    }

    /**
     * トランジションを設定
     * @param name トランジション名
     */
    function setTrans(name, elm) {
        var info;
        if (env.transitions !== void && (info = env.transitions[name]) !== void) {
            // 登録済みトランジション
            var tr = %[];
            // コピー
            (Dictionary.assign incontextof tr)(info, false); 
            // パラメータのコピー
            foreach(elm, function(name, value, elm, tr) {
                if (transitionParam[name] !== void) {
                    tr[name] = value;
                    //delete elm[name];
                }
            }, tr);
            if (!env.transMode && !isSkip()) {
                trans = tr;
                redraw = true;
            }
            return true;
        }
        
        if (elm !== void && (transitionName[name] !== void ||
                             name.substring(0,5) == "trans")) {
            // 規定のトランジション
            var tr = %[];
            // パラメータのコピー
            foreach(elm, function(name, value, elm, tr) {
                if (transitionParam[name] !== void) {
                    tr[name] = value;
                    //delete elm[name];
                }
            }, tr);
            tr.method = name;
            if (!env.transMode && !isSkip()) {
                trans = tr;
                redraw = true;
            }
            return true;
        }
        return false;
    }

    /**
     * トランジションを設定
     * @param param トランジション指定　文字列または辞書
     * @return 設定した場合ｈ
     */
    function setTrans2(param) {
        if (param === void) {
            return false;
        } else if (typeof param == "String") {
            setTrans(param);
            return true;
        } else if (param instanceof "Dictionary") {
            setTrans(param.method, param);
            return true;
        }
        return false;
    }
    
    /**
     * 状態更新処理
     */
    function updateLayer(layer) {
        if (resetFlag) {
            type    = void;
            opacity = void;
            rotate  = void;
            zoom    = void;
            afx     = void;
            afy     = void;
            actionList.clear();
            layer.reset();
            resetFlag = false;
            fadeTime = void;
        } else {
            if (redraw) {
                if (isShowBU()) {
                    if (fadeTime !== void) {
                        opacityFrom = 0;
                        opacity     = 255 if opacity === void;
                        opacityTime = fadeTime > 1 ? fadeTime : fadeValue;
                    }
                    layer.visible = true;
                } else if (disp == FACE) {
                    layer.visible = false;
                } else {
                    // 消す
                    if (fadeTime !== void) {
                        opacityFrom = void;
                        opacity     = 0 if opacity === void;
                        opacityTime = fadeTime > 1 ? fadeTime : fadeValue;
                    } else {
                        layer.visible = false;
                    }
                }
            }
            fadeTime = void;

            if (afx !== void) {
                layer.afx = afx;
                afx = void;
            }
            if (afy !== void) {
                layer.afy = afy;
                afy = void;
            }
            if (opacity !== void) {
                //dm("透明度変更:" + opacity + ":" + opacityTime); 
                if (opacityFrom !== void) {
                    layer.opacity = opacityFrom;
					opacityFrom = void;
                }
                layer.setOpacityTime(opacity, opacityTime);
                opacity = void;
            }
            if (rotate !== void) {
                //dm("回転変更:" + rotate + ":" + rotateTime);
                if (rotateFrom !== void) {
                    layer.rotate = rotateFrom;
					rotateFrom = void;
                }
                layer.setRotateTime(rotate, rotateTime);
                rotate = void;
            }
            if (zoom !== void) {
                //dm("ズーム変更:" + zoom + ":" + zoomTime);
                if (zoomFrom !== void) {
                    layer.zoom = zoomFrom;
					zoomFrom = void;
                }
                layer.setZoomTime(zoom, zoomTime);
                zoom = void;
            }
            if (type !== void) {
                //dm("合成種別変更");
                layer.type = type;
                type = void;
            }
            // アクション処理
            for (var i=0;i<actionList.count;i++) {
                layer.beginAction(actionList[i]);
            }
            actionList.clear();

            // スキップ中は即効でアクションを終了させる
            if (isSkip()) {
                layer.stopAction();
            }
        }
        if (syncMode) {
			dm("アクション待ち");
			if (ret == 0) {
				ret = kag._waitLayerAction(layer);
			}
            syncMode = false;
        }
    }

    var commands = %[
    tagname : null, 
    time : null,
    type : function(param, elm) {
        type =  global[param];
    } incontextof this,
    opacity : function(param, elm) {
        opacityFrom = getFrom(param);
        opacity     = getTo(param);
        opacityTime = isSkip() ? 0 : elm.time;
    } incontextof this,
    fade :  function(param, elm) {
        fadeTime = isSkip() ? void : param;
    } incontextof this,
    rotate : function(param, elm) {
        rotateFrom = getFrom(param);
        rotate     = getTo(param);
        rotateTime = isSkip() ? 0 : elm.time;
    } incontextof this,
    zoom : function(param, elm) {
        zoomFrom = getFrom(param);
        zoom     = getTo(param);
        zoomTime = isSkip() ? 0 : elm.time;
    } incontextof this,
    afx : function(param, elm) {
        afx = param;
    } incontextof this,
    afy : function(param, elm) {
        afy = param;
    } incontextof this,
    reset : function(param, elm) {
        resetFlag = true;
    } incontextof this,
    trans  : function(param, elm) {
        if (param == "void") {
            trans = void;
        } else {
            setTrans(param, elm);
        }
    } incontextof this,
    action : setAction incontextof this,

    sync : function(param) { if (param) { syncMode = true; } } incontextof this,

    show : function(param) { disp = BOTH;   } incontextof this,
    hide : function(param) { disp = CLEAR; } incontextof this,
    visible : function(param) { disp = param ? BOTH : CLEAR; }  incontextof this,
        ];

    /**
     * コマンドの実行
     * @param cmd コマンド
     * @param param パラメータ
     * @param elm 他のコマンドも含む全パラメータ
     * @return 実行が行われた場合 true
     */
    function doCommand(cmd, param, elm) {
        //dm("コマンド処理:" + cmd + " パラメータ:" + param);
        var func;
        if ((func = commands[cmd]) !== void) {
            if (func != null) {
                func(param, elm);
            }
            return true;
        }
        // 後で考えたほうがいいかも？
        if (actionParam[cmd] !== void) {
            return true;
        }
        if (transitionParam[cmd] !== void) {
            return true;
        }
        return false;
    }

    /**
     * セーブ処理
     */
    function onStore(f) {
        f.resetFlag = resetFlag;
        f.type = type;
        f.opacityFrom = opacityFrom;
        f.opacity     = opacity;
        f.opacityTime = opacityTime;
        f.rotateFrom  = rotateFrom;
        f.rotate      = rotate;
        f.rotateTime  = rotateTime;
        f.zoomFrom    = zoomFrom;
        f.zoom        = zoom;
        f.zoomTime    = zoomTime;
        f.actionList  = new Array();
        f.actionList.assign(actionList);
        f.disp = disp;
    }

    /**
     * ロード処理
     */
    function onRestore(f) {
        resetFlag = f.resetFlag;
        type = f.type;
        opacityFrom = f.opacityFrom;
        opacity     = f.opacity;
        opacityTime = f.opacityTime;
        rotateFrom  = f.rotateFrom;
        rotate      = f.rotate;
        rotateTime  = f.rotateTime;
        zoomFrom    = f.zoomFrom;
        zoom        = f.zoom;
        zoomTime    = f.zoomTime;
        actionList.clear();
        if (f.actionList) {
            actionList.assign(f.actionList);
        }
        disp = f.disp;
    }

    // このメソッドを実装する
    // function getLayer(base);
    // function drawLayer(layer);
    // 
    function calcPosition(layer) {
    }

    // トランジション実行
    function beginTransition(trans) {
        kag.fore.base.beginTransition(trans);
        if (trans.transwait !== void) {
            ret = kag.waitTime((int)trans.time + (int)trans.transwait, kag.clickSkipEnabled);
        } else {
            ret = kag.waitTransition(EMPTY);
        }
    }

    function hideMessage() {
        if (trans !== void && trans.msgoff) {
            kag.invisibleCurrentMessageLayer();
        }
    }
    
    /**
     * 画像の描画
     */
    function updateImage(base) {
        kag.updateBeforeCh = 1;
        // 描画更新が必要な場合
        if (redraw) {

            if (base === void && trans) {

                kag.fore.base.stopTransition();
                
                // 更新処理を走らせる場合
                if (trans.time === void) {
                    trans.time = 1000;
                }
                trans.children = true;
    
                // 全レイヤをバックアップ
                kag.backupLayer(%[], true);

                // 裏レイヤが対象
                var layer = getLayer(kag.back);
                if (isShowBU()) {
                    drawLayer(layer);
                    calcPosition(layer);
                }
                updateLayer(layer);
                beginTransition(trans);
                
            } else {

                // 表に描画
                var layer = getLayer(base);

                // フェード判定
                // 既に表示されてるときはトランジションで代用
                if (fadeTime !== void && isShowBU() && layer.visible && layer.opacity > 0) {

                    kag.fore.base.stopTransition();
                    var trans = %[ "method" => "crossfade",
                                   "children" => true,
                                   "time" => fadeTime > 1 ? fadeTime : fadeValue];
                    fadeTime = void;

                    // 全レイヤを裏にバックアップ
                    kag.backupLayer(%[], true);

                    // 裏レイヤが対象
                    layer = getLayer(kag.back);
                    drawLayer(layer);
                    calcPosition(layer);
                    updateLayer(layer);
                    beginTransition(trans);

                } else {
                    // フェード処理
                    //dm("フェードを opacity 処理で実現");
                    if (isShowBU()) {
                        drawLayer(layer);
                        calcPosition(layer);
                    }
                    updateLayer(layer);
                }
            }

            redraw = false;
        } else {
            var layer = getLayer(base);
            calcPosition(layer);
            updateLayer(layer);
        }
        trans = void;
    }
}

/**
 * レイヤ系基底クラス
 */
class KAGEnvLayer extends KAGEnvImage {

    // kag の参照
    property kag {
        getter() {
            return env.kag;
        }
    }

    /// 描画中画像
    var imageFile;
	var xoff;
	var yoff;

    function KAGEnvLayer(env) {
        super.KAGEnvImage(env);
        redraw = false;
    }

    function onStore(f) {
        f.imageFile = imageFile;
        f.xoff = xoff;
        f.yoff = yoff;
        super.onStore(f);
    }
    
    function onRestore(f) {
        imageFile = f.imageFile;
        xoff = f.xoff;
        yoff = f.yoff;
        super.onRestore(f);
    }

    var _layerCommands = %[
    file : function(param, elm) {
        imageFile = param;
        disp = BOTH;
    } incontextof this,
    xoff : function(param, elm) {
        xoff = param;
    } incontextof this,
    yoff : function(param, elm) {
        yoff = param;
    } incontextof this
        ];

    /**
     * コマンドの実行
     * @param cmd コマンド
     * @param param パラメータ
     * @param elm 他のコマンドも含む全パラメータ
     * @return 実行が行われた場合 true
     */
    function doCommand(cmd, param, elm) {

        // 共通コマンド
        if (super.doCommand(cmd, param, elm)) {
            return true;
        }
        // レイヤ共通コマンド
        var func;
        if ((func = _layerCommands[cmd]) !== void) {
            if (func != null) {
                func(param, elm);
            }
            return true;
        }
        // レイヤ拡張コマンド用
        if ((func = layerCommands[cmd]) !== void) {
            if (func != null) {
                func(param, elm);
            }
            return true;
        }

        var find = false;
        
        // アクション
        if (!find) {
            find = setAction(cmd, elm);
        }

        // 更新
        if (!find) {
            find = setTrans(cmd, elm);
        }

        // 画像のロード
        if (!find) {
            imageFile = cmd;
            disp = BOTH;
        }
        return find;
    }

    // このメソッドを実装する
    // function getLayer(base);
    // 
    function drawLayer(layer) {
        if (imageFile !== void) {
            layer.loadImages(%[ "storage" => imageFile]);
            // 座標補正
            if (xoff !== void || yoff== void) {
                layer.left = xoff if xoff !== void;
                layer.top  = yoff if yoff !== void;
            }
        }
    }

    /**
     * KAG タグ処理
     * @param elm 引数
     */
    function tagfunc(elm) {
        //dm("レイヤタグがよばれたよ");
        ret = void;
        foreach(elm, doCommand);
        hideMessage();
        updateImage();
        return ret;
    }
}

/**
 * ベースになっているレイヤ
 * event レイヤの参照に用いる
 */
class KAGEnvBaseLayer extends KAGEnvLayer {

    var name;

	var _imageFile;
	property imageFile {
		setter(v) {
            if (v !== void) {
                // 記録
                kag.sflags["cg_" + (v.toUpperCase())] = true;

                var eventInfo;
                if (env.events !== void) {
                    eventInfo = env.events[v];
                }
                var eventTrans;
                if (eventInfo !== void) {
                    eventTrans = eventInfo.trans;
                    _imageFile = eventInfo.image !== void ? eventInfo.image : v;
                    xoff = eventInfo.xoff;
                    yoff = eventInfo.yoff;
                } else {
                    _imageFile = v;
                    xoff = 0;
                    yoff = 0;
                }
                //dm("画像指定:" + _imageFile);
                
                // トランジション指定
                if (!setTrans2(eventTrans)) {
                    if (!setTrans2(env.envinfo.eventTrans)) {
                        setTrans2(env.envinfo.envTrans);
                    }
                }

            } else {
                _imageFile = void;
                xoff = 0;
                yoff = 0;
            }
        }
		getter() {
			return _imageFile;
		}
	}

    /**
     * コンストラクタ
     * @param env 環境
     * @param name 環境のレイヤ名
     */
    function KAGEnvBaseLayer(env, name) {
        super.KAGEnvLayer(env);
        this.name = name;
    }

    function getLayer(base) {
        if (base === void) {
            base = env.transMode ? kag.back : kag.fore;
        }
        return base[name];
    }

    var layerCommands = %[];
}

/**
 * KAG 前景レイヤのレベル制御用
 */
class KAGEnvLevelLayer {

    // 利用レイヤ
    var layerId;
    /// 表示レベル
    var level;
	/// 表示絶対レベル
	var absolute;

    // 表示位置座標
    var xpos;
    var ypos;
    var xposFrom;
    var yposFrom;
    var moveTime;
    var moveAccel;
    
    /// 位置変更
    var reposition;
    var front;
    var back;

    /**
     * コンストラクタ
     * @param layerId 前景レイヤID
     */
    function KAGEnvLevelLayer(layerId) {
        this.layerId = layerId;
        xpos = 0;
        ypos = 0;
        xposFrom = void;
        yposFrom = void;
        moveTime = void;
        moveAccel = void;
        level = void;
        reposition = true;
    }

    function finalize() {
        // 使っていたレイヤを消去
        var id = env.initLayerCount + layerId;
        if (id < kag.fore.layers.count) {
            kag.fore.layers[id].clearImage();
            kag.back.layers[id].clearImage();
        }
    }

    /**
     * レイヤ情報の取得
     * @param base fore / back      
     * 同時にレベルに応じた front/back の処理も実行する
     */
    function getLayer(base) {
        if (base === void) {
            base = env.transMode ? kag.back : kag.fore;
        }
        if (level === void) {
            level = env.defaultLevel;
        }
        var id = env.initLayerCount + layerId;
        kag.toLevel(id, level, base);
        if (front) {
            kag.toFront(id, base);
        } else if (back) {
            kag.toBack(id, base);
        }
        front = back = void;
		absolute = base.layers[id].absolute;
        return base.layers[id];
    }

    /**
     * レベル内最前列
     */
    function setFront() {
        front = true;
    }

    /**
     * レベル内最後列
     */
    function setBack() {
        back = true;
    }

    /**
     * レベルの指定
     */
    function setLevel(cmd, elm) {
        level = cmd;
		absolute = void;
    } 

    function setXPos(cmd, elm) {
		//dm("X位置指定:" + cmd + ":" + elm.time);
        xposFrom = getFrom(cmd);
        xpos     = getTo(cmd);
        if (xposFrom !== void && moveTime === void) {
            moveTime  = elm.time;
            moveAccel = elm.accel;
        }
        reposition = true;
    } 

    function setYPos(cmd, elm) {
        yposFrom = getFrom(cmd);
        ypos     = getTo(cmd);
        if (yposFrom !== void && moveTime === void) {
            moveTime  = elm.time;
            moveAccel = elm.accel;
        }
        reposition = true;
    }
    
    function onStore(f) {
        f.layerId = layerId;
        f.level = level;
		f.absolute = absolute;
        f.xpos = xpos;
        f.xposFrom = xposFrom;
        f.ypos = ypos;
        f.yposFrom = yposFrom;
        f.moveTime = moveTime;
        f.moveAccel = moveAccel;
    }

    function onRestore(f) {
        layerId = f.layerId;
        level = f.level;
		absolute = f.absolute;
        xpos = f.xpos;
        xposFrom = f.xposFrom;
        ypos = f.ypos;
        yposFrom = f.yposFrom;
        moveTime = f.moveTime;
        moveAccel = f.moveAccel;
		// レベルの復帰
		var layer = kag.fore.layers[env.initLayerCount + layerId];
		if (layer !== void) {
			if (level !== void) {
				layer.level = level;
			}
			if (absolute !== void) {
				layer.absolute = absolute;
			}
	    }
	}
}

/**
 * 単独レイヤ
 */
class KAGEnvSimpleLayer extends KAGEnvLevelLayer, KAGEnvLayer {

    // 名前
    var name;
    
    /**
     * コンストラクタ
     * @param env
     * @param layerId
     * @param name
     * @param level
     */
    function KAGEnvSimpleLayer(env, layerId, name) {
        global.KAGEnvLayer.KAGEnvLayer(env);
        global.KAGEnvLevelLayer.KAGEnvLevelLayer(layerId);
        this.name = name;
    }

    function finalize() {
        // 順番変更不可
        global.KAGEnvLevelLayer.finalize();
        global.KAGEnvLayer.finalize();
    }
    
    function calcPosition(layer) {
        if (reposition) {
            var l = kag.scWidth / 2 + (int)xpos - layer.imageWidth / 2;
            var t = kag.scHeight/ 2 + (int)ypos - layer.imageHeight / 2;
            if (moveTime !== void && moveTime > 0) {
                if (xposFrom !== void || yposFrom !== void) {
                    var fl = xposFrom !== void ? kag.scWidth  / 2 + (int)xposFrom - layer.imageWidth / 2 : l;
                    var ft = yposFrom !== void ? kag.scHeight / 2 + (int)yposFrom - layer.imageHeight / 2 : t;
                    layer.setPos(fl, ft);
                }
                layer.setMove(l, t, moveAccel, moveTime);
            } else {
                layer.setMove(l, t);
            }
            xposFrom = void;
            yposFrom = void;
	        moveTime = void;
            reposition = false;
        }
    }
    
    function onStore(f) {
        global.KAGEnvLayer.onStore(f);
        global.KAGEnvLevelLayer.onStore(f);
        f.name = name;
    }

    function onRestore(f) {
        name = name;
        global.KAGEnvLevelLayer.onRestore(f);
        global.KAGEnvLayer.onRestore(f);
    }

    var layerCommands = %[
    xpos : this.setXPos incontextof this,
    ypos : this.setYPos incontextof this,
    front : this.setFront incontextof this,
    back : this.setBack incontextof this,
    level : this.setLevel incontextof this,
    accel : null, // 無視
        ];
}

class VoiceTrack extends WaveSoundBuffer {

    var owner;
    
    function VoiceTrack(owner) {
        this.owner = owner;
        super.WaveSoundBuffer(owner);
    }

    function play() {
        owner.onStartVoice();
        super.play(...);
    }

    var prevstatus = "unload"; // 直前のステータス

    function onStatusChanged() {
        super.onStatusChanged(...);
        var ps = prevstatus;
        var cs = status;
        prevstatus = cs;
        if(ps == "play" && cs == "stop") {
            owner.onStopVoice();
        }
    }
}

/**
 * キャラクタプラグイン
 */
class KAGEnvCharacter extends KAGEnvLevelLayer, KAGEnvImage {

    // kag の参照
    property kag {
        getter() {
            return env.kag;
        }
    }

    // フェード指定のデフォルト
    property fadeValue {
        getter() {
            if (init !== void && init.fadeValue !== void) {
                return init.fadeValue;
            } else {
                if (env.envinfo !== void && env.envinfo.charFadeValue !== void) {
                    return env.envinfo.charFadeValue;
                } else {
                    return env.fadeValue;
                }
            }
        }
    }

    /// 名前
    var name;
    /// 初期化名
    var initName;

    /// 初期化情報
    var init;
    var poses;

    /// ポーズ
    var _pose;
    property pose {
        getter() {
            if (_pose !== void) {
                return _pose;
            }
            if (init.defaultPose !== void) {
                return init.defaultPose;
            }
            return "通常";
        }
        setter(v) {
            _pose = v;
        }
    };

    /// 服装
    var dress;
    /// 表情
    var face;

    /// ボイス情報
    var voice;
	var strVoice;

    // ベース画像名
    var baseImageName;
    // ベース画像
    var baseImage;
    // 顔画像
    var faceImage;

    // エモーション
    var newEmotionFlag;
    var emotion;
    var emotionLayer;

    /**
     * コンストラクタ
     * @param env 環境オブジェクト
     * @param layerId 描画対象レイヤ番号
     * @param name キャラクタ名
     * @param init キャラクタ初期化データ
     */
    function KAGEnvCharacter(env, layerId, name, initName, init) {

        global.KAGEnvImage.KAGEnvImage(env);
        global.KAGEnvLevelLayer.KAGEnvLevelLayer(layerId);
        
        //dm("キャラクタ登録:" + name + " layerId:" + layerId); 

        this.name     = name;
        this.initName = initName;
        this.init     = init;
        if (init) {
            poses = init.poses; //showKeys("poses", poses);
        }
    }
    
    function finalize() {
        if (baseImage !== void) {
            invalidate baseImage;
        }
        if (faceImage !== void) {
            invalidate faceImage;
        }
        if (soundBuffer !== void) {
            invalidate soundBuffer;
        }
        if (emotionLayer !== void) {
            invalidate emotionLayer;
        }
        // 順番変更不可注意
        global.KAGEnvLevelLayer.finalize();
        global.KAGEnvImage.finalize();
    }

    /**
     * セーブ処理
     */
    function onStore(f) {
        global.KAGEnvImage.onStore(f);
        global.KAGEnvLevelLayer.onStore(f);
        f.name = name;
        f.initName = initName;
        f.pose = pose;
        f.dress = dress;
        f.face = face;
        f.voice = voice;
		f.strVoice = strVoice;
    }

    /**
     * ロード処理
     */
    function onRestore(f) {
        pose  = f.pose;
        dress = f.dress;
        face  = f.face;
        voice = f.voice;
		strVoice = f.strVoice;
        global.KAGEnvLevelLayer.onRestore(f);
        global.KAGEnvImage.onRestore(f);
    }

    /**
     * ポーズの設定
     */
    function setPose(poseName) {
        var info = poses[poseName];
        if (info !== void) {
            if (poseName != pose || disp == CLEAR) {
                pose = poseName;
                if (disp == CLEAR) {
                    disp = BOTH;
                }
                redraw = true;

                // トランジション指定
                if (isShowBU()) {
                    if (!setTrans2(info.trans)) {
                        if (!setTrans2(init.poseTrans)) {
                            if (!setTrans2(init.charTrans)) {
                                if (!setTrans2(env.envinfo.poseTrans)) {
                                    setTrans2(env.envinfo.charTrans);
                                }
                            }
                        }
                    }
                }

            }
            // 服装初期化処理
            if (dress !== void && (info.dresses == void || info.dresses[dress] == void)) {
                dress = void;
            }
            // 表情初期化処理
            if (face !== void && (info.faces == void || info.faces[face] == void)) {
                face = void;
            }
        }
        // ypos はポーズ設定したときに0に戻す
        ypos = 0;
        // エモーションクリア
        setEmotion();
    }

    /**
     * 服装の設定
     */
    function setDress(dressName) {
        if (dressName != dress || disp == CLEAR) {
            dress = dressName;
            if (disp == CLEAR) {
                disp = BOTH;
            }
            redraw = true;
            // トランジション指定
            if (isShowBU()) {
                if (!setTrans2(init.dressTrans)) {
                    if (!setTrans2(init.charTrans)) {
                        if (!setTrans2(env.envinfo.dressTrans)) {
                            setTrans2(env.envinfo.charTrans);
                        }
                    }
                }
            }
        }
    }

    /**
     * 表情の設定
      */
    function setFace(faceName) {
        if (faceName != face || disp == CLEAR) {
            face = faceName;
            if (disp == CLEAR) {
                disp = BOTH;
            }
            redraw = true;
            // トランジション指定
            if (isShowBU()) {
                if (!setTrans2(init.faceTrans)) {
                    if (!setTrans2(init.charTrans)) {
                        if (!setTrans2(env.envinfo.faceTrans)) {
                            setTrans2(env.envinfo.charTrans);
                        }
                    }
                }
            }
        }
    }

    function setPositionTrans(info) {
        // トランジション指定
        if (isShowBU()) {
            if (!setTrans2(info.trans)) {
                setTrans2(env.envinfo.positionTrans);
            }
        }
    }
    
    /**
     * 表示位置の設定
     */
    function setPosition(cmd, elm) {

        var posName = getTo(cmd);
        var info;
        if (env.positions === void || (info = env.positions[posName]) === void) {
            return;
        }
        
        switch (info.type) {
        case global.KAGEnvironment.XPOSITION:
            moveTime = isSkip() ? 0 : elm.time;
            moveAccel = (elm.accel === void) ? 0 : +elm.accel;
            var posFrom = getFrom(cmd);
            var fromInfo;
            if (posFrom !== void && (fromInfo = env.positions[posFrom]) !== void) {
                xpos       = info.xpos;
                xposFrom   = fromInfo.xpos;
                reposition = true;
            } else {
                xpos = info.xpos;
                reposition = true;
                setPositionTrans(info);
            }
            if (disp == CLEAR) {
                disp = BOTH;
            }
            break;
        case global.KAGEnvironment.YPOSITION:
            moveTime = isSkip() ? 0 : elm.time;
            moveAccel = (elm.accel === void) ? 0 : +elm.accel;
            var posFrom = getFrom(cmd);
            var fromInfo;
            if (posFrom !== void && (fromInfo = env.positions[posFrom]) !== void) {
                ypos       = info.ypos;
                yposFrom   = fromInfo.ypos;
                reposition = true;
            } else {
                ypos = info.ypos;
                reposition = true;
                setPositionTrans(info);
            }
            if (!visible) {
                visible = true;
            }
            break;
        case global.KAGEnvironment.DISPPOSITION:
            disp = info.disp;
            redraw = true;
            setPositionTrans(info);
            break;
        case global.KAGEnvironment.LEVEL:
            //dm("レベル:" + info.level);
            level = info.level;
            redraw = true;
            reposition = true;
            if (disp > CLEAR) {
                redraw = true;
            }
            setPositionTrans(info);
            break;
        }
    }

    /**
     * エモーションの設定
     */
    function setEmotion(name, elm) {
        if (name !== void) {
            if (env.emotions !== void) {
                var info;
                if ((info = env.emotions[name]) !== void) {
                    emotion = %[];
                    (Dictionary.assign incontextof emotion)(info, false);
                    newEmotionFlag = true;
                    emotion.time = elm.time if elm.time !== void;
                    return true;
                }
            }
            return false;
        } else {
            // エモーション解除
            newEmotionFlag = false;
            if (emotion !== void) {
                invalidate emotion;
                emotion = void;
            }
            if (emotionLayer !== void) {
                invalidate emotionLayer;
                emotionLayer = void;
            }
        }
    }
    
    var charCommands = %[
    pose    : this.setPose incontextof this,
    dress   : this.setDress incontextof this,
    face    : this.setFace incontextof this,
    pos     : this.setPosition incontextof this,
    emotion : this.setEmotion incontextof this,
    xpos    : this.setXPos incontextof this,
    ypos    : this.setYPos incontextof this,
    front   : this.setFront incontextof this,
    back    : this.setBack incontextof this,
    level   : this.setLevel incontextof this,
    voice   : this.setVoice incontextof this,
    clearVoice : this.clearVoice incontextof this,
    playvoice : this.playVoice2 incontextof this,
    waitvoice : this.waitVoice incontextof this,
    facewin : function(param) { disp = FACE;   } incontextof this,
        ];

    /**
     * コマンドの実行
     * @param cmd コマンド
     * @param param パラメータ
     * @param elm 他のコマンドも含む全パラメータ
     * @return 実行が行われた場合 true
     */
    function doCommand(cmd, param, elm) {

        if (global.KAGEnvImage.doCommand(cmd, param, elm)) {
            return true;
        }

        var func;
        if ((func = charCommands[cmd]) !== void) {
            if (func != null) {
                func(param, elm);
            }
            return true;
        }
        
        var info;
        var find = false;
        if (poses !== void) {
            if (poses[cmd] !== void) {
                find = true;
                setPose(cmd);
            } else {
                var poseInfo;
                if ((poseInfo = poses[pose]) !== void) {
                    var dresses       = poseInfo.dresses;
                    var faces         = poseInfo.faces;
                    if (dresses !== void && dresses[cmd] !== void) {
                        //dm("服装を設定");
                        find = true;
                        setDress(cmd);
                    } else if (faces !== void && faces[cmd] !== void) {
                        //dm("表情を設定");
                        find = true;
                        setFace(cmd);
                    }
                } else {
                    dm("ポーズ情報がありません:" + pose + ":" + cmd);
                }
            }
        }

        var posName = getTo(cmd);
        if (!find && env.positions !== void && env.positions[posName] !== void) {
            setPosition(cmd, elm);
            find = true;
        }            

        // アクション
        if (!find) {
            find = setAction(cmd, elm);
        }

        // エモーション
        if (!find) {
            find = setEmotion(cmd, elm);
        }
        
        // 更新
        if (!find) {
            find = setTrans(cmd, elm);
        }
        
        return find;
    };

    /**
     * 立ち絵の描画
     * @param layer 描画対象レイヤ
     * @param levelName レベル名
     * @param pose ポーズ指定
     * @oaram face 顔描画を示すフラグ
     * @return 成功したら true
     */
	function _drawLayerPose(layer, levelName, pose) {

        var imageName;
        var poseInfo;
        if (poses !== void && (poseInfo = poses[pose]) !== void) {

            // 顔描画の場合は msgImage / msgFaceImage を優先
            var imageName     = (levelName == env.faceLevelName && poseInfo.msgImage !== void)     ? poseInfo.msgImage : poseInfo.image;
            var faceImageName = (levelName == env.faceLevelName && poseInfo.msgFaceImage !== void) ? poseInfo.msgFaceImage : poseInfo.faceImage;
            var dresses       = poseInfo.dresses;
            var faces         = poseInfo.faces;

            imageName = imageName.replace(/LEVEL/, levelName);

            // 服装指定が無い場合はデフォルトを参照
            var dressName;
            if (dress !== void) {
                dressName = dresses[dress];
            } 
            dressName = poseInfo.defaultDress if dressName === void;

            // 表情指定が無い場合はデフォルトを参照
            var faceName;
            if (face !== void) {
                faceName = faces[face];
            }
            faceName = poseInfo.defaultFace if faceName === void;

            //dm("dress:", dressName, "face:", faceName);
            
            if (faceImageName !== void) {
                
                // 顔分離型立ち絵

                // ベース部分
                var imageFile = imageName.replace(/DRESS/, dressName);

                // ベース画像のロード
                if (baseImageName != imageFile) {
                    baseImageName = imageFile;
                    // 画像ベースのキャッシュ用
                    if (baseImage === void) {
                        baseImage = new global.Layer(kag, kag.fore.base);
                        baseImage.name = "立ち絵画像キャッシュ:" + name;
                    }
					try {
                        baseImage.loadImages(imageFile);
                    } catch (e) {
                        dm("立ち絵ベース画像の読み込みに失敗しました:" + imageFile);
                        if (dressName !== poseInfo.defaultDress) {
                            dm("デフォルトの服装を試用します:" + poseInfo.defaultDress);
                            dressName = poseInfo.defaultDress;
                            imageFile = imageName.replace(/DRESS/, dressName);
                            baseImageName = imageFile;
                            try {
                                baseImage.loadImages(imageFile);
                            } catch (e) {
                                dm("立ち絵ベース画像の読み込みに失敗しました:" + imageFile);
                                return false;
                            }
                        } else {
                            return false;
                        }
					} 
                }
                
                // 画像をレイヤに割り当てる
                layer.assignImages(baseImage);
                // 初期化処理XXX ちょっと再検討必要かも
				layer.type    = layer._initTyoe    = baseImage.type;
                layer.opacity = layer._initOpacity = baseImage.opacity;

                // レベル用調整処理
                faceImageName = faceImageName.replace(/LEVEL/, levelName);
                
                //　表情指定
                if (faceName !== void) {
                    imageFile = faceImageName.replace(/DRESS/, dressName);
                    imageFile = imageFile.replace(/FACE/, faceName);
                    var imageInfo;
                    if (faceImage === void) {
                        faceImage = new global.Layer(kag, kag.fore.base);
                        faceImage.name = "立ち絵顔画像処理用:" + name;
                    }
                    try {
                        imageInfo = faceImage.loadImages(imageFile);
                    } catch (e) {
                        var succeeded = false;
                        // 服装のデフォルトおとしこみはエラー扱いではない
                        if (dressName != poseInfo.defaultDress) {
                            dressName = poseInfo.defaultDress;
                            imageFile = faceImageName.replace(/DRESS/, dressName);
                            imageFile = imageFile.replace(/FACE/, faceName);
                            try {
                                imageInfo = faceImage.loadImages(imageFile);
                                succeeded = true;
                            } catch (e) {
                            }
                        }
                        if (!succeeded) {
                            dm("表情画像の読み込みに失敗しました:" + imageFile);
                            if (faceName != poseInfo.defaultFace) {
                                dm("デフォルトの表情を試用します:" + poseInfo.defaultFace);
                                faceName = poseInfo.defaultFace;
                                imageFile = faceImageName.replace(/DRESS/, dressName);
                                imageFile = imageFile.replace(/FACE/, faceName);
                                try {
                                    imageInfo = faceImage.loadImages(imageFile);
                                } catch (e) {
                                    dm("表情画像の読み込みに失敗しました:" + imageFile);
                                }
                            }
                        }
                    }
                    if (imageInfo && imageInfo.offs_x !== void) {
                        layer.operateRect(imageInfo.offs_x, imageInfo.offs_y,
                                          faceImage, 0, 0, faceImage.imageWidth, faceImage.imageHeight);
                    } else {
                        layer.operateRect(0,0,faceImage,0,0,faceImage.imageWidth, faceImage.imageHeight);
                    }
                }
                return true;
                
            } else {
                // 顔合成型立ち絵
                // 服装情報で上書き
                if (dressName !== void) {
                    imageName = imageName.replace(/DRESS/, dressName);
                }
                // 表情で上書き
                if (faceName !== void) {
                    imageName = imageName.replace(/FACE/, faceName);
                }
                //dm("imageName", imageName);
                try {
                    if (layer instanceof "AnimationLayer") {
                        layer.loadImages(%[ "storage" => imageName]);
                    } else {
                        layer.loadImages(imageName);
                    }
                    return true;
                } catch (e) {
                    dm("ERROR:画像ファイルの読み込みに失敗しました:" + imageName);
                }
            }
        } else {
            dm("立ち絵情報がありません:" + pose);
        }
        return false;
    }

    /**
     * 現在の立ち絵の描画
     * @param layer 描画対象レイヤ
     * @param levelName レベル名
     */
    function _drawLayer(layer, levelName) {

        if (!_drawLayerPose(layer, levelName, pose)) {
            dm("立ち絵がロードできませんでした:" + pose);
            if (pose !== init.defaultPose) {
                dm("デフォルトのポーズを試用します:" + init.defaultPose);
                if (!_drawLayerPose(layer, levelName, init.defaultPose)) {
                    dm("立ち絵がロードできませんでした:" + init.defaultPose);
                    return;
                }
            } else {
                return;
            }
        }

        // サイズ補正
        layer.setSizeToImageSize();
        
        // 色補正処理
        var timeInfo;
        if ((timeInfo = env.currentTime) !== void) {
            // レイヤ合成
            if (timeInfo.charLightColor !== void) {
                layer.holdAlpha = true;
                layer.fillOperateRect(0,0,
                                      layer.width,layer.height,
                                      timeInfo.charLightColor,
                                      timeInfo.charLightType);
            }
            // 明度補正
            if (timeInfo.charBrightness !== void) {
                layer.light(timeInfo.charBrightness,
                            timeInfo.charContrast);
            }
        }
    }

    /**
     * 現在の立ち絵の描画
     * @param layer 描画対象レイヤ
     */
    function drawLayer(layer) {
        var levelName;
        var levelInfo = env.levels[level];
        if (levelInfo !== void) {
            levelName = levelInfo.name;
        }
        if (levelName === void) {
            levelName = level;
        }
        var ret = _drawLayer(layer, levelName);
        return ret;
    }
    
    /**
     * 表情を描画する
     */
    function drawFace(layer, faceLevelName) {
        //dm("表情描画:" + name);
        if (faceLevelName !== void && poses !== void) {
            _drawLayer(layer, faceLevelName);
            layer.visible = true;
        } else {
            env.clearFace();
        }
    }
    
    /**
     * レイヤ配置処理
     * @param layer 処理対象レイヤ
     */
    function calcPosition(layer) {
        if (reposition) {

            // レベル別座標補正処理。とりあえず適当で画面中央に向かって縮小処理してある
            var zoom;
			var levelYoffset = 0;
            var levelInfo = env.levels[level];
            if (levelInfo !== void) {
                zoom         = levelInfo.zoom;
				levelYoffset = levelInfo.yoffset;
            }
            if (zoom === void) {
                zoom = 100;
            }
			if (levelYoffset === void) {
				levelYoffset = 0;
			}
            
            var l = kag.scWidth  / 2 + ((int)xpos * zoom / 100) - layer.imageWidth / 2;
            var t = kag.scHeight / 2 + ((env.yoffset - (int)ypos) * zoom / 100) - layer.imageHeight + levelYoffset;
            if (moveTime) {
                if (xposFrom !== void || yposFrom !== void) {
                    var fl = xposFrom !== void ? kag.scWidth  / 2 + ((int)xposFrom * zoom / 100) - layer.imageWidth / 2 : l;
                    var ft = yposFrom !== void ? kag.scHeight / 2 + ((env.yoffset - (int)yposFrom) * zoom / 100) - layer.imageHeight + levelYoffset: t;
                    layer.setPos(fl, ft);
                } 
                layer.setMove(l, t, moveAccel, moveTime);
            } else {
                layer.setMove(l, t);
            }
            xposFrom = void;
            yposFrom = void;
            moveTime = void;
            reposition = false;
        }
    }
    
    /**
     * レイヤ状態更新処理
     * エモーション処理を追加
     */
    function updateLayer(layer) {
        global.KAGEnvImage.updateLayer(...);

        // 表情描画
        env.drawFace(layer.parent === kag.fore ? 0 : 1, this);
        
        // キャラクタが表示されてない場合はエモーションは無効
        if (!isShowBU()) {
            setEmotion();
        }

        // 新規エモーション処理
        if (newEmotionFlag) {
            // エモーション処理実行

            if (emotion) {
                var emoX;
                var emoY;
                var emoRev;

                // 表示座標
                emoX    = init.emoX   if init.emoX   !== void;
                emoY    = init.emoY   if init.emoY   !== void;
                emoRev  = init.emoRev if init.emoRev !== void;

                // ポーズ別補正
                var info;
                if ((info = poses[pose]) !== void) {
                    emoX   = poseInfo.emoX   if info.emoX   !== void;
                    emoY   = poseInfo.emoY   if info.emoY   !== void;
                    emoRev = poseInfo.emoRev if info.emoRev !== void;
                }
                
                // 新規エモーションレイヤ
                emotionLayer = new global.ActionLayer(layer.window, layer.parent);
                
                if (emoRev && emotion.imageRev !== void) {
                    emoitonLayer.loadImages(emotion.imageRev);
                } else {
                    emotionLayer.loadImages(emotion.image);
                }
                emotionLayer.setPos(layer.left + layer.width/2 + emoX,
                                    layer.top  + layer.height  - emoY);
                emotionLayer.visible = true;
                emotion.beginAction(emotion.action);
            }
            
            newEmotionFlag = false;
        }
    }
    
    /**
     * KAG タグ処理
     * @param elm 引数
     */
    function tagfunc(elm) {
        //dm("キャラクタタグの呼び出し:" + name);
        ret = void;
        foreach(elm, doCommand);
        hideMessage();
        updateImage();
        return ret;
    }

    /**
     * 時間用最初期化処理
     */
    function setTime() {
        // 表示中の場合は再描画処理
        var layer = kag.fore.layers[env.initLayerCount + layerId];
        if (layer.visible) {
            redraw = true;
        }
    }
    
    var reNumber = new RegExp("^[0-9][0-9]*$");
    
    /**
     * ボイスファイルの指定
     */
    function setVoice(param) {
        if (typeof param == "Integer") {
            voice = param;
			strVoice = void;
        } else if (typeof param == "String") {
            if (reNumber.test(param)) {
                voice = (int)param;
				strVoice = void;
            } else {
				strVoice = param;
            }
        } else {
            voice = void;
			strVoice = void;
        }
        //dm("ボイス設定:" + param + ":" + voice);
    }

    /**
     * ボイスファイル指定の解除
     */
    function clearVoice() {
        voice = void;
    }
    
    /**
     * 現在のボイスファイル名の取得
     */
    function getVoice(voice) {
        if (typeof voice == "Integer") {
            if (init.voiceFile === void) {
                return void;
            }
            var voiceBase = f.voiceBase !== void ? f.voiceBase : "";
            if (f.name != sf.defaultName || f.family != sf.defaultFamily) {
                // デフォルト以外の名前の場合
                var name = init.voiceFile.sprintf(voiceBase, voice, "N");
                if (Storages.isExistentStorage(name)) {
                    return name;
                }
            }
            // 普通の名前の場合
            return init.voiceFile.sprintf(voiceBase, voice, "");
        } else if (typeof voice == "String") {
            return voice;
        }
        return void;
    }

    function getCurrentVoice() {
        return getVoice(strVoice !== void ? strVoice : voice);
    }
    
    var soundBuffer;
    var voiceEndTime;
    
    /**
     * ボイス開始時処理
     */
    function onStartVoice() {
        //dm("ボイス開始ボリューム制御開始");
        if (kag.sflags.bgmdown && !env.kag.skipMode) {
            kag.bgm.voldown = true;
        }
    }

    /**
     * ボイス停止時処理
     */
    function onStopVoice() {
        //dm("ボイス開始ボリューム制御完了");
        voiceEndTime = void;
        if (kag.bgm.voldown) {
            kag.bgm.voldown = false;
        }
    }

    /**
     * ボイスの再生
     */
    function playVoice(voicename=void) {
        voiceEndTime = void;
        var ret = void;
        if (voicename === void) {
            voicename = getCurrentVoice();
            if (strVoice !== void) {
                strVoice = void;
            } else {
                if (typeof voice == "Integer") {
                    voice++;
                } else {
                    voice = void;
                }
            }
        }

        if (voicename !== void && kag.getVoiceOn(init.voiceName)) {

            // ほかのボイスを消去 XXX
            env.stopAllVoice();

            if (!kag.skipMode && kag.voiceenable) {
                //dm("再生処理:" + voicename);
                // 再生処理実行
                if (soundBuffer == void) {
                    soundBuffer = new VoiceTrack(this);
                }
                // ボリューム補正
                //dm("ボイスボリューム" + kag.voicevolume);
                soundBuffer.volume2 = kag.getVoiceVolume(init.voiceName) * 1000;
                try {
                    soundBuffer.open(voicename);
                    soundBuffer.play();
                    ret = soundBuffer.totalTime;
                } catch (e) {
                    dm("ボイス再生に失敗しました ファイル名:" + voicename);
                }
            }
        }
        if (ret) {
            voiceEndTime = System.getTickCount() + ret;
        }
        return ret;
    }

    /**
     * ボイスの外部からの再生
     */
    function playVoice2(param) {
        //dm("外部ボイス再生:", param);
        if (typeof param == "String") {
            if (reNumber.test(param)) {
                param = (int)param;
            }
        } else {
            param = void;
        }
        playVoice(getVoice(param));
    }
    
    /**
     * ボイスの停止
     */
    function stopVoice() {
        voiceEndTime = void;
        if (soundBuffer !== void) {
            soundBuffer.stop();
        }
    }

    /**
     * ボイス待ち
     * @param true の場合スキップ可能（デフォルト）
     * @param falseの場合スキップ不可能
     */
    function waitVoice(param) {
        if (voiceEndTime !== void) {
            var waitTime = voiceEndTime - System.getTickCount();
            ret = kag.waitTime(waitTime, param == true && kag.clickSkipEnabled);
        }
    }
    
};

/**
 * 環境BGMオブジェクト
 */
class KAGEnvBgm {

    var env;
    
    /**
     * コンストラクタ
     */
    function KAGEnvBgm(env) {
        this.env = env;
    }

    /**
     * 再生処理
     * @param param 再生対象ファイル
     */
    function play(param, elm) {
        if (param !== void) {
            var time = +elm.time;
            if (elm.noxchg) {
                if (time > 0)  {
                    kag.bgm.fadeIn(%[ storage:param, loop:elm.loop, time:time, start:elm.start]);
                } else {
                    kag.bgm.play(%[ storage:param, loop:elm.loop, paused:elm.paused, start:elm.start]);
                }
            } else {
                if (time > 0 || +elm.intime > 0) {
                    kag.bgm.exchange(%[ storage:param, loop:elm.loop, time:time, intime:elm.intime, outtime:elm.outtime, overlap:elm.overlap, start:elm.start]);
                } else {
                    kag.bgm.play(%[ storage:param, loop:elm.loop, paused:elm.paused, start:elm.start]);
                }
            }
            kag.clearBgmStop();
            // 再生既読フラグ
            kag.sflags["bgm_" + (param.toUpperCase())] = true;
        }
    }

    /**
     * 停止処理
     * @param param フェードアウト時間
     */
    function stop(param, elm) {
        if (+param > 0) {
            kag.bgm.fadeOut(%[ time: +param]);
        } else {
            kag.bgm.stop();
        }
    }

    /**
     * ポーズ処理
     * @param param フェードアウト時間
     */
    function pause(param, elm) {
        if (+param > 0) {
            kag.bgm.fadePause(%[ time: +param]);
        } else {
            kag.bgm.pause();
        }
    }

    /**
     * 再開
     */
    function resume(param, elm) {
        kag.bgm.resume();
    }

    /**
     * 音量フェード
     * @param param フェード時間
     */
    function fade(param, elm) {
        kag.bgm.fade(%[ time:elm.time, volume:param ]);
    }

    var bgmcommands = %[
    tagname : null, 
    play : play incontextof this,
    stop : stop incontextof this,
    pause : pause incontextof this,
    resume : resume incontextof this,
    fade : fade incontextof this,
    noxchg : null,
    loop : null,
    time : null,
    start : null,
    paused : null,
    intime : null,
    outtime : null,
    overlap : null,
        ];

	var doflag;
    
    /**
     * コマンドの実行
     * @param cmd コマンド
     * @param param パラメータ
     * @param elm 他のコマンドも含む全パラメータ
     * @return 実行が行われた場合 true
     */
    function doCommand(cmd, param, elm) {
        var func;
        if ((func = bgmcommands[cmd]) !== void) {
            if (func != null) {
                func(param, elm);
                doflag = true;
            }
            return true;
        }
        // 再生コマンドとみなす
        play(cmd, elm);
        doflag = true;
        return true;
    }

    /**
     * KAG タグ処理
     * @param elm 引数
     */
    function tagfunc(elm) {
		//dm("BGM 用ファンクション呼び出し!");
		doflag = false;
        foreach(elm, doCommand);
        // 何もしなかった場合、かつ、タグ名が bgm でなければそれを再生する
        if (!doflag && elm.tagname != "bgm") {
            play(elm.tagname, elm);
        }
        return 0;
    }
    
};

/**
 * 環境BGMオブジェクト
 */
class KAGEnvSE {

    var env;
    var id;
    var name;  // 参照しているファイル名
    var count; // 参照されたカウント値
    
    /**
     * コンストラクタ
     */
    function KAGEnvSE(env, id) {
        this.env = env;
        this.id = id;
    }

    /**
     * 再生処理
     * @param param 再生対象ファイル
     */
    function play(param, elm) {
        if (param !== void) {
            name = param;
            var time = +elm.time;
            if (time > 0)  {
                kag.se[id].fadeIn(%[ storage:param, loop:elm.loop, time:time, start:elm.start]);
            } else {
                kag.se[id].play(%[ storage:param, loop:elm.loop, start:elm.start]);
            }
        }
    }

    /**
     * 停止処理
     * @param param フェードアウト時間
     */
    function stop(param, elm) {
        if (+param > 0) {
            kag.se[id].fadeOut(%[ time: +param]);
        } else {
            kag.se[id].stop();
        }
    }

    /**
     * 音量フェード
     * @param param フェード時間
     */
    function fade(param, elm) {
        kag.se[id].fade(%[ time:elm.time, volume:param ]);
    }

    var secommands = %[
    tagname : null, 
    play : play incontextof this,
    stop : stop incontextof this,
    fade : fade incontextof this,
    loop : null,
    time : null,
    start : null,
        ];

	var doflag;

    /**
     * コマンドの実行
     * @param cmd コマンド
     * @param param パラメータ
     * @param elm 他のコマンドも含む全パラメータ
     * @return 実行が行われた場合 true
     */
    function doCommand(cmd, param, elm) {
        var func;
        if ((func = secommands[cmd]) !== void) {
            if (func != null) {
                func(param, elm);
				doflag = true;
            }
            return true;
        }
        // 再生コマンドとみなす
        play(cmd, elm);
		doflag = true;
        return true;
    }

    /**
     * KAG タグ処理
     * @param elm 引数
     */
    function tagfunc(elm) {
		//dm("SE 用ファンクション呼び出し!");
		doflag = false;
        foreach(elm, doCommand);
        // 何もしなかった場合、かつ、タグ名が se でなければそれを再生する
        if (!doflag && elm.tagname != "se") {
            play(elm.tagname, elm);
        }
        return 0;
    }
};


/**
 * 環境オブジェクト
 */
class KAGEnvironment extends KAGEnvImage {

    /// KAG本体の参照
    var kag;

    // フェード指定のデフォルト値
    // envinit.tjs で定義するか、システムのデフォルトを使う
    property fadeValue {
        getter() {
            if (envinfo !== void && envinfo.fadeValue !== void) {
                return envinfo.fadeValue;
            } else {
                return 500;
            }
        }
    }
    
    /// 初期化情報
    var envinfo;

    var times;        //< 時間情報
    var stages;       //< 舞台情報
	var events;       //< イベント絵情報
    var positions;    //< 配置情報
    var actions;      //< アクション情報
    var transitions;  //< トランジション情報
    var defaultTime;  //< デフォルトの時間
    var yoffset;      //< キャラクタ配置のyoffset 値
    var defaultLevel; //< キャラクタレベルのデフォルト値
    var levels;       //< キャラクタレベル別補正情報
    var faceLevelName;//< フェイスウインドウ用の表示名
	var emotions;
    
    /// 初期化時のレイヤカウント
    var initLayerCount;

    /// 現在のレイヤカウント
    var layerCount;
    
    /// キャラクタ名一覧
    var characterNames;
    /// キャラクタ初期化情報一覧
    var characterInits;
    /// キャラクタ情報
    var characters;
    /// レイヤ情報
    var layers;
    /// イベントレイヤ
    var event;

    // BGM 系
    var bgm;
    var origOnBgmStop;
    function onBGMStop() {
        origOnBgmStop(...);
    }

    // SE 系
    var ses;
    
    //　現在時刻
    property currentTime {
        getter() {
            if (times !== void && time !== void) {
                return times[time];
            }
        }
    }

    // 処理対象になるデフォルトのオブジェクト
    // 名前表示のあと設定される
    var currentObject;
    
    /**
     * コンストラクタ
     * @param kag KAG本体
     */
    function KAGEnvironment(kag) {

        super.KAGEnvImage(this);
        
        this.kag = kag;
        characters = %[];
        characterInits = %[];
        layers = %[];
        event = new KAGEnvBaseLayer(env, "event");

        // BGM オブジェクト
        bgm = new KAGEnvBgm(env);
        // SE 保持用。配列
        ses = [];
        for (var i=0; i<kag.numSEBuffers; i++) {
            ses[i] = new KAGEnvSE(env, i);
        }
        
        // 最初の実行時にその時点で存在しているレイヤの番号までは対象からはずすようにする
        initLayerCount = kag.numCharacterLayers;
        
        try {
            // 初期化情報展開
            envinfo = Scripts.evalStorage("envinit.tjs");
            // デバッグ表示
            if (envinfo) {
                times       = envinfo.times;       showKeys("times", times);
                stages      = envinfo.stages;      showKeys("stages", stages);
                events      = envinfo.events;      showKeys("stages", events);
                positions   = envinfo.positions;   showKeys("positions", positions);
                actions     = envinfo.actions;     showKeys("actions", actions);
                emotions    = envinfo.emotions;    showKeys("actions", actions);
                transitions = envinfo.transitions; showKeys("transitions", transitions);
                defaultTime = envinfo.defaultTime; dm("defaultTime:" + defaultTime);
                yoffset     = envinfo.yoffset;     dm("yoffset:" + yoffset);
                defaultLevel = envinfo.defaultLevel; dm("defaultLevel:" + defaultLevel);
                levels       = envinfo.levels;
                faceLevelName = envinfo.faceLevelName;
           }
        } catch (e) {
            throw new Exception("初期化情報のパースに失敗しました(詳細はコンソール参照)");
        }

        if (envinfo) {
        
            // キャラクタ情報初期化処理
            if (envinfo.characters !== void) {
                
                //dm("キャラクタ情報初期化");
                
                var chinit = [];
                chinit.assign(envinfo.characters);
                
                //dm("キャラクタ情報個数" + chinit.count);
                
                for (var i=0; i<chinit.count; i+=2) {
                    var name = chinit[i];
                    var init = chinit[i+1];
                    characterInits[name] = init;
                }
                
            }
        } else {
            dm("環境情報がありません");
        }
        
        // KAG に自分をコマンドとして登録
        kag.tagHandlers["env"] = this.tagfunc;
        kag.tagHandlers["begintrans"] = this.beginTrans;
        kag.tagHandlers["endtrans"] = this.endTrans;
        kag.tagHandlers["newlay"]  = this.newLayer;
        kag.tagHandlers["newchar"] = this.newCharacter;

        kag.tagHandlers["dispname"] = this.dispname;
        kag.tagHandlers["endline"]  = this.endline;
		kag.tagHandlers["quake"]    = this.quake;

        kag.unknownHandler = this.unknown;
        kag.seStopHandler  = this.onSeStop;
        
        dm("環境初期化完了");
    }

    /**
     * セーブ処理
     */
    function onStore(f) {
        super.onStore(f);
        f.time = time;
        f.stage = stage;

        var chars = %[];
        foreach(characters, function(name, value, dict) {
            var fch = %[];
            value.onStore(fch);
            this[name] = fch;
        } incontextof chars);
        f.characters = chars;
        
        var lays = %[];
        foreach(layers, function(name, value, dict) {
            var fch = %[];
            value.onStore(fch);
            this[name] = fch;
        } incontextof lays);
        f.layers = lays;

        f.event = %[];
        event.onStore(f.event);
    }

    /**
     * ロード処理
     */
    function onRestore(f) {
        init();
        time = f.time;
        stage = f.stage;
        if (f.characters) {
            foreach(f.characters, function(name, value, dict) {
                var ch = getCharacter(value.name, value.initName, value.layerId);
                if (ch != null) {
                    ch.onRestore(value);
                }
            } incontextof this);
        }
        if (f.layers) {
            foreach(f.layers, function(name, value, dict) {
                var l = getEnvLayer(value.name, true, value.layerId);
                if (l !== void) {
                    l.onRestore(value);
                }
            } incontextof this);
        }
        if (f.event) {
            event.onRestore(f.event);
        }
        super.onRestore(f);
		updateImage();
    }
    
    /**
     * ファイナライザ
     */
    function finalize() {
        init();
        dm("環境終了");
        if (kag.unknownHandler === this.unknown) {
            kag.tagHandlers = void;
        }
        invalidate characters;
        invalidate layers;
        invalidate event;
        super.finalize();
    }

    // -----------------------------------------

    /// 時間
    var time;
    /// 舞台
    var stage;
    /// イベント絵
    var event;

    // -----------------------------------------

    /**
     * 環境情報の初期化
     */
    function init() {
        dm("初期化処理");
        time = void;
        stage = void;
        disp       = CLEAR;
        event.disp = CLEAR;
        // キャラクタ情報の破棄
        foreach(characters, function(name,value,dict) {
            invalidate value;
            delete dict[name];
        });
        // 特殊レイヤ情報の破棄
        foreach(layers, function(name,value,dict) {
            invalidate value;
            delete dict[name];
        });
        layerCount = 0;
        kag.allocateCharacterLayers(initLayerCount + layerCount, false);
        transMode = void;
		trans = void;

        // SE 初期化
        for (var i=0;i<ses.count;i++) {
            ses[i].name  = void;
            ses[i].count = 0;
        }
        seCount = 0;

        // カレントオブジェクト初期化
        currentObject = void;
    }

    /**
     * 全キャラクタ消去
     */
    function hideCharacters() {
        // キャラクタ情報の破棄
        foreach(characters, function(name,value,dict) {
            value.disp = global.KAGEnvImage.CLEAR;
        });
		redraw = true;
    }

    /**
     * 全レイヤ消去
     */
    function hideLayers() {
        // 特殊レイヤ情報の破棄
        foreach(layers, function(name,value,dict) {
            value.disp = global.KAGEnvImage.CLEAR;
        });
		redraw = true;
    }

    /**
     * 全要素消去
     */
    function hideAll() {
        hideCharacters();
        hideLayers();
		redraw = true;
    }

    /**
     * 舞台を設定する
     * @param stageName 舞台名
     * @param elm コマンドのほかの要素
     */
    function setStage(stageName, elm) {
        if (stageName != stage || disp == CLEAR) {
            stage = stageName;
            disp = BOTH;

			// ステージ変更時フック
			if (global.setStageHook !== void) {
				global.setStageHook(stageName, elm);
			}

            // 舞台変更時はキャラの立ち絵を消去する？

            // トランジション指定
            if (!setTrans2(stages[stage].trans)) {
                if (!setTrans2(env.envinfo.stageTrans)) {
                    setTrans2(env.envinfo.envTrans);
                }
            }
        }
        // イベント絵は消去
        event.disp = CLEAR;
    }

    /**
     * 時間を設定する
     * @param timeName 時間名
     * @param elm コマンドのほかの要素
     */
    function setTime(timeName, elm) {
        if (timeName != time || disp == CLEAR) {
            time = timeName;
            disp = BOTH;

			// 時間変更時フック
			if (global.setTimeHook !== void) {
				global.setTimeHook(timeName, elm);
			}

            // 時間変更はキャラの立ち絵も再描画の必要がある
            foreach(characters, function(name, value, dict) {
                value.setTime();
            });

            // トランジション指定
            if (!setTrans2(times[time].trans)) {
                if (!setTrans2(env.envinfo.timeTrans)) {
                    setTrans2(env.envinfo.envTrans);
                }
            }
        }
        // イベント絵は消去
        event.disp = CLEAR;
    }

    // トランジションモード
    var transMode;

    /**
     * 全体トランジション開始
     */
    function beginTrans(elm) {
        kag.fore.base.stopTransition();
        kag.backupLayer(%[], true);
        transMode = true;
        return 0;
    }

    /**
     * 全体トランジション終了
     */
    function endTrans(elm) {

        var name = elm.trans;
        ret = void;

        var tr = %[];
        
        // トランジション終了処理
        // 名前指定で上書き
        var info;
        if (env.transitions !== void && (info = env.transitions[name]) !== void) {
            // コピー
            (Dictionary.assign incontextof tr)(info, false); 
            // パラメータのコピー
            foreach(elm, function(name, value, elm, tr) {
                if (transitionParam[name] !== void) {
                    tr[name] = value;
                    //delete elm[name];
                }
            }, tr);
        } else if (transitionName[name] !== void || name.name.substring(0,5) == "trans") {
            // 規定のトランジション
            // パラメータのコピー
            foreach(elm, function(name, value, elm, tr) {
                if (transitionParam[name] !== void) {
                    tr[name] = value;
                    //delete elm[name];
                }
            }, tr);
            tr.method = name;
        }
        var trans = tr;
        if (trans.method === void) {
            trans.method = "universal";
        }
        
        if (trans.time === void) {
            trans.time = 1000;
        }
        trans.children = true;

        if (trans.msgoff) {
            kag.invisibleCurrentMessageLayer();
        }
		kag.syncMessageLayer();
        
        // スキップ処理中
        if (isSkip()) {
            trans.time = 1;
            kag.fore.base.beginTransition(trans);
            ret = kag.waitTransition(EMPTY);
        } else {
            kag.fore.base.beginTransition(trans);
            if (trans.transwait !== void) {
                ret = kag.waitTime((int)trans.time + (int)trans.transwait, kag.clickSkipEnabled);
            } else {
                ret = kag.waitTransition(EMPTY);
            }
        }

        transMode = false;
        return ret;
    }

    /**
     * 新規レイヤ生成
     */
    function newLayer(elm) {
        var lay = getEnvLayer(elm.name, true);
        if (lay != null) {
            delete elm.tagname;
            delete elm.name;
            return lay.tagfunc(elm);
        }
        return 0;
    }

    /**
     * 新規キャラクタ生成
     */
    function newCharacter(elm) {
        var ch = getCharacter(elm.name, elm.initname);
        if (ch != null) {
            delete elm.tagname;
            delete elm.name;
            delete elm.initname;
            return ch.tagfunc(elm);
        }
        return 0;
    }        

    var envCommands = %[
    /**
     * 全体の初期化処理
     */
    init : this.init incontextof this,
    stage : this.setStage incontextof this,
    stime : this.setTime incontextof this,
    hidecharacters : this.hideCharacters incontextof this,
    hidelayers : this.hideLayers incontextof this,
    hideall : this.hideAll incontextof this,
    stopallvoice : this.stopAllVoice incontextof this,
        ];
    
    /**
     * コマンドの実行
     * @param cmd コマンド
     * @param param パラメータ
     * @param elm 他のコマンドも含む全パラメータ
     * @return 実行が行われた場合 true
     */
    function doCommand(cmd, param, elm) {

        //dm("環境コマンド実行:" + cmd);
        
        if (super.doCommand(cmd, param, elm)) {
            return true;
        }
        
        var func;
        if ((func = envCommands[cmd]) !== void) {
            //dm("登録コマンドが存在:" + cmd);
            if (func != null) {
                func(param, elm);
            }                
            return true;
        }

        var find = false;

        // 時間と舞台
        var info;
        if (times !== void && (info = times[cmd]) !== void) {
            setTime(cmd, elm);
            find = true;
        } else if (stages !== void && (info = stages[cmd]) !== void) {
            setStage(cmd, elm);
            find = true;
        }

        // アクション
        if (!find) {
            find = setAction(cmd, elm);
        }

        // 更新
        if (!find) {
            find = setTrans(cmd, elm);
        }

        return find;
    }

    /**
     * 舞台画像の描画
     * @param layer 描画先レイヤ
     */
    function drawStage(layer) {
        var image;
        if (stage !== void) {
            image = stages[stage].image;
            // 時間情報で上書き
            if (time === void) {
                dm("時間の指定がありません");
                time = defaultTime;
            }
            if (time !== void) {
                image = image.replace(/TIME/, times[time].prefix);
                // 画像のロードと座標補正処理
                try {
                    layer.loadImages(%[ "storage" => image ]);
                } catch(e) {
                    // 画像がロードできなかった場合は補正で対応
                    image = stages[stage].image;
                    image = image.replace(/TIME/, times[defaultTime].prefix);

                    try {
                        layer.loadImages(%[ "storage" => image ]);
                        
                        // 色補正処理
                        var timeInfo;
                        if ((timeInfo = currentTime) !== void) {
                            // レイヤ合成
                            if (timeInfo.lightColor !== void) {
                                layer.holdAlpha = true;
                                layer.fillOperateRect(0,0,
                                                      layer.width,layer.height,
                                                      timeInfo.lightColor,
                                                      timeInfo.lightType);
                            }
                            // 明度補正
                            if (timeInfo.brightness !== void) {
                                layer.light(timeInfo.brightness,
                                            timeInfo.contrast);
                            }
                        }
                    } catch (e) {
                        dm("背景画像がロードできません" + image);
                    }
                }
                var xoff = stages[stage].xoff;
                var yoff = stages[stage].yoff;
                layer.left = (kag.scWidth  / 2) - layer.imageWidth / 2  + (xoff !== void ? xoff : 0);
                layer.top  = (kag.scHeight / 2) - layer.imageHeight / 2 + (yoff !== void ? yoff : 0);

            } else {
                dm("時間のデフォルト指定が存在していません");
            }
        } else {
            layer.clearImage(true);
        }
    }

    /**
     * レイヤを作成する
     * @param base
     */
    function getLayer(base) {
        if (base === void) {
            base = env.transMode ? kag.back : kag.fore;
        }
        return base.stage;
    }
    
    function drawAll(base) {
        var layer = getLayer(base);

        drawStage(layer);
        
        // 子要素の再描画
        foreach(characters, function(name, value, dict, base) {
            value.updateImage(base);
        }, base);
        foreach(layers, function(name, value, dict, base) {
            value.updateImage(base);
        }, base);
        event.updateImage(base);
        updateLayer(layer);
    }
    
    /**
     * 再描画処理
     */
    function updateImage() {

        kag.updateBeforeCh = 1;

        // 描画更新が必要な場合
        if (redraw) {
            if (trans) {
                kag.fore.base.stopTransition();
                
                // 更新処理を走らせる場合
                if (trans.time === void) {
                    trans.time = 1000;
                }
                trans.children = true;

                // 全レイヤをバックアップ
                kag.backupLayer(%[], true);
                
                // ステージの描画
                drawAll(kag.back);
                
                // トランジション実行
                kag.fore.base.beginTransition(trans);
                if (ret == 0) {
                    ret = kag.waitTransition(EMPTY);
                } else {
                    kag.waitTransition(EMPTY);
                }
                
            } else {

                var layer = getLayer();

                // フェード判定
                // 既に表示されてるときはトランジションで代用
                if (fadeTime !== void && isShowBU() && layer.visible) {

                    kag.fore.base.stopTransition();
                    var trans = %[ "method" => "crossfade",
                                   "children" => true,
                                   "time" => fadeTime > 1 ? fadeTime : fadeValue];
                    fadeTime = void;

                    // 全レイヤを裏にバックアップ
                    kag.backupLayer(%[], true);

                    // 裏レイヤが対象
                    drawAll(kag.back);
                    
                    // トランジション実行
                    kag.fore.base.beginTransition(trans);
                    if (ret == 0) {
                        ret = kag.waitTransition(EMPTY);
                    } else {
                        kag.waitTransition(EMPTY);
                    }

                } else {
                    drawAll();
                }
            }
            redraw = false;
        } else {
            updateLayer(getLayer());
        }
        trans = void;
    }

    /**
     * KAG タグ処理
     * @param elm 引数
     */
    function tagfunc(elm) {
        //dm("環境タグがよばれたよ");
        var ret;
        foreach(elm, doCommand);
        hideMessage();
        updateImage();
        return ret;
    }

    /**
     * 新規にレイヤID を取得する
     * @param layerId 指定されたレイヤID
     */
    function getLayerId(layerId) {
        if (layerId !== void) {
            if (layerId >= layerCount) {
                layerCount = layerId + 1;
            }
        } else {
            layerId = layerCount++;
        }
        kag.allocateCharacterLayers(env.initLayerCount + layerCount, false);
        return layerId;
    }
    
    /**
     * 指定された名前のキャラクタを返す
     * @param name 名前
     * @param initName 初期化名
     * @param layerId レイヤID
     */
    function getCharacter(name, initName, layerId) {
        var ch = characters[name];
        if (ch === void) {
            if (initName === void) {
                initName = name;
            }
            var init;
            if ((init = characterInits[initName]) !== void) {
                layerId = getLayerId(layerId);
                ch = new KAGEnvCharacter(this, layerId, name, initName, init);
                characters[name] = ch;
            }
        }
        return ch;
    }

    /**
     * 指定された名前のレイヤを返す
     * @param name 名前
     * @param create 生成モード
     */
    function getEnvLayer(name, create=false, layerId) {
        var lay = layers[name];
        if (lay === void && create) {
            layerId = getLayerId(layerId);
            lay = new KAGEnvSimpleLayer(this, layerId, name);
            layers[name] = lay;
        }
        return lay;
    }

    /**
     * 全キャラのボイスを停止する
     */
    function stopAllVoice() {
        foreach(characters, function(name, value, dict) {
            value.stopVoice();
        });
    }

    /*
     * 指定したキャラで指定したボイスファイルを再生
     */
    function playVoice(name, voicename) {
        stopAllVoice();
        var ch = getCharacter(name);
        if (ch !== void && voicename !== void) {
            return ch.playVoice(voicename);
        }
    }


    function quake(elm)	{
        // 揺れをのっとる
        if (!isSkip()) {
            kag.doQuake(elm);
        }
        return 0;
	}
    
    /**
     * 行終了処理ハンドラ
     */
    function endline(elm) {
        if (kag.historyWriteEnabled) {
            kag.historyLayer.clearAction();
            kag.historyLayer.reline();
        }
    }

    /**
     * 表情消去処理
     */
    function clearFace() {
        if (envinfo.clearFace !== void) {
            kag.current.faceLayer.loadImages(envinfo.clearFace);
            kag.current.faceLayer.visible = true;
        } else {
            kag.current.clearFace();
        }
    }

    // 指定されたキャラクタの表情が表示可能なら表示する
    function drawFace(page, ch) {
        if (faceLevelName !== void && currentObject === ch) {
            if (ch !== void && ch.isShowFace()) {
                ch.drawFace(page == 0
                            ? kag.fore.messages[kag.currentNum].faceLayer
                            : kag.back.messages[kag.currentNum].faceLayer,
                            faceLevelName);
            } else {
                clearFace();
            }
        }
    }
    
    /**
     * 名前表示処理ハンドラ
     */
    function dispname(elm) {

        if (kag.sflags.voicecut) {
            stopAllVoice();
        }

        //dm("名前表示ハンドラ");
        if (elm === void || elm.name === void || elm.name == "") {
            // 名前表示初期化ロジック
            // 名前消去
            kag.current.clearName();
            if (faceLevelName !== void) {
                clearFace();
            }
            // 追加waitはなし
            kag.addAutoWait();

            // カレントオブジェクトの解除
            currentObject = void;
            
        } else {

            var name = elm.name;
            var disp = elm.disp;

            var ch = getCharacter(name);
            var voice;

            if (kag.historyWriteEnabled && ch !== void && (voice = ch.getCurrentVoice()) !== void) {
                kag.historyLayer.setNewAction("global.world_object.env.playVoice(\"" + name + "\",\"" + voice + "\")");
            }
            
            // 名前表示処理
            var dispName;
            if (disp !== void && disp != "") {
                dispName = disp;
            } else {
                if (ch !== void && ch.init.nameAlias !== void) {
                    dispName = Scripts.eval(ch.init.nameAlias);
					if (dispName === void) {
						dispName = name;
					}
                } else {
                    dispName = name;
                }
            }

// XXX 名前補正。外だしにすること!
//            if (dispName.length <= 1) {
//                dispName = "　" + dispName + "　";
//            }
//            var dispName = "【" + dispName + "】";


            kag.current.processName(dispName);
            if (kag.historyWriteEnabled) {
				if (typeof kag.historyLayer.storeName !== 'undefined') {
                    kag.historyLayer.storeName(dispName);
				} else {
					kag.historyLayer.store(dispName + " ");
                }
			}

            // 消去状態なら顔表示状態にする
            if (ch !== void && ch.disp == CLEAR) {
                ch.disp = FACE;
            }
            
            // 表情変更処理
            //if (currentObject !== ch) {
                currentObject = ch;
                drawFace(kag.currentPage, ch);
			//}
            
            // ボイス再生
            if (ch !== void) {
                // ボイス再生処理
                kag.addAutoWait(ch.playVoice());
            } else {
                kag.addAutoWait();
            }

        }
        return 0;
    }

    var seCount = 0;
    /**
     * SE 処理用オブジェクトの取得
     * @param id SE番号指定
     * 一番古いSEがわかるようにカウント処理をしている
     */
    function getSe(id) {
        ses[id].count = seCount++;
        return ses[id];
    }

    /**
     * SE 停止時の処理
     * 停止中状態にする
     */
    function onSeStop(id) {
        if (id < ses.count) {
            ses[id].name = void;
        }
    }
    
    /**
     * SE の ID を決定する
     * @param buf バッファIDを指定
     */
    function getSeId(buf) {
        // 直接バッファが指定されている場合はそれを返す
        if (buf !== void && +buf < ses.count) {
            return +buf;
        }
        // 使われてないものをさがす
        var max = seCount;
        var old = void;
        for (var i=0; i<ses.count; i++) {
            if (ses[i].name == void) {
                return i;
            }
            if (ses[i].count < max) {
                max = ses[i].count;
                old = i;
            }
        }
        // 一番古いものを返す
        return old;
    }

    /**
     * SE の ID を決定する
     * @param name SE の名前
     */
    function getSeIdFromName(name) {
        for (var i=0; i<ses.count; i++) {
            if (ses[i].name == name) {
                return i;
            }
        }
        // みつからないのであいている番号を返す
        return getSeId();
    }
    
    /**
     * KAG 不明処理ハンドラ
     */
    function unknown(tagName, elm) {

        //dm("不明コマンドハンドラ:" + tagName);

        // 環境のコマンドか？
        if (doCommand(tagName, true, elm)) {
            // 残りも環境コマンドとして実行する
            ret = void;
            elm.tagname = "env";
            foreach(elm, doCommand);
            showKeys("trans", trans);
            hideMessage();
            updateImage();
            return ret;
        }

        // イベント用処理
        if (tagName == "event" || tagName == "ev") {
            return event.tagfunc(elm);
        } else if (tagName.substring(0,2) == "ev") {
            elm[tagName] = true;
            return event.tagfunc(elm);
        }

        // BGM 処理用
        if (tagName == "bgm") {
            return bgm.tagfunc(elm);
        } else if (tagName.substring(0,3) == "bgm") {
            return bgm.tagfunc(elm);
        }

        // SE 処理用
        if (tagName == "se") {
            return getSe(getSeId(elm.buf)).tagfunc(elm);
        } else if (tagName.substring(0,2) == "se") {
            var se = getSe(getSeIdFromName(tagName));
            return se.tagfunc(elm);
        }
        
        // 該当キャラクタが存在するか？
        var ch = getCharacter(tagName);
        if (ch !== void) {
            return ch.tagfunc(elm);
        }

        // 該当レイヤが存在するか？
        var lay = getEnvLayer(tagName);
        if (lay !== void) {
            return lay.tagfunc(elm);
        }
        
        // カレントオブジェクトのコマンドとして実行してみる
        if (currentObject !== void) {
            elm[tagName] = true;
            return currentObject.tagfunc(elm);
        }
    }
};

KAGEnvironment.XPOSITION    = 1;
KAGEnvironment.LEVEL        = 2;
KAGEnvironment.DISPPOSITION = 3;
KAGEnvironment.YPOSITION    = 4;

KAGEnvImage.BOTH      = 1;
KAGEnvImage.BU        = 2;
KAGEnvImage.CLEAR     = 3;
KAGEnvImage.FACE      = 4;
KAGEnvImage.INVISIBLE = 5;

/**
 * KAG 用ワールド拡張プラグイン
 */
class KAGWorldPlugin extends KAGPlugin
{
	var env;
    function KAGWorldPlugin(kag) {
		super.KAGPlugin();
        env = new KAGEnvironment(kag);
    }

    function finalize() {
        if (env) {
            invalidate env;
        }
    }

	function onStore(f, elm)
	{
		var dic = f.env = %[];
		env.onStore(dic);
	}

	function onRestore(f, clear, elm)
	{
		if (f.env !== void) {
			env.onRestore(f.env);
		}
	}
};

kag.addPlugin(global.world_object = new KAGWorldPlugin(kag));

dm("ワールド環境設定完了");

@endscript
@endif

@return
