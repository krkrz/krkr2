; this is a part of TVP (KIRIKIRI) software source.
; see other sources for license.
; (C)2001-2003 W.Dee <dee@kikyou.info>
; …頼むからテキストエディタさん、少なくともUTF-7として誤認しないでくださいお願いします

; addive alpha blend

%ifndef			GEN_CODE

%define GEN_CODE

%include		"nasm.nah"

globaldef		TVPAdditiveAlphaBlend_mmx_a
globaldef		TVPAdditiveAlphaBlend_emmx_a
globaldef		TVPAdditiveAlphaBlend_o_mmx_a
globaldef		TVPAdditiveAlphaBlend_o_emmx_a
globaldef		TVPAdditiveAlphaBlend_HDA_mmx_a
globaldef		TVPAdditiveAlphaBlend_HDA_emmx_a
globaldef		TVPAdditiveAlphaBlend_a_mmx_a
globaldef		TVPAdditiveAlphaBlend_a_emmx_a



;--------------------------------------------------------------------
		start_const_aligned
;--------------------------------------------------------------------
mask0000ffffffffffff	dd	0xffffffff, 0x0000ffff
mask00ffffff00ffffff	dd	0x00ffffff, 0x00ffffff
;--------------------------------------------------------------------
		end_const_aligned
;--------------------------------------------------------------------





;--------------------------------------------------------------------
; MMX stuff
;--------------------------------------------------------------------
;;[function_replace_by TVPCPUType & TVP_CPU_HAS_MMX] TVPAdditiveAlphaBlend
;;void, TVPAdditiveAlphaBlend_mmx_a, (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len)
%define TVPAdditiveAlphaBlend_name TVPAdditiveAlphaBlend_mmx_a

;;[function_replace_by TVPCPUType & TVP_CPU_HAS_MMX] TVPAdditiveAlphaBlend_o
;;void, TVPAdditiveAlphaBlend_o_mmx_a, (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len, tjs_int opa)
%define TVPAdditiveAlphaBlend_o_name TVPAdditiveAlphaBlend_o_mmx_a

;;[function_replace_by TVPCPUType & TVP_CPU_HAS_MMX] TVPAdditiveAlphaBlend_HDA
;;void, TVPAdditiveAlphaBlend_HDA_mmx_a, (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len)
%define TVPAdditiveAlphaBlend_HDA_name TVPAdditiveAlphaBlend_HDA_mmx_a

;;[function_replace_by TVPCPUType & TVP_CPU_HAS_MMX] TVPAdditiveAlphaBlend_a
;;void, TVPAdditiveAlphaBlend_a_mmx_a, (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len)
%define TVPAdditiveAlphaBlend_a_name TVPAdditiveAlphaBlend_a_mmx_a
;--------------------------------------------------------------------
	%include "addalphablend.nas"
;--------------------------------------------------------------------




;--------------------------------------------------------------------
; EMMX stuff
;--------------------------------------------------------------------
%define			USE_EMMX
;--------------------------------------------------------------------
;;[function_replace_by TVPCPUType & TVP_CPU_HAS_MMX && TVPCPUType & TVP_CPU_HAS_EMMX] TVPAdditiveAlphaBlend
;;void, TVPAdditiveAlphaBlend_emmx_a, (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len)
%define TVPAdditiveAlphaBlend_name TVPAdditiveAlphaBlend_emmx_a

;;[function_replace_by TVPCPUType & TVP_CPU_HAS_MMX && TVPCPUType & TVP_CPU_HAS_EMMX] TVPAdditiveAlphaBlend_o
;;void, TVPAdditiveAlphaBlend_o_emmx_a, (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len, tjs_int opa)
%define TVPAdditiveAlphaBlend_o_name TVPAdditiveAlphaBlend_o_emmx_a

;;[function_replace_by TVPCPUType & TVP_CPU_HAS_MMX && TVPCPUType & TVP_CPU_HAS_EMMX] TVPAdditiveAlphaBlend_HDA
;;void, TVPAdditiveAlphaBlend_HDA_emmx_a, (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len)
%define TVPAdditiveAlphaBlend_HDA_name TVPAdditiveAlphaBlend_HDA_emmx_a

