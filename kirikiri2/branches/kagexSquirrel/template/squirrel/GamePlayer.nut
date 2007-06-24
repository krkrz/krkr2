/**
 * 基本プレイヤー
 * ゲーム処理の基本機能を提供するプレイヤー
 * ・スキップ制御
 * ・画面切り替え処理
 * ・メッセージ窓制御処理
 * ・選択肢処理
 * ・ボイス再生
 * ・SE再生
 * ・BGM再生
 */
class GamePlayer extends Player {

	voiceInfos = null; // ボイス再生用情報

	/**
	 * コンストラクタ
	 */
	constructor() {
		::Player.constructor();
		voiceInfos = dofile("voices.nut");
	}

	/**
	 * 状態のクリア
	 */
	function clear() {
		::Player.clear();
		hideTransLayer();

	}

	/**
	 * スキップ中か
	 */
	function isSkip() {
		//return kag.skipMode != SKIP_FORCE && (kag.skipMode || kag.noeffect);
		return false;
	}

	// ------------------------------------------------------------------
	// 画面切り替え関連
	// ------------------------------------------------------------------

	// XXXX まだ諸機能が実装されてない状態
	// トランジションの代替
	
	/**
	 * 裏画面用処理
	 */
	function backup() {
		//kag.fore.base.stopTransition();
		//kag.backupLayer(EMPTY, true);
	}

	// ----------------------------------------------------
	// レイヤによる画面切り替え処理用
	// ----------------------------------------------------
	
	transLayer = null;
	transShowing = false;

	/**
	 * 画面切り替え処理用レイヤを隠す
	 */
	function hideTransLayer() {
		if (transLayer != null) {
			transLayer.stop();
		}
		transShowing = false;
	}
	
	/**
	 * レイヤ画面切り替えの開始
	 */
	function beginLayerTrans(elm) {

		local time    = getint(elm, "time", 1000);
		local type    = getval(elm, "type", "wipeltor");
		local color   = getint(elm, "color",  0);
		local canskip = getval(elm, "canskip", true);

		if (transLayer == null) {
			transLayer = TransLayer();
		}
		hideTransLayer();
		
		local waitParam = {};
		if (canskip) {
			if (isSkip()) {
				time = 0;
			}
			waitParam.click <- function() { hideTransLayer(); };
		}
		transLayer.start(elm, type, time, color);
		transShowing = true;
		
		time = transLayer.time;
		
		local waitTime;
		if ("wait" in elm) {
			waitTime = time + elm.wait;
			waitParam.timeout <- function() {};
			waitWithTimeout(waitParam, waitTime);
		} else {
			waitParam.layertrans <- function() {};
			if (time > 0) {
				waitWithTimeout(waitParam, waitTime);
			}
		}
	}
	
	/**
	 * レイヤ画面切り替えの終了
	 */
	function endLayerTrans(elm) {
		if (!transShowing) {
			return;
		}

		local time    = getint(elm, "time");
		local type    = getval(elm, "type");
		local canskip = getval(elm, "canskip", true);

		local waitParam = {};
		if (canskip) {
			if (isSkip()) {
				hideTransLayer();
				return;
			}
			waitParam.click <- function() { hideTransLayer(); };
		}
		waitParam.layertrans <- function() { hideTransLayer(); };
		transLayer.start(elm, type, time, null);
		time = transLayer.time;
		if (time > 0) {
			waitWithTimeout(waitParam, null);
		}
	}

	// ----------------------------------------------------
	// スクリーンによる画面フェード処理用
	// ----------------------------------------------------
	
	screenLayer = null; // スクリーン表示用レイヤ
	lastImageId = null;	// 前回指定の画像
	
	/**
	 * スクリーン画像を表示
	 * @image 画像
	 * @opacity 不透明度
	 */
	function screenShow(image, opacity)	{
		if (screenLayer == null) {
			screenLayer = ScreenLayer();
			screenLayer.setPos(0,0);
			screenLayer.absolute = 600000; // XXX いくつが適当か？
			if (image != null) {
				lastImageId = image;
			}
			if (lastImageId == null) {
				lastImageId == "black";
			}
			screenLayer.loadImage(lastImageId);
		} else {
			if (image != null && lastImageId != image) {
				lastImageId = image;
				screenLayer.loadImage(lastImageId);
			}
		}
		screenLayer.opacity = opacity;
		screenLayer.visible = true;
	}
	
