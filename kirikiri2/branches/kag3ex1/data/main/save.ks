;
; セーブ画面
;

; ゲーム中からの呼び出し
*startFromGame
[locksnapshot]
[rclick enabled=true jump=true storage="" target=*backtogame]
[history enabled=false]

; ベース画像読み込み
[stoptrans]
[backlay]
[position layer=message1 frame="save_base" opacity=255 left=0 top=0 width=800 height=600 marginT=0 marginL=0 marginR=0 marginB=0 visible=true  transparent=false page=back]
[current layer=message1 page=back]

; ボタンの配置

;ページ配置
[locate x=312 y=47]
[button name="page1" exp="setSavePageButton(1)"]
[locate x=352 y=47]
[button name="page2" exp="setSavePageButton(2)"]
[locate x=392 y=47]
[button name="page3" exp="setSavePageButton(3)"]
[locate x=432 y=47]
[button name="page4" exp="setSavePageButton(4)"]
[locate x=472 y=47]
[button name="page5" exp="setSavePageButton(5)"]
[locate x=512 y=47]
[button name="page6" exp="setSavePageButton(6)"]
[locate x=552 y=47]
[button name="page7" exp="setSavePageButton(7)"]
[locate x=592 y=47]
[button name="page8" exp="setSavePageButton(8)"]
[locate x=632 y=47]
[button name="page9" exp="setSavePageButton(9)"]

;セーブデータフレーム
;１列目
[locate x=32  y=96 ]
[button name="save0" normal="sl_win_base" exp="saveAction(0)"]
[locate x=32  y=192]
[button name="save1" normal="sl_win_base" exp="saveAction(1)"]
[locate x=32  y=288]
[button name="save2" normal="sl_win_base" exp="saveAction(2)"]
[locate x=32  y=384]
[button name="save3" normal="sl_win_base" exp="saveAction(3)"]
[locate x=32  y=480]
[button name="save4" normal="sl_win_base" exp="saveAction(4)"]

;２列目
[locate x=400 y= 96]
[button name="save5" normal="sl_win_base" exp="saveAction(5)"]
[locate x=400 y=192]
[button name="save6" normal="sl_win_base" exp="saveAction(6)"]
[locate x=400 y=288]
[button name="save7" normal="sl_win_base" exp="saveAction(7)"]
[locate x=400 y=384]
[button name="save8" normal="sl_win_base" exp="saveAction(8)"]
[locate x=400 y=480]
[button name="save9" normal="sl_win_base" exp="saveAction(9)"]

;右上戻るボタン
[locate x=672 y=47]
[button name="return" target=*backtogame]

; ダイアログ処理開始
[eval exp='setSavePageButton()']
[trans method=crossfade time=300]
[wt]
[current layer=message1 page=fore]
[s]

*backtogame
[rclick jump=false]
[stoptrans]
[backlay]
[position layer=message1 page=back visible=false frame="" left=0 top=0 width=8 height=8]
[trans method=crossfade time=300]
[wt]
[current layer=message0 page=fore]
[history enabled=true]
[unlocksnapshot]
[return]
