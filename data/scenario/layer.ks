*start|スタート

; ワールド拡張の特殊環境は解除しておく
[linemode]
[csysbutton]
[autolabelmode mode=false]

[layopt layer=message0 visible]
KAGEX のレイヤ拡張機能のサンプルです。レイヤに対して回転・拡大縮小の制御ができます。
[l][r]

[image layer=base page=fore storage="bg01_01"  visible=true left=0 top=0]
[image layer=0     page=fore storage="ch_0_0_1" visible=true left=400 top=450 dafx=center dafy=center]
[cm]

スタート
[l][r]

*label1|半透明

[layopt layer=0 opacity=200]
半透明
[l][r]

*label2|回転

[layopt layer=0 reset]
[layopt layer=0 rotate=30]
回転
[l][r]

[layopt layer=0 afx=left afy=top]
回転原点変更（左上隅）
[l][r]

[layopt layer=0 afx=100 afy=100]
回転原点変更（座標指定）
[l][r]

[layopt layer=0 afx=center afy=center]
回転原点変更（センターに戻す）
[l][r]

*label3|拡大

[layopt layer=0 reset]
[layopt layer=0 zoom=150]
拡大
[l][r]

*label4|X拡大

[layopt layer=0 reset]
[layopt layer=0 zoomx=200]
X拡大
[l][r]

*label5|合成モード

[layopt layer=0 reset]
[layopt layer=0 type=ltPsDifference5]
合成モード
[l][r]

*label6|ガクブル

[layopt layer=0 reset]
[action layer=0 module=LayerVibrateActionModule vibration=20 waitTime=50]
ガクブル
[l][r]

*label7|縦ゆれ

[layopt layer=0 reset]
[action layer=0 module=LayerJumpActionModule vibration=20 cycle=1000]
縦ゆれ
[l][r]

*label8|横ゆれ

[layopt layer=0 reset]
[action layer=0 module=LayerWaveActionModule vibration=20 cycle=1000]
横ゆれ
[l][r]

*label9|指定角度に

[layopt layer=0 reset]
[action layer=0 module=LayerToRotateModule angle=30 time=1000]
指定角度に
[wact]
[action layer=0 module=LayerToRotateModule angle=-50 time=1000]
移動してみる
[wact]
[action layer=0 module=LayerToRotateModule angle=-0 time=1000]
テストだよ
[wact]
[l][r]

*label10|くるくるまわる

[layopt layer=0 reset]
[action layer=0 module=LayerNormalRotateModule angvel=100]
くるくるまわる
[l][r]

*label11|拡大率

[layopt layer=0 reset]
[action layer=0 module=LayerNormalZoomModule zoom=150 time=1000]
キャラをズーム（時間指定）
[l][r]

*label12|ズーム回転

[layopt layer=0 reset]
[action layer=0 module=LayerVRotateZoomModule angvel=100]
ズームを利用した回転っぽいなにか
[l][r]

*label13|不透明度

[layopt layer=0 reset]
[action layer=0 module=LayerFadeToModeModule opacity=128 time=1000]
指定した不透明度に（時間指定）
[l][r]

*label14|フェード点滅

[layopt layer=0 reset]
[action layer=0 module=LayerFadeModeModule intime=1000 outtime=1000]
フェード点滅
[l][r]

*label15|動画レイヤテスト

[layopt layer=0 reset]
[image layer=1 movie=test.mpg alpha=2 loop visible=true dafx=center dafy=center left=400 top=300]
動画ロード
[l][r]

[layopt layer=1 zoom=50 time=1000]
ズーム変更
[l][r]

[layopt layer=1 rotate=120 time=1000]
回転
[l][r]

[jump storage=s0001.ks]
