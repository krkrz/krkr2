[macro name=newtag]\
[font color=%color|0xff0000]\
こんな風にマクロを作ります\
[resetfont]\
[endmacro]\
[newtag color=0x00ff00]
[newtag]
[newtag color=0x0000ff]
[erasemacro name=newtag]\
[newtag]
[macro name=e][emb *][endmacro]\
[macro name=t][if exp=f.flag]flag is true[endif][ignore exp=f.flag]flag is false[endignore][endmacro]\
[eval exp="f.a = 0"]\
[link cond="f.a==1" target="*ラベル名"]選択肢[endlink cond="f.a==1" ]
円周率は : \
@eval exp="f.pi = Math.PI"
@e exp="f.pi"
[l]
[eval exp=f.flag=true][t]
[eval exp=f.flag=false][t][l]
[iscript]
f.flag = 1633;
[endscript]
[emb exp=f.flag][l]
(2*3+5*6)/6 = [e exp="(2*3+5*6)/6"][l]
f.res = Math.sin(Math.PI/2) = [e exp="f.res = Math.sin(Math.PI/2)"][l]
f.res = [e exp="f.res"][l]
次の文字は見えないはずです : [ch cond=false text="文字"][l]
@if exp=true
1[ch text="a"]
@if exp=false
2[ch text="b"]
@if exp=true
3[ch text="c"]
@endif
4[ch text="d"]
@endif
5[ch text="e"]
@endif
6[ch text="f"]
[p]
