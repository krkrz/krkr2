/**
 * 環境オブジェクト
 */
class Environment extends EnvBase {
	
	xmax = null;
	ymax = null;

    // フェード指定のデフォルト値
    // envinit.tjs で定義するか、システムのデフォルトを使う
	function getFadeValue() {
		if ("fadeValue" in envinfo) {
			return envinfo.fadeValue;
		} else {
			return 500;
		}
    }
    
    /// 初期化情報
	envinfo = null;

    times= null;        //< 時間情報
    stages= null;       //< 舞台情報
    events= null;       //< イベント絵情報
    positions= null;    //< 配置情報
    actions= null;      //< アクション情報
    transitions= null;  //< トランジション情報
    defaultTime= null;  //< デフォルトの時間
    yoffset= null;      //< キャラクタ配置のyoffset 値
    defaultXpos= null;  //< キャラクタ配置の初期X位置
    defaultYpos= null;  //< キャラクタ配置の初期Y位置
    defaultLevel= null; //< キャラクタレベルのデフォルト値
	levels= null;       //< キャラクタレベル別補正情報
    faceLevelName= null;//< フェイスウインドウ用の表示名
    
    /// キャラクタ初期化情報一覧
	characterInits = {};

	/// 舞台レイヤ
	stage= null;
	/// イベントレイヤ
	event= null;
	/// キャラクタ情報
	characters = {};
    /// レイヤ情報
	layers = {};

    // BGM 系
    bgm = null;

    // SE 系
    ses = null;
    
    //　現在時刻
	function getCurrentTime() {
		if (stage.timeName != null && stage.timeName in times) {
			return times[stage.timeName];
		}
    }

    // メッセージ窓処理対象になるデフォルトのオブジェクト
    // 名前表示のあと設定される
    // 環境系命令の後は解除される
	currentNameTarget = null;

	showFaceMode = false; // コマンド実行毎に表情処理を行うモード
	bothFace = true;     // 表情表示を立ち絵同時可能に

	envCommands = null;
	
    /**
     * コンストラクタ
	 * @param player 再生プレイヤー
	 * @param width 画面横幅
	 * @param height 画面縦幅
     */
	function constructor(player, width, height) {

		this.player = player;
		::EnvBase.constructor(this, "env");
		
		xmax = width / 2;
		ymax = height / 2;
		
		characters = {};
		layers = {};
		event = EnvEventLayer(this);
		stage = EnvStageLayer(this);

		// BGM オブジェクト
		bgm = EnvBgm(this);
		// SE オブジェクト群
		ses = [];
		for (local i=0; i<player.senum; i++) {
			ses.append(EnvSE(this, i))
		}
		
		resetEnv();

		envCommands = {
			hidebase = hideBase,
			hideevent = hideEvent,
			hidecharacters = hideCharacters,
			hidechars = hideCharacters,
			hidelayers = hideLayers,
			hidefore = hideFore,
			hideall = hideAll,
		};
    }

	// ------------------------------------------------------

    /**
	 * 環境のリセット
	 */
	function resetEnv() {

		initEnv(null);

		// キャラクタ情報廃棄
		foreach (name,value in characterInits) {
			delete characterInits[name];
		}
		
		// 初期化情報展開
		envinfo = dofile("envinit.nut");
		
		if (envinfo != null) {
            // デバッグ表示 
			times        = getval(envinfo, "times", EMPTY);
			stages       = getval(envinfo, "stages", EMPTY);
			events       = getval(envinfo, "events", EMPTY);
			positions    = getval(envinfo, "positions", EMPTY);
            actions      = getval(envinfo, "actions", EMPTY);
			transitions  = getval(envinfo, "transitions", EMPTY);
			levels       = getval(envinfo, "levels", EMPTY);
			defaultTime  = getval(envinfo, "defaultTime");
			yoffset      = getval(envinfo, "yoffset", 0);
			defaultXpos  = getval(envinfo, "defaultXpos", 0);
			defaultYpos  = getval(envinfo, "defaultYpos", 0);
			defaultLevel = getval(envinfo, "defaultLevel", 0);
			faceLevelName = getval(envinfo, "faceLevelName", "");
			showFaceMode  = getval(envinfo, "showFaceMode", false);
			bothFace      = getval(envinfo, "bothFace", true);
        
			// キャラクタ情報初期化処理
			if ("characters" in envinfo) {
				foreach (name, init in envinfo.characters) {
					characterInits[name] <- init;
				}
			}
			
        } else {
			dm("環境情報がありません");
			envinfo = EMPTY;
			times   = EMPTY;
			stages  = EMPTY;
			events  = EMPTY;
			positions = EMPTY;
			actions   = EMPTY;
			transitions = EMPTY;
			levels = EMPTY;
			defaultTime = null;
			yoffset     = 0;
			defaultXpos = 0;
			defaultYpos = 0;
			defaultLevel = 0;
			faceLevelName = "";
			showFaceMode = false;
			bothFave = true;
		}
	}
	
    // -----------------------------------------

    // -----------------------------------------

