@jump storage=maptest.ks
[link]ほげほげ[endlink][r]
[link]ほげほげ[endlink][r]
[link]ほげほげ[endlink][r]
;[layopt layer=message0 visible=false]
;[layopt layer=message0 visible=true]
@s


@image page=fore layer=base storage=testbg01
@image page=back layer=base storage=testbg02
@l
@position visible=false
@l
@position visible=true
@l
@button graphic="YesButton" onenter="dm('hoge')" onleave="dm('hage')" target=*jump
@s
*jump
yh
@s
@jump storage=movetest.ks


@eval exp="System.touchImages(['_24', 'non-existent-graphic-file-name'])"
@s
@jump storage=snowtest.ks
@position left=0 top=0 width=640 height=480
@layopt layer=message0 page=fore visible=false
@image layer=base page=fore storage=gradbw_v
@image layer=0 page=fore storage=grad_h_transp visible=true
@pimage layer=0 page=fore storage=grad_v dx=0 dy=0 mode=screen opacity=64
@s

@trace exp="kag.current.name"
@video left=0 top=0 width=640 height=480 visible=true
@playvideo storage="tri_p.swf"
@wv
@s

[image storage="uni" page=fore layer=0 visible=true left=140 index=20000000]\
@jump storage=raintest.ks

@layopt layer=message0 page=fore visible=false
@backlay
*start|スタート
;
@l
@trans method=brightness rule=fore_light vague=128 time=600 blur=3
@wt
@l
@jump target=*start
@s
@gotostart

[image storage="_24" page=fore layer=base]\
[layopt layer=0 page=fore visible=true]\
@s
*hoge|hoge
@backlay
@image storage=testbg01 layer=base page=back
@trans time=5000 method=crossfade
@wait time=2500
@eval exp="kag.back.base.fillRect(100, 100, 100, 100, 0xffffffff)"
@wt
@s

@macro name=red
@font color=0x00ff00
@endmacro
*saveable|saveable
@cm
[red]これは赤い文字のマクロのはずです。
[s]

@jump storage=pimagetest.ks

@jump storage=scrolltest.ks

@jump storage=bgmtest.ks
@jump storage=bubbletest.ks
@jump storage=fonttest.ks
@jump storage=oven.ks
@eval exp="kag.conductor.loadScenario('second.ks')"

@jump storage=c:\pwork\longtest.ks
@call storage="rclick_tjs.ks"
@jump storage=animtest.ks
@mappfont storage=c:\pwork\r.tft
@loadplugin module=wuvorbis.dll
@l

@jump storage=rclicktest.ks


@jump storage=buttontest.ks
@jump storage=rclicktest2.ks
@jump storage=hacttest.ks
@jump storage=systembuttontest.ks
@jump storage=movetest.ks
@jump storage=fliptest.ks
@jump storage=zoomtest.ks
@jump storage=cursortest.ks
@jump storage=rclicktest3.ks
@jump storage=setest.ks
@jump storage=movietest.ks
@jump storage=clocktest.ks
@jump storage=unicodetest.ks
@jump storage=clipanimtest.ks
@jump storage=filtertest.ks
@jump storage=calltest.ks
@jump storage=infscroll.ks
@jump storage=staffroll_test.ks
@jump storage=layerdrag.ks

5308
5348

