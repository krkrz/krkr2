/**
 * シーン再生プレイヤー
 * ・シーンの再生機能
 * ・シーンのセーブとロード
 */
class ScenePlayer extends GamePlayer
{

	/**
	 * 未読管理
	 * すべてスキップ
	 */
	function isAllSkip() {
		return false;
	}

	/**
	 * 未読管理
	 * @param シーン
	 */
	function getReaded(scene) {
		return false;
	}

	/**
	 * 未読管理
	 * @param scene シーン
	 * @param 行番号
	 */
	function setReaded(scene, line) {
	}

	/**
	 * キャラ表示速度
	 */
	function getCharSpeed() {
		return 0;
	}

	env = null; // 環境情報
	
	lines = null;    // パース結果
	pendings = []; // 割り込み処理

	/**
	 * タグ割り込み処理
	 */
	function addTag(name, elm) {
		local e = clone elm;
		if (name != null) {
			e.tagname = name;
		}
		pendings.append(e);
    }
	
	handlers = null;      // 処理ハンドラ群
	
	// 実行情報
	curStorage = null;        // 実行中のシナリオ
	cur = 0;               // 実行ポイント
	curLine = 0;           // 通過中の行番号
	targetLine = null;        // ロード処理ターゲット行
	nextScene = null;         // 次のシーン

	/**
	 * 実行ジャンプ中か
	 */
	function isJump() {
		return targetLine != null && targetLine < curLine;
	}
	
	// 選択処理通過記録
	branches = {};
	
	lastTagName = ""; // 直前のタグ名

	sceneInfos = null; // シーン情報
	
	/**
	 * コンストラクタ
	 */
	constructor() {
		::GamePlayer.constructor();
		env = Environment(this, SCWIDTH, SCHEIGHT);
		sceneInfos = dofile("scenes.nut");
		handlers = getHandlers();
	}

	/**
	 * 情報クリア
	 */
	function clear() {
		// XXX 親のクリアを呼ぶ
		lastTagName = "";
		foreach (name,value in branches) {
			delete branches[name];
		}
		curStorage = null;
		cur = 0;
		curLine = 0;
		targetLine = null;
		nextScene = null;
	}

	// ------------------------------------------------------------
	
	/**
	 * タグの処理
	 */
	function onTag(elm) {
		local tagname = elm.tagname;
		//dm("シーンタグ:" + tagname);
		if (tagname in handlers) {
			local handler = handlers[tagname];
			local ret = handler(elm);
			lastTagName = tagname;
			return ret;
		}
		// 無効なタグ
		local ret = env.unknown(tagname, elm);
		if (ret == null) {
			local msg = "タグ/マクロ \"" + tagname + "\" は存在しません";
			errorCmd(msg);
			return 0;
			//throw new Exception(msg);
		}
		return ret;
	}

	// --------------------------------------------------------------

	/**
	 * シーンファイルを変換してデータとして取得する
	 * @param name シーン名
	 */
	function getScene(name) {
		// グローバルのシーン取得関数を使う
		return ::getScene(name);
	}

	/**
	 * 次シーン判定処理
	 * @param name 現在のシーン名
	 */
	function getNextScene(name) {
		if (name in sceneInfos) {
			local info = sceneInfos[name];
			if ("next" in info) {
				local next = info.next;
				for (local i=0;i<next.len();i++) {
					if (!("cond" in next) || next.cond == "" || eval(next.cond)) {
						return next.scene;
					}
				}
			}
		}
	}

	// --------------------------------------------------------------

	
	/**
	 * 実行開始
	 * @param storage 開始シナリオ
	 * @param option パース用オプション
	 */
	function start(storage) {
		clear();
		curStorage = storage;
		lines = getScene(curStorage);
		status = RUN;
	}
	
	/**
	 * 状態保存
	 */
	function save(dic) {
		dic.status <- status;
		if (status != STOP) {
			dic.storage  <- curStorage;
			dic.branches <- clone branches;
			if (curLine > 0) {
				dic.targetLine <- curLine;
			}
		}
		return dic;
	}