;;[function_replace_by TVPCPUType & TVP_CPU_HAS_MMX && TVPCPUType & TVP_CPU_HAS_EMMX] TVPAdditiveAlphaBlend_a
;;void, TVPAdditiveAlphaBlend_a_emmx_a, (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len)
%define TVPAdditiveAlphaBlend_a_name TVPAdditiveAlphaBlend_a_emmx_a
;--------------------------------------------------------------------
	%include "addalphablend.nas"
;--------------------------------------------------------------------



%else
;--------------------------------------------------------------------
		segment_code
;--------------------------------------------------------------------
		function_align
TVPAdditiveAlphaBlend_name:					; additive alpha blend
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
		punpcklbw	mm6,	mm0				; 2 mm1 = 00dd00dd00dd00dd
		punpcklwd	mm2,	mm2				; 1
		movq		mm7,	mm6				; 2
		punpcklbw	mm1,	mm0				; 1 mm1 = 00dd00dd00dd00dd
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
		punpcklbw	mm1,	mm0				; mm1 = 00dd00dd00dd00dd
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

; di = di - di*a*opa + si*opa
;              ~~~~~Df ~~~~~~ Sf
;           ~~~~~~~~Ds
;      ~~~~~~~~~~~~~Dq
;
;


		function_align
TVPAdditiveAlphaBlend_o_name:	; additive alpha blend with opacity
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
		mov			eax,	[esp + 40]		; eax = opa (op)
		mov			ebx,	eax
		shr			ebx,	7
		add			eax,	ebx				; adjust
		movd		mm7,	eax
		punpcklwd	mm7,	mm7
		punpcklwd	mm7,	mm7				; mm7 = op00op00op00op00
		lea			esi,	[edi + ecx*4]	; limit
		sub			esi,	byte 4			; 1*4
		cmp			edi,	esi
		pxor		mm0,	mm0				; mm0 = 0
		jae			near .pfraction			; jump if edi >= esi

		loop_align
.ploop:

		movd		mm4,	[ebp]			; 1 src                  (ss)
		movd		mm2,	[edi]			; 1 dest                 (dd)
		punpcklbw	mm4,	mm0				; 1 mm4 = 00ss00ss00ss00ss
		punpcklbw	mm2,	mm0				; 1 mm2 = 00dd00dd00dd00dd
		pmullw		mm4,	mm7				; 1 mm4 = 00Sf00Sf00Sf00Sf
		movq		mm3,	mm2				; 1
		psrlw		mm4,	8				; 1
		movd		mm6,	[ebp+4]			; 2 src                  (ss)
		punpcklbw	mm6,	mm0				; 2 mm6 = 00ss00ss00ss00ss
		movq		mm1,	mm4				; 1
		pmullw		mm6,	mm7				; 2 mm6 = 00Sf00Sf00Sf00Sf
%ifdef	USE_EMMX
		prefetcht0	[ebp + 16]
%endif
		psrlq		mm1,	48				; 1
		punpcklwd	mm1,	mm1				; 1
		punpcklwd	mm1,	mm1				; 1 mm1 = Df00Df00Df00Df00
		psrlw		mm6,	8				; 2
		pmullw		mm2,	mm1				; 1 mm2 = 00Ds00Ds00Ds00Ds
		psrlw		mm2,	8				; 1
		movq		mm1,	mm6				; 2
		psubw		mm3,	mm2				; 1 mm1 = 00Dq00Dq00Dq00Dq
		paddw		mm3,	mm4				; 1
		movd		mm2,	[edi+4]			; 2 dest                 (dd)
		packuswb	mm3,	mm0				; 1
		psrlq		mm1,	48				; 2
		movd		[edi],	mm3				; 1 store
		punpcklbw	mm2,	mm0				; 2 mm2 = 00dd00dd00dd00dd
		punpcklwd	mm1,	mm1				; 2
		movq		mm3,	mm2				; 2
		punpcklwd	mm1,	mm1				; 2 mm1 = Df00Df00Df00Df00
		pmullw		mm2,	mm1				; 2 mm2 = 00Ds00Ds00Ds00Ds
		add			edi,	byte 8
		psrlw		mm2,	8				; 2
		psubw		mm3,	mm2				; 2 mm1 = 00Dq00Dq00Dq00Dq
		add			ebp,	byte 8
		paddw		mm3,	mm6				; 2
		cmp			edi,	esi
		packuswb	mm3,	mm0				; 2
		movd		[edi+4-8],	mm3			; 2 store

		jb			.ploop

