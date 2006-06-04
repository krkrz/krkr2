@call storage="world.ks"
@loadplugin module=wuvorbis.dll
@loadplugin module=extrans.dll

*start|スタート

[clearlayers]

[seladd text="レイヤテスト" storage=layer.ks target=*start]
[seladd text="キャラクタテスト" storage=char.ks target=*start]
[seladd text="トランジションテスト" storage=transition.ks target=*start]
[seladd text="環境レイヤテスト" storage=envlayer.ks target=*start]
[seladd text="選択肢変数テスト" exp='f.test=100']
[select]

[layopt layer=message0 visible]
設定された値:[emb exp="f.test"][l]

[jump target=*start]