	/**
	 * スクリーン処理の停止 
	 */
	function screenStop() {
		if (screenLayer != null) {
			screenLayer.stop();
		}
	}

	/**
	 * スクリーン処理終了待ち
	 */
	function waitScreen(canskip) {
		if (screenLayer == null || screenLayer.actionTime == null) {
			return;
		}
		if (canskip) {
			if (isSkip()) {
				screenStop();
				return;
			} 
			addWait({
				click = function() { screenStop(); },
				screen = function() {}
			}, null);
		} else {
			addWait({
				screen = function() {}
			}, null);
		}
		suspend(WAIT);
	}
	
	/**
	 * スクリーン画像フェードアウト
	 */
	function fadeOut(image, time) {
		screenShow(image, 0);
		screenLayer.start(1, time, false);
		waitScreen(true);
	}

	function blackOut(time) {
		fadeOut("black", time);
	}

	function whiteOut(time) {
		fadeOut("white", time);
	}

	/**
	 * スクリーン画像フェードイン
	 */
	function fadeIn(time) {
		screenShow(null, 100);
		screenLayer.start(2, time, false);
		waitScreen(true);
	}

	// ------------------------------------------------------------
	
	// 旧スタイルアクション用パラメータ
	actionParam = {
		start = true,
		startx = true,
		starty = true,
		x = true,
		y = true,
		toptime = true,
		vibration = true,
		waittime = true,
		cycle = true,
		distance = true,
		falltime = true,
		zoom = true,
		angvel = true,
		angle = true,
		showtime = true,
		hidetime = true,
		intime = true,
		outtime = true,
		opacity = true,
	};

	/**
	 * アクション情報のコピー
	 * @param act  元アクション情報
	 * @param time 時間上書き指定
	 * @param elm  タグによる上書き用パラメータ情報
	 * @return システム登録用アクション情報
	 */
	function copyActionInfo(act, time, elm) {
		local action = {};
		if (act != null) {
			local names = [];
			names.assign(act);
			if ("module" in act) {
				//dm("module 指定アクション:" + act.module);
				local module = eval(act.module);
				if (module != null) {
					local e = clone value;
					if (time != null) {
						e.time <- time;
					}
					// パラメータのコピー
					if (elm != null) {
						// アクションパラメータをコピー
						foreach (name, value in elm) {
							if (name in actionParam) {
								e[name] <- value;
							}
						};
						if ("time" in elm) {
							e.time <- elm.time;
						}
						if ("delay" in elm) {
							e.delay <- elm.delay;
						}
					}
					// 新スタイルに置き換え
					action = module(e);
				}
			} else {
				// 新スタイル
				foreach (name, value in act) {
					if (typeof value == "table") {
						local e = clone value;
						if (time != null) {
							e.time <- time;
						}
						// パラメータのコピー
						if (elm != null) {
							if ("time" in elm) {
								e.time <- elm.time;
							}
							if ("delay" in elm) {
								e.delay <- elm.delay;
							}
						}
						action[name] <- e;
					} else {
						// そうでない場合はそのまま
						action[name] = value;
					}
				}
			}
		}
		if (elm != null && "nowait" in elm) {
			action.nowait <- elm.nowait;
		}
		return action;
	}

	/**
	 * タグからアクション情報を抽出
	 * @param name アクション名
	 * @param elm  タグによる上書き用パラメータ情報
	 * @return システム登録用アクション情報
	 */
	function getActionInfo(name, elm) {
		if (elm != null && name.slice(0,5) == "Layer") {
			local module = eval(name);
			if (module != null) {
				local action = module(elm);
				if ("nowait" in elm) {
					action.nowait <- elm.nowait;
				}
				return action;
			}
		}
	}
	
	/**
	 * 更新時アクションの実行
	 */
	function beginTransActionUpdate(trans, layer) {
		if ("showaction" in trans) {
			local action = copyActionInfo(trans.showaction, getval(trans,"time"));
			addTag("action", { target=layer, action=action});
		}
		if ("hideaction" in trans) {
			local action = copyActionInfo(trans.hideaction, getval(trans, "time"));
			//addTag("action", { target=layer, target:layer.comp, action:action]);
		}
	}

