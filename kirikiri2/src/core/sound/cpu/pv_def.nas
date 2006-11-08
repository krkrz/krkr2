%include		"nasm.nah"
externdef sin
externdef cos
globaldef def__Z4rdftiiPfPiS_
globaldef def__Z32RisaPCMConvertLoopInt16ToFloat32PvPKvj
globaldef def__Z31RisaInterleaveOverlappingWindowPfPKPKfS_ijj
globaldef def__Z32RisaPCMConvertLoopFloat32ToInt16PvPKvj
globaldef def__Z6makewtiPiPf
globaldef def__Z6makectiPiPf
globaldef def__ZN20tRisaPhaseVocoderDSP11ProcessCoreEi
globaldef def__Z30RisaDeinterleaveApplyingWindowPPfPKfS_ijj
segment_data_aligned
align 16
align 4
LC0:
	DD 939524096
align 16
segment_code
align 2
align 16
def__Z32RisaPCMConvertLoopInt16ToFloat32PvPKvj:
	push	ebp
	mov	ebp, esp
	mov	edx, DWORD  [ebp+12]
	mov	eax, DWORD  [ebp+16]
	mov	ecx, DWORD  [ebp+8]
	lea	eax, [edx+eax*2]
	cmp	edx, eax
	jae	L6
	fld	DWORD [LC0]
align 16
L4:
	fild	WORD  [edx]
	add	edx, 2
	fmul	st0, st1
	fstp	DWORD  [ecx]
	add	ecx, 4
	cmp	edx, eax
	jb	L4
	fstp	st0
L6:
	pop	ebp
	ret
segment_data_aligned
align 16
align 4
LC2:
	DD 1191181824
align 4
LC3:
	DD -956301312
align 4
LC5:
	DD 1056964608
align 16
segment_code
align 2
align 16
def__Z32RisaPCMConvertLoopFloat32ToInt16PvPKvj:
	push	ebp
	mov	ebp, esp
	push	ebx
	sub	esp, 8
	mov	edx, DWORD  [ebp+12]
	mov	eax, DWORD  [ebp+16]
	mov	ecx, DWORD  [ebp+8]
	lea	ebx, [edx+eax*4]
	cmp	edx, ebx
	jae	L22
	fld	DWORD [LC2]
	fldz
	fld	DWORD [LC3]
	fld	DWORD [LC5]
	jmp	L20
align 8
L27:
	fxch	st2
	fucom	st2
	fnstsw	ax
	sahf
	jbe	L14
	fstp	st2
	mov	eax, -32768
	add	edx, 4
	mov	WORD  [ecx], ax
	add	ecx, 2
	cmp	edx, ebx
	jae	L25
align 16
L20:
	fld	DWORD  [edx]
	fmul	st0, st4
	fucom	st4
	fnstsw	ax
	sahf
	jbe	L27
	fstp	st0
	mov	eax, 32767
L12:
	mov	WORD  [ecx], ax
	add	edx, 4
	add	ecx, 2
	cmp	edx, ebx
	jb	L20
L25:
	fstp	st0
	fstp	st0
	fstp	st0
	fstp	st0
L22:
	add	esp, 8
	pop	ebx
	pop	ebp
	ret
align 8
L14:
	fxch	st3
	fucom	st2
	fnstsw	ax
	sahf
	jbe	L17
	fxch	st2
	fsub	st0, st1
L23:
	fnstcw	WORD  [ebp-6]
	movzx	eax, WORD  [ebp-6]
	or	ax, 3072
	mov	WORD  [ebp-8], ax
	fldcw	WORD  [ebp-8]
	fistp	WORD  [ebp-10]
	fldcw	WORD  [ebp-6]
	fxch	st1
	fxch	st2
	fxch	st1
	movzx	eax, WORD  [ebp-10]
	jmp	L12
align 8
L17:
	fxch	st2
	fadd	st0, st1
	jmp	L23
align 2
align 16
_Z6cft1stiPfS_:
	push	ebp
	mov	ebp, esp
	push	edi
	mov	edi, eax
	push	esi
	mov	esi, 16
	push	ebx
	sub	esp, 36
	xor	ebx, ebx
	fld	DWORD  [edx+12]
	cmp	esi, eax
	fld	DWORD  [edx+4]
	fld	DWORD  [edx]
	fld	DWORD  [edx+8]
	fld	st2
	fsub	st0, st4
	fld	DWORD  [edx+16]
	fld	st3
	fadd	st0, st3
	fxch	st2
	fstp	DWORD  [ebp-36]
	fxch	st3
	fsubrp	st2, st0
	fxch	st3
	faddp	st4, st0
	fld	DWORD  [edx+24]
	fld	DWORD  [edx+20]
	fld	DWORD  [edx+28]
	fxch	st4
	fadd	st0, st2
	fxch	st3
	fstp	DWORD  [ebp-32]
	fld	st0
	fadd	st0, st4
	fxch	st2
	fsubr	DWORD  [edx+16]
	fxch	st1
	fsubrp	st4, st0
	fld	st4
	fld	DWORD  [ebp-36]
	fxch	st1
	fadd	st0, st4
	fxch	st6
	fsubrp	st4, st0
	fxch	st1
	fst	DWORD  [ebp-40]
	fsub	st1, st0
	fxch	st5
	fstp	DWORD  [edx]
	fld	st5
	fadd	st0, st2
	fxch	st3
	fstp	DWORD  [edx+16]
	fxch	st5
	fsubrp	st1, st0
	fld	DWORD  [edx+32]
	fxch	st2
	fstp	DWORD  [edx+4]
	fld	DWORD  [ebp-32]
	fld	DWORD  [edx+40]
	fxch	st2
	fstp	DWORD  [edx+20]
	fsub	st0, st3
	fxch	st2
	fadd	st0, st1
	fxch	st1
	fsubr	DWORD  [edx+32]
	fxch	st5
	fstp	DWORD  [edx+28]
	fxch	st1
	fstp	DWORD  [edx+8]
	fld	DWORD  [ebp-36]
	faddp	st3, st0
	fxch	st2
	fstp	DWORD  [edx+12]
	fld	DWORD  [ebp-32]
	faddp	st1, st0
	fld	DWORD  [edx+36]
	fld	st0
	fxch	st2
	fstp	DWORD  [edx+24]
	fld	DWORD  [edx+44]
	fld	DWORD  [ecx+8]
	fxch	st5
	fstp	DWORD  [ebp-32]
	fsub	st1, st0
	faddp	st2, st0
	fld	DWORD  [edx+48]
	fld	DWORD  [edx+60]
	fxch	st2
	fstp	DWORD  [ebp-36]
	fld	DWORD  [edx+52]
	fxch	st1
	fadd	DWORD  [edx+56]
	fld	DWORD  [edx+48]
	fld	st2
	fadd	st0, st4
	fxch	st1
	fsub	DWORD  [edx+56]
	fxch	st3
	fsubrp	st4, st0
	fld	st5
	fadd	st0, st2
	fxch	st6
	fsubrp	st2, st0
	fxch	st3
	fst	DWORD  [ebp-44]
	fxch	st5
	fstp	DWORD  [edx+32]
	fld	st3
	fadd	st0, st3
	fxch	st1
	fstp	DWORD  [edx+52]
	fxch	st2
	fsubrp	st3, st0
	fld	DWORD  [ebp-32]
	fld	DWORD  [ebp-36]
	fxch	st3
	fstp	DWORD  [edx+36]
	fsub	st0, st4
	fxch	st2
	fadd	st0, st1
	fxch	st3
	fstp	DWORD  [edx+48]
	fld	st1
	fadd	st0, st3
	fxch	st2
	fsubrp	st3, st0
	fxch	st1
	fmul	st0, st4
	fxch	st1
	fsub	DWORD  [ebp-36]
	fxch	st2
	fmul	st0, st4
	fxch	st1
	fstp	DWORD  [edx+44]
	fxch	st2
	fadd	DWORD  [ebp-32]
	fxch	st2
	fstp	DWORD  [edx+40]
	fld	st0
	fsub	st0, st2
	fxch	st1
	faddp	st2, st0
	fmul	st0, st2
	fxch	st1
	fmulp	st2, st0
	fstp	DWORD  [edx+56]
	fstp	DWORD  [edx+60]
	jge	L34
	add	edx, 64
