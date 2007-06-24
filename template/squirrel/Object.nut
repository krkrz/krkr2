/**
 * 基底オブジェクト
 * getter/setter メソッドによるプロパティ機能をもつ
 */
class Object {

	/**
	 * コンストラクタ
	 */
	constructor() {
	}
	
	/**
	 * プロパティの存在確認
	 * @param name プロパティ名
	 * getter 関数がある場合はそれを参照する。ない場合は直接参照する
	 */
	function hasProp(name) {
		local name2 = "set" + name.slice(0,1).toupper() + name.slice(1);
		if (name2 in this && typeof this[name2] == "function") {
			return true;
		}
		return (name in this);
	}

	/**
	 * プロパティの取得
	 * @param name プロパティ名
	 * getter 関数を探してそれを呼び出す
	 */
	function _get(name) {
		local name2 = "get" + name.slice(0,1).toupper() + name.slice(1);
		if (name2 in this) {
			return this[name2]()
		}
		throw "no such property " + name;
	}

	/**
	 * プロパティの設定
	 * @param name プロパティ名
	 * @param value 値
	 * setter 関数を探してそれを呼び出す
	 */
	function _set(name, value) {
		local name2 = "set" + name.slice(0,1).toupper() + name.slice(1);
		if (name2 in this) {
			this[name2](value);
			return;
		}
		throw "no such property " + name;
	}
};

class Test extends Object {
	constructor() {
		::Object.constructor();
	}
	_width = null;
	function getWidth() {
		return _width;
	}
	function setWidth(v) {
		_width = v;
	}
};
