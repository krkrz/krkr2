@wc time=20
[delay speed=user]文字表示速度 : ユーザ選択[l]
[delay speed=10]文字表示速度 : 10[l]
[delay speed=100]文字表示速度 : 100[l]
[nowait]文字表示速度 : ノーウェイト[endnowait][l]
文字表示速度 : 100[l]
[delay speed=user]\
*start
5秒待つ。[l]
@wait time=5000
待った。[l]
6秒カウントする。[l]
@resetwait
@wait mode=until time=1000
1 \
@wait mode=until time=2000
2 \
@wait mode=until time=3000
3 \
@wait mode=until time=4000
4 \
@wait mode=until time=5000
5 \
@wait mode=until time=6000
6 \
待った。[p]\
@ct
@jump target=*start