	/**
	 * 状態復帰
	 * パラメータ: storage
	 */
	function load(dic) {
		clear();
		if (dic.status != STOP) {
			curStorage = dic.storage;
			branches = clone dic.branches;
			lines = getScene(curStorage);
			targetLine = dic.targetLine;
			status = RUN;
		}
	}

	/**
	 * 指定ラベルまで移動
	 */
	function goToLabel(label) {
		local newcur = 0;
		local obj;
		while ((obj = lines[newcur++]) != null) {
			if (obj.tagname == "label" && obj.label == label) {
				cur = newcur;
				return;
			}
		}
		throw "ラベル" + label + "がみつかりません";
	}

	
	/**
	 * メインロジック
	 */
	function run()	{
		local obj;
		for(;;)	{
			if (pendings.len() > 0) {
				// 後回しにされたタグがある場合
				obj = pendings[0];
				pendings.remove(0);
			} else {
				obj = lines[cur++];
			}
			
			if(obj == null) {
				// シナリオ終了
				local next = nextScene != null ? nextScene : getNextScene(curStorage);
				if (next != null) {
					clear();
					lines = getScene(next);
					status = RUN;
					continue;
				} else {
					status = STOP;
					return;
				}
			} else if (obj.tagname == "line") {
				if (curLine != obj.line) {
					curLine = obj.line;
					//dm("%06d:%s %s".sprintf(curLine, curStorage, obj.lineStr));
				}
				continue;
			} else if (obj.tagname == "label") {
				continue;
			} else {
				// 実行時判定される cond
				if ("condex" in obj) {
					if (!eval(obj.condex)) {
						continue;
					}
					delete obj.condex;
				} else if ("if" in obj) {
					if (!eval(obj["if"])) {
						continue;
					}
					delete obj["if"];
				}
				
				// onTag を呼ぶ
				local step = onTag(obj);
				if (step == null) {
					throw "onTag が null を返しました (" + obj.tagname + ")";
				}
				step = step.toingter(); // step を数値に
				//dm("タグ戻り値:" + step);
				
				if (step == 0) {
					// ウェイトを掛けずに次へ
					continue;
				}
				if(step < 0) {
					switch(step) {
					case -5: // いったんイベントを処理(現在のタグは後回し)
						pendings.insert(0, obj);
						return;
					case -4: // いったんイベントを処理
						return;
					case -3: // 後回ししてブレーク
						pendings.insert(0, obj);
						return;
					case -2: // ブレーク
						return;
					case -1: // シナリオ終了
						status = STOP;
						return;
					}
				} else {
					// 文字表示待ち用 XXX
					waitTime(step, true);
				}
			}
		}
	}

	// -----------------------------------------------------------

	// トランジション名称
	transitionName = {
		universal = true,
		crossfade = true,
		scroll = true,
		wave = true,
		mosaic = true,
		turn = true,
		rotatezoom = true,
		rotatevanish = true,
		rotateswap = true,
		ripple = true,
	};

	// トランジションパラメータ
	transitionParam = {
		time = true,
		rule = true,
		vague = true,
		from = true,
		stay = true,
		wavetype = true,
		maxh = true,
		maxomega = true,
		bgcolor1 = true,
		bgcolor2 = true,
		mosaic = true,
		maxsize = true,
		turn = true,
		bgcolor = true,
		factor = true,
		accel = true,
		twistaccel = true,
		twist = true,
		centerx = true,
		centery = true,
		rwidth = true,
		roundness = true,
		speed = true,
		maxdrift = true,
		msgoff = true,
		charoff = true,
		transwait = true,
		showaction = true,
		hideaction = true,
	};
	