align 16
L32:
	fld	DWORD  [edx]
	add	ebx, 2
	add	esi, 16
	fld	DWORD  [ecx+4+ebx*4]
	lea	eax, [ebx+ebx]
	fld	DWORD  [ecx+eax*4]
	fld	st1
	fadd	st0, st2
	fld	DWORD  [ecx+4+eax*4]
	fxch	st3
	fstp	DWORD  [ebp-20]
	fld	st0
	fmul	st0, st2
	fld	DWORD  [ecx+ebx*4]
	fxch	st2
	fmul	st0, st4
	fxch	st5
	fadd	DWORD  [edx+8]
	fxch	st2
	fstp	DWORD  [ebp-16]
	fsub	st0, st3
	fld	DWORD  [edx+4]
	fxch	st5
	fsubr	st0, st3
	fld	DWORD  [edx+28]
	fld	st6
	fxch	st3
	fstp	DWORD  [ebp-28]
	fld	DWORD  [edx]
	fsub	DWORD  [edx+8]
	fxch	st2
	fstp	DWORD  [ebp-24]
	fld	DWORD  [edx+12]
	fadd	st3, st0
	fsubp	st7, st0
	fld	DWORD  [edx+16]
	fsub	DWORD  [edx+24]
	fxch	st2
	fstp	DWORD  [ebp-32]
	fld	DWORD  [edx+16]
	fadd	DWORD  [edx+24]
	fxch	st7
	fstp	DWORD  [ebp-36]
	fxch	st1
	fstp	DWORD  [ebp-40]
	fld	st2
	fadd	st0, st6
	fld	DWORD  [edx+20]
	fxch	st4
	fsubrp	st7, st0
	fxch	st1
	fadd	st0, st3
	fxch	st1
	fstp	DWORD  [edx]
	fld	st1
	fadd	st0, st1
	fxch	st3
	fsub	DWORD  [edx+28]
	fxch	st2
	fsubrp	st1, st0
	fxch	st2
	fstp	DWORD  [edx+4]
	fld	DWORD  [ebp-16]
	fxch	st1
	fstp	DWORD  [ebp-44]
	fld	DWORD  [ebp-20]
	fxch	st1
	fmul	st0, st5
	fxch	st1
	fmul	st0, st2
	fxch	st5
	fmul	DWORD  [ebp-20]
	fxch	st2
	fmul	DWORD  [ebp-16]
	fxch	st1
	fsubrp	st5, st0
	fld	st3
	fxch	st5
	fstp	DWORD  [edx+16]
	faddp	st1, st0
	fld	st1
	fld	DWORD  [ebp-32]
	fsub	DWORD  [ebp-44]
	fxch	st2
	fstp	DWORD  [edx+20]
	fld	DWORD  [ebp-36]
	fadd	DWORD  [ebp-40]
	fxch	st1
	fmul	st0, st2
	fxch	st4
	fmulp	st2, st0
	fmul	st4, st0
	fmulp	st2, st0
	fxch	st2
	fsubrp	st3, st0
	faddp	st1, st0
	fxch	st1
	fstp	DWORD  [edx+8]
	fld	DWORD  [ebp-36]
	fsub	DWORD  [ebp-40]
	fld	DWORD  [ebp-32]
	fadd	DWORD  [ebp-44]
	fld	DWORD  [ebp-28]
	fld	DWORD  [ebp-24]
	fxch	st4
	fstp	DWORD  [edx+12]
	fmul	st0, st2
	fxch	st3
	fmul	st0, st1
	fxch	st2
	fmul	DWORD  [ebp-24]
	fxch	st1
	fmul	DWORD  [ebp-28]
	fxch	st2
	fsubrp	st3, st0
	fld	DWORD  [ebp-16]
	fld	DWORD  [edx+60]
	fxch	st2
	faddp	st3, st0
	fadd	st0, st0
	fxch	st3
	fstp	DWORD  [edx+24]
	fld	DWORD  [edx+32]
	fld	st3
	fxch	st3
	fstp	DWORD  [edx+28]
	fld	DWORD  [ecx+8+eax*4]
	fld	DWORD  [ecx+12+eax*4]
	fxch	st2
	fadd	DWORD  [edx+40]
	fxch	st5
	fmul	st0, st1
	fxch	st4
	fmul	st0, st2
	fxch	st4
	fsub	st0, st2
	fxch	st4
	fsubr	st0, st1
	fld	DWORD  [edx+36]
	fxch	st5
	fstp	DWORD  [ebp-28]
	fld	st4
	fld	DWORD  [edx+32]
	fxch	st2
	fstp	DWORD  [ebp-24]
	fld	DWORD  [edx+44]
	fxch	st2
	fsub	DWORD  [edx+40]
	fxch	st6
	fsub	st0, st2
	fxch	st1
	faddp	st2, st0
	fld	DWORD  [edx+48]
	fsub	DWORD  [edx+56]
	fxch	st6
	fstp	DWORD  [ebp-32]
	fld	DWORD  [edx+48]
	fadd	DWORD  [edx+56]
	fxch	st1
	fstp	DWORD  [ebp-36]
	fld	DWORD  [edx+52]
	fxch	st6
	fstp	DWORD  [ebp-40]
	fld	st6
	fxch	st5
	fadd	st0, st6
	fxch	st6
	fsub	DWORD  [edx+60]
	fxch	st5
	fadd	st0, st1
	fxch	st7
	fsubrp	st1, st0
	fxch	st4
	fstp	DWORD  [ebp-44]
	fxch	st5
	fstp	DWORD  [edx+32]
	fld	st4
	fadd	st0, st4
	fld	DWORD  [ebp-20]
	fxch	st6
	fsubrp	st5, st0
	fld	DWORD  [ebp-16]
	fxch	st6
	fchs
	fxch	st1
	fstp	DWORD  [edx+36]
	fxch	st5
	fmul	st0, st4
	fld	st5
	fmul	st0, st4
	fxch	st5
	fmulp	st6, st0
	fxch	st3
	fmul	DWORD  [ebp-16]
	fxch	st4
	fsubrp	st3, st0
	fld	st1
	fxch	st5
	fst	DWORD  [ebp-48]
	fxch	st3
	fstp	DWORD  [edx+48]
	fxch	st2
	fadd	st0, st3
	fxch	st3
	fstp	DWORD  [ebp-16]
	fld	DWORD  [ebp-36]
	fld	DWORD  [ebp-32]
	fsub	DWORD  [ebp-44]
	fxch	st1
	fadd	DWORD  [ebp-40]
	fxch	st4
	fstp	DWORD  [edx+52]
	fld	st2
	fmul	st0, st1
	fxch	st5
	fmul	st0, st4
	fxch	st3
	fmulp	st4, st0
	fmulp	st1, st0
	fld	DWORD  [ebp-36]
	fxch	st4
	fsubrp	st2, st0
	fxch	st3
	fsub	DWORD  [ebp-40]
	fxch	st2
	faddp	st3, st0
	fld	DWORD  [ebp-32]
	fadd	DWORD  [ebp-44]
	fld	DWORD  [ebp-28]
	fxch	st2
	fstp	DWORD  [edx+40]
	fld	DWORD  [ebp-24]
	fxch	st2
	fmul	st0, st3
	fxch	st2
	fmul	st0, st1
	fxch	st3
	fmul	DWORD  [ebp-24]
	fxch	st1
	fmul	DWORD  [ebp-28]
	fxch	st3
	fsubrp	st2, st0
	fxch	st3
	fstp	DWORD  [edx+44]
	fld	st2
	fstp	DWORD  [ebp-24]
	fxch	st2
	fadd	st0, st1
	fxch	st2
	fstp	DWORD  [edx+56]
	fstp	DWORD  [ebp-28]
	fstp	DWORD  [edx+60]
	add	edx, 64
	cmp	esi, edi
	jl	L32
L34:
	add	esp, 36
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
align 2
align 16
_Z6cftmdliiPfS_:
	push	ebp
	mov	ebp, esp
	push	edi
	mov	edi, edx
	push	esi
	xor	esi, esi
	push	ebx
	sub	esp, 72
	mov	DWORD  [ebp-16], eax
	lea	eax, [0+edx*4]
	mov	DWORD  [ebp-28], eax
	jmp	L67
align 8
L71:
	fld	DWORD  [ecx+esi*4]
	lea	edx, [esi+edi]
	lea	eax, [edx+edi]
	fld	DWORD  [ecx+edx*4]
	fld	st1
	lea	ebx, [eax+edi]
	fld	DWORD  [ecx+4+esi*4]
	fxch	st3
	fsub	st0, st2
	fxch	st1
	faddp	st2, st0
	fld	DWORD  [ecx+4+edx*4]
	fld	st3
	fld	DWORD  [ecx+ebx*4]
	fxch	st5
	fsub	st0, st2
	fxch	st1
	faddp	st2, st0
	fxch	st2
	fstp	DWORD  [ebp-56]
	fld	DWORD  [ecx+eax*4]
	fxch	st2
	fstp	DWORD  [ebp-60]
	fld	DWORD  [ecx+4+eax*4]
	fld	st2
	fsub	st0, st5
	fld	DWORD  [ecx+4+ebx*4]
	fxch	st4
	faddp	st6, st0
	fld	st1
	fxch	st1
	fstp	DWORD  [ebp-64]
	fadd	st0, st3
	fxch	st1
	fsubrp	st3, st0
	fld	st3
	fadd	st0, st5
	fxch	st4
	fsubrp	st5, st0
	fxch	st3
	fstp	DWORD  [ecx+esi*4]
	fld	st0
	fadd	st0, st3
	fxch	st1
	fsubrp	st3, st0
	fstp	DWORD  [ecx+4+esi*4]
	add	esi, 2
	fld	DWORD  [ebp-56]
	fxch	st3
	fstp	DWORD  [ecx+eax*4]
	fxch	st1
	fstp	DWORD  [ecx+4+eax*4]
	fsub	st1, st0
	fadd	DWORD  [ebp-56]
	fxch	st1
	fstp	DWORD  [ecx+edx*4]
	fld	DWORD  [ebp-60]
	fadd	DWORD  [ebp-64]
	fxch	st1
	fst	DWORD  [ebp-56]
	fxch	st1
	fstp	DWORD  [ecx+4+edx*4]
	fld	DWORD  [ebp-60]
	fsub	DWORD  [ebp-64]
	fxch	st1
	fstp	DWORD  [ecx+ebx*4]
	fstp	DWORD  [ecx+4+ebx*4]
L67:
	cmp	esi, edi
	jl	L71
	mov	eax, DWORD  [ebp+8]
	mov	esi, DWORD  [ebp-28]
	fld	DWORD  [eax+8]
	lea	eax, [edi+esi]
	cmp	eax, esi
	mov	DWORD  [ebp-84], eax
	fstp	DWORD  [ebp-32]
	jmp	L68
align 8
L72:
	fld	DWORD  [ecx+esi*4]
	lea	edx, [esi+edi]
	lea	eax, [edx+edi]
	fld	DWORD  [ecx+edx*4]
	fld	st1
	lea	ebx, [eax+edi]
	fld	DWORD  [ecx+4+esi*4]
	fxch	st3
	fsub	st0, st2
	fxch	st1
	faddp	st2, st0
	fld	DWORD  [ecx+4+edx*4]
	fld	st3
	fld	DWORD  [ecx+ebx*4]
	fxch	st5
	fsub	st0, st2
	fxch	st1
	faddp	st2, st0
	fxch	st2
	fstp	DWORD  [ebp-56]
	fld	DWORD  [ecx+eax*4]
	fxch	st2
	fstp	DWORD  [ebp-60]
	fld	DWORD  [ecx+4+eax*4]
	fld	st2
	fsub	st0, st5
	fld	DWORD  [ecx+4+ebx*4]
	fxch	st4
	faddp	st6, st0
	fld	st1
	fxch	st1
	fstp	DWORD  [ebp-64]
	fadd	st0, st3
	fxch	st1
	fsubrp	st3, st0
	fld	st3
	fadd	st0, st5
	fxch	st4
	fsubrp	st5, st0
	fxch	st3
	fstp	DWORD  [ecx+esi*4]
	fld	st0
	fadd	st0, st3
	fxch	st3
	fsubrp	st1, st0
	fld	DWORD  [ebp-60]
	fadd	DWORD  [ebp-64]
	fxch	st3
	fstp	DWORD  [ecx+4+esi*4]
	fxch	st3
	add	esi, 2
	fstp	DWORD  [ecx+4+eax*4]
	fld	DWORD  [ebp-56]
	fxch	st3
	fstp	DWORD  [ecx+eax*4]
	cmp	DWORD  [ebp-84], esi
	fsub	st2, st0
	fld	st2
	fadd	st0, st2
	fxch	st3
	fsubrp	st2, st0
	fxch	st2
	fmul	DWORD  [ebp-32]
	fxch	st1
	fmul	DWORD  [ebp-32]
	fld	DWORD  [ebp-64]
	fsub	DWORD  [ebp-60]
	fxch	st2
	fstp	DWORD  [ecx+4+edx*4]
	fld	DWORD  [ebp-56]
	fxch	st1
	fstp	DWORD  [ecx+edx*4]
	fld	st1
	fxch	st1
	faddp	st3, st0
	fsub	st0, st2
	fxch	st1
	faddp	st2, st0
	fmul	DWORD  [ebp-32]
	fxch	st1
	fmul	DWORD  [ebp-32]
	fxch	st1
	fstp	DWORD  [ecx+ebx*4]
	fstp	DWORD  [ecx+4+ebx*4]
