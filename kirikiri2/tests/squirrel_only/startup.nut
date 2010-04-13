notifyAllExceptions(1);
printf("squirrel test start\n");

printf("tjsNull type:%s\n", typeof tjsNull);

local cnt=0;
while (cnt < vargc) {
	print(format("arg%d:%s",cnt,vargv[cnt++]));
}

Window <- ::createTJSClass("Window");
System <- ::createTJSClass("System");
Layer  <- ::createTJSClass("Layer");

local WIDTH=800;
local HEIGHT=600;

// window
local win = Window();
win.setInnerSize(WIDTH, HEIGHT);
win.visible = true;

// primary layer
local lay = Layer(win, tjsNull);
lay.setSize(WIDTH, HEIGHT);

// test layer
local lay2 = Layer(win, lay);
lay2.setSize(100,100);
lay2.fillRect(0,0,100,100,0xffff0000);
lay2.setPos(400,100);
lay2.visible = true;

// ƒƒCƒ“ƒ‹[ƒv‚ð\’z
while (tjsIsValid(win)) {
	if (System.getKeyState.bindenv(System)(13)) { // VK_RETURN
		break;
	}
	local tick = ::getCurrentTick();
	lay2.left = 400 + 200*sin(2 * PI * tick / 1000);
	// ‚±‚ê‚É‚æ‚è•`‰æˆ—‚ª‚¢‚Á‚½‚ñ‹g—¢‹g—¢‚É–ß‚é
	//::suspend();
	::wait(1000);
}

// I—¹
