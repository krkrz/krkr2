@macro name=call_a
call_a_1 
@emb exp=mp.param
@r
@call target=*a
call_a_2 
@emb exp=mp.param
@r
@endmacro
;
@macro name=call_b
call_b_1 
@emb exp=mp.param
@r
@call target=*b
call_b_2 
@emb exp=mp.param
@r
@endmacro
;
@macro name=call_c
call_c_1 
@emb exp=mp.param
@r
@call target=*c
call_c_2 
@emb exp=mp.param
@r
@endmacro
;
@macro name=macro_return
@return
@endmacro
;
@macro name=jump_0
@jump target=*0
@endmacro
;
@macro name=jump_1
@jump target=*1
@endmacro
;
;
;
;
;
;
;
*first
[call_a param=1]\
*savable|savable
@l
@jump target=*first

*a
@call_b param=2
@return

*b
@call_c param=3
@return

*b_2
label b_2
@return

*c
label c
@jump_0
@macro_return


*0|0
label 0
@l
@return target=*b_2


*1|1
label 1
@s

LoadScenario
ClearBuffer
Rewind
GoToLabel

	PopMacroArgsTo(MacroArgStackBase);
		// clear macro argument down to current base stack position