	/**
	 * 表示/消去アクションの実行
	 */
	function beginTransAction(trans, layer, hide, nosync) {
		// アクション実行
		local action = copyActionInfo(getval(trans, hide ? "hideaction" : "showaction"), getval(trans, "time"));
		action["visvalue"] <- { handler="MoveAction", value= hide ? 0 : 100, time=trans.time };
		addTag("action", { target=layer, action=action});
		if (!nosync && !getval(trans, "nosync")) {
			if ("transwait" in trans) {
				// 時間待ち
				local time = trans.time.tointeger() + trans.transwait.tointeger();
				addTag("wait", {time=time, action=true});
			} else {
				// アクション待ち
				addTag("wact", {target=layer});
			}
		}
	}

	// ------------------------------------------------------------------
	// 環境レイヤ制御
	// ------------------------------------------------------------------
	
	// 現在前面にあるレイヤ一覧
	// すべて弱参照で保持すること
	forelayers = [];

	/**
	 * 指定したレイヤを指定したレベルに移動する
	 */
	function toLevel(layer, newlevel) {

		//dm("レベル指定:" + newlevel + ":" + layer.level);
		newlevel = newlevel.tointeger();

		if (layer.level > 0) {
			if (layer.level == newlevel) {
				//dm("同一レベルなので処理しない");
				return;
			}
			local index = layer.absolute;
			// もともといたレベルの調整
			for (local i = 0; i<forelayers.len(); i++) {
				local target = forelayers[i];
				if (target != layer && target.level == layer.level) {
					if (target.absolute > index) {
						target.absolute -= 100;
					}
				}
			}
		}
		// 新規レベルの一番上にもっていく
		local newindex = (newlevel + 1) * 100000;
		for (local i = 0; i<forelayers.len(); i++) {
			local target = forelayers[i];
			if (target != layer && target.level == newlevel && target.absolute > newindex) {
				newindex = target.absolute;
			}
		}
		newindex += 100;
		//dm("レベル設定:" + newlevel + ":" + newindex);
		layer.absolute = newindex;
		layer.level    = newlevel;
    }
	
	/**
	 * 同じレベルのレイヤの中で最前列に移動
	 */
	function toFront(layer) {
		local level = layer.level;
		local index = layer.absolute;
		local maxindex = index;
		for (local i = 0; i<forelayers.len(); i++) {
			local target = forelayers[i];
			if (target != layer && target.level == level) {
				if (target.absolute > maxindex) {
					maxindex = target.absolute;
				}
				if (target.absolute > index) {
					target.absolute -= 100;
				}
			}
		}
		layer.absolute = maxindex;
	}

	/*
	 * レイヤを最後列に移動
	 */
	function toBack(layer) {
		local level = layer.level;
		local index = layer.absolute;
		local minindex = index;
		for(local i = 0; i<forelayers.len(); i++) {
			local target = forelayers[i];
			if (target != layer && target.level == level) {
				if (target.absolute < minindex) {
					minindex = target.absolute;
				}
				if (target.absolute < index) {
					target.absolute += 100;
				}
			}
		}
		layer.absolute = minindex;
    }

	/**
	 * 表にあるレイヤか？
	 */
	function isForeLayer(layer) {
		///return layer.parent === kag.fore.base;
		return true;
	}

	/**
	 * 表に移動させる
	 */
	function toForeLayer(layer) {
		local absolute = layer.absolute;
		//layer.parent   = kag.fore.base; 表に移動
		layer.absolute = absolute;
	}

	/**
	 * レイヤ取得処理
	 * @param fore
	 * @param origLayer 廃棄元レイヤ
	 */
	function getNewLayer(owner, isfore, origLayer) {
		local newlayer = owner.createLayer(isfore);
		newlayer.level = -1;
		forelayers.add(newlayer);
		if (origLayer != null) {
			newlayer.assign(origLayer);
			//newlayer.level    = origLayer.level;
			//newlayer.absolute = origLayer.absolute;
			trashLayer(origLayer);
		}
		return newlayer;
	}
	
