.386p
model flat

ASSUME   CS: FLAT, DS: FLAT, SS: FLAT, ES: FLAT
_TEXT   SEGMENT PUBLIC DWORD USE32 PUBLIC 'CODE'

EXTRN	UIExecLoopTuner			:near
EXTRN	ExitProcess				:near



EntryPoint proc near
	call UIExecLoopTuner
	push 0
	call ExitProcess
EntryPoint endp

_TEXT   ENDS

END EntryPoint