.pfraction:
		add			esi,	byte 4
		cmp			edi,	esi
		jae			.pexit					; jump if edi >= esi

.ploop2:	; fractions
		movd		mm4,	[ebp]			; src                  (ss)

		movd		mm2,	[edi]			; dest                 (dd)
		punpcklbw	mm4,	mm0				; mm4 = 00ss00ss00ss00ss
		punpcklbw	mm2,	mm0				; mm2 = 00dd00dd00dd00dd
		pmullw		mm4,	mm7				; mm4 = 00Sf00Sf00Sf00Sf
		movq		mm3,	mm2

		psrlw		mm4,	8

		movq		mm1,	mm4
		psrlq		mm1,	48
		punpcklwd	mm1,	mm1
		punpcklwd	mm1,	mm1				; mm1 = Df00Df00Df00Df00

		pmullw		mm2,	mm1				; mm2 = 00Ds00Ds00Ds00Ds
		psrlw		mm2,	8
		psubw		mm3,	mm2				; mm1 = 00Dq00Dq00Dq00Dq

		paddw		mm3,	mm4
		packuswb	mm3,	mm0

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
		function_align
TVPAdditiveAlphaBlend_HDA_name:		; additive alpha blend holding destination alpha
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

		loop_align
.ploop:
		mov			eax,	[ebp]			; 1 src
		mov			edx,	[ebp+4]			; 2 src

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
		punpcklbw	mm6,	mm0				; 2 mm1 = 00dd00dd00dd00dd
		punpcklwd	mm2,	mm2				; 1
		movq		mm7,	mm6				; 2
		punpcklbw	mm1,	mm0				; 1 mm1 = 00dd00dd00dd00dd
		pmullw		mm6,	mm5				; 2
%ifdef	USE_EMMX
		prefetcht0	[ebp + 16]
%endif
		movq		mm3,	mm1				; 1
		psrlw		mm6,	8				; 2
		pmullw		mm1,	mm2				; 1
		pand		mm6,	[mask0000ffffffffffff]	; 2
		psrlw		mm1,	8				; 1
		psubw		mm7,	mm6				; 2
		movd		mm5,	[ebp+4]			; 2 src
		pand		mm1,	[mask0000ffffffffffff]	; 1
		pand		mm5,	[mask00ffffff00ffffff]	; 2
		psubw		mm3,	mm1				; 1
		pand		mm4,	[mask00ffffff00ffffff]	; 1
		packuswb	mm3,	mm7
		psllq		mm5,	32
		paddusb		mm3,	mm5
		add			edi,	byte 8
		paddusb		mm3,	mm4				; 1 add src
		add			ebp,	byte 8
		movq		[edi-8],	mm3			; 1 store

		cmp			edi,	esi

		jb			.ploop

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
		pand		mm4,	[mask00ffffff00ffffff]
		punpcklwd	mm2,	mm2
		punpcklbw	mm1,	mm0				; mm1 = 00dd00dd00dd00dd
		movq		mm3,	mm1
		pmullw		mm1,	mm2
		psrlw		mm1,	8
		pand		mm1,	[mask0000ffffffffffff]
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

		function_align
TVPAdditiveAlphaBlend_a_name:					; additive alpha blend on additive alpha
		push		edi
		push		esi
		push		ebx
		push		ecx
		push		edx
		push		ebp
		mov			ecx,	[esp + 36]		; len
		cmp			ecx,	byte 0
		jle			near .pexit
		pxor		mm0,	mm0				; mm0 = 0
		mov			eax,	0xffffff
		movd		mm7,	eax				; mm7 = 0xffffff
		punpckldq	mm7,	mm7				; mm7 = 0x00ffffff00ffffff; for mask
		mov			edi,	[esp + 28]		; dest
		mov			ebp,	[esp + 32]		; src
		lea			esi,	[edi + ecx*4]	; limit
		sub			esi,	byte 4			; 1*4
		cmp			edi,	esi
		jae			near .pfraction			; jump if edi >= esi

		loop_align