L68:
	jg	L72
	mov	DWORD  [ebp-24], 0
	mov	edx, DWORD  [ebp-28]
	mov	eax, DWORD  [ebp-16]
	add	edx, edx
	cmp	edx, eax
	mov	DWORD  [ebp-68], edx
	mov	DWORD  [ebp-20], edx
	jge	L61
align 16
L76:
	add	DWORD  [ebp-24], 2
	mov	esi, DWORD  [ebp-20]
	mov	edx, DWORD  [ebp-24]
	mov	eax, DWORD  [ebp-24]
	add	edx, edx
	mov	DWORD  [ebp-72], edx
	mov	edx, DWORD  [ebp+8]
	fld	DWORD  [edx+eax*4]
	fstp	DWORD  [ebp-40]
	fld	DWORD  [edx+4+eax*4]
	mov	eax, DWORD  [ebp-72]
	fstp	DWORD  [ebp-44]
	fld	DWORD  [edx+eax*4]
	fld	DWORD  [ebp-44]
	fadd	st0, st0
	fxch	st1
	fstp	DWORD  [ebp-32]
	fld	DWORD  [edx+4+eax*4]
	mov	eax, esi
	add	eax, edi
	mov	DWORD  [ebp-76], eax
	cmp	eax, esi
	fst	DWORD  [ebp-36]
	fmul	st0, st1
	fxch	st1
	fmul	DWORD  [ebp-32]
	fxch	st1
	fsubr	DWORD  [ebp-32]
	fstp	DWORD  [ebp-48]
	fsub	DWORD  [ebp-36]
	fstp	DWORD  [ebp-52]
	jmp	L70
align 8
L73:
	fld	DWORD  [ecx+esi*4]
	lea	edx, [esi+edi]
	lea	eax, [edx+edi]
	fld	DWORD  [ecx+edx*4]
	fld	st1
	lea	ebx, [eax+edi]
	fld	DWORD  [ecx+4+esi*4]
	fxch	st3
	fsub	st0, st2
	fxch	st1
	faddp	st2, st0
	fld	DWORD  [ecx+4+edx*4]
	fld	st3
	fld	DWORD  [ecx+ebx*4]
	fxch	st5
	fsub	st0, st2
	fxch	st1
	faddp	st2, st0
	fxch	st2
	fstp	DWORD  [ebp-56]
	fld	DWORD  [ecx+eax*4]
	fxch	st2
	fstp	DWORD  [ebp-60]
	fld	DWORD  [ecx+4+eax*4]
	fld	st2
	fsub	st0, st5
	fld	DWORD  [ecx+4+ebx*4]
	fxch	st4
	faddp	st6, st0
	fld	st1
	fxch	st1
	fstp	DWORD  [ebp-64]
	fadd	st0, st3
	fld	DWORD  [ebp-44]
	fxch	st2
	fsubrp	st4, st0
	fld	st4
	fadd	st0, st6
	fxch	st5
	fsubrp	st6, st0
	fxch	st4
	fstp	DWORD  [ecx+esi*4]
	fld	st1
	fadd	st0, st4
	fxch	st2
	fsubrp	st4, st0
	fmul	st0, st3
	fxch	st1
	fstp	DWORD  [ecx+4+esi*4]
	add	esi, 2
	fld	DWORD  [ebp-40]
	fxch	st3
	fmul	DWORD  [ebp-40]
	fxch	st3
	fmul	st0, st4
	fxch	st4
	fmul	DWORD  [ebp-44]
	fxch	st4
	fsubrp	st1, st0
	fld	DWORD  [ebp-36]
	fxch	st3
	faddp	st4, st0
	fld	DWORD  [ebp-56]
	fxch	st1
	fstp	DWORD  [ecx+eax*4]
	fld	DWORD  [ebp-32]
	fxch	st1
	fsub	st0, st2
	fxch	st4
	fstp	DWORD  [ecx+4+eax*4]
	fld	DWORD  [ebp-60]
	fxch	st1
	fmul	st0, st4
	fxch	st1
	fadd	DWORD  [ebp-64]
	fxch	st4
	fmul	DWORD  [ebp-36]
	fxch	st3
	fmul	st0, st4
	fxch	st4
	fmul	DWORD  [ebp-32]
	fxch	st1
	fsubrp	st4, st0
	fld	DWORD  [ebp-52]
	fxch	st1
	faddp	st3, st0
	fld	DWORD  [ebp-56]
	fxch	st4
	fstp	DWORD  [ecx+edx*4]
	fld	DWORD  [ebp-48]
	fxch	st4
	faddp	st2, st0
	fxch	st2
	fstp	DWORD  [ecx+4+edx*4]
	fld	DWORD  [ebp-60]
	fxch	st3
	fmul	st0, st1
	fxch	st3
	fsub	DWORD  [ebp-64]
	fmul	st2, st0
	fxch	st3
	fsubrp	st2, st0
	fxch	st1
	fstp	DWORD  [ecx+ebx*4]
	fxch	st1
	fmul	DWORD  [ebp-48]
	fxch	st1
	cmp	DWORD  [ebp-76], esi
	fmul	DWORD  [ebp-52]
	faddp	st1, st0
	fstp	DWORD  [ecx+4+ebx*4]
L70:
	jg	L73
	fld	DWORD  [ebp-40]
	mov	eax, DWORD  [ebp-72]
	mov	edx, DWORD  [ebp+8]
	fadd	st0, st0
	mov	esi, DWORD  [ebp-20]
	fld	DWORD  [edx+8+eax*4]
	fstp	DWORD  [ebp-32]
	fld	DWORD  [edx+12+eax*4]
	mov	eax, DWORD  [ebp-28]
	add	esi, eax
	fst	DWORD  [ebp-36]
	fmul	st0, st1
	fxch	st1
	lea	edx, [esi+edi]
	fmul	DWORD  [ebp-32]
	fxch	st1
	cmp	edx, esi
	mov	DWORD  [ebp-80], edx
	fsubr	DWORD  [ebp-32]
	fstp	DWORD  [ebp-48]
	fsub	DWORD  [ebp-36]
	fstp	DWORD  [ebp-52]
	jg	L54
	jmp	L65
align 8
L66:
	lea	edx, [esi+edi]
L54:
	fld	DWORD  [ecx+esi*4]
	lea	eax, [edx+edi]
	lea	ebx, [eax+edi]
	fld	DWORD  [ecx+edx*4]
	fld	st1
	fld	DWORD  [ecx+4+esi*4]
	fxch	st3
	fsub	st0, st2
	fxch	st1
	faddp	st2, st0
	fld	DWORD  [ecx+4+edx*4]
	fld	st3
	fld	DWORD  [ecx+ebx*4]
	fxch	st5
	fsub	st0, st2
	fxch	st1
	faddp	st2, st0
	fxch	st2
	fstp	DWORD  [ebp-56]
	fld	DWORD  [ecx+eax*4]
	fxch	st2
	fstp	DWORD  [ebp-60]
	fld	DWORD  [ecx+4+eax*4]
	fld	st2
	fsub	st0, st5
	fld	DWORD  [ecx+4+ebx*4]
	fxch	st4
	faddp	st6, st0
	fld	st1
	fxch	st1
	fstp	DWORD  [ebp-64]
	fadd	st0, st3
	fld	DWORD  [ebp-44]
	fxch	st2
	fsubrp	st4, st0
	fld	st4
	fxch	st2
	fchs
	fxch	st2
	fadd	st0, st6
	fxch	st5
	fsubrp	st6, st0
	fld	DWORD  [ebp-40]
	fxch	st5
	fstp	DWORD  [ecx+esi*4]
	fld	st2
	fadd	st0, st1
	fxch	st3
	fsubrp	st1, st0
	fmul	st4, st0
	fxch	st2
	fstp	DWORD  [ecx+4+esi*4]
	fld	st0
	add	esi, 2
	fmul	st0, st5
	fxch	st1
	fmulp	st2, st0
	fxch	st4
	fmul	DWORD  [ebp-40]
	fxch	st4
	fsubrp	st3, st0
	fld	DWORD  [ebp-60]
	fadd	DWORD  [ebp-64]
	fld	DWORD  [ebp-36]
	fxch	st2
	faddp	st5, st0
	fld	DWORD  [ebp-56]
	fxch	st2
	fmul	st0, st1
	fxch	st2
	fsub	st0, st3
	fxch	st4
	fstp	DWORD  [ecx+eax*4]
	fld	DWORD  [ebp-32]
	fxch	st1
	fmul	DWORD  [ebp-32]
	fxch	st1
	fmul	st0, st4
	fxch	st4
	fmul	DWORD  [ebp-36]
	fxch	st5
	fstp	DWORD  [ecx+4+eax*4]
	fxch	st3
	fsubrp	st1, st0
	fld	DWORD  [ebp-52]
	fxch	st3
	faddp	st4, st0
	fld	DWORD  [ebp-56]
	fxch	st1
	fstp	DWORD  [ecx+edx*4]
	fld	DWORD  [ebp-48]
	fxch	st1
	faddp	st2, st0
	fxch	st3
	fstp	DWORD  [ecx+4+edx*4]
	fld	DWORD  [ebp-60]
	fxch	st3
	fmul	st0, st1
	fxch	st3
	fsub	DWORD  [ebp-64]
	fxch	st1
	fmul	DWORD  [ebp-52]
	fxch	st2
	fmul	st0, st1
	fxch	st1
	fmul	DWORD  [ebp-48]
	fxch	st3
	fsubrp	st1, st0
	fxch	st2
	faddp	st1, st0
	fxch	st1
	fstp	DWORD  [ecx+ebx*4]
	fstp	DWORD  [ecx+4+ebx*4]
	cmp	DWORD  [ebp-80], esi
	jg	L66
L65:
	mov	eax, DWORD  [ebp-68]
	mov	edx, DWORD  [ebp-16]
	add	DWORD  [ebp-20], eax
	cmp	DWORD  [ebp-20], edx
	jl	L76
L61:
	add	esp, 72
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
align 2
align 16
_Z6bitrv2iPiPf:
	push	ebp
	mov	ebp, esp
	push	edi
	push	esi
	mov	esi, eax
	push	ebx
	sub	esp, 32
	cmp	eax, 8
	mov	DWORD  [ebp-16], edx
	mov	DWORD  [edx], 0
	mov	DWORD  [ebp-20], 1
	jle	L104
align 16
L84:
	xor	ebx, ebx
	sar	esi, 1
	cmp	ebx, DWORD  [ebp-20]
	jge	L106
	mov	edx, DWORD  [ebp-16]
