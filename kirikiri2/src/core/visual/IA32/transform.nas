; -*- Mode: asm; tab-width: 12; -*-
; this is a part of TVP (KIRIKIRI) software source.
; see other sources for license.
; (C)2001-2003 W.Dee <dee@kikyou.info>

; stretching copy / linear interpolation / linear transformation copy

%include		"nasm.nah"

globaldef		TVPStretchCopy_mmx_a
globaldef		TVPStretchAlphaBlend_mmx_a
globaldef		TVPStretchAdditiveAlphaBlend_mmx_a
globaldef		TVPStretchConstAlphaBlend_mmx_a
globaldef		TVPFastLinearInterpV2_mmx_a
globaldef		TVPFastLinearInterpH2F_mmx_a
globaldef		TVPFastLinearInterpH2B_mmx_a
globaldef		TVPLinTransCopy_mmx_a
globaldef		TVPLinTransConstAlphaBlend_mmx_a
globaldef		TVPInterpStretchCopy_mmx_a

;--------------------------------------------------------------------
	segment_code
;--------------------------------------------------------------------

;;[function_replace_by TVPCPUType & TVP_CPU_HAS_MMX] TVPStretchCopy
;;void, TVPStretchCopy_mmx_a, (tjs_uint32 *dest, tjs_int len, const tjs_uint32 *src, tjs_int srcstart, tjs_int srcstep)

	function_align
TVPStretchCopy_mmx_a:					; stretch copy
	push	edi
	push	esi
	push	ebx
	push	ecx
	push	edx
	mov	ecx,	[esp + 28]		; len
	cmp	ecx,	byte 0
	jle	near	.pexit2
	mov	edi,	[esp + 24]		; dest
	mov	esi,	[esp + 32]		; src
	mov	ebx,	[esp + 36]		; srcstart (srcp)
	mov	edx,	[esp + 40]		; srcstep
	push	ebp
	lea	ebp,	[edi+ecx*4]		; limit
	sub	ebp,	byte 4*3
	cmp	edi,	ebp
	jae	near	.pfraction		; jump if edi >= ebp

	loop_align
.ploop:
	mov	eax,	ebx		; 0 0 tmp = srcp
	lea	ecx,	[ebx + edx]		; 0 1 tmp = srcp
	shr	eax,	16		; 0 0 tmp >>= 16
	shr	ecx,	16		; 0 1 tmp >>= 16
	movd	mm0,	[eax*4 + esi]		; 0 0 load to mm0
	movd	mm1,	[ecx*4 + esi]		; 0 1 load to mm1
	movd	[edi],	mm0		; 0 0 store to dest
	lea	ecx,	[ebx + edx*4]		; 1 1 tmp = srcp
	lea	eax,	[ebx + edx*2]		; 1 0 tmp = srcp
	sub	ecx,	edx		; 1 
	movd	[edi+4],	mm1		; 0 1 store to dest
	shr	eax,	16		; 1 0 tmp >>= 16
	shr	ecx,	16		; 1 1 tmp >>= 16
	movd	mm2,	[eax*4 + esi]		; 1 0 load to mm2
	movd	mm3,	[ecx*4 + esi]		; 1 1 load to mm3
	movd	[edi+8],	mm2		; 1 0 store to dest
	movd	[edi+12],	mm3		; 1 1 store to dest

	lea	ebx,	[ebx + edx*4]		; srcp += srcstep

	add	edi,	byte 16		; dest ++

	cmp	edi,	ebp
	jb	near .ploop		; jump if edi < ebp

.pfraction:
	add	ebp,	byte 4*3
	cmp	edi,	ebp
	jae	.pexit		; jump if edi >= ebp

.ploop2:
	mov	eax,	ebx		; tmp = srcp
	shr	eax,	16		; tmp >>= 16
	movd	mm0,	[eax*4 + esi]		; load to mm0
	movd	[edi],	mm0		; store to dest
	add	ebx,	edx		; srcp += srcstep
	add	edi,	byte 4		; dest ++

	cmp	edi,	ebp
	jb	.ploop2		; jump if edi < ebp

.pexit:
	pop	ebp
.pexit2:
	pop	edx
	pop	ecx
	pop	ebx
	pop	esi
	pop	edi
	emms
	ret


;--------------------------------------------------------------------

;;[function_replace_by TVPCPUType & TVP_CPU_HAS_MMX] TVPStretchConstAlphaBlend
;;void, TVPStretchConstAlphaBlend_mmx_a, (tjs_uint32 *dest, tjs_int len, const tjs_uint32 *src, tjs_int srcstart, tjs_int srcstep, tjs_int opa)

	function_align
TVPStretchConstAlphaBlend_mmx_a:					; stretch copy with constant-ratio alpha blending
	push	edi
	push	esi
	push	ebx
	push	ecx
	push	edx
	mov	ecx,	[esp + 28]		; len
	cmp	ecx,	byte 0
	jle	near	.pexit2
	pxor	mm0,	mm0		; mm0 = 0
	mov	edi,	[esp + 24]		; dest
	mov	esi,	[esp + 32]		; src
	mov	ebx,	[esp + 36]		; srcstart (srcp)
	mov	edx,	[esp + 40]		; srcstep
	movd	mm7,	[esp + 44]		; opa
	push	ebp
	punpcklwd	mm7,	mm7
	lea	ebp,	[edi+ecx*4]		; limit
	punpcklwd	mm7,	mm7
	sub	ebp,	byte 4*3
	cmp	edi,	ebp
	jae	near	.pfraction		; jump if edi >= ebp

	loop_align
