[loadplugin module=wuvorbis.dll]
[current page=fore layer=message0]

[playbgm storage="bgm01.ogg"]
[cm]
頭からBGM開始[p]

[playbgm storage="bgm01.ogg" start=ラベル0 loop=false]
[setbgmstop target=*bgmend]
[setbgmlabel name=ラベル1 exp='dm("BGMラベル1通過")']
[setbgmlabel name=ラベル2 target=*bgmlabel2 exp='dm("BGMラベル2通過")']
[cm]
途中からBGM開始[p]
[cm]
待ち中
[s]

*bgmlabel2|
BGMのラベル2を通過したのでジャンプしてきた
[s]

*bgmend|
BGMが終了したのでジャンプしてきた[p]

*start|
[cm]
新規ジャンプ処理のテスト[r]
[link target=*link]通常のリンク[endlink]
[timeout time=5000 target=*timeout se=musicbox.ogg]
[wheel   target=*wheel   se=musicbox.ogg]
[click    target=*click   se=musicbox.ogg]
[s]

*link|
[cm]
リンクがよばれました[p]
[jump target=*start]

*timeout|
[cm]
タイムアウトがよばれました[p]
[jump target=*start]

*wheel|
[cm]
ホイールがよばれました[p]
[jump target=*start]

*click|
[cm]
クリックがよばれました[p]
[jump target=*start]
