@loadcell
@loop
;
@macro name=copyone
@copy dx=0 dy=0 sx=%x sy=0 sw=16 sh=20
@wait time=20
@endmacro


; 0 -> 9 ŒJ‚è•Ô‚µ
@eval exp="f.af0 = 0"
*start
@copyone x=&f.af0
@eval exp="(f.af0 += 2), (f.af0 = 0 if f.af0 > 144)"
@jump target=*start

; 0 -> 9 -> 0 ˆê‰ñ‚¾‚¯
*start1
@macro name=copyone1
@copy dx=16 dy=0 sx=%x sy=0 sw=16 sh=20
@wait time=200
@endmacro

@copyone1 x=0
@copyone1 x=16
@copyone1 x=32
@copyone1 x=48
@copyone1 x=64
@copyone1 x=80
@copyone1 x=96
@copyone1 x=112
@copyone1 x=128
@copyone1 x=144
@copyone1 x=144
@copyone1 x=128
@copyone1 x=112
@copyone1 x=96
@copyone1 x=80
@copyone1 x=64
@copyone1 x=48
@copyone1 x=32
@copyone1 x=16
@copyone1 x=0
@s

; 0 -> 9 -> 0 ŒJ‚è•Ô‚µ
*start2
@loop
@macro name=copyone2
@copy dx=32 dy=0 sx=%x sy=0 sw=16 sh=20
@wait time=200
@endmacro

*loop2
@home
@copyone2 x=0
@copyone2 x=16
@copyone2 x=32
@copyone2 x=48
@copyone2 x=64
@copyone2 x=80
@copyone2 x=96
@copyone2 x=112
@copyone2 x=128
@copyone2 x=144
@copyone2 x=144
@copyone2 x=128
@copyone2 x=112
@copyone2 x=96
@copyone2 x=80
@copyone2 x=64
@copyone2 x=48
@copyone2 x=32
@copyone2 x=16
@copyone2 x=0
@jump target=*loop2