.ploop:
	mov	eax,	ebx		; 0 0 tmp = srcp
	lea	ecx,	[ebx + edx]		; 0 1 tmp = srcp
	shr	eax,	16		; 0 0 tmp >>= 16
	shr	ecx,	16		; 0 1 tmp >>= 16

	movd	mm2,	[eax*4 + esi]		; 0 0 load to mm2
	movd	mm4,	[ecx*4 + esi]		; 0 1 load to mm4
	movd	mm1,	[edi]		; 0 0 load dst to mm1
	movd	mm3,	[edi+4]		; 0 1 load dst to mm3
	punpcklbw	mm2,	mm0		; 0 0 unpack
	lea	ecx,	[ebx + edx*4]		; 1 1 tmp = srcp
	punpcklbw	mm4,	mm0		; 0 1 unpack
	lea	eax,	[ebx + edx*2]		; 1 0 tmp = srcp
	punpcklbw	mm1,	mm0		; 0 0 unpack
	sub	ecx,	edx		; 1 
	punpcklbw	mm3,	mm0		; 0 1 unpack
	psubw	mm2,	mm1		; 0 0
	psubw	mm4,	mm3		; 0 1
	pmullw	mm2,	mm7		; 0 0
	shr	eax,	16		; 1 0 tmp >>= 16
	pmullw	mm4,	mm7		; 0 1
	shr	ecx,	16		; 1 1 tmp >>= 16
	psllw	mm1,	8		; 0 0
	movd	mm5,	[eax*4 + esi]		; 1 0 load to mm5
	psllw	mm3,	8		; 0 1
	movd	mm6,	[ecx*4 + esi]		; 1 1 load to mm6
	paddw	mm1,	mm2		; 0 0
	paddw	mm3,	mm4		; 0 1
	psrlw	mm1,	8		; 0 0
	movd	mm2,	[edi+8]		; 1 0 load dst to mm2
	psrlw	mm3,	8		; 0 1
	movd	mm4,	[edi+12]		; 1 1 load dst to mm4
	packuswb	mm1,	mm0		; 0 0
	packuswb	mm3,	mm0		; 0 1
	movd	[edi],	mm1		; 0 0 store to dest
	punpcklbw	mm5,	mm0		; 1 0 unpack
	movd	[edi+4],	mm3		; 0 1 store to dest
	punpcklbw	mm6,	mm0		; 1 1 unpack

	lea	ebx,	[ebx + edx*4]		; srcp += srcstep

	punpcklbw	mm2,	mm0		; 1 0 unpack
	punpcklbw	mm4,	mm0		; 1 1 unpack
	psubw	mm5,	mm2		; 1 0
	psubw	mm6,	mm4		; 1 1
	pmullw	mm5,	mm7		; 1 0
	add	edi,	byte 16		; dest ++
	pmullw	mm6,	mm7		; 1 1
	psllw	mm2,	8		; 1 0
	psllw	mm4,	8		; 1 1
	paddw	mm2,	mm5		; 1 0
	paddw	mm4,	mm6		; 1 1
	psrlw	mm2,	8		; 1 0
	psrlw	mm4,	8		; 1 1
	packuswb	mm2,	mm0		; 1 0
	movd	[edi+8-16],	mm2		; 1 0 store to dest
	packuswb	mm4,	mm0		; 1 1
	movd	[edi+12-16],	mm4		; 1 1 store to dest

	cmp	edi,	ebp
	jb	near .ploop		; jump if edi < ebp

.pfraction:
	add	ebp,	byte 4*3
	cmp	edi,	ebp
	jae	.pexit		; jump if edi >= ebp

.ploop2:
	mov	eax,	ebx		; tmp = srcp
	shr	eax,	16		; tmp >>= 16

	movd	mm2,	[eax*4 + esi]		; load src to mm2
	movd	mm1,	[edi]		; load dst to mm1
	punpcklbw	mm2,	mm0		; unpack
	punpcklbw	mm1,	mm0		; unpack
	psubw	mm2,	mm1
	pmullw	mm2,	mm7
	psllw	mm1,	8
	paddw	mm1,	mm2
	psrlw	mm1,	8
	packuswb	mm1,	mm0
	movd	[edi],	mm1		; store to dest

	add	ebx,	edx		; srcp += srcstep
	add	edi,	byte 4		; dest ++

	cmp	edi,	ebp
	jb	.ploop2		; jump if edi < ebp

.pexit:
	pop	ebp
.pexit2:
	pop	edx
	pop	ecx
	pop	ebx
	pop	esi
	pop	edi
	emms
	ret


;--------------------------------------------------------------------

;;[function_replace_by TVPCPUType & TVP_CPU_HAS_MMX] TVPStretchAlphaBlend
;;void, TVPStretchAlphaBlend_mmx_a, (tjs_uint32 *dest, tjs_int len, const tjs_uint32 *src, tjs_int srcstart, tjs_int srcstep)

	function_align
TVPStretchAlphaBlend_mmx_a:					; stretch copy with pixel alpha blending
	push	edi
	push	esi
	push	ebx
	push	ecx
	push	edx
	pxor	mm7,	mm7
	mov	ecx,	[esp + 28]		; len
	cmp	ecx,	byte 0
	jle	near	.pexit2
	mov	edi,	[esp + 24]		; dest
	mov	esi,	[esp + 32]		; src
	mov	ebx,	[esp + 36]		; srcstart (srcp)
	mov	edx,	[esp + 40]		; srcstep
	push	ebp
	lea	ebp,	[edi+ecx*4]		; limit
	sub	ebp,	byte 4*3
	cmp	edi,	ebp
	jae	near	.pfraction		; jump if edi >= ebp

	loop_align
.ploop:
	mov	eax,	ebx		; 0 0 tmp = srcp
	lea	ecx,	[ebx + edx]		; 0 1 tmp = srcp
	shr	eax,	16		; 0 0 tmp >>= 16
	shr	ecx,	16		; 0 1 tmp >>= 16

	movd	mm0,	[eax*4 + esi]		; 0 0
	movd	mm3,	[ecx*4 + esi]		; 0 1
	movq	mm2,	mm0		; 0 0
	movq	mm5,	mm3		; 0 1
	psrlq	mm2,	24		; 0 0
	psrlq	mm5,	24		; 0 1
	movq	mm1,	[edi]		; 0 0,1
	punpcklbw	mm0,	mm7		; 0 0
	movq	mm4,	mm1		; 0 1
	punpcklbw	mm3,	mm7		; 0 1
	psrlq	mm4,	32		; 0 1
	punpcklbw	mm1,	mm7		; 0 0
	punpcklbw	mm4,	mm7		; 0 1
	psubw	mm0,	mm1		; 0 0
	punpcklwd	mm2,	mm2		; 0 0
	lea	ecx,	[ebx + edx*4]		; 1 1 tmp = srcp
	punpcklwd	mm5,	mm5		; 0 1
	lea	eax,	[ebx + edx*2]		; 1 0 tmp = srcp
	punpcklwd	mm2,	mm2		; 0 0
	sub	ecx,	edx		; 1 
	punpcklwd	mm5,	mm5		; 0 1
	shr	eax,	16		; 1 0 tmp >>= 16
	psubw	mm3,	mm4		; 0 1
	pmullw	mm0,	mm2		; 0 0
	shr	ecx,	16		; 1 1 tmp >>= 16
	pmullw	mm3,	mm5		; 0 1
	movd	mm6,	[eax*4 + esi]		; 1 0
	psllw	mm1,	8
	psllw	mm4,	8
	paddw	mm0,	mm1		; 0 0
	paddw	mm3,	mm4		; 0 1
	psrlw	mm0,	8
	psrlw	mm3,	8
	packuswb	mm0,	mm3		; 0 0,1
	movq	[edi],	mm0		; 0 0,1
	movd	mm3,	[ecx*4 + esi]		; 1 1
	movq	mm2,	mm6		; 1 0
	movq	mm5,	mm3		; 1 1
	psrlq	mm2,	24		; 1 0
	psrlq	mm5,	24		; 1 1
	movq	mm1,	[edi+8]		; 1 0,1
	punpcklbw	mm6,	mm7		; 1 0
	movq	mm4,	mm1		; 1 1
	punpcklbw	mm3,	mm7		; 1 1
	psrlq	mm4,	32		; 1 1
	punpcklbw	mm1,	mm7		; 1 0
	punpcklbw	mm4,	mm7		; 1 1
	psubw	mm6,	mm1		; 1 0
	punpcklwd	mm2,	mm2		; 1 0
	psubw	mm3,	mm4		; 1 1
	punpcklwd	mm5,	mm5		; 1 1
	punpcklwd	mm2,	mm2		; 1 0
	punpcklwd	mm5,	mm5		; 1 1
	pmullw	mm6,	mm2		; 1 0
	lea	ebx,	[ebx + edx*4]		; srcp += srcstep
	pmullw	mm3,	mm5		; 1 1
	add	edi,	byte 16		; dest ++
	cmp	edi,	ebp
	psllw	mm1,	8
	psllw	mm4,	8
	paddw	mm6,	mm1		; 1 0
	paddw	mm3,	mm4		; 1 1
	psrlw	mm6,	8
	psrlw	mm3,	8
	packuswb	mm6,	mm3		; 1 0,1
	movq	[edi+8-16],	mm6		; 1 0,1

	jb	near .ploop		; jump if edi < ebp

