class DelegateClass {

	name = "DelegateClass";
	
	constructor() {
	}

	function test() {
		print("delegate test called:" + name);
	}
};

DelegateTable <- {

	name = "DelegateTable",
	
	function test() {
		print("delegate test called:" + name);
	}
}

class DelegateTest extends Object {
	name = "DelegateTest";
	constructor(d) {
		::Object.constructor(d);
	}
};

local dtest = DelegateTest(DelegateClass());
dtest.test();

local dtest2 = DelegateTest(DelegateTable);
dtest2.test();
