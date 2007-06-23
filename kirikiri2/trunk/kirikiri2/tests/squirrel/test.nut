local a=10;
local b = ::krkr.a;
local c = ::krkr.b;
::krkr.c = 100;

test <- "てすとのテキスト";


class Foo {

	//constructor
	constructor()
	{
		testy = ["stuff",1,2,3];
	}

	//member function
	function print()
	{
		foreach(i,val in testy)
		{
			::print("idx = "+i+" = "+val+" \n");
		}
	}
	//property
	testy = null;
	data="testdata";
};

function hoge(msg)
{
	print("ファンクション呼び出し!:"+ msg);
}

function main(tick)
{
	print("tick値:" + tick);
}

// ファイル呼び出しテスト
dofile("test2.nut");

// ファイル書き込みテスト
local f = file("output.txt", "w");
f.writen(10, 'l');
f = null;

// 数学関数テスト
print("sin(PI):" + sin(PI));
print("rand():" + rand());

// 文字列関数テスト
print(format("%s %d 0x%02X","this is a test :",123,10));

// 吉里吉里側のオブジェクト生成
local layer2 = ::krkr.MyLayer(::krkr.win, ::krkr.base);
layer2.setSize(100,100);
layer2.setPos(100,100);
layer2.fillRect(0,0,100,100,0xff00ff00);
layer2.visible = true;
return layer2;