.pfraction:
	add	ebp,	byte 4*3
	cmp	edi,	ebp
	jae	.pexit		; jump if edi >= ebp

.ploop2:
	mov	eax,	ebx		; tmp = srcp
	shr	eax,	16		; tmp >>= 16

	movd	mm3,	[eax*4 + esi]
	movq	mm5,	mm3
	psrlq	mm5,	24
	movd	mm4,	[edi]
	punpcklbw	mm3,	mm7
	punpcklbw	mm4,	mm7
	punpcklwd	mm5,	mm5
	punpcklwd	mm5,	mm5
	psubw	mm3,	mm4
	pmullw	mm3,	mm5
	psllw	mm4,	8
	paddw	mm3,	mm4
	psrlw	mm3,	8
	packuswb	mm3,	mm7
	movd	[edi],	mm3

	add	ebx,	edx		; srcp += srcstep
	add	edi,	byte 4		; dest ++

	cmp	edi,	ebp
	jb	.ploop2		; jump if edi < ebp

.pexit:
	pop	ebp
.pexit2:
	pop	edx
	pop	ecx
	pop	ebx
	pop	esi
	pop	edi
	emms
	ret


;--------------------------------------------------------------------

;;[function_replace_by TVPCPUType & TVP_CPU_HAS_MMX] TVPStretchAdditiveAlphaBlend
;;void, TVPStretchAdditiveAlphaBlend_mmx_a, (tjs_uint32 *dest, tjs_int len, const tjs_uint32 *src, tjs_int srcstart, tjs_int srcstep)

	function_align
TVPStretchAdditiveAlphaBlend_mmx_a:					; stretch copy with pixel additive alpha blending
	push	edi
	push	esi
	push	ebx
	push	ecx
	push	edx
	pxor	mm0,	mm0
	mov	ecx,	[esp + 28]		; len
	cmp	ecx,	byte 0
	jle	near	.pexit2
	mov	edi,	[esp + 24]		; dest
	mov	esi,	[esp + 32]		; src
	mov	ebx,	[esp + 36]		; srcstart (srcp)
	mov	edx,	[esp + 40]		; srcstep
	push	ebp
	lea	ebp,	[edi+ecx*4]		; limit
	sub	ebp,	byte 4*3
	cmp	edi,	ebp
	jae	near	.pfraction		; jump if edi >= ebp

	loop_align
.ploop:
	mov	eax,	ebx		; 0 0 tmp = srcp
	lea	ecx,	[ebx + edx]		; 0 1 tmp = srcp
	shr	eax,	16		; 0 0 tmp >>= 16
	shr	ecx,	16		; 0 1 tmp >>= 16

	movd	mm4,	[eax*4 + esi]		; 1 src
	movd	mm5,	[ecx*4 + esi]		; 2 src
	movq	mm2,	mm4		; 1
	movq	mm6,	mm5		; 2
	psrlq	mm2,	24		; 1
	movd	mm1,	[edi]		; 1 dest
	punpcklwd	mm2,	mm2		; 1
	psrlq	mm6,	24		; 2
	punpcklwd	mm2,	mm2		; 1
	punpcklbw	mm1,	mm0		; 1 mm1 = 00dd00dd00dd00dd
	movd	mm7,	[edi+4]		; 2 dest
	movq	mm3,	mm1		; 1
	punpcklwd	mm6,	mm6		; 2
	pmullw	mm1,	mm2		; 1
	punpcklwd	mm6,	mm6		; 2
	psrlw	mm1,	8		; 1
	punpcklbw	mm7,	mm0		; 2 mm7 = 00dd00dd00dd00dd
	psubw	mm3,	mm1		; 1
	movq	mm2,	mm7		; 2
	packuswb	mm3,	mm0		; 1
	pmullw	mm7,	mm6		; 2
	paddusb	mm3,	mm4		; 1 add src
	psrlw	mm7,	8		; 2
	movd	[edi],	mm3		; 1 store
	psubw	mm2,	mm7		; 2
	lea	eax,	[ebx + edx*2]		; tmp = srcp
	packuswb	mm2,	mm0		; 2
	lea	ecx,	[ebx + edx*4]		; tmp = srcp
	paddusb	mm2,	mm5		; 2 add src
	sub	ecx,	edx		;
	movd	[edi+4],	mm2		; 2 store
	shr	eax,	16		; tmp >>= 16
	shr	ecx,	16		; tmp >>= 16
	movd	mm4,	[eax*4 + esi]		; 1 src
	movd	mm5,	[ecx*4 + esi]		; 2 src
	movq	mm2,	mm4		; 1
	movq	mm6,	mm5		; 2
	psrlq	mm2,	24		; 1
	movd	mm1,	[edi+8]		; 1 dest
	punpcklwd	mm2,	mm2		; 1
	psrlq	mm6,	24		; 2
	punpcklwd	mm2,	mm2		; 1
	punpcklbw	mm1,	mm0		; 1 mm1 = 00dd00dd00dd00dd
	movd	mm7,	[edi+12]		; 2 dest
	movq	mm3,	mm1		; 1
	punpcklwd	mm6,	mm6		; 2
	pmullw	mm1,	mm2		; 1
	punpcklwd	mm6,	mm6		; 2
	psrlw	mm1,	8		; 1
	punpcklbw	mm7,	mm0		; 2 mm7 = 00dd00dd00dd00dd
	psubw	mm3,	mm1		; 1
	movq	mm2,	mm7		; 2
	packuswb	mm3,	mm0		; 1
	pmullw	mm7,	mm6		; 2
	paddusb	mm3,	mm4		; 1 add src
	psrlw	mm7,	8		; 2
	movd	[edi+8],	mm3		; 1 store
	psubw	mm2,	mm7		; 2
	add	edi,	byte 16		; dest ++
	packuswb	mm2,	mm0		; 2
	lea	ebx,	[ebx + edx*4]		; srcp += srcstep
	paddusb	mm2,	mm5		; 2 add src
	cmp	edi,	ebp
	movd	[edi+12-16],	mm2		; 2 store

	jb	near .ploop		; jump if edi < ebp


.pfraction:
	add	ebp,	byte 4*3
	cmp	edi,	ebp
	jae	.pexit		; jump if edi >= ebp

