[position left=16 top=16 width=&(640-32) height=&(480-32)]\
[image storage="_24" page=fore layer=base bfloor=128 gfloor=128 rfloor=128]\
*loop1|loop1
@cm
1:トランジションを開始します(children=true)。[l]
[backlay]\
[image storage="uni" page=back layer=0 left=10 visible=true]\
[image storage="_24_3" page=back layer=base]\
[image storage="uni" page=back layer=1 left=100 visible=true index=1002000]\
[trans method=universal rule=trans1 layer=base vague=60 time=5000]\
[wt]\
*loop2|loop2
@cm
2:もう一回(children=true)[l]
[backlay]\
[image storage="uni" page=back layer=0 left=140 visible=true]\
[image storage="_24" page=back layer=base]\
[image storage="uni" page=back layer=1 left=200 visible=true]\
[trans rule=trans1 children=true layer=base vague=60000 time=5000]\
[wt]\
[jump target=*loop1]