.ploop:
		mov			eax,	[ebp]			; 1 src
		mov			edx,	eax				; 1
		shr			eax,	24				; 1 eax = source opa
		movd		mm2,	eax				; 1
		mov			ecx,	[edi]			; 1 dest
		punpcklwd	mm2,	mm2				; 1
		movd		mm1,	ecx				; 1 dest
		punpcklwd	mm2,	mm2				; 1
		pand		mm1,	mm7				; 1
		shr			ecx,	24				; 1 ecx = dest opa
		mov			ebx,	eax				; 1
		punpcklbw	mm1,	mm0				; 1 mm1 = 00dd00dd00dd00dd
		imul		ebx,	ecx				; 1
%ifdef	USE_EMMX
		prefetcht0	[ebp + 16]
%endif
		movq		mm3,	mm1				; 1
		shr			ebx,	8				; 1
		add			eax,	ecx				; 1
		pmullw		mm1,	mm2				; 1
		sub			eax,	ebx				; 1
		psrlw		mm1,	8				; 1
		mov			ecx,	eax				; 1
		psubw		mm3,	mm1				; 1
		shr			ecx,	8				; 1
		packuswb	mm3,	mm0				; 1
		sub			eax,	ecx				; 1
		shl			eax,	24				; 1
		and			edx,	0xffffff		; 1
		or			edx,	eax				; 1
		movd		mm4,	edx				; 1 src
		mov			eax,	[ebp+4]			; 2 src
		paddusb		mm3,	mm4				; 1 add src
		mov			edx,	eax				; 2
		movd		[edi],	mm3				; 1 store
		shr			eax,	24				; 2 eax = source opa
		movd		mm2,	eax				; 2
		mov			ecx,	[edi+4]			; 2 dest
		punpcklwd	mm2,	mm2				; 2
		movd		mm1,	ecx				; 2 dest
		punpcklwd	mm2,	mm2				; 2
		pand		mm1,	mm7				; 2
		shr			ecx,	24				; 2 ecx = dest opa
		mov			ebx,	eax				; 2
		punpcklbw	mm1,	mm0				; 2 mm1 = 00dd00dd00dd00dd
		imul		ebx,	ecx				; 2
		movq		mm3,	mm1				; 2
		shr			ebx,	8				; 2
		add			eax,	ecx				; 2
		pmullw		mm1,	mm2				; 2
		sub			eax,	ebx				; 2
		psrlw		mm1,	8				; 2
		mov			ecx,	eax				; 2
		psubw		mm3,	mm1				; 2
		shr			ecx,	8				; 2
		packuswb	mm3,	mm0				; 2
		sub			eax,	ecx				; 2
		shl			eax,	24				; 2
		and			edx,	0xffffff		; 2
		add			edi,	byte 8
		or			edx,	eax				; 2
		movd		mm4,	edx				; 2 src
		add			ebp,	byte 8
		paddusb		mm3,	mm4				; 2 add src
		cmp			edi,	esi
		movd		[edi+4-8],	mm3			; 2 store


		jb			near .ploop

.pfraction:
		add			esi,	byte 4
		cmp			edi,	esi
		jae			.pexit					; jump if edi >= esi

.ploop2:	; fractions
		mov			eax,	[ebp]			; src
		mov			edx,	eax
		shr			eax,	24				; eax = source opa
		movd		mm2,	eax
		mov			ecx,	[edi]			; dest
		punpcklwd	mm2,	mm2
		movd		mm1,	ecx				; dest
		punpcklwd	mm2,	mm2
		pand		mm1,	mm7
		shr			ecx,	24				; ecx = dest opa
		mov			ebx,	eax
		punpcklbw	mm1,	mm0				; mm1 = 00dd00dd00dd00dd
		imul		ebx,	ecx
		movq		mm3,	mm1
		shr			ebx,	8
		add			eax,	ecx
		pmullw		mm1,	mm2
		sub			eax,	ebx

		psrlw		mm1,	8
		mov			ecx,	eax
		psubw		mm3,	mm1
		shr			ecx,	8
		packuswb	mm3,	mm0
		sub			eax,	ecx

		shl			eax,	24
		and			edx,	0xffffff
		or			edx,	eax

		movd		mm4,	edx				; src
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