.ploop2:
	mov	eax,	ebx		; tmp = srcp
	shr	eax,	16		; tmp >>= 16

	movd	mm4,	[eax*4 + esi]		; src
	movq	mm2,	mm4
	psrlq	mm2,	24
	movd	mm1,	[edi]		; dest
	punpcklwd	mm2,	mm2
	punpcklwd	mm2,	mm2
	punpcklbw	mm1,	mm0		; mm1 = 00dd00dd00dd00dd
	movq	mm3,	mm1
	pmullw	mm1,	mm2
	psrlw	mm1,	8
	psubw	mm3,	mm1
	packuswb	mm3,	mm0
	paddusb	mm3,	mm4		; add src
	movd	[edi],	mm3		; store

	add	ebx,	edx		; srcp += srcstep
	add	edi,	byte 4		; dest ++

	cmp	edi,	ebp
	jb	.ploop2		; jump if edi < ebp

.pexit:
	pop	ebp
.pexit2:
	pop	edx
	pop	ecx
	pop	ebx
	pop	esi
	pop	edi
	emms
	ret


;--------------------------------------------------------------------

;;[function_replace_by TVPCPUType & TVP_CPU_HAS_MMX] TVPFastLinearInterpV2
;;void, TVPFastLinearInterpV2_mmx_a, (tjs_uint32 *dest, tjs_int len, const tjs_uint32 *src0, const tjs_uint32 *src1)

	function_align
TVPFastLinearInterpV2_mmx_a:					; interpolation between two sources (2x)
	push	edi
	push	esi
	push	ebx
	push	ecx
	push	edx
	mov	ecx,	[esp + 28]		; len
	cmp	ecx,	byte 0
	jle	near	.pexit2
	mov	edi,	[esp + 24]		; dest
	mov	esi,	[esp + 32]		; src0
	mov	edx,	[esp + 36]		; src1
	push	ebp
	pxor	mm0,	mm0
	lea	ebp,	[ecx*4]		; limit
	xor	ecx,	ecx
	sub	ebp,	byte 4*3
	cmp	ecx,	ebp
	jge	near	.pfraction		; jump if ecx >= ebp

	loop_align
.ploop:
	movq	mm1,	[esi + ecx]		; 1 src0
	movq	mm2,	[edx + ecx]		; 1 src1
	movq	mm3,	mm1		; 1
	movq	mm4,	mm2		; 1
	punpckhbw	mm3,	mm0		; 1 unpack
	movq	mm5,	[esi + ecx + 8]		; 2 src0
	punpckhbw	mm4,	mm0		; 1 unpack
	movq	mm6,	[edx + ecx + 8]		; 2 src1
	punpcklbw	mm1,	mm0		; 1 unpack
	movq	mm7,	mm5		; 2
	punpcklbw	mm2,	mm0		; 1 unpack
	paddw	mm3,	mm4		; 1 mm3 += mm4
	paddw	mm1,	mm2		; 1 mm1 += mm2
	psrlw	mm3,	1		; 1 mm3 >>= 1
	psrlw	mm1,	1		; 1 mm1 >>= 1
	packuswb	mm1,	mm3		; 1 pack
	movq	mm4,	mm6		; 2
	punpcklbw	mm5,	mm0		; 2 unpack
	movq	[edi + ecx],	mm1		; 1 store
	punpcklbw	mm6,	mm0		; 2 unpack
	paddw	mm5,	mm6		; 2 mm5 += mm6
	psrlw	mm5,	1		; 2 mm5 >>= 1
	punpckhbw	mm7,	mm0		; 2 unpack
	punpckhbw	mm4,	mm0		; 2 unpack
	paddw	mm7,	mm4		; 2 mm7 += mm4
	add	ecx,	byte 16
	psrlw	mm7,	1		; 2 mm7 >>= 1
	cmp	ecx,	ebp
	packuswb	mm5,	mm7		; 2 pack
	movq	[edi + ecx + 8 - 16],	mm5		; 2 store

	jl	near .ploop		; jump if ecx < ebp

.pfraction:
	add	ebp,	byte 4*3
	cmp	ecx,	ebp
	jge	.pexit		; jump if ecx >= ebp

.ploop2:
	movd	mm1,	[esi + ecx]		; src0
	movd	mm2,	[edx + ecx]		; src1
	punpcklbw	mm1,	mm0		; unpack
	punpcklbw	mm2,	mm0		; unpack
	paddw	mm1,	mm2		; mm1 += mm2
	psrlw	mm1,	1		; mm1 >>= 1
	packuswb	mm1,	mm0		; pack
	movd	[edi + ecx],	mm1		; store
	add	ecx,	byte 4

	cmp	ecx,	ebp
	jl	.ploop2		; jump if ecx < ebp

.pexit:
	pop	ebp
.pexit2:
	pop	edx
	pop	ecx
	pop	ebx
	pop	esi
	pop	edi
	emms
	ret


;--------------------------------------------------------------------

;;[function_replace_by TVPCPUType & TVP_CPU_HAS_MMX] TVPFastLinearInterpH2F
;;void, TVPFastLinearInterpH2F_mmx_a, (tjs_uint32 *dest, tjs_int destlen, const tjs_uint32 *src)

	function_align
TVPFastLinearInterpH2F_mmx_a:					; horizontal 2x linear interpolation forward
	push	edi
	push	esi
	push	ebx
	push	ecx
	push	edx
	mov	ecx,	[esp + 28]		; len
	cmp	ecx,	byte 0
	jle	near	.pexit2
	mov	edi,	[esp + 24]		; dest
	mov	ebx,	[esp + 32]		; src
	push	ebp
	pxor	mm0,	mm0
	lea	ebp,	[ecx*4]		; limit
	xor	ecx,	ecx
	sub	ebp,	byte 4*4
	cmp	ecx,	ebp
	jge	near	.pfraction		; jump if ecx >= ebp

	loop_align
.ploop:
	movq	mm1,	[ebx]		; src[0, 1]
	movd	mm2,	[ebx + 8]		; src[2]
	movd	[edi + ecx], mm1		; dest[0]
	movq	mm3,	mm1		; 
	movq	mm4,	mm1		; 
	punpcklbw	mm3,	mm0		; unpack
	psrlq	mm1,	32		; 
	punpckhbw	mm4,	mm0		; unpack
	paddw	mm3,	mm4		; +
	movd	[edi + ecx + 8],	mm1		; dest[2]
	psrlw	mm3,	1		; /= 2
	add	ecx,	byte 16
	punpcklbw	mm2,	mm0		; unpack
	packuswb	mm3,	mm0		; pack
	paddw	mm4,	mm2		; +
	movd	[edi + ecx + 4 - 16],	mm3		; dest[1]
	psrlw	mm4,	1		; /= 2
	add	ebx,	byte 8
	packuswb	mm4,	mm0		; pack
	cmp	ecx,	ebp
	movd	[edi + ecx + 12 - 16],	mm4		; dest[3]

	jl	near .ploop		; jump if ecx < ebp

.pfraction:
	add	ebp,	byte 4*4
	sub	ebp,	byte 4*2
	cmp	ecx,	ebp
	jge	.pfraction2		; jump if ecx >= ebp

