@layopt layer=message0 visible=false
@image layer=base storage="_24_3"
;@image layer=0 page=back storage="ring.tlg" visible=true left=0     top=0
@image layer=0 page=fore storage="x.tlg" visible=true left=320   top=0
@image layer=1 page=fore storage="ring.tlg" visible=true left=0   top=0
@iscript
	kag.fore.layers[0].type = ltAddAlpha;
	kag.fore.layers[0].face = dfAddAlpha;
	kag.fore.layers[0].convertType(dfAlpha);
	kag.fore.layers[1].parent = kag.fore.layers[0];
	kag.fore.layers[1].setPos(0,0);
//	kag.fore.layers[0].opacity = 128;
	kag.fore.layers[1].colorRect(70,30,128,40,0xffffff,127);
	kag.fore.layers[1].colorRect(10,50,127,40,0xffffff,254);
	kag.fore.layers[1].colorRect(10,90,126,40,0xffffff,128);
	kag.fore.layers[1].colorRect(10,130,125,40,0xffff00,128);
	kag.fore.layers[1].colorRect(10,170,124,40,0xffff00,1);
	kag.fore.layers[1].font.height = 40;
	kag.fore.layers[1].drawText(-10, -12, "Š´‚¶Š¿Žš", 0xffffff);
	kag.fore.layers[1].drawText(100, 120, "Š´‚¶Š¿Žš", 0xffffff);
	kag.fore.base.face = dfOpaque;
	kag.fore.base.holdAlpha = false;
	kag.fore.base.operateAffine(kag.fore.layers[1], 0, 0, 256, 256, false, 30, 30, 194, 70, 120, 254);
	kag.fore.base.operateStretch(0, 0, 640, 480, kag.fore.layers[1], 0, 0, 256, 256);
/*
	var a = kag.fore.layers[0];
	var b = kag.back.layers[0];
	var c = kag.fore.layers[1];
	a.type = ltAddAlpha;
	a.face = dfAddAlpha;
	a.convertType(dfAlpha);
	b.type = ltAddAlpha;
	b.face = dfAddAlpha;
	b.convertType(dfAlpha);
	c.type = ltAddAlpha;
	c.face = dfAddAlpha;
	c.convertType(dfAlpha);
*/
@endscript
@l
@trans layer=0 children=false time=10000 rule="trans2" vague=513
@wt
@s
