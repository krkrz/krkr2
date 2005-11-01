[wait time=200]
*start|スタート
[cm]
try saving and loading at stopping with [emb exp="'[p]'"].
[if exp="false"]
x
[elsif exp="true"]
1
[  if exp="false"]
x
[  elsif exp="false"]
x
[  else]
2
[  endif]
3
[else]
x
[endif]
(3)
[r]
[p]
[if exp="true"]
1
[elsif exp="true"]
x
[else]
x
[  if exp="true"]
x
[  else]
x
[  endif]
x
[endif]
(1)
[r]

[if exp="false"]
x
[elsif exp="true"]
1
[call target="*foo"]
3
[  if exp="true"]
4
[  else]
x1
[  endif]
[else]
x2
[endif]
(4)
[r]


[if exp="false"]
x
[else]
1
[  if exp="false"]
x
[  else]
2
[  call target="*bar"]
9
[  endif]
10
[endif]
(10)
[r]

[s]
;;;;;;;;;;;;;;;;

*foo|aa
[if exp="true"]
2[p]
[elsif exp="true"]
x
[else]
x
[  if exp="true"]
x
[  else]
x
[  endif]
x
[endif]
[return]


*bar
[if exp="false"]
x
[elsif exp="true"]
3
[call storage="else-elsif_test2.ks" target="*aaa"]
8
[return]
x
[else]
x
[  if exp="true"]
x
[  else]
x
[  endif]
x
[endif]