align 16
L83:
	mov	eax, DWORD  [edx]
	inc	ebx
	mov	edi, DWORD  [ebp-20]
	add	eax, esi
	mov	DWORD  [edx+edi*4], eax
	add	edx, 4
	cmp	ebx, edi
	jl	L83
L106:
	sal	DWORD  [ebp-20], 1
	mov	eax, DWORD  [ebp-20]
	sal	eax, 3
	cmp	eax, esi
	jl	L84
L104:
	mov	eax, DWORD  [ebp-20]
	mov	edi, DWORD  [ebp-20]
	sal	eax, 3
	add	edi, edi
	cmp	eax, esi
	je	L117
	mov	DWORD  [ebp-44], 1
	mov	esi, DWORD  [ebp-20]
	cmp	DWORD  [ebp-44], esi
	jge	L77
align 16
L122:
	xor	ebx, ebx
	cmp	ebx, DWORD  [ebp-44]
	jge	L113
	mov	edx, DWORD  [ebp-16]
	mov	eax, DWORD  [ebp-44]
	mov	eax, DWORD  [edx+eax*4]
	mov	edx, DWORD  [ebp-44]
	mov	DWORD  [ebp-32], eax
	add	edx, edx
	mov	DWORD  [ebp-28], edx
align 16
L101:
	mov	esi, DWORD  [ebp-32]
	mov	edx, DWORD  [ebp-28]
	lea	eax, [esi+ebx*2]
	fld	DWORD  [ecx+eax*4]
	mov	esi, DWORD  [ebp-16]
	fld	DWORD  [ecx+4+eax*4]
	add	edx, DWORD  [esi+ebx*4]
	inc	ebx
	fld	DWORD  [ecx+edx*4]
	fld	DWORD  [ecx+4+edx*4]
	fxch	st1
	fstp	DWORD  [ecx+eax*4]
	fstp	DWORD  [ecx+4+eax*4]
	fxch	st1
	add	eax, edi
	fstp	DWORD  [ecx+edx*4]
	fstp	DWORD  [ecx+4+edx*4]
	add	edx, edi
	fld	DWORD  [ecx+eax*4]
	fld	DWORD  [ecx+4+eax*4]
	fld	DWORD  [ecx+edx*4]
	fld	DWORD  [ecx+4+edx*4]
	fxch	st1
	cmp	ebx, DWORD  [ebp-44]
	fstp	DWORD  [ecx+eax*4]
	fstp	DWORD  [ecx+4+eax*4]
	fxch	st1
	fstp	DWORD  [ecx+edx*4]
	fstp	DWORD  [ecx+4+edx*4]
	jl	L101
L113:
	inc	DWORD  [ebp-44]
	mov	eax, DWORD  [ebp-20]
	cmp	DWORD  [ebp-44], eax
	jl	L122
L77:
	add	esp, 32
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
L117:
	mov	DWORD  [ebp-44], 0
	mov	eax, DWORD  [ebp-20]
	cmp	DWORD  [ebp-44], eax
	jge	L77
	mov	DWORD  [ebp-40], edi
	sal	eax, 2
	mov	DWORD  [ebp-36], eax
	xor	ebx, ebx
	cmp	ebx, DWORD  [ebp-44]
	jge	L114
L123:
	mov	esi, DWORD  [ebp-16]
	mov	edx, DWORD  [ebp-44]
	mov	edx, DWORD  [esi+edx*4]
	mov	esi, DWORD  [ebp-44]
	mov	DWORD  [ebp-32], edx
	add	esi, esi
	mov	DWORD  [ebp-24], esi
align 16
L92:
	mov	edx, DWORD  [ebp-32]
	mov	esi, DWORD  [ebp-16]
	lea	eax, [edx+ebx*2]
	fld	DWORD  [ecx+eax*4]
	mov	edx, DWORD  [ebp-24]
	add	edx, DWORD  [esi+ebx*4]
	mov	esi, DWORD  [ebp-36]
	inc	ebx
	fld	DWORD  [ecx+4+eax*4]
	fld	DWORD  [ecx+edx*4]
	fld	DWORD  [ecx+4+edx*4]
	fxch	st1
	fstp	DWORD  [ecx+eax*4]
	fstp	DWORD  [ecx+4+eax*4]
	fxch	st1
	add	eax, edi
	fstp	DWORD  [ecx+edx*4]
	fstp	DWORD  [ecx+4+edx*4]
	add	edx, esi
	mov	esi, DWORD  [ebp-36]
	fld	DWORD  [ecx+eax*4]
	fld	DWORD  [ecx+4+eax*4]
	fld	DWORD  [ecx+edx*4]
	fld	DWORD  [ecx+4+edx*4]
	fxch	st1
	fstp	DWORD  [ecx+eax*4]
	fstp	DWORD  [ecx+4+eax*4]
	fxch	st1
	add	eax, edi
	fstp	DWORD  [ecx+edx*4]
	fstp	DWORD  [ecx+4+edx*4]
	sub	edx, edi
	fld	DWORD  [ecx+eax*4]
	fld	DWORD  [ecx+4+eax*4]
	fld	DWORD  [ecx+edx*4]
	fld	DWORD  [ecx+4+edx*4]
	fxch	st1
	fstp	DWORD  [ecx+eax*4]
	fstp	DWORD  [ecx+4+eax*4]
	fxch	st1
	add	eax, edi
	fstp	DWORD  [ecx+edx*4]
	fstp	DWORD  [ecx+4+edx*4]
	add	edx, esi
	fld	DWORD  [ecx+edx*4]
	fld	DWORD  [ecx+eax*4]
	fld	DWORD  [ecx+4+eax*4]
	fld	DWORD  [ecx+4+edx*4]
	fxch	st3
	fstp	DWORD  [ecx+eax*4]
	fxch	st2
	cmp	ebx, DWORD  [ebp-44]
	fstp	DWORD  [ecx+4+eax*4]
	fstp	DWORD  [ecx+edx*4]
	fstp	DWORD  [ecx+4+edx*4]
	jl	L92
L110:
	inc	DWORD  [ebp-44]
	mov	eax, DWORD  [ebp-40]
	mov	edx, DWORD  [ebp-32]
	add	DWORD  [ebp-40], 2
	add	eax, edx
	lea	edx, [eax+edi]
	fld	DWORD  [ecx+edx*4]
	fld	DWORD  [ecx+4+edx*4]
	fld	DWORD  [ecx+eax*4]
	fld	DWORD  [ecx+4+eax*4]
	fxch	st3
	fstp	DWORD  [ecx+eax*4]
	fxch	st1
	fstp	DWORD  [ecx+4+eax*4]
	fstp	DWORD  [ecx+edx*4]
	fstp	DWORD  [ecx+4+edx*4]
	mov	edx, DWORD  [ebp-20]
	cmp	DWORD  [ebp-44], edx
	jge	L77
	xor	ebx, ebx
	cmp	ebx, DWORD  [ebp-44]
	jl	L123
L114:
	mov	eax, DWORD  [ebp-44]
	mov	edx, DWORD  [ebp-16]
	mov	eax, DWORD  [edx+eax*4]
	mov	DWORD  [ebp-32], eax
	jmp	L110
align 2
align 16
_Z7cftfsubiPfS_:
	push	ebp
	mov	ebp, esp
	push	edi
	mov	edi, 2
	push	esi
	mov	esi, eax
	push	ebx
	sub	esp, 12
	cmp	eax, 8
	mov	DWORD  [ebp-16], ecx
	mov	ebx, edx
	jg	L147
L125:
	lea	eax, [0+edi*4]
	cmp	eax, esi
	je	L148
L152:
	xor	esi, esi
	jmp	L144
align 8
L149:
	fld	DWORD  [ebx+esi*4]
	lea	ecx, [esi+edi]
	fld	DWORD  [ebx+ecx*4]
	fld	st1
	fld	DWORD  [ebx+4+esi*4]
	fxch	st3
	fadd	st0, st2
	fxch	st1
	fsubrp	st2, st0
	fld	st2
	fsub	DWORD  [ebx+4+ecx*4]
	fxch	st1
	fstp	DWORD  [ebx+esi*4]
	fxch	st2
	fadd	DWORD  [ebx+4+ecx*4]
	fstp	DWORD  [ebx+4+esi*4]
	add	esi, 2
	fstp	DWORD  [ebx+ecx*4]
	fstp	DWORD  [ebx+4+ecx*4]
L144:
	cmp	esi, edi
	jl	L149
	add	esp, 12
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
L147:
	call	_Z6cft1stiPfS_
	cmp	esi, 32
	mov	edi, 8
	jle	L125
align 16
L128:
	mov	eax, DWORD  [ebp-16]
	mov	edx, edi
	mov	ecx, ebx
	sal	edi, 2
	mov	DWORD  [esp], eax
	mov	eax, esi
	call	_Z6cftmdliiPfS_
	lea	eax, [0+edi*4]
	cmp	eax, esi
	jge	L125
	mov	eax, DWORD  [ebp-16]
	mov	edx, edi
	mov	ecx, ebx
	sal	edi, 2
	mov	DWORD  [esp], eax
	mov	eax, esi
	call	_Z6cftmdliiPfS_
	lea	eax, [0+edi*4]
	cmp	eax, esi
	jl	L128
	lea	eax, [0+edi*4]
	cmp	eax, esi
	jne	L152
L148:
	xor	esi, esi
	jmp	L143
align 8
L150:
	fld	DWORD  [ebx+4+esi*4]
	lea	ecx, [esi+edi]
	lea	eax, [ecx+edi]
	fld	DWORD  [ebx+4+ecx*4]
	fld	st1
	lea	edx, [eax+edi]
	fld	DWORD  [ebx+esi*4]
	fxch	st1
	fadd	st0, st2
	fxch	st3
	fsubrp	st2, st0
	fld	DWORD  [ebx+ecx*4]
	fld	st1
	fld	DWORD  [ebx+eax*4]
	fxch	st1
	fadd	st0, st2
	fxch	st3
	fsubrp	st2, st0
	fsub	DWORD  [ebx+edx*4]
	fld	DWORD  [ebx+eax*4]
	fadd	DWORD  [ebx+edx*4]
	fld	DWORD  [ebx+4+eax*4]
	fld	DWORD  [ebx+4+edx*4]
	fxch	st3
	fstp	DWORD  [ebp-20]
	fld	st4
	fadd	st0, st2
	fxch	st3
	fadd	st0, st1
	fxch	st5
	fsubrp	st2, st0
	fsub	DWORD  [ebx+4+edx*4]
	fxch	st2
	fstp	DWORD  [ebx+esi*4]
	fld	st5
	fadd	st0, st4
	fxch	st6
	fsubrp	st4, st0
	fxch	st5
	fstp	DWORD  [ebx+4+esi*4]
	fld	st1
	fxch	st5
	add	esi, 2
	fstp	DWORD  [ebx+eax*4]
	fsub	st4, st0
	faddp	st1, st0
	fxch	st1
	fstp	DWORD  [ebx+4+eax*4]
	fxch	st2
	fstp	DWORD  [ebx+ecx*4]
	fld	DWORD  [ebp-20]
	fadd	st0, st1
	fxch	st1
	fsub	DWORD  [ebp-20]
	fxch	st1
	fstp	DWORD  [ebx+4+ecx*4]
	fxch	st1
	fstp	DWORD  [ebx+edx*4]
	fstp	DWORD  [ebx+4+edx*4]
