.386p
model flat

ASSUME   CS: FLAT, DS: FLAT, SS: FLAT, ES: FLAT
_TEXT   SEGMENT PUBLIC DWORD USE32 PUBLIC 'CODE'

EXTRN	_callermain				:near
EXTRN	ExitProcess				:near

PUBLIC	EntryPoint


EntryPoint proc near
	call _callermain
	push 0
	call ExitProcess
EntryPoint endp

_TEXT   ENDS

END EntryPoint