.ploop2:
	movq	mm1,	[ebx]		; src[0, 1]
	movd	[edi + ecx], mm1		; dest[0]
	movq	mm2,	mm1
	punpcklbw	mm1,	mm0		; src[0] unpack
	punpckhbw	mm2,	mm0		; src[1] unpack
	paddw	mm1,	mm2		; mm1 += mm2
	psrlw	mm1,	1		; mm1 >>= 1
	packuswb	mm1,	mm0		; pack
	movd	[edi + ecx + 4], mm1		; store

	add	ebx,	byte 4
	add	ecx,	byte 8

	cmp	ecx,	ebp
	jl	near .ploop2		; jump if ecx < ebp

.pfraction2:
	add	ebp,	byte 4*2
	cmp	ecx,	ebp
	jge	.pexit		; jump if ecx >= ebp

.ploop3:
	movd	mm1,	[ebx]		; load
	movd	[edi + ecx],	mm1		; store
	add	ecx,	byte 4
	cmp	ecx,	ebp
	jl	.ploop3		; jump if ecx < ebp

.pexit:
	pop	ebp
.pexit2:
	pop	edx
	pop	ecx
	pop	ebx
	pop	esi
	pop	edi
	emms
	ret


;--------------------------------------------------------------------

;;[function_replace_by TVPCPUType & TVP_CPU_HAS_MMX] TVPFastLinearInterpH2B
;;void, TVPFastLinearInterpH2B_mmx_a, (tjs_uint32 *dest, tjs_int destlen, const tjs_uint32 *src)

	function_align
TVPFastLinearInterpH2B_mmx_a:					; horizontal 2x linear interpolation backward
	push	edi
	push	esi
	push	ebx
	push	ecx
	push	edx
	mov	ecx,	[esp + 28]		; len
	cmp	ecx,	byte 0
	jle	near	.pexit2
	mov	edi,	[esp + 24]		; dest
	mov	ebx,	[esp + 32]		; src
	push	ebp
	pxor	mm0,	mm0
	lea	ebp,	[ecx*4]		; limit
	xor	ecx,	ecx
	sub	ebp,	4*4
	cmp	ecx,	ebp
	jge	near	.pfraction		; jump if ecx >= ebp

	loop_align
.ploop:
	movq	mm1,	[ebx - 4]		; src[0, 1]
	movd	mm2,	[ebx - 8]		; src[2]
	movq	mm5,	mm1                     	; 
	punpcklbw	mm2,	mm0		; unpack
	movq	mm3,	mm1		; 
	psrlq	mm5,	32		; 
	movd	[edi + ecx + 8], mm1		; dest[2]
	movd	[edi + ecx], mm5		; dest[0]
	movq	mm4,	mm1		; 
	punpcklbw	mm3,	mm0		; unpack
	add	ecx,	byte 16
	punpckhbw	mm4,	mm0		; unpack
	paddw	mm4,	mm3		; +
	psrlw	mm4,	1		; /= 2
	paddw	mm3,	mm2		; +
	packuswb	mm4,	mm0		; pack
	movd	[edi + ecx + 4 - 16],	mm4		; dest[1]
	psrlw	mm3,	1		; /= 2
	sub	ebx,	byte 8
	packuswb	mm3,	mm0		; pack
	cmp	ecx,	ebp
	movd	[edi + ecx + 12 - 16],	mm3		; dest[3]

	jl	near .ploop		; jump if ecx < ebp

.pfraction:
	add	ebp,	byte 4*4
	sub	ebp,	byte 4*2
	cmp	ecx,	ebp
	jge	.pfraction2		; jump if ecx >= ebp

.ploop2:
	movq	mm1,	[ebx - 4]		; src[-1, 0]
	movq	mm3,	mm1
	psrlq	mm3,	32
	movd	[edi + ecx],	mm3		; dest[0]
	movq	mm2,	mm1
	punpckhbw	mm1,	mm0		; src[0] unpack
	punpcklbw	mm2,	mm0		; src[-1] unpack
	paddw	mm1,	mm2		; mm1 += mm2
	psrlw	mm1,	1		; mm1 >>= 1
	packuswb	mm1,	mm0		; pack
	movd	[edi + ecx + 4], mm1		; store

	sub	ebx,	byte 4
	add	ecx,	byte 8

	cmp	ecx,	ebp
	jl	near .ploop2		; jump if ecx < ebp

.pfraction2:
	add	ebp,	byte 4*2
	cmp	ecx,	ebp
	jge	.pexit		; jump if ecx >= ebp

.ploop3:
	movd	mm1,	[ebx]		; load
	movd	[edi + ecx],	mm1		; store
	add	ecx,	byte 4
	cmp	ecx,	ebp
	jl	.ploop3		; jump if ecx < ebp

.pexit:
	pop	ebp
.pexit2:
	pop	edx
	pop	ecx
	pop	ebx
	pop	esi
	pop	edi
	emms
	ret

;--------------------------------------------------------------------

;;[function_replace_by TVPCPUType & TVP_CPU_HAS_MMX] TVPLinTransCopy
;;void, TVPLinTransCopy_mmx_a, (tjs_uint32 *dest, tjs_int len, const tjs_uint32 *src,  tjs_int sx, tjs_int sy, tjs_int stepx, tjs_int stepy, tjs_int srcpitch)

	function_align
TVPLinTransCopy_mmx_a:								; linear transforming copy
	push	edi
	push	esi
	push	ebx
	push	ecx
	push	edx
	mov	ecx,	[esp + 28]		; len
	cmp	ecx,	byte 0
	jle	near	.pexit2
	mov	edi,	[esp + 24]		; dest
	mov	esi,	[esp + 32]		; src
	movd	mm0,	[esp + 36]		; sx
	movd	mm1,	[esp + 40]		; sy
	movd	mm2,	[esp + 44]		; stepx
	movd	mm3,	[esp + 48]		; stepy
	psllq	mm1,	32		; mm1 <<= 32
	psllq	mm3,	32		; mm3 <<= 32
	por	mm0,	mm1		; mm0 = (sy << 32) + sx
	por	mm2,	mm3		; mm2 = (stepy << 32) + stepx
	movq	mm7,	mm0		; mm7 = mm0
	movq	mm4,	mm2		; mm4 = mm2
	paddd	mm4,	mm2		; mm4 *= 2
	push	ebp
	paddd	mm7,	mm2		; mm7 += mm2
	lea	ebp,	[edi+ecx*4]		; limit
	sub	ebp,	byte 4*3
	cmp	edi,	ebp
	jae	near	.pfraction		; jump if edi >= ebp

	loop_align