	// 廃棄対象レイヤ一覧
	trashlayers = [];

	/**
	 * レイヤ廃棄処理
	 * @param layer
	 */
	function trashLayer(layer) {
		if (layer != null) {
			local newlayers = [];
			for (local i=0;i<forelayers.len();i++) {
				if (forelayers[i] != null && forelayers[i] != layer) {
					newlayers.append(forelayers[i]);
				}
			}
			forelayers = newlayers;
			for (local i=0;i<trashlayers.len();i++) {
				if (trashlayers[i] == layer) {
					return;
				}
			}
			trashlayers.append(layer);
		}
		return null;
	}

	/**
	 * レイヤ破棄実行
	 */
	function clearTrash() {
		foreach (name,value in trashlayers) {
			delete trashlayers[name];
		}
	}
	
	// -------------------------------------------------------------------
	// ボイス処理系
	// -------------------------------------------------------------------

	voicecut = true;     // ボイスをカットするか
	voicecutpage = true; // ボイスカットをページ処理の後にするかどうか
	bgmdown = false;     // ボイス時にBGMをさげるか
	voiceenable = true;  // ボイスが有効か

	/**
	 * ボイスが有効かどうか
	 * @param コンフィグ名称
	 * @return 有効なら true
	 */
	function getVoiceOn(configName) {
		return true;
	}

	/**
	 * ボイスの音量状態の取得
	 * @param コンフィグ名称
	 * @return 音量
	 */
	function getVoiceVolume(configName) {
		return 100;
	}

	// 名前定義がデフォルト名か？
	function isDefaultName() {
		return true;
		//return kag.flags.name === void || (kag.flags.name == kag.defaultName && kag.flags.family == kag.defaultFamily);
	}

	// ----------------------------------------------------------------------------

	// 会話中ボイス長さ
	talkVoiceLength = null;
	
	/**
	 * ボイス再生
	 * @param name キャラ名
	 * @param voicefile 再生ファイル
	 */
	function playVoice(name, voicefile) {
		if (isSkip() || !voiceenable) {
			return;
		}

		// ボイス情報取得
		local configName = "etc";
		if (name in voiceInfo) {
			local info = voiceInfos[name];
			if ("config" in info) {
				configName = info.config;
			}
		}
		if (!getVoiceOn(configName)) {
			return;
		}
		
		// ボリューム調整
		// volume = getVoiceVolume(configName);

		// ボイス再生処理
		Talk(0, voicefile);
		talkVoiceLength = TalkLength(0);

		// 時間を返す
		return talkVoiceLength;
	}

	/**
	 * ボイス停止
	 * @param name キャラ名
	 */
	function stopVoice(name) {
		if (talkVoiceLength != null) {
			TalkStop(0);
			talkVoiceLength = null;
		}
	}

	/**
	 * 全ボイス停止
	 */
	function stopAllVoice() {
		// XXX
		stopVoice("");
	}
	
	/**
	 * ボイスまち
	 * @param name キャラ名
	 * @param canSkip スキップできる
	 */
	function waitVoice(name, canSkip) {
		if (canSkip) {
			if (isSkip()) {
				stopVoice(name);
				return 0;
			}
			if (Talkingnow(0) && talkVoiceLength > 0) {
				waitWithTimeout({
					click = function(name) { stopVoice(name); },
					click_arg = name,
					timeout = function() {},
				}, talkVoiceLength);
			}
		} else {
			if (Talkingnow(0) && talkVoiceLength > 0) {
				waitWithTimeout({
					timeout = function() {},
				}, talkVoiceLength);
			}
		}
	}
	
	// 自動待ち時間の追加
	function addAutoWait(wait) {
		//kag.addAutoWait(wait);
	}

	// ------------------------------------------------------------
	// SE 系機能
	// ------------------------------------------------------------

	// スキップ時に se を再生しない
	nosewhenskip = true;

	// se 戸数
	senum = 2;

	// フレーム数に変換
	function getTime(time) {
		if (isSkip() || time == null) {
			time = 0;
		}
		return time.tointeger() / 16;
	}

