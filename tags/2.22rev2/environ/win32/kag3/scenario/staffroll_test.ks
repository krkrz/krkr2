@call storage=staffroll.ks
@image storage=_24 page=fore layer=base
*loop
@staffrollinit
@font size=50 color=0xff0000
@staffrolltext x=100 y=480 text="すたっふ"
@resetfont
@staffrolltext x=100 y=80 text="スタッフ１"
@staffrolltext x=100 y=40 text="スタッフ２"
@staffrolltext x=100 y=40 text="スタッフ３"
@staffrolltext x=100 y=40 text="スタッフ４"
@staffrolltext x=100 y=40 text="スタッフ５"
@staffrolltext x=100 y=40 text="スタッフ６"
@staffrolltext x=100 y=40 text="スタッフ７"
@staffrolltext x=100 y=40 text="スタッフ８"
@staffrolltext x=100 y=40 text="スタッフ９"
@staffrolltext x=100 y=40 text="スタッフ１０"
@staffrolltext x=100 y=40 text="スタッフ１１"
@staffrolltext x=100 y=40 text="スタッフ１２"
@staffrolltext x=100 y=40 text="スタッフ１３"
@staffrolltext x=100 y=40 text="スタッフ１４"
@staffrolltext x=100 y=40 text="スタッフ１５"
@staffrolltext x=100 y=40 text="スタッフ１６"
@staffrolltext x=100 y=40 text="スタッフ１７"
@staffrolltext x=100 y=40 text="スタッフ１８"
@staffrolltext x=100 y=40 text="スタッフ１９"
@staffrolltext x=100 y=40 text="スタッフ２０"
@staffrollimage x=100 y=40 storage=YesButton
@l
@staffrollstart height=1500 time=20000
@wait time=20000
@staffrolluninit
@l
@jump target=*loop

*vertical
@position vertical=true
@call storage=staffroll.ks
@image storage=_24 page=fore layer=base
*loopv
@staffrollinit
@font size=50 color=0xff0000
@staffrolltext y=100 x=640 text="すたっふ"
@resetfont
@staffrolltext y=100 x=80 text="スタッフ１"
@staffrolltext y=100 x=40 text="スタッフ２"
@staffrolltext y=100 x=40 text="スタッフ３"
@staffrolltext y=100 x=40 text="スタッフ４"
@staffrolltext y=100 x=40 text="スタッフ５"
@staffrolltext y=100 x=40 text="スタッフ６"
@staffrolltext y=100 x=40 text="スタッフ７"
@staffrolltext y=100 x=40 text="スタッフ８"
@staffrolltext y=100 x=40 text="スタッフ９"
@staffrolltext y=100 x=40 text="スタッフ１０"
@staffrolltext y=100 x=40 text="スタッフ１１"
@staffrolltext y=100 x=40 text="スタッフ１２"
@staffrolltext y=100 x=40 text="スタッフ１３"
@staffrolltext y=100 x=40 text="スタッフ１４"
@staffrolltext y=100 x=40 text="スタッフ１５"
@staffrolltext y=100 x=40 text="スタッフ１６"
@staffrolltext y=100 x=40 text="スタッフ１７"
@staffrolltext y=100 x=40 text="スタッフ１８"
@staffrolltext y=100 x=40 text="スタッフ１９"
@staffrolltext y=100 x=40 text="スタッフ２０"
@staffrollimage y=100 x=40 storage=YesButton
@l
@staffrollstart height=1800 time=20000
@wait time=20000
@staffrolluninit
@l
@jump target=*loopv