.ploop:
	movq	mm5,	mm0		; 1 mm5 = sx,sy
	movq	mm6,	mm7		; 2 mm6 = sx,sy
	psrad	mm5,	16		; 1 mm5 >>= 16
	paddd	mm0,	mm4		; 1 sx,sy += stepx,stepy
	psrad	mm6,	16		; 2 mm6 >>= 16
	movd	eax,	mm5		; 1 eax = (sx >> 16)
	movd	ecx,	mm6		; 2 ecx = (sx >> 16)
	psrlq	mm5,	32		; 1 mm5 >>= 32
	paddd	mm7,	mm4		; 2 sx,sy += stepx,stepy
	psrlq	mm6,	32		; 2 mm6 >>= 32
	movd	ebx,	mm5		; 1 ebx = (sy >> 16)
	movd	edx,	mm6		; 2 edx = (sy >> 16)
	imul	ebx,	[esp + 56]		; 1 ebx *= srcpitch
	imul	edx,	[esp + 56]		; 2 edx *= srcpitch
	add	ebx,	esi		; 1 ebx += src
	add	edx,	esi		; 2 edx += src
	movq	mm3,	mm0		; 3 mm3 = sx,sy
	mov	ebx,	[ebx + eax*4]		; 1 load
	movq	mm1,	mm7		; 4 mm1 = sx,sy
	psrad	mm3,	16		; 3 mm3 >>= 16
	mov	edx,	[edx + ecx*4]		; 2 load
	paddd	mm0,	mm4		; 3 sx,sy += stepx,stepy
	psrad	mm1,	16		; 4 mm1 >>= 16
	mov	[edi],	ebx		; 1 store
	movd	eax,	mm3		; 3 eax = (sx >> 16)
	movd	ecx,	mm1		; 4 ecx = (sx >> 16)
	mov	[edi+4],edx		; 2 store
	psrlq	mm3,	32		; 3 mm3 >>= 32
	paddd	mm7,	mm4		; 4 sx,sy += stepx,stepy
	psrlq	mm1,	32		; 4 mm1 >>= 32
	movd	ebx,	mm3		; 3 ebx = (sy >> 16)
	movd	edx,	mm1		; 4 edx = (sy >> 16)
	imul	ebx,	[esp + 56]		; 3 ebx *= srcpitch
	imul	edx,	[esp + 56]		; 4 edx *= srcpitch
	add	edi,	byte 16
	add	ebx,	esi		; 3 ebx += src
	add	edx,	esi		; 4 edx += src
	mov	ebx,	[ebx + eax*4]		; 3 load
	cmp	edi,	ebp
	mov	edx,	[edx + ecx*4]		; 4 load
	mov	[edi+8-16],	ebx		; 3 store
	mov	[edi+12-16],	edx		; 4 store

	jb	near .ploop		; jump if edi < ebp

.pfraction:
	add	ebp,	byte 4*3
	cmp	edi,	ebp
	jae	.pexit		; jump if edi >= ebp

.ploop2:
	movq	mm5,	mm0		; mm5 = sx,sy
	psrad	mm5,	16		; mm5 >>= 16
	movd	ecx,	mm5		; ecx = (sx >> 16)
	psrlq	mm5,	32		; mm5 >>= 32
	movd	eax,	mm5		; eax = (sy >> 16)
	imul	eax,	[esp + 56]		; eax *= srcpitch
	add	eax,	esi		; eax += src
	mov	eax,	[eax + ecx*4]		; load
	mov	[edi],	eax		; store
	paddd	mm0,	mm2		; sx,sy += stepx,stepy
	add	edi,	byte 4

	cmp	edi,	ebp
	jb	.ploop2		; jump if edi < ebp

.pexit:
	pop	ebp
.pexit2:
	pop	edx
	pop	ecx
	pop	ebx
	pop	esi
	pop	edi
	emms
	ret

;--------------------------------------------------------------------

;;[function_replace_by TVPCPUType & TVP_CPU_HAS_MMX] TVPLinTransConstAlphaBlend
;;void, TVPLinTransConstAlphaBlend_mmx_a, (tjs_uint32 *dest, tjs_int len, const tjs_uint32 *src,  tjs_int sx, tjs_int sy, tjs_int stepx, tjs_int stepy, tjs_int srcpitch, tjs_int opa)


	function_align
TVPLinTransConstAlphaBlend_mmx_a:			; linear transforming copy with constant-ratio alpha blending
	push	edi
	push	esi
	push	ebx
	push	ecx
	push	edx
	mov	ecx,	[esp + 28]		; len
	cmp	ecx,	byte 0
	jle	near	.pexit2
	mov	edi,	[esp + 24]		; dest
	mov	esi,	[esp + 32]		; src
	movd	mm0,	[esp + 36]		; sx
	movd	mm1,	[esp + 40]		; sy
	movd	mm2,	[esp + 44]		; stepx
	movd	mm5,	[esp + 48]		; stepy
	movd	mm3,	[esp + 56]		; opa
	punpcklwd	mm3,	mm3
	punpcklwd	mm3,	mm3
	psllq	mm1,	32		; mm1 <<= 32
	psllq	mm5,	32		; mm5 <<= 32
	por	mm0,	mm1		; mm0 = (sy << 32) + sx
	por	mm2,	mm5		; mm2 = (stepy << 32) + stepx
	movq	mm7,	mm0		; mm7 = mm0
	movq	mm4,	mm2		; mm4 = mm2
	paddd	mm4,	mm2		; mm4 *= 2
	push	ebp
	paddd	mm7,	mm2		; mm7 += mm2
	movq	[esp - 16], mm2		; [esp-16] = tmp
	lea	ebp,	[edi+ecx*4]		; limit
	sub	ebp,	byte 4*3
	cmp	edi,	ebp
	jae	near	.pfraction		; jump if edi >= ebp

	loop_align