    /**
     * トランジション情報の取得
     */
    function getTrans(name, elm) {
		if (name == "void" || name == "none") {
			return {};
		}
		if (name in env.transitions) {
			//dm("登録ずみのトランジション:" + name);
			local tr = clone env.transitions[name];
			foreach (name, value in elm) {
				if (name in transitionParam) {
					tr[name] <- value;
				}
			};
			return tr;
		} else if (elm != null &&
				   (name in transitionName || name.slice(0,5) == "trans")) {
			//dm("規定のシステムトランジション:" + name);
			local tr = {};
			foreach (name, value in elm) {
				if (name in transitionParam) {
					tr[name] <- value;
				}
			};
			tr.method <- name;
			return tr;
		}
    }

	/*
	 * トランジション実行エントリ
	 * @param trans トランジション情報
	 * KAG にトランジション実行命令を差し込む
	 */
	function beginTransition(trans) {
		if (trans == null || !("method" in trans) || !("time" in trans) || trans.time == 0 || isSkip()) {
			addTag("forelay");
		} else {
			// 実際のトランジション実行
			if (trans.method == null) {
				trans.method = "crossfade";
			}
			addTag("trans", trans);
			if ("transwait" in trans) {
				// 時間待ち
				local time = trans.time.tointeger() + trans.transwait.tointeger();
				addTag("wait", {time=time, trans=true});
			} else {
				// トランジション待ち
				addTag("wt");
			}
		}
		// メッセージ窓状態同期
		//addTag("syncmsg");
	}
	
	// ------------------------------------------------------------------
	// メッセージ表示系タグ
	// ------------------------------------------------------------------

	function tag_ch(elm) {
		//return kag.tagHandlers.ch(...);
		return 0;
	}

	function tag_r(elm) {
		//return kag.tagHandlers.r(...);
		return 0;
	}

	function tag_ruby(elm) {
		//return kag.tagHandlers.ruby(...);
		return 0;
	}

	function tag_font(elm) {
		//return kag.tagHandlers.font(...);
		return 0;
	}

	function tag_deffont(elm) {
		//return kag.tagHandlers.deffont(...);
		return 0;
	}

	function tag_resetfont(elm) {
		//return kag.tagHandlers.resetfont(...);
		return 0;
	}

	function tag_style(elm) {
		//return kag.tagHandlers.style(...);
		return 0;
	}

	function tag_defstyle(elm) {
		//return kag.tagHandlers.defstyle(...);
		return 0;
	}

	function tag_resetstyle(elm) {
		//return kag.tagHandlers.resetstyle(...);
		return 0;
	}

	function tag_l(elm) {
		//return kag.tagHandlers.l(...);
		return 0;
	}

	function tag_p(elm) {
		//return kag.tagHandlers.p(...);
		return 0;
	}

	function tag_er(elm) {
		//return kag.tagHandlers.er(...);
		return 0;
	}

	function tag_indent(elm) {
		//return kag.tagHandlers.indent(...);
		return 0;
	}
	
	function tag_endindent(elm) {
		//return kag.tagHandlers.endindent(...);
		return 0;
	}

	function tag_delay(elm) {
		//return kag.tagHandlers.delay(...);
		return 0;
	}

	// --------------------------------------------------
	// 制御系タグ
	// --------------------------------------------------
	
	function tag_cancelskip(elm) {
		//return kag.tagHandlers.cancelskip(...);
		return 0;
	}

	timeOrigin = null;
	lastWaitTime = 0;
	
	function tag_resetwait(elm) {
		timeOrigin = currentTick;
		return 0;
	}
	
	function tag_wait(elm) {
		local waittime;
		if (getval(elm, "mode") == "until")	{
			// until モード
			waittime = timeOrigin + getint(elm, "time") - currentTick;
			if (waittime < 0) { lastWaitTime = 0; return 0; } // すでに時間が経過している
			lastWaitTime = waittime;
			if (waittime < 10) return 0; // あまりに待ち時間が短いので待たない
		} else {
			waittime = getint(elm, "time");
		}
		return waitTime(waittime, getint(elm, "canskip", true));
						//getint(elm, "trans"),
						//getint(elm, "action"));
	}

