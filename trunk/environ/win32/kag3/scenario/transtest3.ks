@loadplugin module=extrans.dll
[position left=16 top=16 width=&(640-32) height=&(480-32)]\
[image storage="_24" page=fore layer=base]\
*loop1|loop1
@cm
1:トランジションを開始します(method=wave)。[l]
[backlay]\
[image storage="_24_3" page=back layer=base]\
[trans method=wave time=2500]\
[wt]\
@l
@cm
1:トランジションを開始します(method=mosaic)。[l]
[backlay]\
[image storage="_24" page=back layer=base]\
[trans method=mosaic time=2500]\
[wt]\
@l
@cm
1:トランジションを開始します(method=turn)。[l]
[backlay]\
[image storage="_24_3" page=back layer=base]\
[trans method=turn time=2500]\
[wt]\
@l
@cm
1:トランジションを開始します(method=rotatezoom factor=0 accel=-2 twistaccel=-2)。[l]
[backlay]\
[image storage="_24" page=back layer=base]\
[trans method=rotatezoom time=2500 factor=0 accel=-2 twistaccel=-2]\
[wt]\
@l
@cm
1:トランジションを開始します(method=rotatezoom factor=5 accel=2)。[l]
[backlay]\
[image storage="_24_3" page=back layer=base]\
[trans method=rotatezoom time=2500 factor=5 accel=2]\
[wt]\
@l
@cm
1:トランジションを開始します(method=rotatezoom factor=5 accel=2 twist=1)。[l]
[backlay]\
[image storage="_24" page=back layer=base]\
[trans method=rotatezoom time=2500 factor=5 accel=2 twist=1]\
[wt]\
@l
@cm
1:トランジションを開始します(method=rotatevanish)。[l]
[backlay]\
[image storage="_24_3" page=back layer=base]\
[trans method=rotatevanish time=2500]\
[wt]\
@l
@cm
1:トランジションを開始します(method=rotateswap)。[l]
[backlay]\
[image storage="_24" page=back layer=base]\
[trans method=rotateswap time=2500]\
[wt]\
@l
[link target=*loop1]もう一回[endlink]\
@s
