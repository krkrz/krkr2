;
; リプレイモード
;

[rclick enabled=true jump=true storage="" target=*backtotitle]

[eval exp='replayInit()']

*loop

; ベース画像読み込み
[rclick enabled=true jump=true storage="" target=*backtotitle]
[backlay]
[position layer=message0 frame="replaymode_base" opacity=255 left=0 top=0 width=800 height=600 marginT=0 marginL=0 marginR=0 marginB=0 visible=true transparent=false page=back]
[current layer=message0 page=back]

;遷移用ボタン
[locate x=330 y=48]
[button name="extra_cgmode" storage="cgmode.ks"]
[locate x=456 y=47]
[button name="extra_replaymode" disabled]
[locate x=563 y=47]
[button name="extra_musicmode" storage="musicmode.ks"]
[locate x=692 y=47]
[button name="return" target=*backtotitle]

;キャラ選択
[locate x=10 y=101]
[button name="cgmode_page0" exp="tf.cgmodepage=0" target="*loop"]
[locate x=10 y=177]
[button name="cgmode_page1" exp="tf.cgmodepage=1" target="*loop"]
[locate x=10 y=256]
[button name="cgmode_page2" exp="tf.cgmodepage=2" target="*loop"]
[locate x=10 y=336]
[button name="cgmode_page3" exp="tf.cgmodepage=3" target="*loop"]
[locate x=10 y=416]
[button name="cgmode_page4" exp="tf.cgmodepage=4" target="*loop"]
[locate x=10 y=496]
[button name="cgmode_page5" exp="tf.cgmodepage=5" target="*loop"]

;リプレイ選択
;１列目
[locate x=284 y=122]
[button name="cg0" normal="cgmode_thum_normal" exp='recollect(0)']
[locate x=410 y=122]
[button name="cg1" normal="cgmode_thum_normal" exp='recollect(1)']
[locate x=536 y=122]
[button name="cg2" normal="cgmode_thum_normal" exp='recollect(2)']
[locate x=662 y=122]
[button name="cg3" normal="cgmode_thum_normal" exp='recollect(3)']
;２列目
[locate x=284 y=232]
[button name="cg4" normal="cgmode_thum_normal" exp='recollect(4)']
[locate x=410 y=232]
[button name="cg5" normal="cgmode_thum_normal" exp='recollect(5)']
[locate x=536 y=232]
[button name="cg6" normal="cgmode_thum_normal" exp='recollect(6)']
[locate x=662 y=232]
[button name="cg7" normal="cgmode_thum_normal" exp='recollect(7)']
;３列目
[locate x=284 y=342]
[button name="cg8" normal="cgmode_thum_normal" exp='recollect(8)']
[locate x=410 y=342]
[button name="cg9" normal="cgmode_thum_normal" exp='recollect(9)']
[locate x=536 y=342]
[button name="cg10" normal="cgmode_thum_normal" exp='recollect(10)']
[locate x=662 y=342]
[button name="cg11" normal="cgmode_thum_normal" exp='recollect(11)']
;４列目
[locate x=284 y=452]
[button name="cg12" normal="cgmode_thum_normal" exp='recollect(12)']
[locate x=410 y=452]
[button name="cg13" normal="cgmode_thum_normal" exp='recollect(13)']
[locate x=536 y=452]
[button name="cg14" normal="cgmode_thum_normal" exp='recollect(14)']
[locate x=662 y=452]
[button name="cg15" normal="cgmode_thum_normal" exp='recollect(15)']

[eval exp='setReplaymodePageButton()']
[trans method=crossfade time=500]
[wt]
[current layer=message0 page=fore]
[s]

*backtotitle
[rclick enabled=false jump=false]
[jump storage="title.ks" target=*backtotitle]
