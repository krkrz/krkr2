// グローバル変数
gname <- "global";

class PropertyTest extends Object {

	_name = "PropertyTest";
	
	constructor() {
		::Object.constructor();
	}

	function getName() {
		return _name;
	}

	function setName(value) {
		_name = value;
	}

	function test() {

		// プロパティを参照
		name  = "name";
		print("name:" + name);
		
		// グローバル変数を参照
		gname = "gname";
		print("gname:" + gname);

		// 存在しない変数を参照
		try {
			xname = "aaaa";
		} catch(e) {
			print(e);
		}
		try {
			print("xname:" + xname);
		} catch(e) {
			print(e);
		}
	}
};

local ptest = PropertyTest();
ptest.test();