	function tag_waittrig(elm) {
		return waitTrigger(getval(elm, "name"),
						   getval(elm, "timeout"),
						   getval(elm, "canskip", true),
						   getval(elm, "onskip"));
	}
	
	function tag_beginskip(elm) {
		print("beginSkip\n");
		if (!isJump()) {
			if (prevSkipMode != null) {
				print("beginSkip は入れ子できません");
			} else {
				prevSkipMode = skipMode;
				if (skipMode) {
				skipMode = 3;
				}
			}
		}
	}

	function tag_endskip(elm) {
		if (!isJump()) {
			if (skipMode == 3) {
				syncAll();
			}
			skipMode = prevSkipMode;
			prevSkipMode = null;
		}
		return 0;
	}
	
	// --------------------------------------------------
	// 画面制御系
	
	function tag_backlay(elm) {
		//return kag.tagHandlers.backlay(...);
		return 0;
	}

	function tag_forelay(elm) {
		//return kag.tagHandlers.forelay(...);
		return 0;
	}
	
	function tag_trans(elm) {
		//return kag.tagHandlers.trans(...);
		return 0;
	}

	function tag_wt(elm) {
		//return kag.tagHandlers.wt(...);
		return 0;
	}

	// --------------------------------------------------
	// アクション制御系
	
	function tag_action(elm) {
		local target = getval(elm, "target");
		if (typeof target == "string") {
			target = eval(target);
		}
		if (target != null && typeof target == "instance") {
			local action;
			if ("action" in elm) {
				action = elm.action;
			} else {
				action = clone elm;
				delete action.tagname;
				delete action.layer;
				delete action.page;
				delete action.target;
			}
			beginAction(target, action, onActionCompleted, getint(elm, "nowait"));
		} else {
			error("アクションはオブジェクトに指定する必要があります");
		}
		return 0;
	}
	
	function tag_stopaction(elm) {
		if ("target" in elm) {
			local target = elm.target;
			if (typeof target == "string") {
				target = eval(target);
			}
			if (target != null && typeof target == "instance") {
				stopAction(target);
			} else {
				dm("アクションはオブジェクトに指定する必要があります");
			}
		} else {
			stopAllActions(getint(elm, "all", true));
		}
		return 0;
	}

	function tag_wact(elm) {
		if ("target" in elm) {
			local target = elm.target;
			if (typeof target == "string") {
				target = eval(target);
			}
			if (target != null && typeof target == "instance") {
				return waitAction(target, getint(elm, "canskip"));
			} else {
				dm("アクション待ちはオブジェクトに指定する必要があります");
			}
		} else {
			return waitAllAction(getint(elm, "canskip"));
		}
		return 0;
	}

	// --------------------------------------------------
	// 変数系
	
	function tag_eval(elm) {
		if ("exp" in elm) {
			eval(elm.exp);
		}
		return 0;
	}

	function tag_clearsyslocal() {
		::sf = {};
		return 0;
	}

	function tag_clearlocal() {
		::f = {};
		return 0;
	}
	
	function tag_clearplaytime() {
		//return kag.tagHandlers.clearplaytime(...);
		return 0;
	}

	// --------------------------------------------------------------
	// メッセージ窓制御
	// --------------------------------------------------------------

	/**
	 * メッセージ窓の初期化
	 */
	function tag_msgwin(elm) {
		msgsize(getval(elm, "left", msgX), getval(elm, "top", msgY),
				getval(elm, "width", msgWidth), getval(elm, "height", msgHeight));
		return 0;
	}

	/**
	 * メッセージ窓の表示
	 */
	function tag_msgon(elm) {
		if (!isJump()) {
			_msgon();
		}
		return 0;
	}

	/**
	 * メッセージ窓の消去
	 */
	function tag_msgoff(elm) {
		if (!isJump()) {
			_msgoff();
		}
		return 0;
	}
	
	// ------------------------------------------------------------------
	// 特殊コマンド
	// ------------------------------------------------------------------

	// トランジションモード
	transMode = 0;

