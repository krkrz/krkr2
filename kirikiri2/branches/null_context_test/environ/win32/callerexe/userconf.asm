.386p
model flat

ASSUME   CS: FLAT, DS: FLAT, SS: FLAT, ES: FLAT
_TEXT   SEGMENT PUBLIC DWORD USE32 PUBLIC 'CODE'

EXTRN	_UIExecUserConfig		:near
EXTRN	ExitProcess				:near
PUBLIC	EntryPoint


EntryPoint proc near
	call _UIExecUserConfig
	push 0
	call ExitProcess
EntryPoint endp

_TEXT   ENDS

END EntryPoint