L143:
	cmp	esi, edi
	jl	L150
	add	esp, 12
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
segment_data_aligned
align 16
align 8
LC11:
	DD 1413754136
	DD 1072243195
align 16
segment_code
align 2
align 16
def__Z6makewtiPiPf:
	push	ebp
	mov	ebp, esp
	push	edi
	push	esi
	push	ebx
	sub	esp, 60
	mov	edx, DWORD  [ebp+8]
	mov	eax, DWORD  [ebp+12]
	mov	edi, DWORD  [ebp+16]
	cmp	edx, 2
	mov	DWORD  [eax], edx
	mov	DWORD  [eax+4], 1
	jle	L153
	sar	edx, 1
	mov	eax, 0x3f800000
	mov	DWORD  [ebp-32], edx
	fild	DWORD  [ebp-32]
	fdivr	QWORD [LC11]
	mov	DWORD  [edi], eax
	mov	eax, 0x00000000
	mov	DWORD  [edi+4], eax
	fstp	DWORD  [ebp-36]
	fild	DWORD  [ebp-32]
	fmul	DWORD  [ebp-36]
	fstp	QWORD  [esp]
	call	cos
	fstp	DWORD  [ebp-28]
	mov	edx, DWORD  [ebp-32]
	mov	eax, DWORD  [ebp-28]
	cmp	edx, 2
	mov	DWORD  [edi+edx*4], eax
	mov	DWORD  [edi+4+edx*4], eax
	jle	L153
	mov	esi, 2
	cmp	esi, edx
	jmp	L162
align 8
L163:
	push	esi
	fild	DWORD  [esp]
	add	esp, 4
	fmul	DWORD  [ebp-36]
	fst	QWORD  [esp]
	fstp	DWORD  [ebp-56]
	call	cos
	fld	DWORD  [ebp-56]
	fxch	st1
	fstp	DWORD  [ebp-28]
	mov	ebx, DWORD  [ebp-28]
	fstp	QWORD  [esp]
	call	sin
	fstp	DWORD  [ebp-28]
	mov	eax, DWORD  [ebp+8]
	mov	edx, DWORD  [ebp-28]
	mov	DWORD  [edi+esi*4], ebx
	sub	eax, esi
	mov	DWORD  [edi+4+esi*4], edx
	add	esi, 2
	cmp	esi, DWORD  [ebp-32]
	mov	DWORD  [edi+eax*4], edx
	mov	DWORD  [edi+4+eax*4], ebx
L162:
	jl	L163
	mov	edx, DWORD  [ebp+12]
	mov	ecx, edi
	mov	eax, DWORD  [ebp+8]
	add	edx, 8
	call	_Z6bitrv2iPiPf
align 16
L153:
	add	esp, 60
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
segment_data_aligned
align 16
align 8
LC15:
	DD 1413754136
	DD 1072243195
align 4
LC16:
	DD 1056964608
align 8
LC17:
	DD 0
	DD 1071644672
align 16
segment_code
align 2
align 16
def__Z6makectiPiPf:
	push	ebp
	mov	ebp, esp
	push	edi
	push	esi
	push	ebx
	sub	esp, 60
	mov	edx, DWORD  [ebp+8]
	mov	eax, DWORD  [ebp+12]
	cmp	edx, 1
	mov	DWORD  [eax+4], edx
	jle	L164
	mov	edi, edx
	mov	esi, 1
	sar	edi, 1
	push	edi
	fild	DWORD  [esp]
	fdivr	QWORD [LC15]
	mov	DWORD  [esp], edi
	fstp	DWORD  [ebp-32]
	fild	DWORD  [esp]
	add	esp, 4
	fmul	DWORD  [ebp-32]
	fstp	QWORD  [esp]
	call	cos
	fstp	DWORD  [ebp-28]
	mov	eax, DWORD  [ebp+16]
	fld	DWORD  [ebp-28]
	fst	DWORD  [eax]
	fmul	DWORD [LC16]
	fstp	DWORD  [eax+edi*4]
	jmp	L171
align 8
L172:
	push	esi
	fild	DWORD  [esp]
	add	esp, 4
	fmul	DWORD  [ebp-32]
	fst	QWORD  [esp]
	fstp	DWORD  [ebp-56]
	call	cos
	fmul	QWORD [LC17]
	mov	edx, DWORD  [ebp+16]
	mov	ebx, DWORD  [ebp+8]
	fld	DWORD  [ebp-56]
	fxch	st1
	sub	ebx, esi
	fstp	DWORD  [edx+esi*4]
	inc	esi
	fstp	QWORD  [esp]
	call	sin
	fmul	QWORD [LC17]
	mov	eax, DWORD  [ebp+16]
	fstp	DWORD  [eax+ebx*4]
L171:
	cmp	esi, edi
	jl	L172
L164:
	add	esp, 60
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
segment_data_aligned
align 16
align 4
LC19:
	DD 1056964608
align 16
segment_code
align 2
align 16
def__Z4rdftiiPfPiS_:
	push	ebp
	mov	ebp, esp
	push	edi
	push	esi
	push	ebx
	sub	esp, 76
	mov	edx, DWORD  [ebp+20]
	mov	edi, DWORD  [ebp+24]
	mov	eax, DWORD  [ebp+12]
	mov	esi, DWORD  [ebp+8]
	mov	DWORD  [ebp-24], edi
	mov	edi, DWORD  [edx]
	mov	ebx, DWORD  [ebp+16]
	mov	DWORD  [ebp-16], eax
	lea	eax, [0+edi*4]
	cmp	eax, esi
	mov	DWORD  [ebp-20], edx
	jl	L221
L174:
	mov	edx, DWORD  [ebp-20]
	mov	edx, DWORD  [edx+4]
	mov	eax, edx
	mov	DWORD  [ebp-28], edx
	sal	eax, 2
	cmp	eax, esi
	jl	L222
	mov	eax, DWORD  [ebp-16]
	test	eax, eax
	js	L176
L232:
	cmp	esi, 4
	jg	L223
	je	L224
L183:
	fld	DWORD  [ebx]
	fld	DWORD  [ebx+4]
	fld	st1
	fsub	st0, st1
	fxch	st2
	faddp	st1, st0
	fxch	st1
	fstp	DWORD  [ebx+4]
	fstp	DWORD  [ebx]
L231:
	add	esp, 76
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
align 8
L222:
	mov	edx, DWORD  [ebp-24]
	mov	eax, esi
	sar	eax, 2
	mov	DWORD  [ebp-28], eax
	lea	eax, [edx+edi*4]
	mov	DWORD  [esp+8], eax
	mov	eax, DWORD  [ebp-20]
	mov	DWORD  [esp+4], eax
	mov	edx, DWORD  [ebp-28]
	mov	DWORD  [esp], edx
	call	def__Z6makectiPiPf
	mov	eax, DWORD  [ebp-16]
	test	eax, eax
	jns	L232
align 16
L176:
	fld	DWORD  [ebx]
	cmp	esi, 4
	fld	st0
	fsub	DWORD  [ebx+4]
	fmul	DWORD [LC19]
	fsub	st1, st0
	fxch	st1
	fstp	DWORD  [ebx]
	jle	L186
	fchs
	mov	edx, esi
	mov	eax, DWORD  [ebp-24]
	mov	DWORD  [ebp-76], 0
	sar	edx, 1
	mov	ecx, 2
	mov	DWORD  [ebp-48], edx
	mov	edx, DWORD  [ebp-28]
	lea	edi, [eax+edi*4]
	mov	DWORD  [ebp-44], edi
	fstp	DWORD  [ebx+4]
	add	edx, edx
	mov	eax, edx
	cdq
	idiv	DWORD  [ebp-48]
	cmp	ecx, DWORD  [ebp-48]
	mov	DWORD  [ebp-52], eax
	jge	L212
	lea	edx, [ebx-8+esi*4]
align 16
L190:
	fld	DWORD  [ebx+ecx*4]
	mov	edi, DWORD  [ebp-52]
	mov	eax, DWORD  [ebp-28]
	add	DWORD  [ebp-76], edi
	fld	st0
	fld	DWORD  [ebx+4+ecx*4]
	fld	DWORD [LC19]
	mov	edi, DWORD  [ebp-76]
	fld	st1
	fxch	st3
	fsub	DWORD  [edx]
	fxch	st3
	sub	eax, edi
	mov	edi, DWORD  [ebp-44]
	mov	DWORD  [ebp-68], eax
	fadd	DWORD  [edx+4]
	fxch	st1
	fsub	DWORD  [edi+eax*4]
	mov	eax, DWORD  [ebp-76]
	fld	st0
	fld	DWORD  [edi+eax*4]
	fxch	st1
	fmul	st0, st5
	fld	st1
	fmul	st0, st4
	fxch	st3
	fmulp	st4, st0
	fxch	st1
	fmulp	st5, st0
	faddp	st1, st0
	fxch	st1
	fsubrp	st3, st0
	fsub	st3, st0
	fxch	st1
	fsubr	st0, st2
	fxch	st3
	fstp	DWORD  [ebx+ecx*4]
	fxch	st2
	fstp	DWORD  [ebx+4+ecx*4]
	fxch	st1
	add	ecx, 2
	fadd	DWORD  [edx]
	fxch	st1
	fsub	DWORD  [edx+4]
	fxch	st1
	fstp	DWORD  [edx]
	fstp	DWORD  [edx+4]
	sub	edx, 8
	cmp	ecx, DWORD  [ebp-48]
	jl	L190
L212:
	mov	edx, DWORD  [ebp-48]
	mov	ecx, ebx
	mov	eax, esi
	mov	edi, 2
	xor	BYTE  [ebx+7+edx*4], -128
	mov	edx, DWORD  [ebp-20]
	add	edx, 8
	call	_Z6bitrv2iPiPf
	cmp	esi, 8
	jg	L226
L192:
	lea	eax, [0+edi*4]
	cmp	eax, esi
	je	L227
	xor	esi, esi
	jmp	L219
