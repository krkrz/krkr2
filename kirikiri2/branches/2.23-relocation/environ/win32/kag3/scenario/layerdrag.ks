@iscript
if(typeof(global.enableDrag)!="undefined")
	throw new Exception("このスクリプトを２回以上実行しないでください");
@endscript
@iscript
function enableDrag(elm)
{
	var layer = kag.getLayerFromElm(elm);
	if(typeof(layer.org_onMouseDown) != "undefined")
		return; // すでに関数が置き換えられているので処理しない
	layer.org_onMouseDown = layer.onMouseDown;
	layer.org_onMouseMove = layer.onMouseMove;
	layer.org_onMouseUp = layer.onMouseUp;
	layer.org_onHitTest = layer.onHitTest;
	layer.dragging = false;
	layer.dragOriginX = layer.dragOriginY = 0;
	layer.onMouseDown = function(x, y, button)
	{
		if(button == mbLeft && !window.messageLayerHiding)
			dragging = true,
			dragOriginX = x,
			dragOriginY = y;
		else
			org_onMouseDown(...);
	} incontextof layer;
	layer.onMouseMove = function()
	{
		if(dragging)
			setPos(parent.cursorX - dragOriginX,
				parent.cursorY - dragOriginY);
		else
			org_onMouseMove(...);
	} incontextof layer;
	layer.onMouseUp = function()
	{
		dragging = false;
		org_onMouseUp(...);
		// ドラッグが終了したときになにかやりたい場合はここに書く
	} incontextof layer;
	layer.onHitTest = function(x, y, hit)
	{
		if(!hit)
			global.Layer.onHitTest(x, y, false);
		else
			global.Layer.onHitTest(x, y, true);
	} incontextof layer;
}
function disableDrag(elm)
{
	var layer = kag.getLayerFromElm(elm);
	if(typeof(layer.org_onMouseDown) == "undefined")
		return;
	layer.onMouseDown = layer.org_onMouseDown;
	layer.onMouseMove = layer.org_onMouseMove;
	layer.onMouseUp = layer.org_onMouseUp;
	layer.onHitTest = layer.org_onHitTest;
	delete layer.org_onMouseDown;
	delete layer.org_onMouseMove;
	delete layer.org_onMouseUp;
	delete layer.org_onHitTest;
}
// 注意 : enabledrag タグでドラッグを可能にした場合、
// トランジション前には disabledrag タグでドラッグを無効にしてください
@endscript
@macro name=enabledrag
@eval exp="enableDrag(mp)"
@endmacro
@macro name=disabledrag
@eval exp="disableDrag(mp)"
@endmacro
;
@image layer=0 page=fore storage="uni" visible=true
;@layopt layer=messaege0 page=fore visible=false
*loop
@enabledrag layer=0 page=fore
@l
@disabledrag layer=0 page=fore
@l
@jump target=*loop
