@if exp="typeof(global.world_object) == 'undefined'"
@iscript

KAGLoadScript('YAML.tjs');

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
function getTo(value)
{
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

function toNumber(value)
{
    if (value === void || value === null) {
        return value;
    } else {
        return +value;
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
    "time" => true,
    "delay" => true,
    "x" => true,
    "y" => true,
    "toptime" => true,
    "vibration" => true,
    "waittime" => true,
    "cycle" => true,
    "distance" => true,
    "falltime" => true,
    "zoom" => true,
    "angvel" => true,
    "angle" => true,
    "showtime" => true,
    "hidetime" => true,
    "intime" => true,
    "outtime" => true,
    "opacity" => true,
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

    // スキップ状態か
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
    var _type;
    var doType;
    property type {
        getter() {
            return _type;
        }
        setter(v) {
            _type = v;
            doType = (v !== void);
        }
    }

    // 透明度
    var opacityFrom;
    var opacityTime;
    var _opacity;
    var doOpacity;
    property opacity {
        getter() {
            return _opacity;
        }
        setter(v) {
            _opacity = v;
            doOpacity = (v !== void);
        }
    }

    // 回転
    var rotateFrom;
    var rotateTime;
    var _rotate;
    var doRotate;
    property rotate {
        getter() {
            return _rotate;
        }
        setter(v) {
            _rotate = v;
            doRotate = (v !== void);
        }
    }

    // ズーム処理
    var zoomFrom;
    var zoomTime;
    var _zoom;
    var doZoom;
    property zoom {
        getter() {
            return _zoom;
        }
        setter(v) {
            _zoom = v;
            doZoom = (v !== void);
        }
    }

    // 回転原点指定
    var _afx;
    var _afy;
    var doAffine;
    property afx {
        getter() {
            return _afx;
        }
        setter(v) {
            _afx = v;
            doAffine = (v !== void);
        }
    }
    property afy {
        getter() {
            return _afy;
        }
        setter(v) {
            _afy = v;
            doAffine = (v !== void);
        }
    }

    // 表示位置座標
    var xpos;
    var ypos;
    var xposFrom;
    var yposFrom;
    var moveTime;
    var moveAccel;

    // 位置変更
    var reposition;
    
    // アクション処理
    var doStopAction;
    var actionList;
    var currentActionList;
    var syncMode;

    // 画像補整指定
    var _grayscale;
    property grayscale {
        getter() {
            if (env.colorall) {
                return env.grayscale;
            } else {
                return _grayscale;
            }
        }
        setter(v) {
            _grayscale = v;
            if (!env.colorall) {
                redraw = true;
            }
        }
    }

    var _rgamma;
    property rgamma {
        getter() {
            if (env.colorall) {
                return env.rgamma;
            } else {
                return _rgamma;
            }
        }
        setter(v) {
            _rgamma = v;
            if (!env.colorall) {
                redraw = true;
            }
        }
    }

    var _ggamma;
    property ggamma {
        getter() {
            if (env.colorall) {
                return env.ggamma;
            } else {
                return _ggamma;
            }
        }
        setter(v) {
            _ggamma = v;
            if (!env.colorall) {
                redraw = true;
            }
        }
    }
    
    var _bgamma;
    property bgamma {
        getter() {
            if (env.colorall) {
                return env.bgamma;
            } else {
                return _bgamma;
            }
        }
        setter(v) {
            _bgamma = v;
            if (!env.colorall) {
                redraw = true;
            }
        }
    }

    function resetColor(param, elm) {
        _grayscale = void;
        _rgamma = void;
        _ggamma = void;
        _bgamma = void;
        if (!env.colorall) {
            redraw = true;
        }
    }
    
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
        actionList = new Array();
        currentActionList = new Array();
        _disp = CLEAR;
    }

    function initImage() {
        type = void;
        opacityFrom = void;
        opacityTime = void;
        opacity = void;
        rotateFrom = void;
        rotateTime = void;
        rotate = void;
        zoomFrom = void;
        zoomTime = void;
        zoom = void;
        afx = void;
        afy = void;
        xpos     = void;
        ypos     = void;
        xposFrom = void;
        yposFrom = void;
        moveTime = void;
        moveAccel = void;
        reposition = false;

        rgamma = void;
        ggamma = void;
        bgamma = void;
        grayscale = void;
        
        resetFlag = false;
        fadeTime = void;
        actionList.clear();
        currentActionList.clear();
        _disp = CLEAR;
    }

    function initLayer(layer) {
        layer.reset();
        if (!doType) {
            _type    = void;
        }
        if (!doOpacity) {
            _opacity = void;
        }
        if (!doRotate) {
            _rotate = void;
        }
        if (!doZoom) {
            _zoom = void;
        }
        if (!doAffine) {
            _afx = void;
            _afy = void;
        }
        clearAction();
    }
    
    function finalize() {
        clearAction();
        invalidate actionList;
        invalidate currentActionList;
    }

    function setXPos(cmd, elm) {
        xposFrom = getFrom(cmd);
        xpos     = getTo(cmd);
        if (moveTime === void) {
            moveTime  = elm.time;
            moveAccel = elm.accel;
        }
        //dm("X位置指定:", xpos, xposFrom, moveTime);
        reposition = true;
    } 

    function setYPos(cmd, elm) {
        yposFrom = getFrom(cmd);
        ypos     = getTo(cmd);
        if (moveTime === void) {
            moveTime  = elm.time;
            moveAccel = elm.accel;
        }
        //dm("Y位置指定:", ypos, yposFrom, moveTime);
        reposition = true;
    }

    /**
     * アクション情報の追加
     */
    function addAction(action) {
        // 規定のモジュールは排除する
        if (action.module == "LayerFadeToModeModule") {
            opacity     = action.opacity;
            opacityTime = action.time;
        } else if (action.module == "LayerToRotateModule") {
            rotate = action.angle;
            rotateTime = action.time;
        } else if (action.module == "LayerNormalZoomModule") {
            zoom = action.zoom;
            zoomTime = action.time;
        } else if (action.module == "LayerAccelMoveModule") {
            xpos = action.x;
            ypos = action.y;
            moveTime = action.time;
            moveAccel = 1;
            reposition = true;
        } else if (action.module == "LayerNormalMoveModule") {
            xpos = action.x;
            ypos = action.y;
            moveTime = action.time;
            moveAccel = 0;
            reposition = true;
        } else if (action.module == "LayerDecelMoveModule") {
            xpos = action.x;
            ypos = action.y;
            moveTime = action.time;
            moveAccel = -1;
            reposition = true;
        } else {
            actionList.add(action);
        }
    }

    /**
     * アクション情報の消去
     */
    function clearAction() {
        currentActionList.clear();
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
                if (info instanceof "Dictionary") {
                    var action = %[];
                    (Dictionary.assign incontextof action)(info, false); 
                    foreach(elm, function(name, value, elm, action) {
                        if (actionParam[name] !== void) {
                            action[name] = value;
                        }
                    }, action);
                    addAction(action);
                    return true;
                } else if (info instanceof "Array") {
                    for (var i=0;i<info.count;i++) {
                        var action = %[];
                        (Dictionary.assign incontextof action)(info[i], false); 
                        foreach(elm, function(name, value, elm, action) {
                            if (actionParam[name] !== void) {
                                action[name] = value;
                            }
                        }, action);
                        addAction(action);
                    }
                    return true;
                }
                return false;
            }
            if (elm !== void && name.substring(0,5) == "Layer") {
                var moduleType;
                try {
                    moduleType = global[name];
                    if (moduleType !== void && typeof moduleType == "Object" && moduleType.LayerModule != void) {
                        // アクションモジュール名の場合
                        var action = %[];
                        foreach(elm, function(name, value, elm, action) {
                            if (actionParam[name] !== void) {
                                action[name] = value;
                            }
                        }, action);
                        action.module = name;
                        action.time = elm.time if elm.time !== void;
                        if (action.time !== void) {
                            action.time *= kag.drawspeed;
                        }
                        addAction(action);
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
     * アクションを解除
     */
    function stopAction() {
        clearAction();
        doStopAction = true;
    }
    
    /**
     * トランジション情報の取得
     */
    function getTrans(name, elm) {
        var tr = %[];
        // 名前指定で上書き
        var info;
        if (env.transitions !== void && (info = env.transitions[name]) !== void) {
            // コピー
            (Dictionary.assign incontextof tr)(info, false); 
            // パラメータのコピー
            foreach(elm, function(name, value, elm, tr) {
                if (transitionParam[name] !== void) {
                    tr[name] = value;
                }
            }, tr);
        } else if (elm != null && (transitionName[name] !== void || name.substring(0,5) == "trans")) {
            // 規定のトランジション
            // パラメータのコピー
            foreach(elm, function(name, value, elm, tr) {
                if (transitionParam[name] !== void) {
                    tr[name] = value;
                }
            }, tr);
            tr.method = name;
        }
        return tr;
    }
    
    /**
     * トランジションを設定
     * @param name トランジション名
     */
    function _setTrans(name, elm) {
        var tr = getTrans(name, elm);
        if (tr.method !== void) {
            if (trans === void) {
                if (!env.transMode && !isSkip()) {
                    trans = tr;
                } else {
                    // 無効なトランジションを指定
                    trans = %[];
                }
            }
            redraw = true;
            return true;
        }
        return false;
    }

    function setTrans(name, elm) {
        if (name == "void") {
            trans = %[];
        } else {
            _setTrans(name, elm);
        }
    }
    
    /**
     * トランジションを設定
     * @param param トランジション指定　文字列または辞書
     * @return 設定した場合ｈ
     */
    function setTrans2(param) {
        if (trans == void) {
            //dm("トランジション設定2:" + param);
            if (param === void) {
                return false;
            } else if (typeof param == "String") {
                _setTrans(param);
                return true;
            } else if (param instanceof "Dictionary") {
                _setTrans(param.method, param);
                return true;
            }
            return false;
        } else {
            return true;
        }
    }
    
    /**
     * 状態更新処理
     */
    function updateLayer(layer) {
        if (resetFlag) {
            layer.reset();
            _type    = void;
            _opacity = void;
            _rotate = void;
            _zoom = void;
            _afx = void;
            _afy = void;
            clearAction();
            _rgamma = void;
            _ggamma = void;
            _bgamma = void;
            _grayscale = void;
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

            if (doAffine) {
                layer.afx = afx;
                layer.afy = afy;
                doAffine = false;
            }
            if (doOpacity) {
                //dm("透明度変更:" + opacity + ":" + opacityTime); 
                if (opacityFrom !== void) {
                    layer.opacity = opacityFrom;
                    opacityFrom = void;
                }
                layer.setOpacityTime(opacity, opacityTime);
                doOpacity = false;
            }
            if (doRotate) {
                //dm("回転変更:" + rotate + ":" + rotateTime);
                if (rotateFrom !== void) {
                    layer.rotate = rotateFrom;
                    rotateFrom = void;
                }
                layer.setRotateTime(rotate, rotateTime);
                doRotate = false;
            }
            if (doZoom) {
                //dm("ズーム変更:" + zoom + ":" + zoomTime);
                if (zoomFrom !== void) {
                    layer.zoom = zoomFrom;
                    zoomFrom = void;
                }
                layer.setZoomTime(zoom, zoomTime);
                doZoom = false;
            }
            if (doType) {
                //dm("合成種別変更");
                layer.type = type;
                doType = false;
            }

            if (doStopAction) {
                layer.stopAction();
                doStopAction = false;
            }
            // アクション処理
            if (actionList.count > 0) {
                for (var i=0;i<actionList.count;i++) {
                    var action = actionList[i];
                    layer.beginAction(action);
                    if (action.time == void || action.time == 0) {
                        currentActionList.add(action);
                    }
                }
                actionList.clear();
            }
        }
        if (syncMode) {
            ret = kag._waitLayerAction(layer);
            //dm("アクション待ち:" + ret);
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
        opacityFrom = toNumber(getFrom(param));
        opacity     = toNumber(getTo(param));
        opacityTime = isSkip() ? 0 : elm.time;
    } incontextof this,
    fade :  function(param, elm) {
        fadeTime = isSkip() || env.transMode ? void : param;
    } incontextof this,
    rotate : function(param, elm) {
        rotateFrom = toNumber(getFrom(param));
        rotate     = toNumber(getTo(param));
        rotateTime = isSkip() ? 0 : elm.time;
    } incontextof this,
    zoom : function(param, elm) {
        zoomFrom = toNumber(getFrom(param));
        zoom     = toNumber(getTo(param));
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
    grayscale : function(param, elm) {
        grayscale = param;
    } incontextof this,
    rgamma : function(param, elm) {
        rgamma = param;
    } incontextof this,
    ggamma : function(param, elm) {
        ggamma = param;
    } incontextof this,
    bgamma : function(param, elm) {
        bgamma = param;
    } incontextof this,
    resetcolor : this.resetColor incontextof this,
    trans  : setTrans incontextof this,
    action : setAction incontextof this,
    stopaction : stopAction incontextof this,

    sync : function(param) { if (param) { syncMode = true; } } incontextof this,

    show : function(param) { disp = BOTH;   } incontextof this,
    hide : function(param) { disp = CLEAR; } incontextof this,
    visible : function(param) { disp = param ? BOTH : CLEAR; }  incontextof this,
    xpos : this.setXPos incontextof this,
    ypos : this.setYPos incontextof this,
    accel : null, // 無視
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
        f.type    = type;
        f.opacity = opacity;
        f.rotate  = rotate;
        f.zoom    = zoom;
        f.xpos    = xpos;
        f.ypos    = ypos;
        f.disp    = disp;
        f.actionList = [];
        (Array.assign incontextof f.actionList)(currentActionList);
        //for (var i=0;i<currentActionList.count;i++) {
        //    dm("アクション保存:" + currentActionList[i].module);
        //}

        f.grayscale = _grayscale;
        f.rgamma    = _rgamma;
        f.ggamma    = _ggamma;
        f.bgamma    = _bgamma;
    }

    /**
     * ロード処理
     */
    function onRestore(f) {

        type = f.type;
        opacity     = f.opacity;
        opacityFrom = void;
        opacityTime = void;
        rotate      = f.rotate;
        rotateFrom  = void;
        rotateTime  = void;
        zoom        = f.zoom;
        zoomFrom    = void;
        zoomTime    = void;
        if (f.actionList !== void) {
            for (var i=0;i<f.actionList.count;i++) {
                addAction(f.actionList[i]);
                //dm("アクション復帰:" + f.actionList[i].module);
            }
        }
        xpos      = f.xpos;
        xposFrom  = void;
        ypos      = f.ypos;
        yposFrom  = void;
        moveTime  = void;
        moveAccel = void;
        reposition = true;

        _grayscale = f.grayscale;
        _rgamma    = f.rgamma;
        _ggamma    = f.ggamma;
        _bgamma    = f.bgamma;
        if (_grayscale != void ||
            _rgamma    != void ||
            _ggamma    != void ||
            _bgamma    != void) {
            redraw = true;
        }
        
        disp = f.disp;
    }

    // このメソッドを実装する
    // function getLayer(base);
    // function drawLayer(layer);

    /**
     * レイヤ配置処理(標準)：左上原点
     * @param layer 処理対象レイヤ
     */
    function calcPosition(layer) {
        if (reposition) {
            //dm("位置指定1",xpos,ypos);
            var l = (int)xpos;
            var t = (int)ypos;
            if (moveTime !== void && moveTime > 0) {
                if (xposFrom !== void || yposFrom !== void) {
                    var fl = xposFrom !== void ? (int)xposFrom : l;
                    var ft = yposFrom !== void ? (int)yposFrom : t;
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

    // トランジション実行
    function beginTransition(trans) {
        if (trans.time === void || isSkip()) {
            trans.time = 0;
        }
        trans.children = true;
        // 処理を割り込ませる
        trans.tagname = "trans";
        kag.conductor.pendings.insert(0, %[ tagname : "syncmsg" ]);
        if (trans.transwait !== void) {
            kag.conductor.pendings.insert(0, %[ tagname : "wait", time : (int)trans.time + (int)trans.transwait, trans:true ]);
        } else {
            kag.conductor.pendings.insert(0, %[ tagname : "wt" ]);
        }
        kag.conductor.pendings.insert(0, trans);
    }

    /**
     * メッセージ窓消去処理
     */
    function hideMessage() {
        if (trans !== void && trans.msgoff) {
            kag.conductor.pendings.insert(0, %[ tagname : "er", all:true]);
            if (kag.setCurrentMessageLayerVisible(false)) {
                ret = -2;
            }
        }
    }
    
    /**
     * 画像の描画
     */
    function updateImage(base) {
        kag.updateBeforeCh = 1;
        // 描画更新が必要な場合
        if (redraw) {
            
            if (base === void && trans !== void && trans.method !== void) {

                kag.fore.base.stopTransition();
                
                // 全レイヤをバックアップ
                kag.backupLayer(EMPTY, true);

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
                // 既に表示されてるときや表情のみの場合はトランジションで代用
                if (fadeTime !== void /**&& ((isShowBU() && layer.visible && layer.opacity > 0) || _disp == FACE)*/) {

                    kag.fore.base.stopTransition();
                    var trans = %[ "method" => "crossfade",
                                   "children" => true,
                                   "time" => fadeTime > 1 ? fadeTime : fadeValue];
                    fadeTime = void;

                    // 全レイヤを裏にバックアップ
                    kag.backupLayer(EMPTY, true);

                    // 裏レイヤが対象
                    layer = getLayer(kag.back);
                    if (isShowBU()) {
                        drawLayer(layer);
                        calcPosition(layer);
                    }
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

    function KAGEnvLayer(env) {
        super.KAGEnvImage(env);
        redraw = false;
    }

    function onStore(f) {
        super.onStore(f);
        f.imageFile = imageFile;
    }
    
    function onRestore(f) {
        imageFile = f.imageFile;
        super.onRestore(f);
    }

    function setImageFile(file, elm) {

        imageFile = file;
        disp = BOTH;

        // 背景指定時に座標指定がなければ場所情報を初期化する
        if (elm.xpos === void) {
            xpos = void;
        }
        if (elm.ypos === void) {
            ypos = void;
        }
        reposition = true;

        // 記録
        kag.sflags["cg_" + (file.toUpperCase())] = true;

        return true;
    }

    var _layerCommands = %[
    file : function(param, elm) {
        setImageFile(param, elm);
    } incontextof this,
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
            find = _setTrans(cmd, elm);
        }

        // 画像のロード
        if (!find) {
            find = setImageFile(cmd, elm);
        }
        return find;
    }

    // このメソッドを実装する
    // function getLayer(base);
    // 
    function drawLayer(layer) {
        initLayer(layer);
        if (imageFile !== void) {
            try {
                layer.loadImages(%[ "storage" => imageFile]);
            } catch (e) {
                dm("画像の読み込みに失敗しました:" + imageFile);
            }
        }
        if (grayscale) {
            layer.doGrayScale();
        }
        if (rgamma != void || ggamma != void || bgamma != void) {
            layer.adjustGamma(rgamma == void ? 1.0 : rgamma, 0, 255,
                              ggamma == void ? 1.0 : ggamma, 0, 255,
                              bgamma == void ? 1.0 : bgamma, 0, 255);
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

    /**
     * 時間用最初期化処理
     */
    function setRedraw() {
        // 表示中の場合は再描画処理
        var layer = kag.fore[name];
        if (layer.visible) {
            redraw = true;
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

    function setImageFile(file, elm) {

        var eventTrans;
        
        if (file !== void) {

            var eventInfo;
            if (env.events !== void) {
                eventInfo = env.events[file];
            }
            if (eventInfo !== void) {
                eventTrans = eventInfo.trans;
                imageFile = eventInfo.image !== void ? eventInfo.image : v;
                xpos = elm.xpos !== void ? (int)elm.xpos : eventInfo.xoff;
                ypos = elm.ypos !== void ? (int)elm.ypos : eventInfo.yoff;
                reposition = true;
            } else {
                imageFile = file;
                xpos = elm.xpos !== void ? (int)elm.xpos : void;
                ypos = elm.ypos !== void ? (int)elm.ypos : void;
                reposition = true;
            }

            disp = BOTH;
            reposition = true;
            
            // 記録
            kag.sflags["cg_" + (file.toUpperCase())] = true;
            
            // トランジション指定
            if (!setTrans2(eventTrans)) {
                if (!setTrans2(env.envinfo.eventTrans)) {
                    setTrans2(env.envinfo.envTrans);
                }
            }

        } else {
            imageFile = void;
            disp = CLEAR;
            xpos = elm.xpos !== void ? (int)elm.xpos : void;
            ypos = elm.ypos !== void ? (int)elm.ypos : void;
            reposition = true;
        }

        return true;
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

    /// 位置変更
    var front;
    var back;

    /**
     * 時間用最初期化処理
     */
    function setRedraw() {
        // 表示中の場合は再描画処理
        var layer = kag.fore.layers[env.initLayerCount + layerId];
        if (layer.visible) {
            redraw = true;
        }
    }
    
    /**
     * コンストラクタ
     * @param layerId 前景レイヤID
     */
    function KAGEnvLevelLayer(layerId) {
        this.layerId = layerId;
        level = void;
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

    function onStore(f) {
        f.layerId = layerId;
        f.level = level;
        f.absolute = absolute;
    }

    function onRestore(f) {
        layerId = f.layerId;
        level = f.level;
        absolute = f.absolute;

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
    
    /**
     * レイヤ配置処理(環境レイヤ用)：中央原点
     * @param layer 処理対象レイヤ
     */
    function calcPosition(layer) {
        if (reposition) {
            //dm("位置指定2");
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
    front : this.setFront incontextof this,
    back : this.setBack incontextof this,
    level : this.setLevel incontextof this,
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

    var facePoseMap;
    
    /// ボイス情報
    var voice;
    var strVoice;
    var incVoice;   // true なら文字列指定時もカウントする
    var noincVoice; // true ならカウントしない
    
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
            
            // 表情ポーズ同期機能
            if (init.facePose) {
                // 表情からポーズに対するマップを作成する
                facePoseMap = %[];
                foreach(poses, function(name, value, dict, facePoseMap) {
                    var faces = [];
                    faces.assign(value.faces);
                    for (var i=0; i<faces.count; i+= 2) {
                        facePoseMap[faces[i]] = name;
                    }
                }, facePoseMap);
            }

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

    // 表情描画処理を行う
    var redrawFace;
    
    /**
     * ポーズの設定
     */
    function setPose(poseName, elm) {
        var info = poses[poseName];
        if (info !== void) {
            if (poseName != pose || disp == CLEAR) {
                pose = poseName;
                if (disp == CLEAR) {
                    disp = init.noPose ? FACE : BOTH;
                    reposition = true;
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
                
                // 表情表示取得
                if (isShowFace() && (elm.faceon || env.showFaceMode) && !elm.faceoff) {
                    redrawFace = true;
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
    function setDress(dressName, elm) {
        if (dressName != dress || disp == CLEAR) {
            dress = dressName;
            if (disp == CLEAR) {
                disp = init.noPose ? FACE : BOTH;
                reposition = true;
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
            // 表情表示取得
            if (isShowFace() && (elm.faceon || env.showFaceMode) && !elm.faceoff) {
                redrawFace = true;
            }
        }
    }
    /**
     * 表情の設定
      */
    function setFace(faceName, elm) {
        if (faceName != face || disp == CLEAR) {
            face = faceName;
            if (disp == CLEAR) {
                disp = init.noPose ? FACE : BOTH;
                reposition = true;
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

            // 表情表示取得
            if (isShowFace() && (elm.faceon || env.showFaceMode) && !elm.faceoff) {
                redrawFace = true;
            }
        }
    }

    /**
     * 場所表示用トランジション設定
     */
    function setPositionTrans(info, force=false) {
        // トランジション指定
        if (force || isShowBU()) {
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
                disp = init.noPose ? FACE : BOTH;
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
            reposition = true;
            setPositionTrans(info, true);
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
    face    : function(cmd,elm) {
        if (facePoseMap !== void) {
            var p;
            if ((p = facePoseMap[cmd]) !== void) {
                setPose(p,elm);
                setFace(cmd,elm);
            }
        } else {
            setFace(cmd,elm);
        }
    }incontextof this,
    pos     : this.setPosition incontextof this,
    emotion : this.setEmotion incontextof this,
    xpos    : this.setXPos incontextof this,
    ypos    : this.setYPos incontextof this,
    front   : this.setFront incontextof this,
    back    : this.setBack incontextof this,
    level   : this.setLevel incontextof this,
    voice   : this.setVoice incontextof this,
    nextvoice : this.setNextVoice incontextof this,
    clearVoice : this.clearVoice incontextof this,
    playvoice : this.playVoice2 incontextof this,
    waitvoice : this.waitVoice incontextof this,
    facewin : function(param) { disp = FACE; } incontextof this,
    faceon : null,
    faceoff : null,
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

        // 顔ポーズマップが存在する場合
        if (facePoseMap !== void) {
            var p;
            if ((p = facePoseMap[cmd]) !== void) {
                setPose(p,elm);
                setFace(cmd,elm);
                find = true;
            }
        }

        if (!find && poses !== void) {
            if (poses[cmd] !== void) {
                find = true;
                setPose(cmd,elm);
            } else {
                var poseInfo;
                if ((poseInfo = poses[pose]) !== void) {
                    var dresses       = poseInfo.dresses;
                    var faces         = poseInfo.faces;
                    if (dresses !== void && dresses[cmd] !== void) {
                        //dm("服装を設定");
                        find = true;
                        setDress(cmd,elm);
                    } else if (faces !== void && faces[cmd] !== void) {
                        //dm("表情を設定");
                        find = true;
                        setFace(cmd,elm);
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
            find = _setTrans(cmd, elm);
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
                layer.type    = layer._initType    = baseImage.type;
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
                    return false;
                }
            } else {
                return false;
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
        
        if (grayscale) {
            layer.doGrayScale();
        }
        if (rgamma != void || ggamma != void || bgamma != void) {
            layer.adjustGamma(rgamma == void ? 1.0 : rgamma, 0, 255,
                              ggamma == void ? 1.0 : ggamma, 0, 255,
                              bgamma == void ? 1.0 : bgamma, 0, 255);
        }
    }

    /**
     * 現在の立ち絵の描画
     * @param layer 描画対象レイヤ
     */
    function drawLayer(layer) {
        initLayer(layer);
        var levelName;
        var levelInfo = env.levels[level];
        if (levelInfo !== void) {
            levelName = levelInfo.name;
        }
        if (levelName === void) {
            levelName = level;
        }
        return _drawLayer(layer, levelName);
    }

    /**
     * 表情を描画する
     */
    function drawFace(layer, faceLevelName) {
        //dm("表情描画:" + name + " face:" + face + " layer:" + layer.name + " level:" + faceLevelName);
        if (faceLevelName !== void && poses !== void) {
            _drawLayer(layer, faceLevelName);
            layer.visible = true;
        } else {
            layer.visible = false;
        }
    }

    property yoffset {
        getter() {
            return init.yoffset !== void ? +env.yoffset + +init.yoffset : +env.yoffset;
        }
    }
    
    /**
     * レイヤ配置処理(キャラクタ用)：中央原点＋レベル補正
     * @param layer 処理対象レイヤ
     */
    function calcPosition(layer) {
        if (reposition) {
            //dm("位置指定3");

            // 未初期化時デフォルト
            if (xpos === void) {
                xpos = env.defaultXpos;
            }
            if (ypos === void) {
                ypos = env.defaultYpos;
            }
            
            // レベル別座標補正処理。とりあえず適当で画面中央に向かって縮小処理してある
            var zoom;
            var levelYoffset = 0;
            var levelInfo = env.levels[level];
            if (levelInfo !== void) {
                zoom         = (int)levelInfo.zoom;
                levelYoffset = (int)levelInfo.yoffset;
            }
            if (zoom === void) {
                zoom = 100;
            }
            if (levelYoffset === void) {
                levelYoffset = 0;
            }
            
            var l = kag.scWidth  / 2 + ((int)xpos * zoom / 100) - layer.imageWidth / 2;
            var t = kag.scHeight / 2 + ((yoffset - (int)ypos) * zoom / 100) - layer.imageHeight + levelYoffset;
            if (moveTime) {
                if (xposFrom !== void || yposFrom !== void) {
                    var fl = xposFrom !== void ? kag.scWidth  / 2 + ((int)xposFrom * zoom / 100) - layer.imageWidth / 2 : l;
                    var ft = yposFrom !== void ? kag.scHeight / 2 + ((yoffset - (int)yposFrom) * zoom / 100) - layer.imageHeight + levelYoffset: t;
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
        if (redrawFace) {
            if (env.currentNameTarget != this) {
                env.drawName();
            }
            env.drawFacePage(layer.parent === kag.fore.base ? kag.fore : kag.back, this);
            env.currentNameTarget = this;

            redrawFace = false;
        }
        
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

    var reNumber = new RegExp("^[0-9][0-9]*$");

    // 加算しないフラグ
    var noinc;
    
    /**
     * ボイスファイルの指定
     */
    function setVoice(param, elm) {
        if (typeof param == "Integer") {
            if (elm.once) {
                strVoice = getVoice(param);
            } else {
                voice = param;
                strVoice = void;
            }
        } else if (typeof param == "String") {
            if (reNumber.test(param)) {
                if (elm.once) {
                    strVoice = getVoice((int)param);
                } else {
                    voice = (int)param;
                    strVoice = void;
                }
            } else {
                strVoice = param;
            }
        } else {
            voice = void;
            strVoice = void;
        }
        incVoice   = elm.incvoice;
        noincVoice = elm.noincvoice;
        //dm("ボイス設定:" + param + ":" + voice);
    }

    /**
     * ボイスファイル指定＋次回再生エントリ
     */
    function setNextVoice(param) {
        env.entryNextVoice(this);
    }
    
    /**
     * ボイスファイル指定の解除
     */
    function clearVoice() {
        voice = void;
        strVoice = void;
    }
    
    /**
     * ボイスファイル名の取得
     * @param voice パラメータ　数値の場合は書式処理、文字列の場合はそのまま返す
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
        return strVoice == "ignore" ? void : getVoice(strVoice !== void ? strVoice : voice);
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
            if (strVoice === void || incVoice) {
                if (typeof voice == "Integer") {
                    if (!noincVoice) {
                        voice++;
                    }
                } else {
                    voice = void;
                }
            }
            incVoice   = void;
            noincVoice = void;
            strVoice   = void;
        }

        if (voicename !== void && kag.getVoiceOn(init.voiceName)) {
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
                    // 拡張子補完処理
                    if (!Storages.isExistentStorage(voicename)) {
                        var test;
                        if(test = voicename + ".wav", Storages.isExistentStorage(test))
                            voicename = test;
                        else if(test = voicename + ".ogg", Storages.isExistentStorage(test))
                            voicename = test;
                    }
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
        //dm("外部ボイス再生:" + param);
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
    

    /**
     * ボイスのボリュームの再調整
     */
    function resetVoiceVolume() {
        if (soundBuffer !== void) {
            soundBuffer.volume2 = kag.getVoiceVolume(init.voiceName) * 1000;
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
            kag.clearBgmLabel();
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

    /**
     * 終了まち
     * @param param フェード時間
     */
    function wait(param, elm) {
        var e = %[];
        (Dictionary.assign incontextof e)(elm, false);
        if (e.canskip === void) {
            e.canskip = true;
        }
        ret = kag.waitBGMStop(e);
    }

    /**
     * 終了まち
     * @param param フェード時間
     */
    function waitFade(param, elm) {
        var e = %[];
        (Dictionary.assign incontextof e)(elm, false);
        if (e.canskip === void) {
            e.canskip = true;
        }
        ret = kag.waitBGMFade(e);
    }


    var bgmcommands = %[
    tagname : null, 
    play : play incontextof this,
    stop : stop incontextof this,
    pause : pause incontextof this,
    resume : resume incontextof this,
    fade : fade incontextof this,
    wait : wait incontextof this,
    waitfade : waitFade incontextof this,
    noxchg : null,
    loop : null,
    time : null,
    start : null,
    paused : null,
    intime : null,
    outtime : null,
    overlap : null,
    canskip : null,
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

    var ret;

    /**
     * KAG タグ処理
     * @param elm 引数
     */
    function tagfunc(elm) {
        //dm("BGM 用ファンクション呼び出し!");
        ret = 0;
        doflag = false;
        foreach(elm, doCommand);
        // 何もしなかった場合、かつ、タグ名が bgm でなければそれを再生する
        if (!doflag && elm.tagname != "bgm") {
            play(elm.tagname, elm);
        }
        return ret;
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

    /**
     * 終了待ち
     */
    function wait(param, elm) {
        var e = %[];
        (Dictionary.assign incontextof e)(elm, false);
        e.buf = id;
        if (e.canskip === void) {
            e.canskip = true;
        }
        ret = kag.waitSEStop(e);
    }

    /**
     * フェード終了待ち
     */
    function waitFade(param, elm) {
        var e = %[];
        (Dictionary.assign incontextof e)(elm, false);
        e.buf = id;
        if (e.canskip === void) {
            e.canskip = true;
        }
        ret = kag.waitSEStop(e);
    }

    var secommands = %[
    tagname : null, 
    play : play incontextof this,
    stop : stop incontextof this,
    fade : fade incontextof this,
    wait : wait incontextof this,
    waitfade : waitFade incontextof this,
    loop : null,
    time : null,
    start : null,
    canskip : null,
    buf : null
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

    var ret;
    
    /**
     * KAG タグ処理
     * @param elm 引数
     */
    function tagfunc(elm) {
        // dm("SE 用ファンクション呼び出し!");
        ret = 0;
        doflag = false;
        foreach(elm, doCommand);
        // 何もしなかった場合、かつ、タグ名が se でなければそれを再生する
        if (!doflag && elm.tagname != "se") {
            play(elm.tagname, elm);
        }
        return ret;
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
    var defaultXpos;  //< キャラクタ配置の初期X位置
    var defaultYpos;  //< キャラクタ配置の初期Y位置
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

    // メッセージ窓処理対象になるデフォルトのオブジェクト
    // 名前表示のあと設定される
    // 環境系命令の後は解除される
    var currentNameTarget;
    // コマンド実行毎に表情処理を行うモード
    var showFaceMode;
    // 表情のフェード指定
    var faceFadeTime;

    // 全カラー制御
    var colorall;

    // 画像補整指定

    property grayscale {
        getter() {
            return _grayscale;
        }
        setter(v) {
            _grayscale = v;
            if (colorall) {
                setColorAll(true);
            }
            redraw = true;
        }
    }

    property rgamma {
        getter() {
            return _rgamma;
        }
        setter(v) {
            _rgamma = v;
            if (colorall) {
                setColorAll(true);
            }
            redraw = true;
        }
    }

    property ggamma {
        getter() {
            return _ggamma;
        }
        setter(v) {
            _ggamma = v;
            if (colorall) {
                setColorAll(true);
            }
            redraw = true;
        }
    }
    
    property bgamma {
        getter() {
            return _bgamma;
        }
        setter(v) {
            _bgamma = v;
            if (colorall) {
                setColorAll(true);
            }
            redraw = true;
        }
    }

    function resetColor(param, elm) {
        _grayscale = void;
        _rgamma = void;
        _ggamma = void;
        _bgamma = void;
        if (colorall) {
            setColorAll(true);
        }
        redraw = true;
    }

    function setColorAll(param) {
        colorall = param;
        // すべて再描画処理
        event.setRedraw();
        foreach(characters, function(name, value, dict) {
            value.setRedraw();
        });
        foreach(layers, function(name, value, dict) {
            value.setRedraw();
        });
        redraw = true;
    }

    
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

        // 初期化情報展開
        envinfo = loadEnvinfo();

        if (envinfo) {
            // デバッグ表示 
            times       = envinfo.times;       showKeys("times", times);
            stages      = envinfo.stages;      showKeys("stages", stages);
            events      = envinfo.events;      showKeys("events", events);
            positions   = envinfo.positions;   showKeys("positions", positions);
            actions     = envinfo.actions;     showKeys("actions", actions);
            emotions    = envinfo.emotions;    showKeys("emotions", emotions);
            transitions = envinfo.transitions; showKeys("transitions", transitions);
            defaultTime = envinfo.defaultTime;      dm("defaultTime:" + defaultTime);
            yoffset     = (int)envinfo.yoffset;     dm("yoffset:" + yoffset);
            defaultXpos = (int)envinfo.defaultXpos;
            defaultYpos = (int)envinfo.defaultYpos;
            defaultLevel = (int)envinfo.defaultLevel;
            levels       = envinfo.levels;
            faceLevelName = envinfo.faceLevelName;
            showFaceMode  = envinfo.showFaceMode;
            faceFadeTime  = envinfo.faceFadeTime;

        
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
        kag.tagHandlers["env"]        = this.tagfunc;
        kag.tagHandlers["allchar"]   = this.allchar;
        kag.tagHandlers["alllayer"]   = this.alllayer;

        kag.tagHandlers["begintrans"] = this.beginTrans;
        kag.tagHandlers["endtrans"]   = this.endTrans;
        kag.tagHandlers["newlay"]     = this.newLayer;
        kag.tagHandlers["newchar"]    = this.newCharacter;

        kag.tagHandlers["msgoff"]     = this.msgoff;
        kag.tagHandlers["msgon"]      = this.msgon;
        kag.tagHandlers["clear"]      = this.clear;
        
        kag.tagHandlers["dispname"]        = this.dispname;
        kag.tagHandlers["dispnameVoice"]   = this.dispnameVoice;
        kag.tagHandlers["endline"]         = this.endline;
        kag.tagHandlers["quake"]           = this.quake;
        kag.tagHandlers["afterpage"]       = this.afterpage;

        // 特殊ハンドラ登録
        kag.unknownHandler         = this.unknown;
        kag.seStopHandler          = this.onSeStop;
        kag.stopActionHandler      = this.onStopAction;
        
        dm("環境初期化完了");
    }

    /**
     * 初期化情報のロード
     */
    function loadEnvinfo() {
        try {
            var yamlFile = 'envinit.yaml';
            if (Storages.isExistentStorage(yamlFile)) {
                return YAML.parse([].load(yamlFile));
            }

            return Scripts.evalStorage("envinit.tjs");
        } catch (e) {
            throw new Exception("初期化情報のパースに失敗しました(詳細はコンソール参照)");
        }
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

        f.colorall = colorall;
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

        colorall = f.colorall;
        if (colorall) {
            setColorAll(colorall);
        }

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
        if (kag.seStopHandler== this.onSeStop) {
            kag.seStopHandler          = void;
        }
        if (kag.stopActionHandler === this.onStopAction) {
            kag.stopActionHandler = void;
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

    // -----------------------------------------

    /**
     * 環境情報の初期化
     */
    function init() {

        //dm("初期化処理");
        time = void;
        stage = void;
        transMode = void;

        colorall = false;

        initImage();
        event.initImage();

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

        // SE 初期化
        for (var i=0;i<ses.count;i++) {
            ses[i].name  = void;
            ses[i].count = 0;
        }
        seCount = 0;

        // カレントオブジェクト初期化
        currentNameTarget = void;
        redraw = true;
        
        voiceCharacters.clear();
    }

    /**
     * イベント絵の消去
     */
    function hideEvent() {
        event.disp = CLEAR;
        redraw = true;
    }

    /**
     * 背景とイベント絵の消去
     */
    function hideBase() {
        disp = CLEAR;
        event.disp = CLEAR;
        redraw = true;
    }
    
    /**
     * 全キャラクタ消去
     */
    function hideCharacters() {
        // キャラクタ情報の破棄
        foreach(characters, function(name,value,dict) {
            value.disp = global.KAGEnvImage.CLEAR;
        });
        currentNameTarget = void;
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
     * 前景要素消去
     */
    function hideFore() {
        hideCharacters();
        hideLayers();
    }

    /**
     * 全要素消去
     */
    function hideAll() {
        hideBase();
        hideCharacters();
        hideLayers();
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

            // 背景指定時に座標指定がなければ場所情報を初期化する
            if (elm.xpos === void) {
                xpos = void;
            }
            if (elm.ypos === void) {
                ypos = void;
            }
            reposition = true;

            // ステージ変更時フック
            if (global.setStageHook !== void) {
                global.setStageHook(stageName, elm);
            }

            // トランジション指定
            if (!setTrans2(stages[stage].trans)) {
                if (!setTrans2(env.envinfo.stageTrans)) {
                    setTrans2(env.envinfo.envTrans);
                }
            }

        }
        // イベント絵は消去
        if (event.disp != CLEAR) {
            event.disp = CLEAR;
            redraw = true;
        }
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
                value.setRedraw();
            });

            // トランジション指定
            if (!setTrans2(times[time].trans)) {
                if (!setTrans2(env.envinfo.timeTrans)) {
                    setTrans2(env.envinfo.envTrans);
                }
            }
        }
        // イベント絵は消去
        if (event.disp != CLEAR) {
            event.disp = CLEAR;
            redraw = true;
        }
    }

    // トランジションモード
    var transMode;

    /**
     * 全体トランジション開始
     */
    function beginTrans(elm) {
        kag.fore.base.stopTransition();
        kag.backupLayer(EMPTY, true);
        transMode = true;
        return 0;
    }

    /**
     * トランジション指定をコマンドから探す処理
     */
    function checkTrans(cmd, param, elm) {
        if (cmd == "tagname") {
            // ignore
        } else if (cmd == "trans") {
            setTrans(param, elm);
        } else if (cmd == "fade") {
            var fadeTime = +param;
            trans = %[ "method" => "crossfade",
                       "children" => true,
                       "time" => fadeTime > 1 ? fadeTime : fadeValue];
        } else {
            _setTrans(cmd, elm);
        }
    }

    /**
     * 全体トランジション終了
     */
    function endTrans(elm) {
        // まず解除（トランジション処理中で true だと処理が登録されないので)
        transMode = false;
        ret = void;
        foreach(elm, checkTrans);
        // 未指定時
        if (trans !== void && trans.method !== void) {
            hideMessage();
            beginTransition(trans);
        } else {
            // 裏画面から書き戻す
            kag.backupLayer(EMPTY, false);
        }
        trans = void;
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
        // 既存データは廃棄
        if (characters[elm.name] !== void) {
            invalidate characters[elm.name];
            delete characters[elm.name];
        }
        // 新規生成
        var ch = getCharacter(elm.name, elm.initname);
        if (ch != null) {
            delete elm.tagname;
            delete elm.name;
            delete elm.initname;
            return ch.tagfunc(elm);
        }
        return 0;
    }        

    /**
     * メッセージ窓のトランジション処理をくみこんだ ON/OFF
     */
    function msgonoff(elm, v) {
        ret = void;
        if (!v) {
            kag.conductor.pendings.insert(0, %[ tagname : "er", all:true]);
        }
        if (!transMode && !isSkip() && elm.nofade === void) {
            foreach(elm, checkTrans);
            if (trans !== void && trans.method !== void) {
                kag.updateBeforeCh = 1;
                kag.fore.base.stopTransition();
                kag.backupLayer(EMPTY, true);
                kag.setCurrentMessageLayerVisibleFast(1, v);
                beginTransition(trans);
                return ret;
            } else {
                return kag.setCurrentMessageLayerVisible(v) ? -2 : 0;
            }
        }
        kag.setCurrentMessageLayerVisibleFast(transMode ? 1: 0, v);
        return ret;
    }

    function msgon(elm) { return msgonoff(elm, true); }
    function msgoff(elm) { return msgonoff(elm, false); }

    /**
     * 画面の表示要素の全消去
     */
    function clear(elm) {
        ret = void;
        kag.conductor.pendings.insert(0, %[ tagname : "er", all:true]);
        hideAll();
        if (!transMode && !isSkip()) {
            foreach(elm, checkTrans);
            if (trans !== void && trans.method !== void) {
                kag.updateBeforeCh = 1;
                kag.fore.base.stopTransition();
                kag.backupLayer(EMPTY, true);
                kag.setCurrentMessageLayerVisibleFast(1, false);
                drawAll(kag.back);
                beginTransition(trans);
                return ret;
            }
        }
        kag.setCurrentMessageLayerVisibleFast(transMode ? 1: 0, false);
        drawAll();
        return ret;
    }

    var envCommands = %[
    /**
     * 全体の初期化処理
     */
    init : this.init incontextof this,
    stage : this.setStage incontextof this,
    stime : this.setTime incontextof this,
    hidebase : this.hideBase incontextof this,
    hideevent : this.hideEvent incontextof this,
    hidecharacters : this.hideCharacters incontextof this,
    hidelayers : this.hideLayers incontextof this,
    hidefore : this.hideFore incontextof this,
    hideall : this.hideAll incontextof this,
    stopallvoice : this.stopAllVoice incontextof this,
    colorall : this.setColorAll incontextof this,
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
                    
                    if (reposition) {
                        if (xpos === void) {
                            xpos = (int)stages[stage].xoff;
                        }
                        if (ypos === void) {
                            ypos = (int)stages[stage].yoff;
                        }
                    }

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

                        if (reposition) {
                            if (xpos === void) {
                                xpos = (int)stages[stage].xoff;
                            }
                            if (ypos === void) {
                                ypos = (int)stages[stage].yoff;
                            }
                        }
                        
                    } catch (e) {
                        dm("背景画像がロードできません" + image);
                    }
                }

            } else {
                dm("時間のデフォルト指定が存在していません");
            }


        } else {
            layer.clearImage(true);
        }

        if (grayscale) {
            layer.doGrayScale();
        }
        if (rgamma != void || ggamma != void || bgamma != void) {
            layer.adjustGamma(rgamma == void ? 1.0 : rgamma, 0, 255,
                              ggamma == void ? 1.0 : ggamma, 0, 255,
                              bgamma == void ? 1.0 : bgamma, 0, 255);
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
        initLayer(layer);

        drawStage(layer);
        
        // 子要素の再描画
        foreach(characters, function(name, value, dict, base) {
            value.updateImage(base);
        }, base);
        foreach(layers, function(name, value, dict, base) {
            value.updateImage(base);
        }, base);
        event.updateImage(base);

        // メッセージ窓初期化
        if (currentNameTarget == void) {
            drawNamePage(base);
            if (faceLevelName !== void) {
                clearFacePage(base);
            }
        }
        
        updateLayer(layer);
        calcPosition(layer);
    }

    /**
     * レイヤ配置処理(背景用)：中央原点
     * @param layer 処理対象レイヤ
     */
    function calcPosition(layer) {
        if (reposition) { 
            //dm("位置指定2");
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
    
    /**
     * 再描画処理
     */
    function updateImage() {

        kag.updateBeforeCh = 1;

        // 描画更新が必要な場合
        if (redraw) {
            
            if (trans !== void && trans.method !== void) {
                kag.fore.base.stopTransition();
                
                // 更新処理を走らせる場合
                if (trans.time === void) {
                    trans.time = 1000;
                }
                trans.children = true;

                // 全レイヤをバックアップ
                kag.backupLayer(EMPTY, true);
                
                // ステージの描画
                drawAll(kag.back);
                
                // トランジション実行
                beginTransition(trans);
                
            } else {

                var layer = getLayer();

                // フェード判定
                // 環境に対する命令の場合は常にトランジションを使う
                if (fadeTime !== void) {

                    kag.fore.base.stopTransition();
                    var trans = %[ "method" => "crossfade",
                                   "children" => true,
                                   "time" => fadeTime > 1 ? fadeTime : fadeValue];
                    fadeTime = void;

                    // 全レイヤを裏にバックアップ
                    kag.backupLayer(EMPTY, true);

                    // 裏レイヤが対象
                    drawAll(kag.back);

                    // トランジション実行
                    beginTransition(trans);

                } else {
                    drawAll();
                }
            }
            redraw = false;
        } else {
            var layer = getLayer();
            updateLayer(layer);
            calcPosition(layer);
        }
        trans = void;
    }

    /**
     * KAG タグ処理
     * @param elm 引数
     */
    function tagfunc(elm) {
        //dm("環境タグがよばれたよ");
        ret = void;
        foreach(elm, doCommand);
        hideMessage();
        updateImage();

        // カレントオブジェクトの解除 XXX ここでいいのか？
        return ret;
    }

    /**
     * 全キャラにコマンド実行
     * @param elm 引数
     */
    function allchar(elm) {
        ret = void;
        var names = [];
        names.assign(characters);
        for (var i=0; i<names.count; i+= 2) {
            ret = names[i+1].tagfunc(elm);
        }
        return ret;
    }

    /**
     * 全レイヤにコマンド実行
     * @param elm 引数
     */
    function alllayer(elm) {
        ret = void;
        var names = [];
        names.assign(layers);
        for (var i=0; i<names.count; i+= 2) {
            ret = names[i+1].tagfunc(elm);
        }
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

    /**
     * 全キャラのボイスを停止する
     */
    function resetAllVoiceVolume() {
        foreach(characters, function(name, value, dict) {
            value.resetVoiceVolume();
        });
    }
    
    /*
     * 指定したキャラで指定したボイスファイルを再生
     */
    function playVoice(name, voicename) {
        var ch = getCharacter(name);
        if (ch !== void && voicename !== void) {
            return ch.playVoice(voicename);
        }
    }

    function quake(elm) {
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
            kag.historyLayer.reline(); // XXX 要検討
        }
    }

    function getMessageLayer(base) {
        if (base === void) {
            base = transMode ? kag.back : kag.fore;
        }
        return base.messages[kag.currentNum];
    }

    function getFaceLayer(base) {
        return getMessageLayer(base).faceLayer;
    }
    
    /**
     * 表情表示処理下請け
     */
    function loadFacePage(base, name) {
        var faceLayer = getFaceLayer(base);
        try {
            faceLayer.loadImages(name);
        } catch (e) {
            dm("表情画像のロードに失敗しました:" + name);
        }
        faceLayer.visible = true;
    }

    /**
     * 表情消去処理下請け
     */
    function clearFacePage(base) {
        if (envinfo.clearFace !== void) {
            loadFacePage(base, envinfo.clearFace);
        } else {
            var faceLayer = getFaceLayer(base);
            faceLayer.visible = false;
        }
    }
    
    /**
     * 表情消去処理
     */
    function clearFace() {
        if (!transMode) {
            clearFacePage(kag.fore);
        }
        clearFacePage(kag.back);
    }

    // 指定されたキャラクタの表情が表示可能なら表示する
    function drawFacePage(base, ch) {
        if (faceLevelName !== void) {
            if (ch !== void) {
                var layer = getFaceLayer(base);
                if (ch.isShowFace()) {
                    ch.drawFace(layer, faceLevelName);
                } else {
                    layer.visible = false;
                }
            } else {
                clearFacePage(base);
            }
        }
    }

    /**
     * 名前の表示（ページ指定あり)
     */
    function drawNamePage(base, name="") {
        var msg = getMessageLayer(base);
        msg.processName(name);
    }

    /**
     * 名前の表示
     */
    function drawName(name = "") {
        if (!transMode) {
            drawNamePage(kag.fore, name);
        }
        drawNamePage(kag.back, name);
    }

    var voiceCharacters = [];

    function getVoicePlayingScript(ch) {
        var voice;
        if (ch !== void && (voice = ch.getCurrentVoice()) !== void) {
            return "global.world_object.env.playVoice(\"" + ch.name + "\",\"" + voice + "\");";
        } else {
            return "";
        }
    }
    
    /**
     * 次回同時に鳴らすボイスの追加
     */
    function entryNextVoice(ch) {
        voiceCharacters.add(ch);
    }
    
    function getNextVoiceScript() {
        var ret = "";
        for (var i=voiceCharacters.count-1;i>=0;i--) {
            ret += getVoicePlayingScript(voiceCharacters[i]);
        }
        return ret;
    }

    /**
     * ならすボイスがあるか
     */
    function checkNextVoice() {
        for (var i=voiceCharacters.count-1;i>=0;i--) {
            var voice = voiceCharacters[i].getCurrentVoice();
            if (voice !== void) {
                return true;
            }
        }
        return false;
    }

    /**
     * 複数ボイスを鳴らす
     */
    function playNextVoice() {
        var ret = void;
        for (var i=voiceCharacters.count-1;i>=0;i--) {
            var ch = voiceCharacters[i]; 
            var voice = ch.getCurrentVoice();
            if (voice !== void) {
                var r = ch.playVoice();
                if (ret === void || (r !== void && r > ret)) {
                    ret = r;
                }
            }
        }
        voiceCharacters.clear();
        return ret;
    }
    
    /**
     * 名前表示処理ハンドラ
     */
    function dispname(elm) {

        ret = void;
        
        if (kag.sflags.voicecut) {
            stopAllVoice();
        }

        // 表情表示部
        
        //dm("名前表示ハンドラ");
        if (elm === void || elm.name === void || elm.name == "") {
            
            // キャラ指定が無い場合
            drawName();
            
            // 表情処理
            if (faceLevelName !== void && currentNameTarget !== void && currentNameTarget.isShowFace() && !transMode && !isSkip() && faceFadeTime > 0) {
                // フェースをフェードで消去する場合
                kag.backupLayer(EMPTY, true);
                clearFacePage(kag.back);
                beginTransition(%[ method: "crossfade", time: faceFadeTime]);
                
            } else {
                if (faceLevelName !== void) {
                    clearFace();
                }
            }

            currentNameTarget = void;
            dispnameVoice(elm);
            
        } else {

            var name = elm.name;
            var disp = elm.disp;
            var ch = getCharacter(name);

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
            elm.dispName = dispName;
            
            // 消去状態なら顔表示状態にする
            if (ch !== void && ch.disp == CLEAR && ch.poses) {
                ch.disp = FACE;
            }
            
            // 表情変更処理
            if (faceLevelName !== void && currentNameTarget != ch &&
                ((currentNameTarget !== void && currentNameTarget.isShowFace()) || (ch !== void && ch.isShowFace())) &&
                !transMode && !isSkip() && faceFadeTime > 0) {
                
                // フェードで表示する場合

                // XXX 名前消去
                drawName();
                
                currentNameTarget = ch;
                
                kag.backupLayer(EMPTY, true);
                if (ch !== void && ch.isShowFace()) {
                    drawFacePage(kag.back, ch);
                } else {
                    var img;
                    if (envinfo.nameFaces !== void && (img = envinfo.nameFaces[envinfo.dispNameFace ? dispName : name]) !== void) {
                        loadFacePage(kag.back, img);
                    } else {
                        clearFacePage(kag.back);
                    }
                }

                // ボイス再生処理の先送り
                var e = %[];
                (Dictionary.assign incontextof e)(elm, false);
                e.tagname = "dispnameVoice";
                kag.conductor.pendings.insert(0, e);

                beginTransition(%[ method: "crossfade", time: faceFadeTime]);

            } else {

                // 瞬間表示する場合
                // たぶんここのロジック必要ない？
                
                currentNameTarget = ch;
                
                if (ch !== void && ch.isShowFace()) {
                    if (!transMode) {
                        drawFacePage(kag.fore, ch);
                    }
                    drawFacePage(kag.back, ch);
                } else {
                    var img;
                    if (envinfo.nameFaces !== void && (img = envinfo.nameFaces[envinfo.dispNameFace ? dispName : name]) !== void) {
                        if (!transMode) {
                            loadFacePage(kag.fore, img);
                        }
                        loadFacePage(kag.back, img);
                    } else {
                        clearFace();
                    }
                }
                dispnameVoice(elm);
            }
        }
        return ret;
    }

    /**
     * 名前表示＋ボイス再生部分
     * ボイスの状態に応じた待ち時間補正も行う
     */
    function dispnameVoice(elm) {

        // -------  名前表示（瞬間）

        if (elm === void || elm.name === void || elm.name == "") {

            // すでに名前表示済み
            
        } else {
            var name     = elm.name;
            var dispName = elm.dispName;
            if (dispName === void) {
                dispName = name;
            }

            var ch = getCharacter(name);

            // ボイスの登録
            var nextVoice;
            if (kag.historyWriteEnabled && (nextVoice = getNextVoiceScript() + getVoicePlayingScript(ch)) != "") {
                kag.historyLayer.setNewAction("global.world_object.env.stopAllVoice();" + nextVoice);
            }

            // 名前加工処理
            if (typeof global.dispNameFilter !== 'undefined') {
                dispName = global.dispNameFilter(dispName);
            }
            // ヒストリ用
            if (kag.historyWriteEnabled) {
                if (typeof kag.historyLayer.storeName !== 'undefined') {
                    kag.historyLayer.storeName(dispName);
                } else {
                    kag.historyLayer.store(dispName + " ");
                }
            }

            drawName(dispName);
        }

        // -------  ボイス再生
        
        // ボイス再生
        var ret;
        if (currentNameTarget !== void) {
            // ボイス再生がある場合
            if (currentNameTarget.getCurrentVoice() !== void || checkNextVoice()) {
                // ほかのボイスを消去する
                env.stopAllVoice();
                // 同時再生ボイスの再生
                ret = currentNameTarget.playVoice();
                var r = playNextVoice();
                if (ret === void || (r !== void && r > ret)) {
                    ret = r;
                }
            }
        } else {
            // ボイス再生がある場合
            if (checkNextVoice()) {
                // ほかのボイスを消去する
                env.stopAllVoice();
                // 同時再生ボイスの再生
                ret = playNextVoice();
            }
        }
        kag.addAutoWait(ret);
        return 0;
    }

    /**
     * ページ処理後に呼び出される処理
     */
    function afterpage(elm) {
        if (kag.sflags.voicecutpage) {
            stopAllVoice();
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
        } else if (tagName.substring(0,2) == "ev" ||
                   (events !== void && events[tagName] !== void)) {
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
        
        // カレントのコマンドとして実行してみる
        if (currentNameTarget !== void) {
            elm[tagName] = true;
            return currentNameTarget.tagfunc(elm);
        }
    }

    /**
     * アクション停止
     */
    function onStopAction() {
        stopAction();
        event.stopAction();
        foreach(characters, function(name, value, dict) {
            value.stopAction();
        });
        foreach(layers, function(name, value, dict) {
            value.stopAction();
        });
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