align 8
L228:
	fld	DWORD  [ebx+esi*4]
	lea	ecx, [esi+edi]
	fld	DWORD  [ebx+ecx*4]
	fld	st1
	fld	DWORD  [ebx+4+ecx*4]
	fxch	st1
	fsub	st0, st2
	fxch	st3
	faddp	st2, st0
	fxch	st2
	fstp	DWORD  [ebp-56]
	fld	DWORD  [ebx+4+esi*4]
	fxch	st1
	fstp	DWORD  [ebx+esi*4]
	fld	DWORD  [ebp-56]
	fxch	st2
	fsub	st0, st1
	fxch	st1
	fchs
	fsub	DWORD  [ebx+4+ecx*4]
	fstp	DWORD  [ebx+4+esi*4]
	fxch	st1
	add	esi, 2
	fstp	DWORD  [ebx+ecx*4]
	fstp	DWORD  [ebx+4+ecx*4]
L219:
	cmp	esi, edi
	jl	L228
	add	esp, 76
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
align 8
L221:
	mov	eax, DWORD  [ebp-24]
	mov	edi, esi
	sar	edi, 2
	mov	DWORD  [esp+4], edx
	mov	DWORD  [esp+8], eax
	mov	DWORD  [esp], edi
	call	def__Z6makewtiPiPf
	jmp	L174
align 8
L223:
	mov	edx, DWORD  [ebp-20]
	mov	ecx, ebx
	mov	eax, esi
	add	edx, 8
	call	_Z6bitrv2iPiPf
	mov	ecx, DWORD  [ebp-24]
	mov	edx, ebx
	mov	eax, esi
	call	_Z7cftfsubiPfS_
	mov	edx, esi
	mov	eax, DWORD  [ebp-24]
	mov	ecx, 2
	sar	edx, 1
	mov	DWORD  [ebp-36], edx
	mov	edx, DWORD  [ebp-28]
	lea	edi, [eax+edi*4]
	mov	DWORD  [ebp-32], edi
	xor	edi, edi
	add	edx, edx
	mov	eax, edx
	cdq
	idiv	DWORD  [ebp-36]
	cmp	ecx, DWORD  [ebp-36]
	mov	DWORD  [ebp-40], eax
	jge	L183
	lea	edx, [ebx-8+esi*4]
align 16
L181:
	fld	DWORD  [ebx+ecx*4]
	mov	eax, DWORD  [ebp-40]
	mov	esi, DWORD  [ebp-32]
	fld	DWORD  [ebx+4+ecx*4]
	add	edi, eax
	mov	eax, DWORD  [ebp-28]
	fld	DWORD [LC19]
	fld	st2
	fxch	st1
	sub	eax, edi
	fsub	DWORD  [esi+eax*4]
	fld	st2
	fld	DWORD  [esi+edi*4]
	fxch	st3
	fsub	DWORD  [edx]
	fld	st2
	fld	st4
	fxch	st3
	fadd	DWORD  [edx+4]
	fxch	st1
	fmul	st0, st2
	fxch	st5
	fmulp	st2, st0
	fmul	st2, st0
	fmulp	st3, st0
	fxch	st3
	fsubrp	st1, st0
	fxch	st1
	faddp	st2, st0
	fsub	st3, st0
	fxch	st2
	fsub	st0, st1
	fxch	st3
	fstp	DWORD  [ebx+ecx*4]
	fxch	st2
	fstp	DWORD  [ebx+4+ecx*4]
	add	ecx, 2
	fadd	DWORD  [edx]
	fxch	st1
	fsubr	DWORD  [edx+4]
	fxch	st1
	fstp	DWORD  [edx]
	fstp	DWORD  [edx+4]
	sub	edx, 8
	cmp	ecx, DWORD  [ebp-36]
	jl	L181
	jmp	L183
L186:
	fstp	DWORD  [ebx+4]
	jne	L231
	mov	ecx, DWORD  [ebp-24]
	add	esp, 76
	mov	edx, ebx
	pop	ebx
	mov	eax, 4
	pop	esi
	pop	edi
	pop	ebp
	jmp	_Z7cftfsubiPfS_
L224:
	mov	ecx, DWORD  [ebp-24]
	mov	edx, ebx
	mov	eax, 4
	call	_Z7cftfsubiPfS_
	jmp	L183
L226:
	mov	ecx, DWORD  [ebp-24]
	mov	edx, ebx
	mov	eax, esi
	mov	edi, 8
	call	_Z6cft1stiPfS_
	cmp	esi, 32
	jle	L192
L195:
	mov	eax, DWORD  [ebp-24]
	mov	edx, edi
	mov	ecx, ebx
	sal	edi, 2
	mov	DWORD  [esp], eax
	mov	eax, esi
	call	_Z6cftmdliiPfS_
	lea	eax, [0+edi*4]
	cmp	eax, esi
	jl	L195
	jmp	L192
L227:
	xor	esi, esi
	jmp	L218
align 8
L229:
	fld	DWORD  [ebx+esi*4]
	lea	ecx, [esi+edi]
	lea	eax, [ecx+edi]
	fld	DWORD  [ebx+ecx*4]
	fld	st1
	lea	edx, [eax+edi]
	mov	DWORD  [ebp-72], edx
	fadd	st0, st1
	fxch	st2
	fsubrp	st1, st0
	fxch	st1
	fstp	DWORD  [ebp-56]
	fld	DWORD  [ebx+4+esi*4]
	fst	DWORD  [ebp-64]
	mov	edx, DWORD  [ebp-64]
	fld	DWORD  [ebx+4+ecx*4]
	fld	DWORD  [ebx+eax*4]
	xor	edx, -2147483648
	mov	DWORD  [ebp-60], edx
	mov	edx, DWORD  [ebp-72]
	fld	st0
	fld	DWORD  [ebp-60]
	fld	DWORD  [ebx+4+eax*4]
	fxch	st1
	fsub	st0, st4
	fxch	st4
	fsubrp	st5, st0
	fld	DWORD  [ebx+4+edx*4]
	fld	st1
	fxch	st5
	fstp	DWORD  [ebp-60]
	fadd	st4, st0
	fsubp	st1, st0
	fld	DWORD  [ebx+edx*4]
	fld	DWORD  [ebp-56]
	fxch	st3
	fadd	st0, st1
	fxch	st4
	fsubrp	st1, st0
	fxch	st2
	fadd	st0, st3
	fxch	st3
	fsubr	DWORD  [ebp-56]
	fxch	st3
	fstp	DWORD  [ebx+esi*4]
	fld	DWORD  [ebp-60]
	fxch	st3
	fst	DWORD  [ebp-56]
	fxch	st3
	fsub	st0, st4
	fxch	st4
	fadd	DWORD  [ebp-60]
	fxch	st4
	fstp	DWORD  [ebx+4+esi*4]
	fld	st5
	fxch	st3
	add	esi, 2
	fstp	DWORD  [ebx+eax*4]
	fsub	st2, st0
	faddp	st5, st0
	fxch	st2
	fst	DWORD  [ebx+4+eax*4]
	fstp	DWORD  [ebp-60]
	fstp	DWORD  [ebx+ecx*4]
	fld	st1
	fsub	st0, st1
	fxch	st2
	faddp	st1, st0
	fxch	st1
	fstp	DWORD  [ebx+4+ecx*4]
	fxch	st1
	fstp	DWORD  [ebx+edx*4]
	fstp	DWORD  [ebx+4+edx*4]
L218:
	cmp	esi, edi
	jl	L229
	jmp	L231
segment_data_aligned
align 16
align 4
ZZ17RisaVFast_arctan2ffE7coeff_1:
	DD 1061752795
align 4
ZZ17RisaVFast_arctan2ffE7coeff_2:
	DD 1075235812
align 8
LC22:
	DD -640172613
	DD 1037794527
align 4
LC24:
	DD 1061752795
align 4
LC25:
	DD 1075235812
align 8
LC26:
	DD 1841940611
	DD 1070882608
align 8
LC27:
	DD 1413754136
	DD 1074340347
align 4
LC28:
	DD 1070141403
align 4
LC29:
	DD -1088070177
align 4
LC30:
	DD 1034104910
align 4
LC31:
	DD -1147733212
align 4
LC32:
	DD -1080170033
align 4
LC33:
	DD 1048697085
align 4
LC34:
	DD -1129877496
align 4
LC35:
	DD 1042479491
align 4
LC37:
	DD 1073741824
align 8
LC38:
	DD 0
	DD 1071644672
align 16
segment_code
align 2
align 16
def__ZN20tRisaPhaseVocoderDSP11ProcessCoreEi:
	push	ebp
	mov	ebp, esp
	push	edi
	push	esi
	push	ebx
	sub	esp, 92
	mov	eax, DWORD  [ebp+8]
	mov	DWORD  [ebp-60], eax
	mov	edx, DWORD  [eax+32]
	mov	eax, DWORD  [eax]
	shr	edx, 1
	mov	DWORD  [ebp-64], edx
	mov	edx, DWORD  [ebp+12]
	mov	esi, DWORD  [eax+edx*4]
	mov	edx, DWORD  [ebp-60]
	mov	eax, DWORD  [edx+4]
	mov	edx, DWORD  [ebp+12]
	mov	eax, DWORD  [eax+edx*4]
	mov	edx, DWORD  [ebp-60]
	mov	DWORD  [ebp-68], eax
	mov	eax, DWORD  [edx+20]
	mov	DWORD  [esp+16], eax
	mov	eax, DWORD  [edx+16]
	mov	DWORD  [esp+8], esi
	mov	DWORD  [esp+12], eax
	mov	eax, 1
	mov	DWORD  [esp+4], eax
	mov	eax, DWORD  [edx+32]
	mov	DWORD  [esp], eax
	call	def__Z4rdftiiPfPiS_
	mov	eax, DWORD  [ebp-60]
	fld1
	fld	DWORD  [eax+60]
	fucom	st1
	fnstsw	ax
	fstp	st1
	sahf
	jp	L339
	je	L234
L339:
	xor	ecx, ecx
	cmp	ecx, DWORD  [ebp-64]
	jae	L332
	fstp	st0
	fld	DWORD [LC24]
	mov	eax, DWORD  [ebp-60]
	fldz
	fld	DWORD [LC25]
	mov	edi, DWORD  [eax+8]
	fnstcw	WORD  [ebp-38]
	movzx	eax, WORD  [ebp-38]
	or	ax, 3072
	mov	WORD  [ebp-40], ax
	jmp	L249
align 8
L355:
	fld	st2
	fadd	st0, st1
	fxch	st1
	fsubrp	st3, st0
	fdivrp	st2, st0
	fxch	st1
	fmul	st0, st4
	fsubr	st0, st2
L241:
	fxch	st3
	fucom	st1
	fnstsw	ax
	fstp	st1
	sahf
	jbe	L244
	fxch	st2
	fchs
	fxch	st2
