ideal
p386
model use32 flat

extrn UIExecConfig:near
extrn ExitProcess:near


codeseg

start:
	call	UIExecConfig
	push	0
	call	ExitProcess


end start