	/**
	 * 初期化
	 */
	function tag_init(elm) {
		transMode = 0;
		env.initEnv(elm);
		clearTrash();
		return 0;
	}
	
    /**
     * 全体トランジション開始
     */
	function tag_begintrans(elm) {
		backup();
		transMode = 1;
        return 0;
    }

    /**
     * 全体トランジション終了
     */
	function tag_endtrans(elm) {
		// まず解除（トランジション処理中で true だと処理が登録されないので)
		transMode = 0;
		local trans = null;
		foreach (cmd, param in elm) {
			if (cmd == "tagname") {
				// ignore
			} else if (cmd == "trans") {
				local tr = getTrans(param, elm);
				if (tr != null) {
					trans = tr;
				}
			} else if (cmd == "fade") {
				local time = param.tointeger();
				trans = {
					method = "crossfade",
					children = true,
					time = time > 1 ? time : getFadeValue()
					};
			} else {
				local tr = getTrans(cmd, elm);
				if (tr != null) {
					trans = tr;
				}
			}
		}
		if (trans != null && getval(trans, "msgoff")) {
			addTag("msgoff");
		}
		beginTransition(trans);
		return 0;
    }

	/**
	 * レイヤトランジション開始
	 */
	function tag_beginlt(elm) {
		transMode = 2;
		return beginLayerTrans(elm);
	}

	/**
	 * レイヤトランジション終了
	 */
	function tag_endlt() {
		transMode = 0;
		return endLayerTrans(elm);
	}

	// ------------------------------------------------------------------
	
	// 次回再生するボイス情報
	// キー:キャラクタ名
	// 値:ボイス
	nextvoices = {};
	nextvoicecount = 0;

	/**
	 * 次回同時に鳴らすボイスの追加
	 * @param name キャラ名
	 * @param voice  ボイス指定
	 * @param voicen 非デフォルト時ボイス指定
	 */
	function entryNextVoice(name, voice, voicen) {
		if (name != null && voice != null) {
			nextvoices[name] = {voice=voice, voicen=voicen};
			nextvoicecount++;
		}
    }

	/**
	 * 次回鳴らすボイスの情報をクリア
	 */
	function clearNextVoice() {
		nextvoices = {};
		nextvoicecount = 0;
	}

	/**
	 * 複数ボイスを鳴らす
	 * @return 最長のボイス再生時間
     */
	function playVoices(voices) {
		local ret = null;
		if (voices != null) {
			foreach (name, info in voices) {
				local r;
				// デフォルト名の場合の処理
				if (isDefaultName() && info.voicen != null) {
					r = playVoice(name, info.voicen);
				} else {
					r = playVoice(name, info.voice);
				}
				if (ret == null || r != null && r > ret) {
					ret = r;
				}
			}
		}
		return ret;
    }
	
	/**
	 * ボイス再生実行
	 */
	function playNextVoice() {
		if (nextvoicecount > 0) {
			local ret = playVoices(nextvoices);
			clearNextVoice();
			return ret;
		}
	}
	
	// ------------------------

	/**
	 * ボイス追加
	 */
	function tag_entryvoice(elm) {
		entryNextVoice(elm.name, elm.voice, elm.voicen);
		return 0;
	}

	/**
	 * ボイス再生
	 */
	function tag_playvoice(elm) {
		playNextVoice();
		return 0;
	}

	/**
	 * ボイス待ち
	 */
	function tag_waitvoice(elm) {
		if ("name" in elm) {
			return waitVoice(elm.name);
		}
		return 0;
	}
	