	/**
	 * 環境情報の初期化
     */
	function initEnv(elm) {

		//dm("初期化処理");
		stage.initImage();
		event.initImage();

		// キャラクタ情報の破棄
		foreach (name,value in characters) {
			delete characters[name];
		}
        // 特殊レイヤ情報の破棄
		foreach (name,value in layers) {
			delete layers[name];
		}

        // SE 初期化
		for (local i=0;i<ses.len();i++) {
			ses[i].init();
			ses[i].sync();
		}
        seCount = 0;

		// BGM初期化
		if (elm == null || !getval(elm, "bgmcont")) {
			bgm.init();
		}
		
        // カレントオブジェクト初期化
		currentNameTarget = null;
    }

    /**
     * イベント絵の消去
     */
    function hideEvent(param, elm) {
		if (event.isShow()) {
			event.disp = EnvImage.CLEAR;
		}
    }

    /**
     * イベント絵の消去
     */
	function hideStage(param, elm) {
		if (stage.isShow()) {
			stage.disp = EnvImage.CLEAR;
		}
    }
	
    /**
     * 背景とイベント絵の消去
     */
    function hideBase(param, elm) {
		hideEvent(param, elm);
		hideStage(param, elm);
    }
    
    /**
     * 全キャラクタ消去
     */
	function hideCharacters(param, elm) {
		// キャラクタレイヤの消去
		foreach (name,value in characters) {
			if (value.isShow()) {
				value.disp = EnvImage.CLEAR;
			}
		}
    }

    /**
     * 全レイヤ消去
     */
    function hideLayers(param, elm) {
        // 特殊レイヤの消去
		foreach (name,value in layers) {
			if (value.isShow()) {
				value.disp = EnvImage.CLEAR;
			}
		}
    }

    /**
     * 前景要素消去
     */
    function hideFore(param, elm) {
        hideCharacters(param, elm);
        hideLayers(param, elm);
    }

    /**
     * 全要素消去
     */
    function hideAll(param, elm) {
		hideBase(param, elm);
		hideCharacters(param, elm);
        hideLayers(param, elm);
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
		
		if (cmd in envCommands) {
			local func = envCommands[cmd];
			if (func != null) {
				func(param, elm);
			}                
			return true;
        }

		return false;
    }

	function update(isfore) {
		//dm("環境更新");
		foreach (name,value in layers) {
			value.update(isfore);
		};
		event.update(isfore);
		// XXX キャラ更新はステージ更新に含まれる
		stage.update(isfore);
	}

	// ------------------------------------------------------------------
	// 一括実行
	// ------------------------------------------------------------------

	/**
	 * 全キャラにコマンド実行
	 * @param elm 引数
	 * poscond パラメータで表示場所限定可能
     */
    function allchar(elm) {
		ret = null;
		local posName;
		if ("poscond" in elm) {
			posName = elm.poscond;
			delete elm.poscond;
		}
		foreach (name,ch in characters) {
			if (posName != null) {
				if (ch.posName == posName && ch.isShowBU()) {
					ret = ch.tagfunc(elm);
				}
			} else {
				ret = ch.tagfunc(elm);
			}
        }
        return ret;
    }

    /**
     * 全レイヤにコマンド実行
     * @param elm 引数
     */
    function alllayer(elm) {
		ret = null;
		foreach (name,value in layers) {
			ret = value.tagfunc(elm);
        }
        return ret;
    }

    /**
     * 全SEにコマンド実行
     * @param elm 引数
     */
    function allse(elm) {
        ret = null;
		for (local i=0;i<ses.len();i++) {
			if (ses[i].name != null) {
				ret = ses[i].tagfunc(elm);
			}
		}
		return ret;
    }

    /**
     * 新規レイヤ生成
     */
    function newLayer(elm) {
		// 既存データは廃棄
		delLayer(elm);
		// 新規生成
		if ("name" in elm) {
			local lay = getEnvLayer(elm.name, true);
			if (lay != null) {
				delete elm.tagname;
				delete elm.name;
				return lay.tagfunc(elm);
			}
		} else {
			player.error("レイヤ名が指定されていません");
		}
		return 0;
    }

    /**
     * 新規レイヤ生成
     */
    function delLayer(elm) {
		if ("name" in elm && elm.name in layers) {
			delete layers[elm.name];
		}
        return 0;
    }
    
    /**
     * 新規キャラクタ生成
     */
    function newCharacter(elm) {
		// 既存データは廃棄
		delCharacter(elm);
        // 新規生成
		if ("name" in elm && "initname" in elm) {
			local ch = getCharacter(elm.name, elm.initname);
			if (ch != null) {
				delete elm.tagname;
				delete elm.name;
				delete elm.initname;
				return ch.tagfunc(elm);
			}
		} else {
			player.error("キャラクタ名または初期化名が指定されていません");
		}
        return 0;
    }        

	/**
     * キャラクタ操作
     */
	function delCharacter(elm) {
		if ("name" in elm && elm.name in characters) {
			delete characters[elm.name];
		}
		return 0;
	}
    
