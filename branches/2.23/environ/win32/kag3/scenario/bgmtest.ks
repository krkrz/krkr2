@loadplugin module=wuvorbis.dll
@l
@call storage=gvolume.ks
@gvolmenu name="BGM(&B)" control=bgm
@gvolmenu name="効果音その１(&1)" control=0
@gvolmenu name="効果音その２(&2)" control=1
*start|はじめ
@startanchor
@ct
[playbgm storage=c:\mss\musicbox.ogg loop=false]\
[wl canskip=true][l]\
*1|１
@ct
[fadeinbgm time=6000 storage="c:\mss\trk.wav"]\
音楽をフェードインしながら再生[l]
[wb canskip=true][l]\
@pausebgm
@l
@resumebgm
@l
*1b|１ｂ
@ct
音楽をフェードインしながら再生した。[l]
*2|２
@ct
[xchgbgm storage="c:\mss\trk.ogg" time=5000 overlap=5000]\
音楽をクロスフェード
[wb][l]\
;[xchgbgm storage="c:\mss\trk_e_03.ogg" outtime=5000 intime=0 overlap=3000]\
;[xchgbgm storage="c:\mss\trk_e_03.ogg" overlap=5000]\
[xchgbgm storage="c:\mss\trk_e_03.ogg" overlap=5000 volume=50]\
音楽をクロスフェード
[wb canskip=true][l]\
[l]\
*2b|２ｂ
@ct
音楽をクロスフェードした。[l]
[fadebgm volume=100 time=5000]\
音楽を100%音量までフェード
[wb][l]\
*3|３
@ct
[fadebgm volume=50 time=5000]\
音楽を50%音量までフェード
[wb][l]\
*3b|３ｂ
@ct
音楽を50%音量までフェードした。[l]
*4|４
@ct
[bgmopt volume=100]\
音楽を100%音量にした。[l]
*4b|４ｂ
@ct
音楽を100%音量にしたその２。[l]
*5|５
@ct
@stopbgm
音楽を停止した。[l]
*5b|５ｂ
@ct
音楽を停止したその２。[l]
*6|６
@ct
[playbgm storage="c:\mss\trk.wav"]\
音楽を再生した。[l]\
*6b|６ｂ
@ct
音楽を再生したその２。[l]
*7|７
@ct
[fadeoutbgm time=4000]\
音楽をフェードアウトする[l]
[wb]\
*7b|７ｂ
@ct
音楽をフェードアウトした[l]
[s]
