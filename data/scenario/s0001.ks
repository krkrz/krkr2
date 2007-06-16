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
[scenestart storage=char.sc]
[sceneplay]
[jump target=*start]

*envlayer|
[scenestart storage=envlayer.sc]
[sceneplay]
[jump target=*start]

*transition|
[scenestart storage=transition.sc]
[sceneplay]
[jump target=*start]

*action|
[scenestart storage=action.sc]
[sceneplay]
[jump target=*start]
