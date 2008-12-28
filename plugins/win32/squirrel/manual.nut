//
// squirrel 疑似スレッド処理支援ライブラリ
//

/**
 * 基底オブジェクト
 * スレッドはオブジェクトを「待つ」ことができます。
 */
class Object {

	/**
	 * コンストラクタ
	 * @param delegate 処理を委譲するオブジェクトを指定します。オブジェクトには
	 * インスタンスまたはテーブルが指定可能です。
	 * クロージャを呼び出す場合、オブジェクトがインスタンスの場合は、そのインスタンスを
	 * 環境として bindenv された状態で呼び出されます。
	 * table の場合はこのオブジェクトの環境で実行されます。
	 */
	constructor(delegate=null);

	/**
	 * このオブジェクトに対する委譲を設定します(コンストラクタ指定と同機能)
	 * @param delegate 委譲先オブジェクト
	 */
	function setDelegate(delegate=null);

	/**
	 * @param name プロパティ名
	 * @return 指定された名前のプロパティの setter があれば true
	 */
	function hasSetProp(name);
	
	/**
	 * このオブジェクトを待っているスレッド1つに終了を通知する
	 */
	function notify();

	/**
	 * このオブジェクトを待っている全スレッドに終了を通知する
	 * ※このメソッドはオブジェクト廃棄時にも実行されます。
	 */
	function notifyAll();

	/**
	 * プロパティ機能について
	 * Object を継承したオブジェクトは、プロパティ機能を持ちます。
	 * 該当するメンバが存在しなかった場合、自動的に getter/setter ファンクションを探して、
	 * それがあればそれを呼び出します。
	 * val = obj.name; → val = obj.getName();
	 * obj.name = val; → obj.setName(val)
	 */ 
};

/**
 * スレッド制御用オブジェクト
 * 疑似スレッドを制御するためのオブジェクトです。
 * スレッドを実行中の場合、ユーザの参照がなくなってもシステムが参照を維持します。
 */
class Thread extends Object {

	// スレッドのステート
	THREAD_STOP = 0; // 停止
	THREAD_RUN = 1;  // 実行中
	THREAD_WAIT = 2; // 処理待ち
	THREAD_END = 3;  // 終了
	
	/**
	 * コンストラクタ
	 * @param func スレッドを生成後実行するファンクションまたはファイル名
	 * @param delegate 機能の委譲先の指定
	 */
	constructor(func=null, delegate=null);

	/**
	 * @return このスレッドの実行時間
	 */
	function getCurrentTick();

	/**
	 * @return このスレッドの実行ステータス THREAD_STOP/THREAD_RUN/THREAD_WAIT/THREAD_END
	 */
	function getStatus();

	/**
	 * スレッドの実行開始
	 * @param func 呼び出すグローバル関数またはスクリプトファイル名
	 */
	function exec(func);

	/**
	 * スレッドの終了
	 */
	function exit();

	/**
	 * スレッドの一時停止
	 */
	function stop();

	/**
	 * 一時停止したスレッドの再開
	 */
	function run();

	/**
	 * スレッドの実行待ち。いずれかの条件で解除されます。引数を指定しなかった場合でも、
	 * いったん処理が中断して、システム側のイベント/更新処理完了後に復帰します。
	 * @param param 待ち条件指定 文字列:トリガ待ち オブジェクト:オブジェクト待ち 数値:時間待ち(ms)※最小の指定が有効
	 * @return wait解除の原因
	 */
	function wait(param, ...);
};

/**
 * @return 稼働中スレッドの一覧(配列)を返します。
 */
function getThreadList();

/**
 * @return 現在実行中のスレッドを返します。
 */
function getCurrentThread();

/**
 * 新しいスレッドを生成して実行します。Thread(func) と等価です。
 * @param func 呼び出すメソッド、またはファイル名
 * @return 新規スレッド制御オブジェクト(Thread)
 */
function fork(func);

/**
 * 現在実行中のスレッドを別の実行に切り替えます
 * @param func 呼び出すグローバル関数またはスクリプトファイル名
 */
function exec(func);

/**
 * 現在実行中のスレッドを終了します
 */
function exit();

/**
 * 現在実行中のスレッドの実行待ち。いずれかの条件で解除されます。引数を指定しなかった場合でも、
 * いったん処理が中断して、システム側のイベント/更新処理完了後に復帰します。
 * @param param 待ち条件指定 文字列:トリガ待ち オブジェクト:オブジェクト待ち 数値:時間待ち(ms)※最小の指定が有効
 * @return wait解除の原因
 */
function wait(param, ...);

/**
 * トリガ送信
 * 全スレッドに対してトリガを送信します。
 * 該当するトリガを待っていたスレッドの待ちが解除されます。
 * @param name トリガ名
 */
function trigger(name);
