.386p
model flat

ASSUME   CS: FLAT, DS: FLAT, SS: FLAT, ES: FLAT
_TEXT   SEGMENT PUBLIC DWORD USE32 PUBLIC 'CODE'

EXTRN	UIExecSign			:near
EXTRN	ExitProcess				:near



EntryPoint proc near
	call UIExecSign
	push 0
	call ExitProcess
EntryPoint endp

_TEXT   ENDS

END EntryPoint
