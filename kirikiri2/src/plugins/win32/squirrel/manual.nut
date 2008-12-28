//
// squirrel 疑似スレッド処理支援ライブラリ
//

/**
 * 基底オブジェクト
 * スレッドはオブジェクトを「待つ」ことができます。
 * オブジェクトに対する待ちは、オブジェクトが notify/notifyAll することで解除されます。
 *
 * ●プロパティ機能
 * Object を継承したオブジェクトは、通常の squirrel のオブジェクトにはないプロパティ機能が
 * 拡張されています。該当するメンバが存在しなかった場合、自動的に getter/setter
 * ファンクションを探して、それがあればそれを呼び出して値が処理されます。
 * val = obj.name; → val = obj.getName();
 * obj.name = val; → obj.setName(val)
 *
 * ●delegate機能
 * Object を継承したオブジェクトは、squirrel のテーブル/ユーザデータがサポートしてるような
 * delegate 機能を使うことができます。委譲先のオブジェクトは、squirrel の標準機能同様の
 * テーブルの他、別のオブジェクトインスタンスが指定可能です。
 * 委譲オブジェクトがインスタンスの場合は、クロージャを参照する際、そのインスタンスを環境
 * として bindenv された状態で取得されます(TJSのデフォルトの挙動と同じようになります)。
 * テーブルの場合は環境を再設定しないので、クロージャは元のオブジェクトの環境で実行されます
 */ 
class Object {

	/**
	 * コンストラクタ
	 * @param delegate 処理を委譲するオブジェクトを指定します。
	 */
	constructor(delegate=null);

	/**
	 * このオブジェクトに対する委譲を設定します(コンストラクタ指定と同機能)
	 * @param delegate 委譲先オブジェクト
	 */
	function setDelegate(delegate=null);

	/**
	 * このオブジェクトに対する委譲を取得します。
	 * @return 委譲先オブジェクト
	 */
	function getDelegate();
	
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
	 */
	function wait(param, ...);

	/**
	 * 現在の待ちをすべて強制的にキャンセルします
	 */
	function cancelWait();
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
 * @return wait解除の原因。待ちが無いかキャンセルされた場合は null
 */
function wait(param, ...);

/**
 * トリガ送信
 * 全スレッドに対してトリガを送信します。
 * 該当するトリガを待っていたスレッドの待ちが解除されます。
 * @param name トリガ名
 */
function trigger(name);
