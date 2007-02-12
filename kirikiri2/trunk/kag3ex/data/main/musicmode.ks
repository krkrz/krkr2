;
; 音楽mode
;

[rclick enabled=true jump=true storage="" target=*backtotitle]

[fadeoutbgm time=500]
[wb]

[eval exp='musicInit()']

*loop

; ベース画像読み込み
[stoptrans]
[backlay]
[position layer=message0 frame="musicmode_base" opacity=255 left=0 top=0 width=800 height=600 marginT=0 marginL=0 marginR=0 marginB=0 visible=true  transparent=false page=back]
[current layer=message0 page=back]

;遷移用ボタン
[locate x=330 y=48]
[button name="extra_cgmode" storage="cgmode.ks"]
[locate x=456 y=47]
[button name="extra_replaymode" storage="replay.ks"]
[locate x=563 y=47]
[button name="extra_musicmode" disabled]
[locate x=692 y=47]
[button name="return" target=*backtotitle]

;機能ボタン
[locate x=140 y=484]
[button name="musicmode_play"      exp="startBgm()"]
[locate x=  7 y=484]
[button name="musicmode_stop"      exp="stopBgm()"]
[locate x=  7 y=530]
[button name="musicmode_repeat"    exp='setRepeat(true)']
[locate x=140 y=530]
[button name="musicmode_allrepeat" exp='setRepeat(false)']

;音楽ボタン
[locate x=295 y=126]
[button name="track01" exp='playBgm(0)']
[locate x=295 y=162]
[button name="track02" exp='playBgm(1)']
[locate x=295 y=198]
[button name="track03" exp='playBgm(2)']
[locate x=295 y=234]
[button name="track04" exp='playBgm(3)']
[locate x=295 y=270]
[button name="track05" exp='playBgm(4)']
[locate x=295 y=306]
[button name="track06" exp='playBgm(5)']
[locate x=295 y=342]
[button name="track07" exp='playBgm(6)']
[locate x=295 y=378]
[button name="track08" exp='playBgm(7)']
[locate x=295 y=414]
[button name="track09" exp='playBgm(8)']
[locate x=295 y=450]
[button name="track10" exp='playBgm(9)']
[locate x=295 y=486]
[button name="track11" exp='playBgm(10)']
[locate x=295 y=522]
[button name="track12" exp='playBgm(11)']

[eval exp='setRepeat()']
[eval exp='setMusicmodePageButton()']

[trans method=crossfade time=500]
[wt]
[current layer=message0 page=fore]
[s]

*backtotitle
[rclick enabled=false jump=false]
[eval exp='musicEnd()']
[jump storage="title.ks" target=*backtotitle]