	/**
	 * 名前表示
	 */
	function tag_dispname(elm) {

		if (voicecut && !voicecutpage) {
			stopAllVoice();
		}

		local dispName = null;
		local faceName = null;
		
		//dm("名前表示ハンドラ");
		if (elm == null || !("name" in elm)  || elm.name == "") {
			
			env.currentNameTarget = null;

		} else {

			local name = elm.name;
			local disp = elm.disp;

			local ch = env.getCharacter(name);
			env.currentNameTarget = ch;

			if (elm.voice != null) {
				entryNextVoice(name, elm.voice, elm.voicen);
			}

			// 名前の決定
			if (disp != null && disp != "") {
				dispName = disp;
			} else {
				if (ch != null && "nameAlias" in ch.init) {
					dispName = eval(ch.init.nameAlias);
					if (dispName == null) {
						dispName = name;
					}
				} else {
					dispName = name;
				}
			}
			
			// 名前加工処理 XXX
			//if (typeof global.dispNameFilter !== 'undefined') {
			//dispName = global.dispNameFilter(dispName);
			//}
			
			// 表情判定
			if (env.faceLevelName != null) {
				if (ch != null) {
					if (ch.isShowFace()) {
						faceName = ch.getPoseImage(env.faceLevelName);
					}
				} else {
					if ("nameFaces" in env.envinfo) {
						faceName = env.envinfo.nameFaces[env.envinfo.dispNameFace ? dispName : name];
					}
				}
				// 指定がきまらなかった場合
				if (faceName == null) {
					faceName = env.envinfo.clearFace;
				}
			}
		}

		// 名前表示
		drawName(dispName, faceName, nextvoices);

		// ボイス再生
		if (nextvoicecount > 0) {
			local ret = playNextVoice();
			if (ret > 0) {
				addAutoWait(ret);
			}
		}
		return 0;
	}

	// ---------------------------------------------
	
	/**
	 * 行待ち終了後に呼び出される処理
	 */
	function tag_afterline(elm) {
		if (voicecut && voicecutpage) {
			stopAllVoice();
		}
		//clearHistory(true);
		return 0;
	}

    /**
     * ページ処理後に呼び出される処理
     */
	function tag_afterpage(elm) {
		if (voicecut && voicecutpage) {
			stopAllVoice();
		}
		env.currentNameTarget = null;
		//clearHistory();
		stopAllActions(false);
		return 0;
    }

	function tag_stopallvoice(elm) {
		stopAllVoice();
	}

	// ---------------------------------------------------
	// 選択肢用処理
	// ---------------------------------------------------

	function addSelect(elm) {
		if (!("selectInfos" in f)) {
			f.selectInfos = [];
		}
		if (getint(f, "selectDoneFlag")) {
			f.selectInfos.resize(0);
			f.selDoneTarget  <- null;
			f.selectDoneFlag <- false;
		}
		f.selectInfos.append(clone elm);
		return 0;
	}

    /**
     * 選択肢の表示
     */
	function doSelect(elm) {
		f.selectDoneFlag <- true;
		if ("target" in elm) {
			f.selDoneTarget <- elm.target;
		}
		f.selTarget <- null;
		// 終了処理をさしこむ
		addTag("er");
		addTag("selend");
		// f.selectInfo の内容で選択肢を表示して終了をまつ
		// kag.showSelect();
		// return kag.waitTrigger(%[name:"selectdone"]);
		return 0;
	}

	/**
	 * 選択肢の終了
	 */
	function endSelect(elm) {
		//選択肢消去
		//kag.hideSelect();
		if (f.selTarget != "") {
			goToLabel(f.selTarget);
			return 0;
		} else {
			return -1;
		}
	}

	/**
	 * 選択肢処理の完了
	 */
	function doneSelect(elm) {
		if (f.selDoneTarget != "") {
			goToLabel(f.selDoneTarget);
			return 0;
		} else {
			return -1;
        }
	}
	
	// ------------------------------------------------------------------

