;
; CGモード
;

[rclick enabled=true jump=true target=*backtotitle]

[eval exp='cgInit()']

*loop

; ベース画像読み込み
[rclick enabled=true jump=true storage="" target=*backtotitle]
[backlay]
[position layer=message0 frame="cgmode_base" opacity=255 left=0 top=0 width=800 height=600 marginT=0 marginL=0 marginR=0 marginB=0 visible=true transparent=false page=back]
[current layer=message0 page=back]

;遷移用ボタン
[locate x=330 y=48]
[button name="extra_cgmode" disabled]
[locate x=456 y=47]
[button name="extra_replaymode" storage="replay.ks"]
[locate x=563 y=47]
[button name="extra_musicmode" storage="musicmode.ks"]
[locate x=692 y=47]
[button name="return" target=*backtotitle]

;キャラ選択
[locate x=10 y=101]
[button name="cgmode_page0" exp="tf.cgmodepage=0" target=*loop]
[locate x=10 y=177]
[button name="cgmode_page1" exp="tf.cgmodepage=1" target=*loop]
[locate x=10 y=256]
[button name="cgmode_page2" exp="tf.cgmodepage=2" target=*loop]
[locate x=10 y=336]
[button name="cgmode_page3" exp="tf.cgmodepage=3" target=*loop]
[locate x=10 y=416]
[button name="cgmode_page4" exp="tf.cgmodepage=4" target=*loop]
[locate x=10 y=496]
[button name="cgmode_page5" exp="tf.cgmodepage=5" target=*loop]

;CG選択
;１列目
[locate x=284 y=122]
[button name="cg0" normal="cgmode_thum_normal" exp="tf.cgnum=0" target=*cgview]
[locate x=410 y=122]
[button name="cg1" normal="cgmode_thum_normal" exp="tf.cgnum=1" target=*cgview]
[locate x=536 y=122]
[button name="cg2" normal="cgmode_thum_normal" exp="tf.cgnum=2" target=*cgview]
[locate x=662 y=122]
[button name="cg3" normal="cgmode_thum_normal" exp="tf.cgnum=3" target=*cgview]
;２列目
[locate x=284 y=232]
[button name="cg4" normal="cgmode_thum_normal" exp="tf.cgnum=4" target=*cgview]
[locate x=410 y=232]
[button name="cg5" normal="cgmode_thum_normal" exp="tf.cgnum=5" target=*cgview]
[locate x=536 y=232]
[button name="cg6" normal="cgmode_thum_normal" exp="tf.cgnum=6" target=*cgview]
[locate x=662 y=232]
[button name="cg7" normal="cgmode_thum_normal" exp="tf.cgnum=7" target=*cgview]
;３列目
[locate x=284 y=342]
[button name="cg8" normal="cgmode_thum_normal" exp="tf.cgnum=8" target=*cgview]
[locate x=410 y=342]
[button name="cg9" normal="cgmode_thum_normal" exp="tf.cgnum=9" target=*cgview]
[locate x=536 y=342]
[button name="cg10" normal="cgmode_thum_normal" exp="tf.cgnum=10" target=*cgview]
[locate x=662 y=342]
[button name="cg11" normal="cgmode_thum_normal" exp="tf.cgnum=11" target=*cgview]
;４列目
[locate x=284 y=452]
[button name="cg12" normal="cgmode_thum_normal" exp="tf.cgnum=12" target=*cgview]
[locate x=410 y=452]
[button name="cg13" normal="cgmode_thum_normal" exp="tf.cgnum=13" target=*cgview]
[locate x=536 y=452]
[button name="cg14" normal="cgmode_thum_normal" exp="tf.cgnum=14" target=*cgview]
[locate x=662 y=452]
[button name="cg15" normal="cgmode_thum_normal" exp="tf.cgnum=15" target=*cgview]

[eval exp='setCgmodePageButton()']
[trans method=crossfade time=500]
[wt]
[current layer=message0 page=fore]
[s]

*cgview
[eval exp='cgViewInit()']

*cgviewloop
[rclick enabled=true jump=true storage="" target=*loop]
[if exp='isCgView()']
[stoptrans]
[backlay]
[position page=back layer=message0 frame="" opacity=255 left=0 top=0 width=800 height=600 marginT=0 marginL=0 marginR=0 marginB=0 visible=true]
[current page=back layer=message0]
[eval exp='cgView()']
[trans method=crossfade time=500]
[wt]
[current page=fore layer=message0]
[click target=*cgviewloop]
[s]
[endif]
[jump target=*loop]

*backtotitle
[rclick enabled=false jump=false]
[jump storage="title.ks" target=*backtotitle]