L244:
	mov	edx, DWORD  [ebp+12]
	mov	eax, DWORD  [edi+edx*4]
	xor	edx, edx
	fld	DWORD  [eax+ecx*4]
	fxch	st3
	fst	DWORD  [eax+ecx*4]
	mov	eax, DWORD  [ebp-60]
	fsubp	st3, st0
	push	edx
	push	ecx
	fild	QWORD  [esp]
	add	esp, 8
	fmul	DWORD  [eax+64]
	fsubp	st3, st0
	fld	st2
	fmul	QWORD [LC26]
	fldcw	WORD  [ebp-40]
	fistp	DWORD  [ebp-44]
	fldcw	WORD  [ebp-38]
	mov	edx, DWORD  [ebp-44]
	test	edx, edx
	js	L246
	mov	eax, edx
	and	eax, 1
	add	edx, eax
L247:
	push	edx
	mov	edx, DWORD  [ebp-60]
	mov	eax, DWORD  [ebp-60]
	fild	DWORD  [esp]
	fmul	QWORD [LC27]
	fsubp	st3, st0
	fxch	st2
	fstp	DWORD  [ebp-20]
	fld	DWORD  [ebp-20]
	fmul	DWORD  [edx+68]
	xor	edx, edx
	mov	DWORD  [esp], edx
	push	ecx
	fild	QWORD  [esp]
	add	esp, 8
	faddp	st1, st0
	fmul	DWORD  [eax+72]
	mov	DWORD  [esi+ecx*8], ebx
	fstp	DWORD  [esi+4+ecx*8]
	inc	ecx
	cmp	ecx, DWORD  [ebp-64]
	jae	L342
L249:
	fld	DWORD  [esi+ecx*8]
	fld	DWORD  [esi+4+ecx*8]
	fld	st1
	fld	st1
	fmul	st0, st2
	fxch	st3
	fucom	st5
	fnstsw	ax
	sahf
	fmul	st1, st0
	fxch	st1
	faddp	st3, st0
	fld	QWORD [LC22]
	fxch	st3
	fsqrt
	fstp	DWORD  [ebp-20]
	fld	st1
	mov	ebx, DWORD  [ebp-20]
	fabs
	faddp	st3, st0
	fxch	st2
	fstp	DWORD  [ebp-20]
	fld	DWORD  [ebp-20]
	jb	L355
	fld	st2
	fsub	st0, st1
	fxch	st3
	faddp	st1, st0
	fdivp	st2, st0
	fxch	st1
	fmul	st0, st4
	fsubr	st0, st4
	jmp	L241
align 8
L234:
	fstp	st0
	xor	ecx, ecx
	cmp	ecx, DWORD  [ebp-64]
	jae	L289
	fld	DWORD [LC24]
	mov	edx, DWORD  [ebp-60]
	fldz
	mov	edi, DWORD  [edx+8]
	mov	ebx, DWORD  [edx+12]
	fnstcw	WORD  [ebp-38]
	movzx	eax, WORD  [ebp-38]
	or	ax, 3072
	mov	WORD  [ebp-40], ax
	jmp	L330
align 8
L356:
	fld	st1
	fadd	st0, st1
	fxch	st1
	fsubrp	st2, st0
	fxch	st4
	fucom	st2
	fnstsw	ax
	fstp	st2
	sahf
	fdivp	st3, st0
	fxch	st2
	fmul	st0, st3
	fsubr	DWORD [LC25]
L352:
	jbe	L298
	fchs
L298:
	mov	edx, DWORD  [ebp+12]
	mov	eax, DWORD  [edi+edx*4]
	xor	edx, edx
	fld	DWORD  [eax+ecx*4]
	fxch	st1
	fst	DWORD  [eax+ecx*4]
	mov	eax, DWORD  [ebp-60]
	fsubp	st1, st0
	push	edx
	push	ecx
	fild	QWORD  [esp]
	add	esp, 8
	fmul	DWORD  [eax+64]
	fsub	st1, st0
	fld	st1
	fmul	QWORD [LC26]
	fldcw	WORD  [ebp-40]
	fistp	DWORD  [ebp-44]
	fldcw	WORD  [ebp-38]
	mov	edx, DWORD  [ebp-44]
	test	edx, edx
	js	L300
	mov	eax, edx
	and	eax, 1
	add	edx, eax
L301:
	push	edx
	mov	edx, DWORD  [ebp-60]
	fild	DWORD  [esp]
	add	esp, 4
	fmul	QWORD [LC27]
	fsubp	st2, st0
	fxch	st1
	fstp	DWORD  [ebp-20]
	fld	DWORD  [ebp-20]
	faddp	st1, st0
	fmul	DWORD  [edx+80]
	mov	edx, DWORD  [ebp+12]
	mov	eax, DWORD  [ebx+edx*4]
	fsubr	DWORD  [eax+ecx*4]
	fld	st0
	fmul	DWORD [LC35]
	fxch	st1
	fstp	DWORD  [eax+ecx*4]
	fadd	st0, st2
	fucom	st2
	fnstsw	ax
	sahf
	jbe	L304
	fld	QWORD [LC38]
	fadd	st0, st1
L341:
	fldcw	WORD  [ebp-40]
	fistp	DWORD  [ebp-44]
	fldcw	WORD  [ebp-38]
	mov	eax, DWORD  [ebp-44]
	push	eax
	mov	eax, DWORD  [ebp-68]
	fild	DWORD  [esp]
	add	esp, 4
	fsubrp	st1, st0
	fchs
	fld	st0
	fmul	st0, st1
	fadd	st0, st3
	fld	st0
	fmul	DWORD [LC31]
	fadd	DWORD [LC30]
	fmul	st0, st1
	fadd	DWORD [LC29]
	fmul	st0, st1
	fadd	DWORD [LC28]
	fmulp	st2, st0
	fld	st0
	fmul	DWORD [LC34]
	fxch	st2
	fadd	st0, st4
	fxch	st2
	fadd	DWORD [LC33]
	fmul	st0, st1
	fadd	DWORD [LC32]
	fmulp	st1, st0
	fld1
	faddp	st1, st0
	fld	st1
	fmul	st0, st2
	fld	st1
	fmul	st0, st2
	fxch	st3
	fmulp	st2, st0
	fxch	st2
	fadd	st0, st4
	fxch	st1
	fadd	st0, st4
	fxch	st2
	fsubrp	st1, st0
	fxch	st1
	fadd	st0, st0
	fxch	st1
	fchs
	fxch	st1
	fadd	st0, st3
	fld	st1
	fmul	st0, st2
	fld	st1
	fmul	st0, st2
	fld	st1
	fxch	st4
	fmulp	st3, st0
	fxch	st1
	fadd	st0, st5
	fxch	st3
	fsub	DWORD [LC37]
	fld	st4
	fxch	st3
	fadd	st0, st6
	fxch	st1
	fadd	st0, st2
	fxch	st2
	fsubrp	st4, st0
	fadd	st0, st0
	fxch	st1
	fchs
	fxch	st1
	fadd	st0, st5
	fxch	st3
	fmul	st0, st1
	fxch	st3
	fmulp	st1, st0
	fxch	st2
	fsubr	st0, st4
	fxch	st2
	fadd	st0, st4
	fxch	st1
	fmul	st0, st2
	fxch	st2
	fstp	DWORD  [ebp-36]
	fstp	DWORD  [ebp-32]
	fstp	DWORD  [eax+ecx*8]
	fmul	DWORD  [ebp-32]
	fstp	DWORD  [eax+4+ecx*8]
	inc	ecx
	cmp	ecx, DWORD  [ebp-64]
	jae	L348
L330:
	fld	DWORD  [esi+ecx*8]
	fld	DWORD  [esi+4+ecx*8]
	fld	st1
	fld	st1
	fmul	st0, st2
	fxch	st3
	fucom	st4
	fnstsw	ax
	sahf
	fmul	st1, st0
	fxch	st1
	faddp	st3, st0
	fld	QWORD [LC22]
	fxch	st3
	fsqrt
	fstp	DWORD  [ebp-20]
	fld	st1
	fabs
	fld	DWORD  [ebp-20]
	fxch	st1
	faddp	st4, st0
	fxch	st3
	fstp	DWORD  [ebp-20]
	fld	DWORD  [ebp-20]
	jb	L356
	fld	st1
	fsub	st0, st1
	fxch	st5
	fucom	st3
	fnstsw	ax
	fstp	st3
	faddp	st1, st0
	sahf
	fdivp	st3, st0
	fxch	st2
	fmul	st0, st3
	fsubr	st0, st3
	jmp	L352
align 8
L348:
	fstp	st0
L351:
	fstp	st0
L289:
	mov	edx, DWORD  [ebp-60]
	mov	eax, DWORD  [edx+20]
	mov	DWORD  [esp+16], eax
	mov	eax, DWORD  [edx+16]
	mov	DWORD  [esp+12], eax
	mov	eax, DWORD  [edx+4]
	mov	edx, DWORD  [ebp+12]
	mov	eax, DWORD  [eax+edx*4]
	mov	edx, DWORD  [ebp-60]
	mov	DWORD  [esp+8], eax
	mov	eax, -1
	mov	DWORD  [esp+4], eax
	mov	eax, DWORD  [edx+32]
	mov	DWORD  [esp], eax
	call	def__Z4rdftiiPfPiS_
	add	esp, 92
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
align 8
L342:
	fstp	st0
	fstp	st0
	fstp	st0
	fld	DWORD  [eax+60]
L332:
	xor	edi, edi
	fld1
	fdivrp	st1, st0
	cmp	edi, DWORD  [ebp-64]
	jae	L353
	fnstcw	WORD  [ebp-38]
	mov	ebx, DWORD  [ebp-68]
	fldz
	movzx	eax, WORD  [ebp-38]
	or	ax, 3072
	mov	WORD  [ebp-40], ax
	jmp	L257
align 8
L357:
	fstp	st0
	cmp	ecx, DWORD  [ebp-64]
	jae	L255
	mov	eax, DWORD  [esi+ecx*8]
	inc	edi
	mov	edx, DWORD  [ebp-60]
	mov	DWORD  [ebx], eax
	fld	DWORD  [edx+60]
	fmul	DWORD  [esi+4+ecx*8]
	fstp	DWORD  [ebx+4]
	add	ebx, 8
	cmp	edi, DWORD  [ebp-64]
	jae	L350
align 16
L257:
	xor	edx, edx
	push	edx
	xor	edx, edx
	push	edi
	fild	QWORD  [esp]
	add	esp, 8
	push	edx
	fmul	st0, st2
	fld	st0
	fldcw	WORD  [ebp-40]
	fistp	QWORD  [ebp-56]
	fldcw	WORD  [ebp-38]
	mov	eax, DWORD  [ebp-56]
	push	eax
	mov	ecx, eax
	lea	eax, [ecx+1]
	fild	QWORD  [esp]
	add	esp, 8
	cmp	eax, DWORD  [ebp-64]
	fsubp	st1, st0
	jae	L357
	fld	DWORD  [esi+ecx*8]
	mov	eax, DWORD  [ebp-60]
	fld	DWORD  [esi+8+ecx*8]
	fsub	st0, st1
	fmul	st0, st2
	faddp	st1, st0
	fstp	DWORD  [ebx]
	fld	DWORD  [esi+4+ecx*8]
	fld	DWORD  [esi+12+ecx*8]
	fsub	st0, st1
	fmulp	st2, st0
	faddp	st1, st0
	fmul	DWORD  [eax+60]
	fstp	DWORD  [ebx+4]
	inc	edi
