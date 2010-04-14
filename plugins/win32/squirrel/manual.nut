/**
 * 吉里吉里クラスの取得
 * @param className 吉里吉里クラス名指定(文字列)
 * @param ... 継承している親クラスを列挙
 * @return squirrelクラス
 *
 * 吉里吉里のクラスを squirrelクラスとして取得します。
 * このクラスは継承可能です。
 *
 * ※吉里吉里側で親クラス情報を参照生成できないため、
 * 親クラスが継承しているクラスの名前をすべて手動で列挙する必要があります。
 * またこの機能で作成した吉里吉里クラスのインスタンスが吉里吉里側から
 * 返される場合は、squirrel のクラスでのインスタンスとしてラッピングされます。
 */
function createTJSClass(className, ...);

/**
 * 吉里吉里クラスの squirrel における基底クラス構造
 * このインスタンスに対する吉里吉里側からのメンバ参照は、元の吉里吉里インスタンスのそれが呼ばれますが、
 * 存在してないメンバの場合は、missing 機能により squirrel 側オブジェクトの同名メンバが参照されます。
 */
class TJSObject extends Object {

	/**
	 * コンストラクタ
	 */
	constructor();

	/**
	 * 吉里吉里オブジェクトの有効性の確認
	 * @return valid なら true
	 */
	function tjsIsValid();
	
	/**
	 * 吉里吉里オブジェクトの強制オーバライド処理
	 * 吉里吉里インスタンスのメンバを強制的に置き換えます。
	 * イベント処理を squirrel 側で行いたい場合、元にしたクラス側で定義が無い場合は、missing 処理で
	 * 自動的に squirrel メンバが参照されますが、元にしたクラス側で定義が既にあって、それを上書きしたい
	 * 場合に使います。
	 * @param name メンバ名
	 * @param value 登録する値
	 */
	function tjsOverride(name, value);
};

/**
 * TJS用のNULL値。tjsのメソッドに null を渡す必要がある場合などに使います。
 * squirrel の null は tjs では void 扱いです。
 */
tjsNull;
