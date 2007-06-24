// ------------------------------------------------------
// イベント処理用
// ------------------------------------------------------

currentPlayer <- null;  // 現在の処理プレイヤー

// キーマップ
keyMap <- [];

function keyClear() {
	keyMap = [];
}

function onKeyPress(key) {
	//keyMap[key.tointeger()] <- true;
	if (currentPlayer) {
		currentPlayer.onKeyPress(key);
	}
}

function onKeyRelease(key) {
	//keyMap[key.tointeger()] <- false;
	if (currentPlayer) {
		currentPlayer.onKeyRelease(key);
	}
}

/**
 * フェード終了時処理
 */
function onMusicFadeCompleted(ch)
{
	if (currentPlayer) {
		if (ch == 0) {
			currentPlayer.onBgmFadeCompleted();
		} else {
			currentPlayer.onSEFadeComplsted(ch);
		}
	}
}

/**
 * 再生終了時処理
 */
function onMusicStop(ch)
{
	if (currentPlayer) {
		if (ch == 0) {
			currentPlayer.onBgmStop();
		} else {
			currentPlayer.onSEStop(ch);
		}
	}
}

// ------------------------------------------------------

currentTime   <- 0;     // 現在時刻
endFlag       <- false; // 終了フラグ

// ゲーム全体のステート
STATE_TITLE  <- 0;
STATE_CONFIG <- 1;
STATE_GAME   <- 2;
state <- 0;

scenePlayer <- null;

/**
 * ゲーム状態変更指定
 */
function setState(state) {
	::state = state;
	switch (state) {
	case STATE_TITLE:
		currentPlayer = null;
		break;
	case STATE_CONFIG:
		currentPlayer = null;
		break;
	case STATE_GAME:
		currentPlayer = scenePlayer;
		break;
	}
}


// タイトル処理用
function title(diff)
{
}

// コンフィグ処理用
function config(diff)
{
}

// ゲームメイン処理用
function game(diff)
{
	local ret = currentPlayer.main(diff);
	switch (ret) {
	case currentPlayer.WAIT:
		break;
	case currentPlayer.STOP:
		setState(STATE_TITLE);
		break;
	}
}

/**
 * ゲーム本体メインループ
 */
function main(diff)
{
	currentTime += diff;
	switch (state) {
	case STATE_TITLE:
		title(diff);
		break;
	case STATE_CONFIG:
		config(diff);
		break;
	case STATE_GAME:
		game(diff);
		break;
	}
	return endFlag;
}

startScene<-null;

function setStartScene(startscene)
{
	::startScene = startscene;
}

/**
 * システム初期化処理
 */
function init()
{
	scenePlayer <- ScenePlayer();
	if (startScene != null) {
		print("シーン:" + startScene + "から再生開始します");
		scenePlayer.start(startScene);
		setState(STATE_GAME);
	}
}

function start(name)
{
	setStartScene(name);
	init();
}
