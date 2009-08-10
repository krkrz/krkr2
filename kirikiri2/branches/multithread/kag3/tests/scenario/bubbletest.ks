@call storage="bubble.ks"
@image layer=base page=fore storage=_24_3
*start1|–A‚Í”ñ•\Ž¦
–A‚ð‚Ó‚ç‚¹‚Ü‚·[l]
@backlay
@bubbleinit forevisible=false backvisible=true
@trans method=crossfade time=2000
@wt
;
;@jump target=*test
;
*start2|–A‚Í•\Ž¦’†
”wŒi‚ð“ü‚ê‘Ö‚¦‚Ü‚·[l]
@backlay
@image layer=base page=back storage=_24
@trans method=crossfade time=2000
@wt
@l
@backlay
@image layer=base page=back storage=_24_3
@trans children=false method=crossfade time=2000
@wt
@l
–A‚ð‚â‚Ü‚¹‚Ü‚·[l]
@backlay
@bubbleopt backvisible=false
@trans method=crossfade time=2000
@wt
@bubbleuninit
@jump target=*start1



*test
@layopt layer=message0 page=fore visible=false
@backlay
@image layer=base page=back storage=_24
@trans children=false method=crossfade time=2000
@wt
@backlay
@image layer=base page=back storage=_24_3
@trans children=false method=crossfade time=2000
@wt
@jump target=*test

