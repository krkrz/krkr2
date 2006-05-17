@call storage="snow.ks"
@image layer=base page=fore storage=_24_3
*start1|á‚Í”ñ•\¦
á‚ğ‚Ó‚ç‚¹‚Ü‚·[l]
@backlay
@snowinit forevisible=false backvisible=true
@trans method=crossfade time=2000
@wt
;
;@jump target=*test
;
*start2|á‚Í•\¦’†
”wŒi‚ğ“ü‚ê‘Ö‚¦‚Ü‚·[l]
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
á‚ğ‚â‚Ü‚¹‚Ü‚·[l]
@backlay
@snowopt backvisible=false
@trans method=crossfade time=2000
@wt
@snowuninit
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

