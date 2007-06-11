*start|テストシナリオ

[clearlayers]
[seladd text="KAGEX 拡張レイヤ"            target=*layer]
[seladd text="ワールド拡張:キャラクタ"     target=*char]
[seladd text="ワールド拡張:レイヤ"         target=*envlayer]
[seladd text="ワールド拡張:トランジション" target=*transition]
[seladd text="ワールド拡張:拡張アクション" target=*action]
[select]

*layer|
[call storage=layer.ks]
[jump target=*start]

*char|
[initscene storage=char.sc]
[scenemain]
[jump target=*start]

*envlayer|
[initscene storage=envlayer.sc]
[scenemain]
[jump target=*start]

*transition|
[initscene storage=transition.sc]
[scenemain]
[jump target=*start]

*action|
[initscene storage=action.sc]
[scenemain]
[jump target=*start]
