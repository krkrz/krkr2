*start|スタート

; 環境初期化処理
[initscene]

; BGM再生
[道路 normal]

[あい 通常 中]
キャラクタ出現

[newlay name=starf fire=star firecolor=red xpos=0 ypos=0 show]
炎を配置

[newlay name=star1 file=star xpos=0 ypos=0 show]
元の画像を重ねて配置

制御パラメータは world.txt参照。
タネ画像より広い範囲にはでれないので,余裕のある画像にしておいてください


[dellay name=starf]
[dellay name=star1]
消去
