[position left=16 top=16 width=&(640-32) height=&(480-32)]\
[image storage="_24" page=fore layer=base bfloor=128 gfloor=128 rfloor=128]\
[link]トランジション[endlink]を開始します。[l]\
[backlay]\
[image storage="_24_3" page=back layer=base]\
[image storage="uni" page=back layer=0 left=0 visible=true]\
;[image storage="PageBreak" page=back layer=1 left=200 top=100 visible=true]\
[trans rule=trans1 children=true layer=base vague=64 time=1000]\
[wt]\
*loop
もう[link]一回[endlink](children=true)[l]\
[backlay]\
[image storage="_24" page=back layer=base]\
[image storage="uni" page=back layer=0 left=100 visible=true]\
[trans rule=trans1 children=true layer=base vague=64 time=1000]\
[wt]\
もう[link]一回[endlink][l]\
[backlay]\
[image storage="_24_3" page=back layer=base]\
[image storage="uni" page=back layer=0 left=0 visible=true]\
[trans rule=trans1 children=false layer=base vague=64 time=1000]\
[wt]\
[jump target=*loop]