L359:
	add	ebx, 8
	cmp	edi, DWORD  [ebp-64]
	jb	L257
L350:
	fstp	st0
L353:
	fstp	st0
	xor	ecx, ecx
	cmp	ecx, DWORD  [ebp-64]
	jae	L289
	mov	eax, DWORD  [ebp-60]
	fldz
	mov	edx, DWORD  [ebp+12]
	lea	edi, [ebp-24]
	mov	ebx, DWORD  [eax+12]
	lea	esi, [ebp-28]
	mov	ebx, DWORD  [ebx+edx*4]
	fnstcw	WORD  [ebp-38]
	movzx	eax, WORD  [ebp-38]
	or	ax, 3072
	mov	WORD  [ebp-40], ax
	jmp	L288
align 8
L358:
	fld	QWORD [LC38]
	fsubr	st0, st1
L340:
	fldcw	WORD  [ebp-40]
	fistp	DWORD  [ebp-44]
	fldcw	WORD  [ebp-38]
	mov	eax, DWORD  [ebp-44]
	push	eax
	mov	eax, DWORD  [ebp-68]
	fild	DWORD  [esp]
	add	esp, 4
	fsubrp	st1, st0
	fchs
	fld	st0
	fmul	st0, st1
	fadd	st0, st3
	fld	st0
	fmul	DWORD [LC31]
	fadd	DWORD [LC30]
	fmul	st0, st1
	fadd	DWORD [LC29]
	fmul	st0, st1
	fadd	DWORD [LC28]
	fmulp	st2, st0
	fld	st0
	fmul	DWORD [LC34]
	fxch	st2
	fadd	st0, st4
	fxch	st2
	fadd	DWORD [LC33]
	fmul	st0, st1
	fadd	DWORD [LC32]
	fmulp	st1, st0
	fld1
	faddp	st1, st0
	fld	st1
	fmul	st0, st2
	fld	st1
	fmul	st0, st2
	fxch	st3
	fmulp	st2, st0
	fxch	st2
	fadd	st0, st4
	fxch	st1
	fadd	st0, st4
	fxch	st2
	fsubrp	st1, st0
	fxch	st1
	fadd	st0, st0
	fxch	st1
	fchs
	fxch	st1
	fadd	st0, st3
	fld	st1
	fmul	st0, st2
	fxch	st2
	fmul	st0, st1
	fld	st1
	fmulp	st2, st0
	fld	st2
	fxch	st1
	fadd	st0, st5
	fxch	st1
	fsub	DWORD [LC37]
	fxch	st3
	fadd	st0, st5
	fxch	st1
	fadd	st0, st0
	fld	st4
	fxch	st4
	fadd	st0, st3
	fxch	st1
	fadd	st0, st6
	fxch	st3
	fsubrp	st2, st0
	fchs
	fmul	st1, st0
	fmulp	st2, st0
	fsubr	st0, st4
	fxch	st1
	fadd	st0, st4
	fxch	st1
	fstp	DWORD  [esi]
	fstp	DWORD  [edi]
	fmul	DWORD  [ebp-28]
	fstp	DWORD  [eax+ecx*8]
	fmul	DWORD  [ebp-24]
	fstp	DWORD  [eax+4+ecx*8]
	inc	ecx
	cmp	ecx, DWORD  [ebp-64]
	jae	L351
L288:
	mov	edx, DWORD  [ebp-60]
	mov	eax, DWORD  [ebp-68]
	fld	DWORD  [edx+76]
	xor	edx, edx
	fmul	DWORD  [eax+4+ecx*8]
	fld	DWORD  [eax+ecx*8]
	mov	eax, DWORD  [ebp-60]
	push	edx
	push	ecx
	fild	QWORD  [esp]
	add	esp, 8
	fld	DWORD  [eax+64]
	fxch	st3
	fsub	st0, st1
	fxch	st1
	fmul	st0, st3
	fxch	st1
	fmulp	st3, st0
	faddp	st2, st0
	fxch	st1
	fmul	DWORD  [eax+80]
	fsubr	DWORD  [ebx+ecx*4]
	fld	st0
	fmul	DWORD [LC35]
	fxch	st1
	fstp	DWORD  [ebx+ecx*4]
	fadd	st0, st2
	fucom	st2
	fnstsw	ax
	sahf
	jbe	L358
	fld	QWORD [LC38]
	fadd	st0, st1
	jmp	L340
align 8
L304:
	fld	QWORD [LC38]
	fsubr	st0, st1
	jmp	L341
align 8
L255:
	fst	DWORD  [ebx]
	fst	DWORD  [ebx+4]
	inc	edi
	jmp	L359
align 8
L300:
	and	edx, -2
	jmp	L301
align 8
L246:
	and	edx, -2
	jmp	L247
align 2
align 16
def__Z30RisaDeinterleaveApplyingWindowPPfPKfS_ijj:
	push	ebp
	mov	ebp, esp
	push	edi
	push	esi
	push	ebx
	sub	esp, 4
	mov	edi, DWORD  [ebp+20]
	mov	esi, DWORD  [ebp+12]
	cmp	edi, 1
	je	L362
	cmp	edi, 2
	je	L367
	xor	ecx, ecx
	cmp	ecx, DWORD  [ebp+28]
	jae	L397
align 16
L398:
	xor	edx, edx
	cmp	edx, edi
	jge	L387
	mov	eax, DWORD  [ebp+24]
	add	eax, ecx
	mov	DWORD  [ebp-16], eax
align 16
L379:
	mov	ebx, DWORD  [ebp+8]
	mov	eax, DWORD  [ebx+edx*4]
	mov	ebx, DWORD  [ebp+16]
	inc	edx
	fld	DWORD  [ebx+ecx*4]
	mov	ebx, DWORD  [ebp-16]
	fmul	DWORD  [esi]
	add	esi, 4
	cmp	edx, edi
	fstp	DWORD  [eax+ebx*4]
	jl	L379
L387:
	inc	ecx
	cmp	ecx, DWORD  [ebp+28]
	jb	L398
L397:
	pop	eax
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
L362:
	mov	edx, DWORD  [ebp+8]
	mov	ecx, DWORD  [ebp+24]
	mov	eax, DWORD  [edx]
	lea	eax, [eax+ecx*4]
	xor	ecx, ecx
	jmp	L388
L394:
	mov	ebx, DWORD  [ebp+16]
	fld	DWORD  [ebx+ecx*4]
	fmul	DWORD  [esi+ecx*4]
	fstp	DWORD  [eax+ecx*4]
	inc	ecx
L388:
	cmp	ecx, DWORD  [ebp+28]
	jb	L394
	pop	eax
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
L367:
	mov	ecx, DWORD  [ebp+8]
	mov	eax, DWORD  [ebp+24]
	mov	edx, DWORD  [ecx]
	mov	ebx, DWORD  [ecx+4]
	sal	eax, 2
	xor	ecx, ecx
	add	edx, eax
	add	eax, ebx
	jmp	L389
L395:
	mov	ebx, DWORD  [ebp+16]
	fld	DWORD  [ebx+ecx*4]
	fmul	DWORD  [esi+ecx*8]
	fstp	DWORD  [edx+ecx*4]
	fld	DWORD  [ebx+ecx*4]
	fmul	DWORD  [esi+4+ecx*8]
	fstp	DWORD  [eax+ecx*4]
	inc	ecx
L389:
	cmp	ecx, DWORD  [ebp+28]
	jb	L395
	pop	eax
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
align 2
align 16
def__Z31RisaInterleaveOverlappingWindowPfPKPKfS_ijj:
	push	ebp
	mov	ebp, esp
	push	edi
	push	esi
	push	ebx
	sub	esp, 4
	mov	edi, DWORD  [ebp+20]
	mov	ecx, DWORD  [ebp+8]
	cmp	edi, 1
	je	L401
	cmp	edi, 2
	je	L406
	xor	ebx, ebx
	cmp	ebx, DWORD  [ebp+28]
	jae	L436
align 16
L437:
	xor	edx, edx
	cmp	edx, edi
	jge	L426
	mov	eax, DWORD  [ebp+24]
	add	eax, ebx
	mov	DWORD  [ebp-16], eax
align 16
L418:
	mov	esi, DWORD  [ebp+12]
	mov	eax, DWORD  [esi+edx*4]
	mov	esi, DWORD  [ebp+16]
	inc	edx
	fld	DWORD  [esi+ebx*4]
	mov	esi, DWORD  [ebp-16]
	fmul	DWORD  [eax+esi*4]
	fadd	DWORD  [ecx]
	fstp	DWORD  [ecx]
	add	ecx, 4
	cmp	edx, edi
	jl	L418
L426:
	inc	ebx
	cmp	ebx, DWORD  [ebp+28]
	jb	L437
L436:
	pop	esi
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
L401:
	mov	edx, DWORD  [ebp+12]
	mov	ebx, DWORD  [ebp+24]
	mov	eax, DWORD  [edx]
	lea	eax, [eax+ebx*4]
	xor	ebx, ebx
	jmp	L427
L433:
	mov	esi, DWORD  [ebp+16]
	fld	DWORD  [esi+ebx*4]
	fmul	DWORD  [eax+ebx*4]
	fadd	DWORD  [ecx+ebx*4]
	fstp	DWORD  [ecx+ebx*4]
	inc	ebx
L427:
	cmp	ebx, DWORD  [ebp+28]
	jb	L433
	pop	esi
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
L406:
	mov	ebx, DWORD  [ebp+12]
	mov	eax, DWORD  [ebp+24]
	mov	edx, DWORD  [ebx]
	mov	edi, DWORD  [ebx+4]
	sal	eax, 2
	xor	ebx, ebx
	add	edx, eax
	add	eax, edi
	jmp	L428
L434:
	mov	esi, DWORD  [ebp+16]
	fld	DWORD  [esi+ebx*4]
	fmul	DWORD  [edx+ebx*4]
	fadd	DWORD  [ecx+ebx*8]
	fstp	DWORD  [ecx+ebx*8]
	fld	DWORD  [esi+ebx*4]
	fmul	DWORD  [eax+ebx*4]
	fadd	DWORD  [ecx+4+ebx*8]
	fstp	DWORD  [ecx+4+ebx*8]
	inc	ebx
L428:
	cmp	ebx, DWORD  [ebp+28]
	jb	L434
	pop	esi
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
align 16
