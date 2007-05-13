*start
@deffont color=0
@position left=0 top=0 width=640 height=480 color=0xffffff opacity=255
ムービーを再生[l]
@video left=0 top=0 width=640 height=480 visible=false
;
;@playvideo storage="d:\pwork\lake.mpg"
;@playvideo storage="d:\pwork\f01.mpg"
;@playvideo storage="d:\pwork\test3.avi"
;@playvideo storage="d:\pwork\test.avi"
@playvideo storage="d:\pwork\t.swf"
;
@wait time=300
@video left=0 top=0 width=640 height=480 visible=true
@wv canskip=true
@video visible=false
再生終了、非表示に[l]
@jump target=*start


*label1
@stopvideo
@video visible=false
ラベル1
