ideal
p386
model use32 flat

extrn UIExecReleaser:near
extrn ExitProcess:near


codeseg

start:
	call	UIExecReleaser
	push	0
	call	ExitProcess


end start
