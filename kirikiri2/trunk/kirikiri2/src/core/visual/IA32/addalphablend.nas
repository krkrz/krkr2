; this is a part of TVP (KIRIKIRI) software source.
; see other sources for license.
; (C)2001-2003 W.Dee <dee@kikyou.info>
; …頼むからテキストエディタさん、少なくともUTF-7として誤認しないでくださいお願いします

; addive alpha blend

%ifndef			GEN_CODE

%include		"nasm.nah"

globaldef		TVPAdditiveAlphaBlend_mmx_a
globaldef		TVPAdditiveAlphaBlend_emmx_a

;--------------------------------------------------------------------
		segment_code
;--------------------------------------------------------------------
%define GEN_CODE
;;[function_replace_by TVPCPUType & TVP_CPU_HAS_MMX] TVPAdditiveAlphaBlend
;;void, TVPAdditiveAlphaBlend_mmx_a, (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len)
%define FUNCTION_LABEL TVPAdditiveAlphaBlend_mmx_a
	%include "addalphablend.nas"

;;[function_replace_by TVPCPUType & TVP_CPU_HAS_MMX && TVPCPUType & TVP_CPU_HAS_EMMX] TVPAdditiveAlphaBlend
;;void, TVPAdditiveAlphaBlend_emmx_a, (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len)
%define			USE_EMMX
%define FUNCTION_LABEL TVPAdditiveAlphaBlend_emmx_a
	%include "addalphablend.nas"



%else
;--------------------------------------------------------------------
		segment_code
;--------------------------------------------------------------------
FUNCTION_LABEL:					; additive alpha blend
		push		edi
		push		esi
		push		ebx
		push		ecx
		push		edx
		push		ebp
		mov			ecx,	[esp + 36]		; len
		cmp			ecx,	byte 0
		jle			near .pexit
		mov			edi,	[esp + 28]		; dest
		mov			ebp,	[esp + 32]		; src
		lea			esi,	[edi + ecx*4]	; limit
		sub			esi,	byte 4			; 1*4
		cmp			edi,	esi
		pxor		mm0,	mm0				; mm0 = 0
		jae			near .pfraction			; jump if edi >= esi

		jmp			near	.ploop

.ptransp:									; completely transparent
		add			ebp,	byte 8
		add			edi,	byte 8
		cmp			edi,	esi
		jae			near	.pfraction

		loop_align
.ploop:
;	 full transparent checking will make the routine faster in usual usage.
		mov			eax,	[ebp]			; 1 src
		mov			edx,	[ebp+4]			; 2 src
		mov			ebx,	eax

		or			ebx,	edx
		jz			near	.ptransp		; completely transparent

		shr			edx,	24				; 2 eax = opa
		movd		mm4,	eax				; 1 src
		movd		mm5,	edx				; 2
		shr			eax,	24				; 1 eax = opa
		movd		mm6,	[edi+4]			; 2 dest
		movd		mm2,	eax				; 1
		punpcklwd	mm5,	mm5				; 2
		movd		mm1,	[edi]			; 1 dest
		punpcklwd	mm5,	mm5				; 2
		punpcklwd	mm2,	mm2				; 1
		punpcklbw	mm6,	mm0				; 2 mm1 = 00ss00ss00ss00ss
		punpcklwd	mm2,	mm2				; 1
		movq		mm7,	mm6				; 2
		punpcklbw	mm1,	mm0				; 1 mm1 = 00ss00ss00ss00ss
		pmullw		mm6,	mm5				; 2
%ifdef	USE_EMMX
		prefetcht0	[ebp + 16]
%endif
		movq		mm3,	mm1				; 1
		psrlw		mm6,	8				; 2
		pmullw		mm1,	mm2				; 1
		psubw		mm7,	mm6				; 2
		psrlw		mm1,	8				; 1
		movd		mm5,	[ebp+4]			; 2 src
		psubw		mm3,	mm1				; 1
		packuswb	mm3,	mm7
		psllq		mm5,	32
		paddusb		mm3,	mm5
		add			edi,	byte 8
		paddusb		mm3,	mm4				; 1 add src
		add			ebp,	byte 8
		movq		[edi-8],	mm3			; 1 store

		cmp			edi,	esi

		jb			short .ploop

.pfraction:
		add			esi,	byte 4
		cmp			edi,	esi
		jae			.pexit					; jump if edi >= esi

.ploop2:	; fractions
		mov			eax,	[ebp]			; src
		movd		mm4,	eax				; src
		shr			eax,	24				; eax = opa
		movd		mm2,	eax
		movd		mm1,	[edi]			; dest
		punpcklwd	mm2,	mm2
		punpcklwd	mm2,	mm2
		punpcklbw	mm1,	mm0				; mm1 = 00ss00ss00ss00ss
		movq		mm3,	mm1
		pmullw		mm1,	mm2
		psrlw		mm1,	8
		psubw		mm3,	mm1
		packuswb	mm3,	mm0
		paddusb		mm3,	mm4				; add src
		movd		[edi],	mm3				; store

		add			ebp,	byte 4
		add			edi,	byte 4

		cmp			edi,	esi

		jb			short .ploop2			; jump if edi < esi

.pexit:
		pop			ebp
		pop			edx
		pop			ecx
		pop			ebx
		pop			esi
		pop			edi
		emms
		ret
;--------------------------------------------------------------------
%endif
