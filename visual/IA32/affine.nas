; -*- Mode: asm; tab-width: 12; -*-
; this is a part of TVP (KIRIKIRI) software source.
; see other sources for license.
; (C)2001-2003 W.Dee <dee@kikyou.info>

; stretching copy / linear interpolation / linear transformation copy

%include		"nasm.nah"

globaldef		TVPLinTransCopy_mmx_a
globaldef		TVPLinTransConstAlphaBlend_mmx_a

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