.ploop:
	movq	mm5,	mm0		; 1 mm5 = sx,sy
	movq	mm6,	mm7		; 2 mm6 = sx,sy
	psrad	mm5,	16		; 1 mm5 >>= 16
	paddd	mm0,	mm4		; 1 sx,sy += stepx,stepy
	psrad	mm6,	16		; 2 mm6 >>= 16
	movd	eax,	mm5		; 1 eax = (sx >> 16)
	movd	ecx,	mm6		; 2 ecx = (sx >> 16)
	psrlq	mm5,	32		; 1 mm5 >>= 32
	paddd	mm7,	mm4		; 2 sx,sy += stepx,stepy
	psrlq	mm6,	32		; 2 mm6 >>= 32
	movd	ebx,	mm5		; 1 ebx = (sy >> 16)
	movd	edx,	mm6		; 2 edx = (sy >> 16)
	imul	ebx,	[esp + 56]		; 1 ebx *= srcpitch
	imul	edx,	[esp + 56]		; 2 edx *= srcpitch
	add	ebx,	esi		; 1 ebx += src
	add	edx,	esi		; 2 edx += src

	pxor	mm6,	mm6
	movd	mm2,	[ebx + eax*4]		; load src to mm2
	movd	mm1,	[edi]		; load dst to mm1
	punpcklbw	mm2,	mm6		; unpack
	punpcklbw	mm1,	mm6		; unpack
	psubw	mm2,	mm1
	pmullw	mm2,	mm3
	psllw	mm1,	8
	paddw	mm1,	mm2
	psrlw	mm1,	8
	packuswb	mm1,	mm6
	movd	[edi],	mm1		; store to dest

	movd	mm2,	[edx + ecx*4]		; load src to mm2
	movd	mm1,	[edi+4]		; load dst to mm1
	punpcklbw	mm2,	mm6		; unpack
	punpcklbw	mm1,	mm6		; unpack
	psubw	mm2,	mm1
	pmullw	mm2,	mm3
	psllw	mm1,	8
	paddw	mm1,	mm2
	psrlw	mm1,	8
	packuswb	mm1,	mm6
	movd	[edi+4],	mm1		; store to dest

	movq	mm5,	mm0		; 3 mm5 = sx,sy
	movq	mm1,	mm7		; 4 mm1 = sx,sy
	psrad	mm5,	16		; 3 mm5 >>= 16
	paddd	mm0,	mm4		; 3 sx,sy += stepx,stepy
	psrad	mm1,	16		; 4 mm1 >>= 16
	movd	eax,	mm5		; 3 eax = (sx >> 16)
	movd	ecx,	mm1		; 4 ecx = (sx >> 16)
	psrlq	mm5,	32		; 3 mm5 >>= 32
	paddd	mm7,	mm4		; 4 sx,sy += stepx,stepy
	psrlq	mm1,	32		; 4 mm1 >>= 32
	movd	ebx,	mm5		; 3 ebx = (sy >> 16)
	movd	edx,	mm1		; 4 edx = (sy >> 16)
	imul	ebx,	[esp + 56]		; 3 ebx *= srcpitch
	imul	edx,	[esp + 56]		; 4 edx *= srcpitch
	add	edi,	byte 16
	add	ebx,	esi		; 3 ebx += src
	add	edx,	esi		; 4 edx += src

	movd	mm2,	[ebx + eax*4]		; load src to mm2
	movd	mm1,	[edi+8-16]		; load dst to mm1
	punpcklbw	mm2,	mm6		; unpack
	punpcklbw	mm1,	mm6		; unpack
	psubw	mm2,	mm1
	pmullw	mm2,	mm3
	psllw	mm1,	8
	paddw	mm1,	mm2
	psrlw	mm1,	8
	packuswb	mm1,	mm6
	movd	[edi+8-16],	mm1		; store to dest

	movd	mm2,	[edx + ecx*4]		; load src to mm2
	movd	mm1,	[edi+12-16]		; load dst to mm1
	punpcklbw	mm2,	mm6		; unpack
	punpcklbw	mm1,	mm6		; unpack
	psubw	mm2,	mm1
	pmullw	mm2,	mm3
	psllw	mm1,	8
	paddw	mm1,	mm2
	psrlw	mm1,	8
	packuswb	mm1,	mm6
	movd	[edi+12-16],	mm1		; store to dest

	cmp	edi,	ebp
	jb	near .ploop		; jump if edi < ebp

.pfraction:
	add	ebp,	byte 4*3
	cmp	edi,	ebp
	jae	.pexit		; jump if edi >= ebp

	movq	mm4, [esp - 16]		; [esp-16] = tmp
	pxor	mm6,	mm6

.ploop2:
	movq	mm5,	mm0		; mm5 = sx,sy
	psrad	mm5,	16		; mm5 >>= 16
	movd	ecx,	mm5		; ecx = (sx >> 16)
	psrlq	mm5,	32		; mm5 >>= 32
	movd	eax,	mm5		; eax = (sy >> 16)
	imul	eax,	[esp + 56]		; eax *= srcpitch
	add	eax,	esi		; eax += src

	movd	mm2,	[eax + ecx*4]		; load src to mm2
	movd	mm1,	[edi]		; load dst to mm1
	punpcklbw	mm2,	mm6		; unpack
	punpcklbw	mm1,	mm6		; unpack
	psubw	mm2,	mm1
	pmullw	mm2,	mm3
	psllw	mm1,	8
	paddw	mm1,	mm2
	psrlw	mm1,	8
	packuswb	mm1,	mm6
	movd	[edi],	mm1		; store to dest

	paddd	mm0,	mm4		; sx,sy += stepx,stepy
	add	edi,	byte 4

	cmp	edi,	ebp
	jb	.ploop2		; jump if edi < ebp

.pexit:
	pop	ebp
.pexit2:
	pop	edx
	pop	ecx
	pop	ebx
	pop	esi
	pop	edi
	emms
	ret



;--------------------------------------------------------------------
	segment_code
;--------------------------------------------------------------------
; memo
;          s0p0   s0p1
;          s1p0   s1p1
;
;	dst = (s0p0 * (1-bx) + s0p1 * bx ) * (1-by) +
;	      (s1p0 * (1-bx) + s1p1 * bx ) * by

;	dst = (s0p0 - s0p0 * bx + s0p1 * bx ) * (1-by) +
;	      (s1p0 - s1p0 * bx + s1p1 * bx ) * by

;	dst = (s0p0 + s0p1 * bx - s0p0 * bx ) * (1-by) +
;	      (s1p0 + s1p1 * bx - s1p0 * bx ) * by

;	dst = (s0p0 + (s0p1 - s0p0) * bx ) * (1-by) +
;	      (s1p0 + (s1p1 - s1p0) * bx ) * by

;	dst = S0 * (1-by) +
;	      S1 * by

;	dst = S0 - S0 * by + S1 * by

;	dst = S0 + (S1 - S0) * by



;;[function_replace_by TVPCPUType & TVP_CPU_HAS_MMX] TVPInterpStretchCopy
;;void, TVPInterpStretchCopy_mmx_a, (tjs_uint32 *dest, tjs_int destlen, const tjs_uint32 *src1, const tjs_uint32 *src2, tjs_int blend_y, tjs_int srcstart, tjs_int srcstep)

	function_align
TVPInterpStretchCopy_mmx_a:			; bilinear stretch copy
	push	edi
	push	esi
	push	ebx
	push	ecx
	push	edx
	mov	ecx,	[esp + 28]		; len
	cmp	ecx,	byte 0
	jle	near	.pexit2

	mov	eax,	[esp + 40]		; blend_y (0..255)
	mov	ebx,	eax
	shr	ebx,	7
	add	eax,	ebx		; adjust by
	movd	mm7,	eax
	punpcklwd	mm7,	mm7
	punpcklwd	mm7,	mm7		; mm7 = bx

	mov	edi,	[esp + 24]		; dest
	mov	esi,	[esp + 32]		; src1
	mov	ebx,	[esp + 44]		; srcstart (srcp)
	mov	edx,	[esp + 48]		; srcstep
	mov	eax,	[esp + 36]		; src2

	pxor	mm0,	mm0		; mm0 = 0

	push	ebp
	lea	ebp,	[edi+ecx*4]		; limit
	mov	ecx,	eax		; src2
	sub	ebp,	byte 4*1
	cmp	edi,	ebp
	jae	near	.pfraction		; jump if edi >= ebp

	loop_align