	/**
	 * レイヤ一括複製指定
	 */
	function onCopyLayer(toback, backlay) {
		foreach (name, value in characters) {
			value.onCopyLayer(toback, backlay);
		}
		foreach (name, value in layers) {
			value.onCopyLayer(toback, backlay);
		}
		stage.onCopyLayer(toback, backlay);
		event.onCopyLayer(toback, backlay);
	}

 	// ------------------------------------------------------------------
	// キャラクタ関係処理
	// ------------------------------------------------------------------

    /**
     * 指定された名前のキャラクタを返す
     * @param name 名前
     * @param initName 初期化名
     */
    function getCharacter(name, initName) {
		local ch;
		if (name in characters) {
			ch = characters[name];
		} else {
			if (initName == null) {
				initName = name;
			}
			if (initName in characterInits) {
				ch = EnvCharacter(env, name, initName, characterInits[initName]);
				characters[name] = ch;
            }
        }
        return ch;
    }

	// ------------------------------------------------------------------
	// レイヤ関係処理
	// ------------------------------------------------------------------

	/**
    * 指定された名前のレイヤを返す
     * @param name 名前
	 * @param create 生成モード
     */
	function getEnvLayer(name, create) {
		local lay;
		if (name in layers) {
			lay = layers[name];
		}
		if (create) {
			lay = EnvSimpleLayer(env, name);
			layers[name] = lay;
		}
        return lay;
    }

	// --------------------------------------------------------------
	// SE 処理用
	// --------------------------------------------------------------

	seCount = 0;

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
     * SE の ID を決定する
     * @param buf バッファIDを指定
     */
    function getSeId(buf) {
		// 直接バッファが指定されている場合はそれを返す
		if (buf != null && buf.tointeger() < ses.len()) {
			return buf.tointeger();
        }
        // 使われてないものをさがす
        local max = seCount;
		local old = null;
        for (local i=0; i<ses.len(); i++) {
            if (ses[i].name == null) {
				return i;
            }
			if (ses[i].len() < max) {
				max = ses[i].len();
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
        for (local i=0; i<ses.len(); i++) {
            if (ses[i].name == name) {
                return i;
            }
		}
        // みつからないのであいている番号を返す
        return getSeId();
    }

    /**
     * SE 停止時の処理
     * 停止中状態にする
     */
    function onSeStop(id) {
        if (id < ses.len()) {
            ses[id].name = null;
        }
    }

	// --------------------------------------------------------------
	
    /**
	 * 不明コマンド処理ハンドラ
     */
    function unknown(tagName, elm) {

		//dm("環境不明コマンド処理:" + tagName);

		// ステージ用処理
		if (tagName == "stage") {
			return stage.tagfunc(elm);
		}
		
		// イベント用処理
		if (tagName == "event" || tagName == "ev") {
			return event.tagfunc(elm);
		} else if (tagName.slice(0,2) == "ev" || (tagName in events)) {
			elm[tagName] <- true;
			return event.tagfunc(elm);
        }

        // BGM 処理用
        if (tagName == "bgm") {
            return bgm.tagfunc(elm);
        } else if (tagName.slice(0,3) == "bgm") {
            return bgm.tagfunc(elm);
        }

        // SE 処理用
        if (tagName == "se") {
			if (elm.name != null) {
				return getSe(getSeIdFromName(elm.name)).tagfunc(elm);
			} else {
				return getSe(getSeId(elm.buf)).tagfunc(elm);
			}
        } else if (tagName.slice(0,2) == "se") {
			local se = getSe(getSeIdFromName(tagName));
			return se.tagfunc(elm);
        }
        
        // キャラクタ
        if (tagName == "char") {
            local ch = getCharacter(elm.name);
            if (ch != null) {
                return ch.tagfunc(elm);
            }
		} else {
			local ch = getCharacter(tagName);
			if (ch != null) {
				return ch.tagfunc(elm);
			}
		}

        if (tagName == "layer") {
            local lay = getEnvLayer(elm.name, false);
			if (lay != null) {
                return lay.tagfunc(elm);
            }
		} else { 
			local lay = getEnvLayer(tagName, false);
			if (lay != null) {
				return lay.tagfunc(elm);
			}
		}

		// 時間指定か背景指定ならステージ用コマンドとみなす
		if (tagName in times || tagName in stages) {
			elm.tagname  <- "stage";
			elm[tagName] <- true;
			return stage.tagfunc(elm);
		}
		
		// 環境のコマンド
		if (tagName in envCommands) {
			elm.tagname  <- "env";
			elm[tagName] <- true;
			return tagfunc(elm);
		}

		// カレントのターゲットがある場合はそのコマンドとして実行
		if (currentNameTarget != null) {
			elm[tagName] <- true;
			return currentNameTarget.tagfunc(elm);
		}

		// それ以外だとエラーになる
	}

    /**
	 * 環境同期
	 */
	function syncAll() {
		dm("環境全同期");
		for (local i=0; i<player.senum; i++) {
			ses[i].sync();
		}
		bgm.sync();
		foreach (name,value in layers) {
			value.sync();
		};
		event.sync();
		stage.sync();
		return 0;
	}
};