	function getHandlers()
	{
		return { // 辞書配列オブジェクト

	//----------------------------------------- タグハンドラ群(テキスト表示系) --

			ch = tag_ch,
			r = tag_r,
			ruby = tag_ruby,
			font = tag_font,
			deffont = tag_deffont,
			resetfont = tag_resetfont,
			style = tag_style,
			defstyle = tag_defstyle,
			l = tag_l,
			p = tag_p,
			er = tag_er,
			indent = tag_indent,
			endindent = tag_endindent,
			delay = tag_delay,
			
			cancelskip = tag_cancelskip,
			resetwait = tag_resetwait,
			wait = tag_wait,
			waittrig = tag_waittrig,
			beginskip = tag_beginskip,
			endskip = tag_endskip,
			
			backlay = tag_backlay,
			forelay = tag_forelay,
			trans = tag_trans,
			wt = tag_wt,
			action = tag_action,
			stopaction = tag_stopaction,
			wact = tag_wact,
			
			eval = tag_eval,
			clearsyslocal = tag_clearsyslocal,
			clearlocal = tag_clearlocal,
			clearplaytime = tag_clearplaytime,

			//----------------------------------------- タグハンドラ群(選択肢分岐) --
			
			msgwin = tag_msgwin,
			msgoff = tag_msgoff,
			msgon  = tag_msgon,
			
			//----------------------------------------- タグハンドラ群(選択肢分岐) --
			
			seladd = addSelect,
			select = doSelect,
			seldone = doneSelect,
			selend = endSelect,

			//----------------------------------------- タグハンドラ群(シナリオ内分岐) --
			
			["goto"] = function(elm) {
				if ("target" in elm) {
					goToLabel(elm.target);
				}
				return 0;
			},
			
			//----------------------------------------- タグハンドラ群(次シナリオ指定) --
			
			next = function(elm) {
				if ("storage" in elm) {
					nextScene = elm.storage;
				}
				return 0;
			},

			exit = function(elm) {
				cur = lines.len();
				return 0;
			},

			//----------------------------------------------- 環境系機能 --

			env      = env.tagfunc,
			allchar  = env.allchar,
			alllayer = env.alllayer,
			allse    = env.allse,
			
			init       = tag_init,
			begintrans = tag_begintrans,
			endtrans   = tag_endtrans,
			beginlt    = tag_beginlt,
			endlt      = tag_endlt,
			
			newlay     = env.newLayer,
			newlayer   = env.newLayer,
			dellay     = env.delLayer,
			dellayer   = env.delLayer,
			newchar    = env.newCharacter,
			delchar    = env.delCharacter,
			
			entryvoice    = tag_entryvoice,
			playvoice     = tag_playvoice,
			waitvoice     = tag_waitvoice,
			dispname      = tag_dispname,
			afterpage     = tag_afterpage,
			afterline     = tag_afterline,
			stopallvoice  = tag_stopallvoice,
		};
	}

	// ----------------------------------------------
	// システム用ファンクション群
	// ----------------------------------------------

	// 自動再生
	autoMode   = false;
	
	// スキップモード
	// 0: 通常
	// 1: スキップ中
	// 2: 強制スキップ
	// 3: とばし処理中
	skipMode = 0;
	prevSkipMode = null;

	/**
	 * スキップ中かどうか
	 */
	function isSkip() {
		return skipMode != 0;
	}
	
	// 自動待ちを開始
	function startAuto() {
		local lastLength;
		if (Talkingnow(0) && talkVoiceLength > 0) {
			lastLength  = talkVoiceLength;
		} else {
			lastLength  = 0;
		}
		lastLength += getAutoWait();
		addWait({
			click = function() { stopAllVoice(); msgWait = false; },
			timeout = function() { stopAllVoice(); msgWait = false; },
		}, lastLength);
	}

	// 自動待ちの解除処理
	function stopAuto() {
		if ("timeout" in waitUntil) {
			delete waitUntil.timeout;
		}
	}

	
	/**
	 * オートモードの解除
	 */
	function cancelAuto() {
		if (autoMode) {
			autoMode = false;
			// もし会話待ち中ならそれを解除
			if (msgWait) {
				stopAuto();
			}
		}
	}

	/**
	 * 自動モードの変更
	 */
	function onAuto() {
		if (msgWinShow) {
			if (autoMode) {
				cancelAuto();
			} else {
				autoMode = true;
				// もし会話待ちならそのまま自動待ちに変更
				if (msgWait) {
					startAuto();
				}
			}
		}
	}
	