.ploop:
	mov	eax,	ebx		; 1 tmp = srcp
	shr	eax,	16		; 1 tmp >>= 16

	movd	mm6,	[eax*4+esi]		; 1 load: load s0p0
	movd	mm2,	[eax*4+esi+4]	; 1 load: load s0p1
	movd	mm3,	[eax*4+ecx]		; 1 load: load s1p0
	movd	mm4,	[eax*4+ecx+4]	; 1 load: load s1p1

	mov	eax,	ebx		; 1 bx:
	shr	eax,	8		; 1 bx:
	punpcklbw	mm6,	mm0		; 1 load: unpack s0p0
	and	eax,	0xff		; 1 bx: eax = bx
	punpcklbw	mm2,	mm0		; 1 load: unpack s0p1
	movd	mm5,	eax		; 1 bx:
	punpcklbw	mm3,	mm0		; 1 load: unpack s1p0
	punpcklwd	mm5,	mm5		; 1 bx:
	punpcklbw	mm4,	mm0		; 1 load: unpack s1p1
	punpcklwd	mm5,	mm5		; 1 bx: mm5 = bx

	psubw	mm2,	mm6		; 1 s0: mm2 = s0p1 - s0p0
	psubw	mm4,	mm3		; 1 s1: mm4 = s1p1 - s1p0
	pmullw	mm2,	mm5		; 1 s0:
	pmullw	mm4,	mm5		; 1 s1:
	psllw	mm6,	8		; 1 s0:
	psllw	mm3,	8		; 1 s1:
	paddw	mm6,	mm2		; 1 s0:
	paddw	mm3,	mm4		; 1 s1:
	psrlw	mm6,	8		; 1 s0: mm6 = s0p0 + (s0p1 - s0p0) * bx = S0
	psrlw	mm3,	8		; 1 s1: mm3 = s1p0 + (s1p1 - s1p0) * bx = S1

	add	ebx,	edx		; 1 srcp += srcstep
	psubw	mm3,	mm6		; 1 s0/s1: mm3 = S1 - S0
	mov	eax,	ebx		; 2 tmp = srcp
	pmullw	mm3,	mm7		; 1 s0/s1:
	shr	eax,	16		; 2 tmp >>= 16

	psllw	mm6,	8		; 1 s0/s1:
	movd	mm1,	[eax*4+esi]		; 2 load: load s0p0
	paddw	mm6,	mm3		; 1 s0/s1:
	movd	mm2,	[eax*4+esi+4]	; 2 load: load s0p1
	movd	mm3,	[eax*4+ecx]		; 2 load: load s1p0
	psrlw	mm6,	8		; 1 s0/s1: mm6 = S0 + (S1 - S0) * by = dst
	movd	mm4,	[eax*4+ecx+4]	; 2 load: load s1p1
	packuswb	mm6,	mm0		; 1 pack

	mov	eax,	ebx		; 2 bx:
	movd	[edi],	mm6		; 1 store to dest
	shr	eax,	8		; 2 bx:
	punpcklbw	mm1,	mm0		; 2 load: unpack s0p0
	and	eax,	0xff		; 2 bx: eax = bx
	punpcklbw	mm2,	mm0		; 2 load: unpack s0p1
	movd	mm5,	eax		; 2 bx:
	punpcklbw	mm3,	mm0		; 2 load: unpack s1p0
	punpcklwd	mm5,	mm5		; 2 bx:
	punpcklbw	mm4,	mm0		; 2 load: unpack s1p1
	punpcklwd	mm5,	mm5		; 2 bx: mm5 = bx

	psubw	mm2,	mm1		; 2 s0: mm2 = s0p1 - s0p0
	psubw	mm4,	mm3		; 2 s1: mm4 = s1p1 - s1p0
	pmullw	mm2,	mm5		; 2 s0:
	pmullw	mm4,	mm5		; 2 s1:
	psllw	mm1,	8		; 2 s0:
	psllw	mm3,	8		; 2 s1:
	paddw	mm1,	mm2		; 2 s0:
	paddw	mm3,	mm4		; 2 s1:
	psrlw	mm1,	8		; 2 s0: mm1 = s0p0 + (s0p1 - s0p0) * bx = S0
	psrlw	mm3,	8		; 2 s1: mm3 = s1p0 + (s1p1 - s1p0) * bx = S1

	add	ebx,	edx		; 2 srcp += srcstep
	psubw	mm3,	mm1		; 2 s0/s1: mm3 = S1 - S0
	add	edi,	byte 8		; 2 dest ++
	pmullw	mm3,	mm7		; 2 s0/s1:
	psllw	mm1,	8		; 2 s0/s1:
	cmp	edi,	ebp
	paddw	mm1,	mm3		; 2 s0/s1:
	psrlw	mm1,	8		; 2 s0/s1: mm1 = S0 + (S1 - S0) * by = dst

	packuswb	mm1,	mm0		; 2 pack

	movd	[edi+4-8],	mm1		; 2 store to dest

	jb	.ploop			; jump if edi < ebp

.pfraction:
	add	ebp,	byte 4*1
	cmp	edi,	ebp
	jae	near .pexit			; jump if edi >= ebp

.ploop2:

	mov	eax,	ebx		; tmp = srcp
	shr	eax,	16		; tmp >>= 16

	movd	mm1,	[eax*4+esi]		; load: load s0p0
	movd	mm2,	[eax*4+esi+4]	; load: load s0p1
	movd	mm3,	[eax*4+ecx]		; load: load s1p0
	movd	mm4,	[eax*4+ecx+4]	; load: load s1p1

	mov	eax,	ebx		; bx:
	shr	eax,	8		; bx:
	punpcklbw	mm1,	mm0		; load: unpack s0p0
	and	eax,	0xff		; bx: eax = bx
	punpcklbw	mm2,	mm0		; load: unpack s0p1
	movd	mm5,	eax		; bx:
	punpcklbw	mm3,	mm0		; load: unpack s1p0
	punpcklwd	mm5,	mm5		; bx:
	punpcklbw	mm4,	mm0		; load: unpack s1p1
	punpcklwd	mm5,	mm5		; bx: mm5 = bx

	psubw	mm2,	mm1		; s0: mm2 = s0p1 - s0p0
	psubw	mm4,	mm3		; s1: mm4 = s1p1 - s1p0
	pmullw	mm2,	mm5		; s0:
	pmullw	mm4,	mm5		; s1:
	psllw	mm1,	8		; s0:
	psllw	mm3,	8		; s1:
	paddw	mm1,	mm2		; s0:
	paddw	mm3,	mm4		; s1:
	psrlw	mm1,	8		; s0: mm1 = s0p0 + (s0p1 - s0p0) * bx = S0
	psrlw	mm3,	8		; s1: mm3 = s1p0 + (s1p1 - s1p0) * bx = S1

	psubw	mm3,	mm1		; s0/s1: mm3 = S1 - S0
	pmullw	mm3,	mm7		; s0/s1:
	psllw	mm1,	8		; s0/s1:
	paddw	mm1,	mm3		; s0/s1:
	psrlw	mm1,	8		; s0/s1: mm1 = S0 + (S1 - S0) * by = dst

	packuswb	mm1,	mm0		; pack

	movd	[edi],	mm1		; store to dest

	add	ebx,	edx		; srcp += srcstep
	add	edi,	byte 4		; dest ++

	cmp	edi,	ebp
	jb	.ploop2			; jump if edi < ebp

.pexit:
	pop	ebp
.pexit2:
	pop	edx
	pop	ecx
	pop	ebx
	pop	esi
	pop	edi
	emms
	ret