	// 規定のボリューム値に変換
	function getVolume(volume) {
		if (volume == null) {
			volume = 100;
		}
		return 0x3fff * volume.tointeger() / 100;
	}

	function playSE(id, time, play) {
		//kag.se[id].fadeIn({ storage:play.storage, loop:play.loop, time:time, start:play.start});
		MusicStop(id, 0);
		MusicPlay(id, getSoundId(play.storage), getVolume(play.volume), getTime(time))
	}

	function fadeSE(id, time, volume) {
		MusicFade(id, getVolume(volume), getTime(time));
	}
	
	function stopSE(id, time) {
		MusicStop(id, getTime(time));
	}

	function waitSEFade(id, waittime, canskip) {
		return 0;
	}

	function waitSEStop(id, waittime, canskip) {
		if (MusicPlayingNow(id)) {
			if (canskip) {
				// スキップできる場合
				if (isSkip()) {
					// スキップ中の場合
					stopSE(id,0);
					return 0;
				}
				waitWithTimeout({
					click = function (id) {	stopSE(id,0);},
					["click_arg"] = id, // ハンドラへの引数
					["sestop"+id] = function (id) {	},
					["sestop"+id+"_arg"] = id // ハンドラへの引数
					}, timeout);
			} else {
				// スキップできない場合
				waitWithTimeout({
					["sestop"+id] = function (id) {},
					["sestop"+id+"_arg"] = id // ハンドラへの引数
					}, timeout);
			}
			return -2;
		}
		return 0;
	}
	
	// ------------------------------------------------------------
	// BGM 系機能
	// ------------------------------------------------------------

	// BGM ボリュームの自動制御系
	function setBGMVolDown(down) {
		if (down) {
			if (bgmdown && !isSkip()) {
				//kag.bgm.voldown = true;
			}
		} else {
			//if (kag.bgm.voldown) {
			//	kag.bgm.voldown = false;
			//}
		}
	}
	
	function playBGM(time, play) {
		//kag.bgm.fadeIn(%[ storage:play.storage, loop:play.loop, time:time, start:play.start]);
		local name = getSoundId(play.storage);
		if (!MusicPlayingNow(0) || MusicCurrent(0) != name) {
			MusicPlay(0, name, getVolume(play.volume), getTime(time));
		}
	}

	function pauseBGM(time) {
		// ポーズ処理 XXX
	}
	
	function resumeBGM() {
		// ポーズ復帰処理 XXX
	}

	function fadeBGM(time, volume) {
		MusicFade(0, getVolume(volume), getTime(time));
	}
	
	function stopBGM(time) {
		MusicStop(0, getTime(time));
	}

	function waitBGMFade(waittime, canskip) {
		//return kag.waitBGMFade(waitFade);
		return 0;
	}

	function waitBGMStop(waittime, canskip) {
		if (MusicPlayingNow(0)) {
			if (canskip) {
				if (isSkip()) {
					stopBGM(0);
					return 0;
				}
				waitWithTimeout({
					click = function() { stopBGM(0); },
					bgmstop = function() {},
					timeout = function() { stopBGM(0); },
				}, waittime);
			} else {
				waitWithTimeout({
					bgmstop = function() {},
					timeout = function() { stopBGM();},
				}, waittime);
			}
			return -2;
		}
		return 0;
	}

	// ----------------------------------------------------
	// メッセージ窓
	// ----------------------------------------------------
	
	msg = null;
	msgWinState = false;
	msgCurrent = null;
	msgText = "";
	msgAll = "";
	msgName = null;
	msgCursor = false;
	msgX = 20;
	msgY = 24;
	msgWidth = 600;
	msgHeight = 400;

	/**
	 * メッセージ窓サイズ変更
	 */
	function msgsize(x, y, w, h) {
		msghide();
		msgX = x;
		msgY = y;
		msgWidth = w;
		msgHeight = h;
	}
	
	/**
	 * メッセージ窓生成
	 */
	function msgshow() {
		if (msg == null) {
			msg = MessageWindow(msgX, msgY, msgWidth, msgHeight);
		}
		if (msgWinState) {
			if (msgText != null && msgText != "") {
				msg.write(msgText);
			} else {
				msg.clear();
			}
		if (msgName != null) {
			msg.showNamePlate(msgName);
		} else {
			msg.showNamePlate(0);
		}
			msg.showCursor(msgCursor);
			msg.show(true);
		} else {
			msg.show(false);
		}
	}