	// ---------------------------------------------------
	
	/**
	 * スキップモードの解除
	 */
	function cancelSkip() {
		if (skipMode == 1 || skipMode == 2) {
			//print("クリックでスキップ中断\n");
			skipMode = 0;
		} else if (prevSkipMode != null) {
			skipMode = 3;
		}
	}

	/**
	 * スキップ開始
	 */
	function onSkip() {
		if (msgWinShow) {
			if (skipMode == 1) {
				//print("スキップ解除\n");
				skipMode = 0;
			} else if (skipMode == 0) {
				if (prevSkipMode != null) {
					if (prevSkipMode == 0) {
						if (isAllSkip() || getReaded(scene) >= currentLine) {
							prevSkipMode = 1;
							//stopVoice();
						}
					}
					skipMode = 3;
				} else {
					onClick();
					if (isAllSkip() || getReaded(scene) >= currentLine) {
						//print("スキップ開始\n");
						skipMode = 1;
						//stopVoice();
					} else {
						//print("未読でスキップ開始できず\n");
					}
				}
			}
		}
	}

	/**
	 * 強制スキップON
	 */
	function onSkipOn() {
		if (msgWinShow) {
			if (skipMode < 2) {
				//print("強制スキップON\n");
				cancelAuto();
				onClick();
				skipMode = 2;
				//stopVoice();
			}
		}
	}

	/**
	 * 強制スキップOFF
	 */
	function onSkipOff(param) {
		if (skipMode == 2) {
			//print("強制スキップOFF\n");
			skipMode = 0;
		}
	}
	
	// ---------------------------------------------------

	msgWinShow = true;
	
	/**
	 * メッセージ窓消去モードの解除
	 */
	function cancelMsg() {
		if (!msgWinShow) {
			msgshow();
			msgWinShow = true;
			return true;
		}
		return false;
	}
	
	/**
	 * メッセージ窓の ON/OFF 制御
	 */
	function onMeswin(param) {
		cancelAuto();
		cancelSkip();
		// 表示してる場合のみ有効
		if (msgWinState) {
			if (msgWinShow) {
				// 窓を消去して ON 指令がくるまで停止する
				msghide();
				msgWinShow = false;
			} else {
				// 窓を表示して待ちの解除
				msgshow();
				msgWinShow = true;
			}
		}
	}

	// ---------------------------------------------------
	
	/**
	 * クリックされた
	 */
	function onClick() {
		// 状態に応じた処理を行う 
		if (cancelMsg()) { return; }
		cancelAuto();
		cancelSkip();
		//if (cancelTalk()) { return; }
		clickFunc();
		trigger("click");
	}
	
	/**
	 * キャンセルクリックされた
	 */
	function onCancel() {
		// 状態に応じた処理を行う 
		if (cancelMsg()) { return; }
		cancelAuto();
		cancelSkip();
		trigger("cancel");
	}

	// ------------------------------------------------------
	
	/**
	 * クリック待ち
	 */
	function waitClick() {
		print("クリック待ち\n");
		if (!isJump()) {
			if (isSkip()) {
				return;
			}
			addWait({
				click = function() {},
			}, null);
			suspend(WAIT);
		}
	}
	
	// ------------------------------------------------------
	// 外部イベント
	// ------------------------------------------------------

	function onKeyPress(key) {
		if (key == KEY_ENTER) {
			onClick();
		} else if (key == KEY_ESCAPE) {
			onCancel();
		}
	}
	
	/**
	 * SE 用情報のクリア
	 */
	function onSEStop(id) {
		::GamePlayer.onSEStop(id);
		env.onSeStop(id);
	}

	/**
	 * 表裏コピーが実行された場合の処理
	 */
	function onCopyLayer(toback, backlay) {
		env.onCopyLayer(toback, backlay);
		if (backlay && !toback) {
			clearTrash();
		}
	}

	/**
	 * 全リセット時の処理
	 */
	function onResetAll() {
		env.resetEnv();
		env.syncAll();
	}
};
