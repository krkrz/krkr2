ideal
p386
model use32 flat

extrn UIExecFontMaker:near
extrn ExitProcess:near


codeseg

start:
	call	UIExecFontMaker
	push	0
	call	ExitProcess


end start