	/**
	 * メッセージ窓消滅
	 */
	function msghide() {
		msg = null;
	}
	
	/**
	 * メッセージ窓表示
	 */
	function _msgon() {
		msgWinState = true;
		msgshow();
	}
	
	/**
	 * メッセージ窓消去
	 */
	function _msgoff() {
		print("msgoff\n");
		msgWinState = false;
		msgCurrent = null;
		msgText = "";
		msgAll = "";
		msgName = null;
		msgCursor = false;
		if (msg != null) {
			msg.clear();
			msg.showCursor(false);
			msg.showNamePlate(0);
			msg.show(msgWinState);
		}
	}
	
	/**
	 * メッセージ表示
	 */
	function msgwrite(text) {
		_msgon();
		if (text != null) {
			msgText = msgAll + text;
			msg.write(msgText);
		} else {
			msg.clear();
		}
	}

	function msgdone() {
		msgAll = msgText;
	}

	/**
	 * メッセージ窓名前指定
	 */
	function msgname(name) {
		_msgon();
		msgName = name;
		if (msgName != null) {
			msg.showNamePlate(msgName);
		} else {
			msg.showNamePlate(0);
		}
	}

	// 会話待ち中
	msgWait = false;
	
	/**
	 * メッセージ消去
	 */
	function msgclear() {
		msgAll = "";
		msgText = "";
		msgCursor = false;
		if (msg != null) {
			msg.clear();
			msg.showCursor(false);
		}
		msgWait = false;
	}
	
	/**
	 * メッセージ窓待ちカーソル表示
	 */
	function msgwait() {
		msgCursor = true;
		if (msg != null) {
			msg.showCursor(true);
		}
		msgWait = true;
	}

	// ------------------------------------------------------
	// エラー処理系
	// ------------------------------------------------------

	function errorImage(msg) {
		print(msg + "\n");
	}

	function errorCmd(msg) {
		print(msg + "\n");
	}
	
	function error(msg) {
		print(msg + "\n");
	}

	function errorVoice(msg) {
		print(msg + "\n");
	}

	// ------------------------------------------------------
	// フラグ処理系
	// ------------------------------------------------------

	function setBGMFlag(name) {
		//kag.sflags["bgm_" + (name.toUpperCase())] = true;
	}

	function setCGFlag(name) {
		//kag.sflags["cg_" + (name.toUpperCase())] = true;
	}

	// --------------------------------------------------------------
	// メッセージ窓制御
	// --------------------------------------------------------------

	/**
	 * 名前の表示
	 * @param name 表示名
	 * @param faceName 表情名
	 * @param voices 再生ボイスデータ
     */
	function drawName(name, faceName, voices) {
		// 履歴に名前を指定
		// 名前の表示
		msgname(name);
		// 表情表示
	}

	// ------------------------------------------------------
	// メイン処理用
	// ------------------------------------------------------
	
	function update(diff) {
		::Player.update(diff);
		
		// 画面切り替え用レイヤ
		if (transLayer != null) {
			transLayer.update(diff);
		}
		// 画面マスク切り替え用レイヤ
		if (screenLayer != null) {
			screenLayer.update(diff);
		}

		// ボイス用更新
		if (talkVoiceLength > 0) {
			talkVoiceLength -= diff;
		}
	}

	// ------------------------------------------------------
	// 外部イベント
	// ------------------------------------------------------

	/**
	 * BGMのフェードが終了した
	 */
	function onBgmFadeCompleted() {
		trigger("bgmfade");
	}

	function onBgmStop() {
		trigger("bgmstop");
	}

	function onSEFadeCompleted(id) {
		trigger("sefade" + id);
	}

	function onSEStop(id) {
		trigger("sestop" + id);
	}
	
	/**
	 * トランジション終了時処理
	 */
	function onExchangeForeBack() {
		clearTrash();
	}
	
	/**
	 * 表裏コピーが実行された場合の処理
	 */
	function onCopyLayer(toback, backlay) {
	}

};
