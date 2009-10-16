local cnt=0;
while (cnt < vargc) {
	print(format("arg%d:%s",cnt,vargv[cnt++]));
}

Window <- ::createTJSClass("Window");
System <- ::createTJSClass("System");

local win = Window();
win.visible = true;

while (true) {
	if (System.getKeyState.bindenv(System)(13)) { // VK_RETURN
		break;
	}
	::wait();
}
