; 
; 環境設定
;

; ゲーム中からの呼び出し
*startFromGame
[rclick enabled=true jump=true storage="" target=*backtogame]
[eval exp='tf.fromTitle = 0']
[history enabled=false]
[jump target=*buttons]

; タイトルからの呼び出し
*startFromTitle
[rclick enabled=true jump=true storage="" target=*backtotitle]
[eval exp='tf.fromTitle = 1']

*buttons

; ベース画像読み込み
[backlay]
[position layer=message1 frame="config_base" left=0 top=0 width=800 height=600 marginT=0 marginL=0 marginR=0 marginB=0 visible=true  transparent=false page=back]
[current layer=message1 page=back]

; ボタンの配置

;画面モード
[locate x=64  y=146]
[button name="config_window"     exp='setScreenButton(true)']
[locate x=223 y=146]
[button name="config_fullscreen" exp='setScreenButton(false)']

;テキスト速度
[locate x=63 y=241]
[slider width=303 height=28 opacity=0 normal="slidebar" over="slidebar_over" min=0 max=10 value=kag.textspeed nohilight]

;オート速度
[locate x=63 y=336]
[slider width=303 height=28 opacity=0 normal="slidebar" over="slidebar_over" min=0 max=10 value=kag.autospeed nohilight]

;画面効果
[locate x=64 y=435]
[button name=config_effecton exp="setNoEffectButton(false)"]
[locate x=223 y=435]
[button name=config_effectoff exp="setNoEffectButton(true)"]

;スキップ
[locate x=64 y=534]
[button name="config_skipall" exp="setSkipButton(true)"]
[locate x=223 y=534]
[button name="config_skipalrdy" exp="setSkipButton(false)"]

; BGM音量
[locate x=431 y=147]
[slider width=303 height=28 opacity=0 normal="slidebar" over="slidebar_over" min=0 max=100 value=kag.bgmvolume nohilight]

; SE音量
[locate x=431 y=241]
[slider width=303 height=28 opacity=0 normal="slidebar" over="slidebar_over" min=0 max=100 value=kag.sevolume nohilight]

; VOICE音量
[locate x=431 y=336]
[slider width=303 height=28 opacity=0 normal="slidebar" over="slidebar_over" min=0 max=100 value=kag.voicevolume nohilight]

; ボイスボタン
[locate x=432 y=434]
[button name="config_voice0" normal="config_voice0_on" exp="setVoiceOnButton('voice0')"]
[locate x=526 y=434]
[button name="config_voice1" normal="config_voice1_on" exp="setVoiceOnButton('voice1')"]
[locate x=621 y=434]
[button name="config_voice2" normal="config_voice2_on" exp="setVoiceOnButton('voice2')"]
[locate x=432 y=480]
[button name="config_voice3" normal="config_voice3_on" exp="setVoiceOnButton('voice3')"]
[locate x=526 y=480]
[button name="config_voice4" normal="config_voice4_on" exp="setVoiceOnButton('voice4')"]
[locate x=621 y=480]
[button name="config_voice5" normal="config_voice5_on" exp="setVoiceOnButton('voice5')"]

;初期化
[locate x=680 y=531]
[button name="config_initial" exp="initial()"]

;戻るボタン
[if exp="tf.fromTitle"]
[locate x=672 y=47]
[button name="config_title" target=*backtotitle]
[else]
[locate x=672 y=47]
[button name="return" target=*backtogame]
[locate x=557 y=47]
[button name="config_title" exp="gotoTitle()"]
[endif]

[eval exp='configInit()']
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
[current layer=message0]
[history enabled=true]
[return]

*backtotitle
[rclick enabled=false jump=false]
[current layer=message0]
[jump storage="title.ks" target=*backtotitle]
