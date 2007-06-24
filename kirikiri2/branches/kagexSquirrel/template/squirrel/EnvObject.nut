/**
 * 環境オブジェクトの基底クラス
 */
class EnvObject {

	// 実行環境
	env = null;

	// 実行プレイヤー
	player = null;

	// 名前
	name = null;

	constructor(env, name) {
		this.env    = env;
		this.player = env.player;
		this.name   = name;
	}

	// メッセージ表示用
	function dm(msg) {
		return player.dm(msg);
	}

	/**
	 * コマンドの処理
	 */
	function tagfunc(elm) {
		//dm("コマンド処理:" + name);
		tagcommand(elm);
		if (!player.isJump()) {
			return sync(elm);
		}
		return 0;
	}

	/**
	 * コマンド実行
	 */
	function tagcommand(elm) {
	}
	
	/**
	 * 状態同期
	 */
	function sync(elm) {
		return 0;
	}
}
