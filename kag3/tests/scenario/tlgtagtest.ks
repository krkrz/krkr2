@layopt layer=message0 visible=false
@image layer=base storage="_24_3"
@image layer=0 page=fore storage="ring.tlg" visible=true left=0     top=0
@image layer=0 page=back storage="x.tlg" visible=true left=0   top=0
@image layer=1 page=fore storage="x.tlg" visible=true left=300   top=0
@iscript
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
