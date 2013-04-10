@title name="このソフトについて"
@rclick enabled=false
@clickskip enabled=false
@position left=0 top=0 width=320 height=200 color=0xffffff opacity=255 marginl=0 margint=0 marginr=0 marginb=0
@style align=center
@font size=24 shadow=false color=0
@wait time=200
@nowait
@backlay
@current page=back
吉里吉里２
[emb exp="System.versionString"]
@trans method=crossfade time=500
@wt
KAG3
[emb exp="kagVersion"]
@trans method=crossfade time=500
@wt
[font size=12]サポートページ
[link hint="クリックするとサポートページを開きます" exp="System.shellExecute('http://kikyou.info/tvp/')"]http://kikyou.info/tvp/[endlink]
@trans method=crossfade time=500
@wt
[link target=*exit]閉じる[endlink]
@trans method=crossfade time=500
@wt
@s
*exit
@close
