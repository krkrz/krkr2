;@mappfont storage="D:\Documents and Settings\Administrator\デスクトップ\font.tft"
;[deffont size=24 antialiased=false bold=false]\
;[position vertical=true]\
@image storage="_24" page=fore layer=base
;@image storage="pagebreak" layer=0 left=100 top=100 visible=true fliplr=true
;@image storage="linebreak" layer=1 left=150 top=100 visible=true flipud=true
;@image storage="linebreak" layer=2 left=200 top=100 visible=true flipud=true fliplr=true
;@position vertical=true
@wait time=300
@eval exp="f.hoge = [1,2,3]"
*s0|&'電子レンジの歴史'
@ct
[link hint="電子レンジの歴史"]電子レンジ之歴史[endlink][l]
あ[edit length=420]
@eval exp="kag.fore.messages[0].links[1].object.focus()"
[checkbox] チェックボックス
;@eval exp="kag.conductor.callLabel('*sub1')"
@call target="&'*sub1'"
[nowait]彼らは[endnowait]火を使わないかわりに、[l]手から[link hint="気"]「気」[endlink]と呼ばれる[link hint="未知"]未知[endlink]なるものを放出して物を温めている。[p]
@jump target=*s1 storage=_oven2.ks

*sub1
[DEFFONT FACE="ＭＳ Ｐゴシック"][resetfont]中国の奥地に未だ[deffont face="user"][resetfont]火を使わない[link hint="民族"]民族[endlink]が住んでいる。[l]
@return
