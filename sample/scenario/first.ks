@call storage="world.ks"
@loadplugin module=wuvorbis.dll
@loadplugin module=extrans.dll

*start|スタート

[cm]
[clearlayers]
[layopt layer=message0 visible]
[current layer=message0]
[link storage=layer.ks target=*start]レイヤテスト[endlink][r]
[link storage=char.ks  target=*start]キャラクタテスト[endlink][r]
[link storage=transition.ks  target=*start]トランジションテスト[endlink][r]
[link storage=envlayer.ks  target=*start]環境レイヤテスト[endlink][r]
[s]
