*start
@if exp="typeof global.infiniteScrollPluginObject == 'undefined'"
@iscript
dm("initializing InfiniteScrollPlugin ...");
class InfiniteScrollPlugin extends KAGPlugin
{
	var target; // 対象のレイヤ
	var speedX; // スピード ( pixel/s )
	var speedY; // スピード ( pixel/s )
	var originX; // 初期位置
	var originY; // 初期位置
	var moving = false; // 動いているかどうか
	var lastTick;
	var x;
	var y;

	function InfiniteScrollPlugin()
	{
		super.KAGPlugin();
	}

	function finalize()
	{
		stop();
		super.finalize(...);
	}

	function start(elm)
	{
		if(moving) return;
		target = kag.getLayerFromElm(elm);
		speedX = +elm.speedx;
		speedY = +elm.speedy;
		originX = +elm.orgx;
		originY = +elm.orgy;
		System.addContinuousHandler(handler);
		moving = true;
		lastTick = System.getTickCount();
		x = originX;
		y = originY;
	}

	function stop()
	{
		if(!moving) return;
		System.removeContinuousHandler(handler);
		target = void;
		moving = false;
	}

	function handler()
	{
		var curtime;
		var difftime = (curtime = System.getTickCount()) - lastTick;
		x = originX + speedX * difftime \ 1000;
		y = originY + speedY * difftime \ 1000;
		var dispw = target.width;
		var disph = target.height;
		x %= dispw;
		y %= disph;
		if(x < 0) x += dispw;
		if(y < 0) y += disph;
		target.setImagePos(-(int)x, -(int)y);
		if(difftime > 1000)
		{
			originX = x;
			originY = y;
			lastTick = curtime;
		}
	}

	function onRestore(f, clear, elm)
	{
		stop();
	}

}

var infiniteScrollPluginObject;

infiniteScrollPluginObject = new InfiniteScrollPlugin();
kag.addPlugin(infiniteScrollPluginObject);
@endscript
@endif
@macro name=infscrstart
@eval exp="infiniteScrollPluginObject.start(mp)"
@endmacro
@macro name=infscrstop
@eval exp="infiniteScrollPluginObject.stop()"
@endmacro
;
;
; 以下、テスト
@image storage=d:\temp\24_big.bmp layer=0 page=fore mode=rect visible=true clipleft=100 cliptop=110 clipwidth=640 clipheight=480
@backlay
@infscrstart layer=0 page=fore speedx=-100 speedy=100 orgx=640 orgy=0
@l
@infscrstop
@jump target=*start
