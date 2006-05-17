*start
@layopt layer=message0 page=fore visible=false
@image storage=_24 page=fore layer=base
@image storage=ball page=fore layer=0 left=100 top=100 visible=true
@wait time=200
[move layer=0 time=3000 path="(0,80,255)(320,480,255)"]\
@wm
@stopmove
@l
@jump target=*start


*start2
@image storage=uni page=fore layer=0 visible=true left=100 top=0
;@image storage=uni page=back layer=0 visible=true left=100 top=0
@position left=0 top=0 width=640 height=50 opacity=0
hogehogehogehogeohhogehoghoeho\
@backlay
@layopt layer=0 page=back visible=false
@trans method=crossfade time=5000
@move layer=message0 page=fore path=0,80,255 time=5000
@move layer=message0 page=back path=0,80,255 time=5000
@s
