@iscript
function draw_2chars(text)
{
	var current = kag.current;
	var ll = current.lineLayer;
	var orgfont = ll.font.face;
	var fontsize = current.fontSize;
	var dx = int(current.lineLayerBase+(fontsize>>1)), dy = int(current.lineLayerPos);
	ll.font.face = orgfont.substring(1);
	ll.font.angle = 0;
	ll.drawText(dx - fontsize, dy, text[0], current.chColor, 255, current.antialiased, 255,
		current.shadowColor, 0, 2, 2);
	ll.drawText(dx - (fontsize>>1), dy, text[1], current.chColor, 255, current.antialiased, 255,
		current.shadowColor, 0, 2, 2);
	current.lineLayerPos += fontsize;
	current.lineLayerLength += fontsize;
	ll.font.face = orgfont;
	ll.font.angle = 2700;
}
@endscript
@macro name="yoko"
@eval exp="draw_2chars(mp.text)"
@wc time=1
@endmacro
;
;
@position vertical=true
ïΩê¨[yoko text="13"]îN[yoko text="12"]åé[yoko text="31"]ì˙


