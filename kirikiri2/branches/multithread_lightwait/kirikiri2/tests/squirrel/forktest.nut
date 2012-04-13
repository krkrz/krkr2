print("fork‚ÌƒeƒXƒg");

function forktest1() {
	print("fork1 start");
	for (local i=0;i<10;i++) {
		print("test1:" + i);
		wait(1000);
	}
	print("fork1 done");
}

function forktest2() {
	print("fork2 start");
	for (local i=0;i<10;i++) {
		print("test2:" + i);
		wait(1000);
	}
	print("fork2 done");
}

local th1 = ::fork(forktest1);
local th2 = ::fork(forktest2);
print("th1:" + th1);
print("th2:" + th2);

th2.wait(th1);

local thlist = ::getThreadList();
for (local i=0;i<thlist.len();i++) {
	print(i + ":" + thlist[i]);
}

wait(th2);

system("test2.nut");
print("system done");
fork("test2.nut");
print("fork done");
