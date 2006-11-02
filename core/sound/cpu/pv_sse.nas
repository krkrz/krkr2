%include		"nasm.nah"
externdef sin
externdef cos
globaldef sse_PSTARTEDGEM1
globaldef sse_PCS_RRRR
globaldef sse_PFV_0P5
globaldef sse__Z31RisaInterleaveOverlappingWindowPfPKPKfS_ijj
globaldef sse_PFV_1
globaldef sse_PFV_0
globaldef sse_RISA_V_R_2PI
globaldef sse__Z6makectiPiPf
globaldef sse_PCS_NRRN
globaldef sse_PCS_RNRN
globaldef sse_PMASKTABLE
globaldef sse_PABSMASK
globaldef sse_RISA_VFASTATAN2_E
globaldef sse__Z33RisaInterleaveOverlappingWindow_rPfPKPKfS_ijj
globaldef sse_PCS_NNRR
globaldef sse_PCS_RNNR
globaldef sse_RISA_V_R_PI
globaldef sse_RISA_VFASTSINCOS_SS1
globaldef sse_RISA_VFASTSINCOS_CC2
globaldef sse_PCS_NRNR
globaldef sse_PCS_NNNR
globaldef sse_RISA_V_I32_1
globaldef sse__Z32RisaDeinterleaveApplyingWindow_rPPfPKfS_ijj
globaldef sse_PCS_NRNN
globaldef sse_PENDEDGEM2
globaldef sse_PCS_NRRR
globaldef sse_RISA_VFASTATAN2_C2
globaldef sse_PSTARTEDGEM3
globaldef sse__ZN35tRisaPhaseVocoderDSP_SSE_Trampoline13__ProcessCoreEi
globaldef sse_RISA_VFASTATAN2_C1
globaldef sse__Z6rdft_riiPfPiS_
globaldef sse_PCS_NNRN
globaldef sse_PCS_RNRR
globaldef sse_RISA_VFASTSINCOS_SS2
globaldef sse_PENDEDGEM3
globaldef sse_RISA_VFASTSINCOS_SS3
globaldef sse_RISA_VFASTSINCOS_CC3
globaldef sse_PSTARTEDGEM2
globaldef sse_RISA_VFASTSINCOS_CC1
globaldef sse_PFV_4
globaldef sse__Z4rdftiiPfPiS_
globaldef sse_RISA_V_PI
globaldef sse_PFV_INIT
globaldef sse__Z6makewtiPiPf
globaldef sse__ZN35tRisaPhaseVocoderDSP_SSE_Trampoline12_ProcessCoreEi
globaldef sse_RISA_VFASTSINCOS_SS4
globaldef sse_PFV_8
globaldef sse_PFV_2
globaldef sse__ZN20tRisaPhaseVocoderDSP11ProcessCoreEi
globaldef sse__ZN35tRisaPhaseVocoderDSP_SSE_Trampoline14_ProcessCore_rEi
globaldef sse_RISA_V_2PI
globaldef sse_PCS_RRNN
globaldef sse__Z30RisaDeinterleaveApplyingWindowPPfPKfS_ijj
globaldef sse_PCS_RNNN
globaldef sse_PENDEDGEM1
segment_data_aligned
align 16
align 16
sse_RISA_V_I32_1:
	DD 1
	DD 1
	DD 1
	DD 1
align 16
sse_RISA_V_2PI:
	DD 1086918619
	DD 1086918619
	DD 1086918619
	DD 1086918619
align 16
sse_RISA_V_PI:
	DD 1078530011
	DD 1078530011
	DD 1078530011
	DD 1078530011
align 16
sse_RISA_V_R_2PI:
	DD 1042479491
	DD 1042479491
	DD 1042479491
	DD 1042479491
align 16
sse_RISA_V_R_PI:
	DD 1050868099
	DD 1050868099
	DD 1050868099
	DD 1050868099
align 16
sse_RISA_VFASTSINCOS_CC3:
	DD -1129877496
	DD -1129877496
	DD -1129877496
	DD -1129877496
align 16
sse_RISA_VFASTSINCOS_CC2:
	DD 1048697085
	DD 1048697085
	DD 1048697085
	DD 1048697085
align 16
sse_RISA_VFASTSINCOS_CC1:
	DD -1080170033
	DD -1080170033
	DD -1080170033
	DD -1080170033
align 16
sse_RISA_VFASTSINCOS_SS4:
	DD -1147733212
	DD -1147733212
	DD -1147733212
	DD -1147733212
align 16
sse_RISA_VFASTSINCOS_SS3:
	DD 1034104910
	DD 1034104910
	DD 1034104910
	DD 1034104910
align 16
sse_RISA_VFASTSINCOS_SS2:
	DD -1088070177
	DD -1088070177
	DD -1088070177
	DD -1088070177
align 16
sse_RISA_VFASTSINCOS_SS1:
	DD 1070141403
	DD 1070141403
	DD 1070141403
	DD 1070141403
align 16
sse_RISA_VFASTATAN2_E:
	DD 786163455
	DD 786163455
	DD 786163455
	DD 786163455
align 16
sse_RISA_VFASTATAN2_C2:
	DD 1075235812
	DD 1075235812
	DD 1075235812
	DD 1075235812
align 16
sse_RISA_VFASTATAN2_C1:
	DD 1061752795
	DD 1061752795
	DD 1061752795
	DD 1061752795
align 16
sse_PFV_0P5:
	DD 1056964608
	DD 1056964608
	DD 1056964608
	DD 1056964608
align 16
sse_PFV_INIT:
	DD 0
	DD 1065353216
	DD 1073741824
	DD 1077936128
align 16
sse_PFV_8:
	DD 1090519040
	DD 1090519040
	DD 1090519040
	DD 1090519040
align 16
sse_PFV_4:
	DD 1082130432
	DD 1082130432
	DD 1082130432
	DD 1082130432
align 16
sse_PFV_2:
	DD 1073741824
	DD 1073741824
	DD 1073741824
	DD 1073741824
align 16
sse_PFV_1:
	DD 1065353216
	DD 1065353216
	DD 1065353216
	DD 1065353216
align 16
sse_PFV_0:
	DD 0
	DD 0
	DD 0
	DD 0
align 16
sse_PMASKTABLE:
	DD 0
	DD 0
	DD 0
	DD 0
	DD -1
	DD 0
	DD 0
	DD 0
	DD 0
	DD -1
	DD 0
	DD 0
	DD -1
	DD -1
	DD 0
	DD 0
	DD 0
	DD 0
	DD -1
	DD 0
	DD -1
	DD 0
	DD -1
	DD 0
	DD 0
	DD -1
	DD -1
	DD 0
	DD -1
	DD -1
	DD -1
	DD 0
	DD 0
	DD 0
	DD 0
	DD -1
	DD -1
	DD 0
	DD 0
	DD -1
	DD 0
	DD -1
	DD 0
	DD -1
	DD -1
	DD -1
	DD 0
	DD -1
	DD 0
	DD 0
	DD -1
	DD -1
	DD -1
	DD 0
	DD -1
	DD -1
	DD 0
	DD -1
	DD -1
	DD -1
	DD -1
	DD -1
	DD -1
	DD -1
align 16
sse_PENDEDGEM3:
	DD -1
	DD -1
	DD -1
	DD 0
align 16
sse_PENDEDGEM2:
	DD -1
	DD -1
	DD 0
	DD 0
align 16
sse_PENDEDGEM1:
	DD -1
	DD 0
	DD 0
	DD 0
align 16
sse_PSTARTEDGEM3:
	DD 0
	DD 0
	DD 0
	DD -1
align 16
sse_PSTARTEDGEM2:
	DD 0
	DD 0
	DD -1
	DD -1
align 16
sse_PSTARTEDGEM1:
	DD 0
	DD -1
	DD -1
	DD -1
align 16
sse_PABSMASK:
	DD 2147483647
	DD 2147483647
	DD 2147483647
	DD 2147483647
align 16
sse_PCS_NNNR:
	DD -2147483648
	DD 0
	DD 0
	DD 0
align 16
sse_PCS_RRRR:
	DD -2147483648
	DD -2147483648
	DD -2147483648
	DD -2147483648
align 16
sse_PCS_RNNR:
	DD -2147483648
	DD 0
	DD 0
	DD -2147483648
align 16
sse_PCS_RRNN:
	DD 0
	DD 0
	DD -2147483648
	DD -2147483648
align 16
sse_PCS_RNRR:
	DD -2147483648
	DD -2147483648
	DD 0
	DD -2147483648
align 16
sse_PCS_RNRN:
	DD 0
	DD -2147483648
	DD 0
	DD -2147483648
align 16
sse_PCS_RNNN:
	DD 0
	DD 0
	DD 0
	DD -2147483648
align 16
sse_PCS_NRRR:
	DD -2147483648
	DD -2147483648
	DD -2147483648
	DD 0
align 16
sse_PCS_NRRN:
	DD 0
	DD -2147483648
	DD -2147483648
	DD 0
align 16
sse_PCS_NRNR:
	DD -2147483648
	DD 0
	DD -2147483648
	DD 0
align 16
sse_PCS_NRNN:
	DD 0
	DD 0
	DD -2147483648
	DD 0
align 16
sse_PCS_NNRR:
	DD -2147483648
	DD -2147483648
	DD 0
	DD 0
align 16
sse_PCS_NNRN:
	DD 0
	DD -2147483648
	DD 0
	DD 0
align 16
segment_code
align 2
align 16
_Z23RisaVFast_sincos_F4_SSEU8__vectorfRS_S0_:
	mulps	xmm0,  [sse_RISA_V_R_2PI]
	movaps	xmm4,  [sse_PFV_2]
	push	ebp
	mov	ebp, esp
	movaps	xmm1, xmm0
	movhlps	xmm1, xmm0
	cvtps2pi	mm1, xmm0
	cvtps2pi	mm0, xmm1
	movaps	xmm1, xmm2
	cvtpi2ps	xmm1, mm1
	cvtpi2ps	xmm2, mm0
	movlhps	xmm1, xmm2
	subps	xmm0, xmm1
	movaps	xmm2, xmm0
	mulps	xmm2, xmm0
	movaps	xmm1, xmm2
	mulps	xmm1,  [sse_RISA_VFASTSINCOS_SS4]
	addps	xmm1,  [sse_RISA_VFASTSINCOS_SS3]
	mulps	xmm1, xmm2
	addps	xmm1,  [sse_RISA_VFASTSINCOS_SS2]
	mulps	xmm1, xmm2
	addps	xmm1,  [sse_RISA_VFASTSINCOS_SS1]
	mulps	xmm1, xmm0
	movaps	xmm0, xmm2
	mulps	xmm0,  [sse_RISA_VFASTSINCOS_CC3]
	addps	xmm0,  [sse_RISA_VFASTSINCOS_CC2]
	mulps	xmm0, xmm2
	addps	xmm0,  [sse_RISA_VFASTSINCOS_CC1]
	mulps	xmm0, xmm2
	movaps	xmm2, xmm1
	addps	xmm0,  [sse_PFV_1]
	mulps	xmm2, xmm1
	movaps	xmm3, xmm0
	mulps	xmm3, xmm0
	mulps	xmm1, xmm0
	subps	xmm3, xmm2
	mulps	xmm1, xmm4
	movaps	xmm0, xmm3
	mulps	xmm0, xmm3
	movaps	xmm5, xmm1
	movaps	xmm2, xmm4
	mulps	xmm5, xmm1
	subps	xmm2, xmm0
	mulps	xmm1, xmm3
	subps	xmm2, xmm5
	subps	xmm0, xmm5
	mulps	xmm1, xmm4
	mulps	xmm0, xmm2
	movaps	  [edx], xmm0
	mulps	xmm1, xmm2
	movaps	  [eax], xmm1
	emms
	pop	ebp
	ret
align 2
align 16
_Z6cft1stiPfS_:
	push	ebp
	movaps	xmm5, xmm3
	mov	ebp, esp
	push	esi
	mov	esi, eax
	push	ebx
	movlps	xmm5,   [edx]
	movlps	xmm6,   [edx+8]
	movhps	xmm5,   [edx+16]
	movhps	xmm6,   [edx+24]
	movaps	xmm3, xmm5
	addps	xmm3, xmm6
	subps	xmm5, xmm6
	movaps	xmm0, xmm3
	movaps	xmm6, xmm0
	movaps	xmm4, xmm5
	movhlps	xmm6, xmm0
	xorps	xmm6,  [sse_PCS_RRNN]
	shufps	xmm4, xmm4, 187
	movlhps	xmm3, xmm3
	xorps	xmm4,  [sse_PCS_RNNR]
	movlhps	xmm5, xmm5
	mov	ebx, 16
	addps	xmm3, xmm6
	movlps	xmm6,   [edx+40]
	movhps	xmm6,   [edx+56]
	movhps	  [edx+16], xmm3
	addps	xmm5, xmm4
	cmp	ebx, eax
	movlps	  [edx], xmm3
	movhps	  [edx+24], xmm5
	movlps	  [edx+8], xmm5
	movaps	xmm5, xmm3
	movlps	xmm5,   [edx+32]
	movhps	xmm5,   [edx+48]
	movaps	xmm3, xmm5
	addps	xmm3, xmm6
	movaps	xmm0, xmm3
	subps	xmm5, xmm6
	movaps	xmm6, xmm0
	movaps	xmm4, xmm5
	shufps	xmm6, xmm0, 158
	shufps	xmm4, xmm4, 75
	xorps	xmm6,  [sse_PCS_RRNN]
	shufps	xmm3, xmm3, 52
	shufps	xmm5, xmm5, 180
	xorps	xmm4,  [sse_PCS_RNNR]
	addps	xmm3, xmm6
	movhps	  [edx+48], xmm3
	addps	xmm5, xmm4
	movaps	xmm6, xmm5
	movlps	  [edx+32], xmm3
	shufps	xmm6, xmm6, 165
	shufps	xmm5, xmm5, 240
	movss	xmm4,[ecx+8]
	xorps	xmm6,  [sse_PCS_NRNR]
	shufps	xmm4, xmm4, 0
	addps	xmm5, xmm6
	mulps	xmm5, xmm4
	movlps	  [edx+40], xmm5
	movhps	  [edx+56], xmm5
	jge	L146
	add	edx, 64
	mov	eax, ecx
align 16
L144:
	add	eax, 8
	add	ecx, 16
	movaps	xmm2, xmm7
	movhps	xmm2,   [eax]
	movlps	xmm1,   [ecx]
	movaps	xmm3, xmm1
	movaps	xmm5, xmm2
	shufps	xmm3, xmm1, 17
	shufps	xmm5, xmm2, 255
	mulps	xmm3, xmm5
	movlps	xmm6,   [edx+8]
	addps	xmm3, xmm3
	subps	xmm3, xmm1
	movhps	xmm6,   [edx+24]
	movaps	xmm7, xmm2
	xorps	xmm3,  [sse_PCS_NRNR]
	shufps	xmm7, xmm2, 170
	add	ebx, 16
	movaps	xmm5, xmm3
	movlhps	xmm1, xmm3
	movlps	xmm5,   [edx]
	movhps	xmm5,   [edx+16]
	movaps	xmm3, xmm5
	addps	xmm3, xmm6
	movaps	xmm0, xmm3
	subps	xmm5, xmm6
	movaps	xmm6, xmm0
	movaps	xmm4, xmm5
	movhlps	xmm6, xmm0
	shufps	xmm4, xmm4, 187
	xorps	xmm6,  [sse_PCS_RRNN]
	movlhps	xmm3, xmm3
	movlhps	xmm5, xmm5
	xorps	xmm4,  [sse_PCS_RNNR]
	addps	xmm3, xmm6
	movaps	xmm6, xmm3
	movlps	  [edx], xmm3
	addps	xmm5, xmm4
	movaps	xmm4, xmm2
	shufps	xmm4, xmm2, 255
	shufps	xmm6, xmm3, 187
	mulps	xmm6, xmm4
	xorps	xmm6,  [sse_PCS_NRNR]
	mulps	xmm3, xmm7
	movaps	xmm4, xmm1
	movaps	xmm0, xmm5
	shufps	xmm4, xmm4, 245
	shufps	xmm0, xmm5, 177
	addps	xmm3, xmm6
	mulps	xmm0, xmm4
	shufps	xmm1, xmm1, 160
	movhps	  [edx+16], xmm3
	mulps	xmm5, xmm1
	movaps	xmm2, xmm1
	movlps	xmm6,   [edx+40]
	movhps	xmm6,   [edx+56]
	xorps	xmm0,  [sse_PCS_NRNR]
	addps	xmm5, xmm0
	movhps	  [edx+24], xmm5
	movlps	  [edx+8], xmm5
	movhps	xmm0,   [eax]
	movlps	xmm2,   [ecx+8]
	movaps	xmm3, xmm2
	movaps	xmm5, xmm0
	shufps	xmm5, xmm0, 170
	shufps	xmm3, xmm2, 17
	movaps	xmm7, xmm0
	mulps	xmm3, xmm5
	addps	xmm3, xmm3
	shufps	xmm7, xmm0, 170
	subps	xmm3, xmm2
	xorps	xmm3,  [sse_PCS_NRNR]
	movaps	xmm5, xmm3
	movlhps	xmm2, xmm3
	movlps	xmm5,   [edx+32]
	movhps	xmm5,   [edx+48]
	movaps	xmm3, xmm5
	addps	xmm3, xmm6
	subps	xmm5, xmm6
	movaps	xmm1, xmm3
	movaps	xmm6, xmm1
	movaps	xmm4, xmm5
	movhlps	xmm6, xmm1
	xorps	xmm6,  [sse_PCS_RRNN]
	movlhps	xmm3, xmm3
	shufps	xmm4, xmm4, 187
	xorps	xmm4,  [sse_PCS_RNNR]
	movlhps	xmm5, xmm5
	movaps	xmm1, xmm2
	addps	xmm3, xmm6
	shufps	xmm1, xmm2, 160
	movaps	xmm6, xmm3
	movlps	  [edx+32], xmm3
	shufps	xmm3, xmm3, 187
	mulps	xmm3, xmm7
	addps	xmm5, xmm4
	movaps	xmm4, xmm0
	shufps	xmm4, xmm0, 255
	xorps	xmm3,  [sse_PCS_NRNR]
	mulps	xmm6, xmm4
	movaps	xmm0, xmm5
	movaps	xmm4, xmm2
	shufps	xmm0, xmm5, 177
	shufps	xmm4, xmm2, 245
	subps	xmm3, xmm6
	mulps	xmm0, xmm4
	mulps	xmm5, xmm1
	movhps	  [edx+48], xmm3
	xorps	xmm0,  [sse_PCS_NRNR]
	addps	xmm5, xmm0
	movaps	xmm7, xmm0
	movlps	  [edx+40], xmm5
	movhps	  [edx+56], xmm5
	add	edx, 64
	cmp	ebx, esi
	jl	L144
L146:
	pop	ebx
	pop	esi
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
	mov	esi, ecx
	push	ebx
	sub	esp, 204
	mov	DWORD  [ebp-84], 0
	cmp	DWORD  [ebp-84], edx
	mov	DWORD  [ebp-80], eax
	lea	eax, [0+edx*4]
	mov	DWORD  [ebp-96], eax
	jge	L282
	mov	edx, ecx
	lea	eax, [edi+edi*2]
align 16
L167:
	movaps	xmm2,   [edx]
	movaps	xmm1,   [edx+edi*4]
	movaps	xmm0,   [edx+edi*8]
	movaps	xmm3, xmm2
	addps	xmm3, xmm1
	subps	xmm2, xmm1
	movaps	xmm1,   [esi+eax*4]
	movaps	xmm4, xmm0
	movaps	xmm5, xmm3
	subps	xmm0, xmm1
	addps	xmm4, xmm1
	shufps	xmm0, xmm0, 177
	xorps	xmm0,  [sse_PCS_NRNR]
	movaps	xmm1, xmm2
	addps	xmm5, xmm4
	movaps	  [edx], xmm5
	subps	xmm3, xmm4
	addps	xmm1, xmm0
	movaps	  [edx+edi*4], xmm1
	subps	xmm2, xmm0
	movaps	  [edx+edi*8], xmm3
	add	edx, 16
	add	DWORD  [ebp-84], 4
	movaps	  [esi+eax*4], xmm2
	lea	eax, [eax+4]
	cmp	DWORD  [ebp-84], edi
	jl	L167
L282:
	mov	edx, DWORD  [ebp+8]
	mov	ecx, DWORD  [ebp-96]
	mov	eax, DWORD  [edx+8]
	mov	DWORD  [ebp-84], ecx
	lea	ecx, [edi+ecx]
	mov	DWORD  [ebp-28], eax
	movss	xmm6,[ebp-28]
	cmp	ecx, DWORD  [ebp-84]
	shufps	xmm6, xmm6, 0
	jle	L284
	mov	ebx, DWORD  [ebp-84]
	lea	eax, [edi+edi*2]
	lea	edx, [esi+ebx*4]
	add	eax, ebx
align 16
L194:
	movaps	xmm3,   [edx]
	movaps	xmm0,   [edx+edi*4]
	movaps	xmm2,   [edx+edi*8]
	movaps	xmm5, xmm3
	addps	xmm5, xmm0
	subps	xmm3, xmm0
	movaps	xmm0,   [esi+eax*4]
	movaps	xmm4, xmm2
	movaps	xmm1, xmm5
	addps	xmm4, xmm0
	subps	xmm2, xmm0
	movaps	xmm0, xmm5
	shufps	xmm1, xmm4, 216
	shufps	xmm0, xmm4, 141
	shufps	xmm0, xmm0, 216
	shufps	xmm1, xmm1, 114
	addps	xmm5, xmm4
	subps	xmm1, xmm0
	movaps	  [edx], xmm5
	movaps	xmm0, xmm2
	shufps	xmm0, xmm2, 160
	movaps	  [edx+edi*8], xmm1
	movaps	xmm1, xmm3
	shufps	xmm1, xmm3, 160
	shufps	xmm2, xmm2, 245
	movaps	xmm5, xmm1
	subps	xmm5, xmm2
	addps	xmm2, xmm1
	movaps	xmm1,  [sse_PCS_NRNR]
	shufps	xmm3, xmm3, 245
	movaps	xmm4, xmm3
	addps	xmm4, xmm0
	xorps	xmm4, xmm1
	subps	xmm0, xmm3
	addps	xmm5, xmm4
	xorps	xmm2, xmm1
	mulps	xmm5, xmm6
	movaps	  [edx+edi*4], xmm5
	add	edx, 16
	addps	xmm0, xmm2
	add	DWORD  [ebp-84], 4
	mulps	xmm0, xmm6
	movaps	  [esi+eax*4], xmm0
	lea	eax, [eax+4]
	cmp	ecx, DWORD  [ebp-84]
	jg	L194
L284:
	mov	DWORD  [ebp-92], 0
	mov	eax, DWORD  [ebp-96]
	mov	edx, DWORD  [ebp-80]
	add	eax, eax
	cmp	eax, edx
	mov	DWORD  [ebp-188], eax
	mov	DWORD  [ebp-88], eax
	jge	L286
	lea	ecx, [edi+edi*2]
	add	eax, edi
	mov	DWORD  [ebp-196], ecx
	mov	DWORD  [ebp-200], eax
align 16
L280:
	add	DWORD  [ebp-92], 2
	mov	edx, DWORD  [ebp+8]
	mov	eax, DWORD  [ebp-92]
	mov	ebx, DWORD  [ebp-92]
	fld	DWORD  [edx+4+eax*4]
	add	ebx, ebx
	fld	DWORD  [edx+ebx*4]
	fld	st1
	fadd	st0, st2
	fld	DWORD  [edx+4+ebx*4]
	fld	DWORD  [edx+eax*4]
	fld	st2
	fxch	st4
	mov	eax, DWORD  [ebp-88]
	fst	DWORD  [ebp-32]
	fxch	st4
	fmul	st0, st2
	fxch	st3
	movss	xmm0,[ebp-32]
	mov	DWORD  [ebp-208], ebx
	fmul	st0, st4
	fxch	st2
	shufps	xmm0, xmm0, 0
	fst	DWORD  [ebp-36]
	fxch	st3
	fsubp	st4, st0
	fxch	st1
	fsubrp	st2, st0
	fst	DWORD  [ebp-40]
	fxch	st3
	movaps	  [ebp-120], xmm0
	movss	xmm1,[ebp-36]
	movss	xmm0,[ebp-40]
	fst	DWORD  [ebp-44]
	fxch	st2
	fstp	DWORD  [ebp-48]
	shufps	xmm1, xmm1, 0
	shufps	xmm0, xmm0, 0
	movaps	  [ebp-136], xmm1
	movaps	  [ebp-152], xmm0
	movss	xmm1,[ebp-44]
	movss	xmm0,[ebp-48]
	fstp	DWORD  [ebp-52]
	movss	xmm7,[ebp-52]
	shufps	xmm1, xmm1, 0
	shufps	xmm0, xmm0, 0
	movaps	  [ebp-168], xmm1
	shufps	xmm7, xmm7, 0
	movaps	xmm1,   [ebp-136]
	movaps	  [ebp-184], xmm0
	movaps	xmm0,  [sse_PCS_NRNR]
	xorps	xmm1, xmm0
	xorps	xmm7, xmm0
	movaps	  [ebp-136], xmm1
	movaps	xmm1,   [ebp-168]
	xorps	xmm1, xmm0
	movaps	  [ebp-168], xmm1
	mov	DWORD  [ebp-84], eax
	cmp	DWORD  [ebp-200], eax
	jle	L288
	mov	ecx, DWORD  [ebp-196]
	lea	edx, [esi+eax*4]
	lea	eax, [ecx+eax]
align 16
L238:
	movaps	xmm3,   [edx]
	movaps	xmm0,   [edx+edi*4]
	movaps	xmm4,   [edx+edi*8]
	movaps	xmm5, xmm3
	addps	xmm5, xmm0
	subps	xmm3, xmm0
	movaps	xmm0,   [esi+eax*4]
	movaps	xmm2, xmm4
	movaps	xmm1, xmm5
	addps	xmm2, xmm0
	subps	xmm4, xmm0
	movaps	xmm6, xmm2
	movaps	xmm0, xmm5
	shufps	xmm6, xmm2, 177
	shufps	xmm0, xmm5, 177
	subps	xmm1, xmm2
	subps	xmm0, xmm6
	addps	xmm5, xmm2
	mulps	xmm0,   [ebp-168]
	mulps	xmm1,   [ebp-152]
	movaps	  [edx], xmm5
	movaps	xmm5, xmm3
	shufps	xmm5, xmm3, 177
	addps	xmm1, xmm0
	movaps	xmm2, xmm5
	movaps	  [edx+edi*8], xmm1
	movaps	xmm1, xmm4
	shufps	xmm1, xmm4, 177
	movaps	xmm0,  [sse_PCS_NRNR]
	xorps	xmm1, xmm0
	xorps	xmm4, xmm0
	movaps	xmm0, xmm3
	subps	xmm2, xmm4
	addps	xmm0, xmm1
	mulps	xmm0,   [ebp-120]
	subps	xmm3, xmm1
	addps	xmm5, xmm4
	mulps	xmm2,   [ebp-136]
	mulps	xmm5, xmm7
	mulps	xmm3,   [ebp-184]
	addps	xmm0, xmm2
	addps	xmm3, xmm5
	movaps	  [edx+edi*4], xmm0
	add	edx, 16
	add	DWORD  [ebp-84], 4
	movaps	  [esi+eax*4], xmm3
	lea	eax, [eax+4]
	mov	ebx, DWORD  [ebp-84]
	cmp	DWORD  [ebp-200], ebx
	jg	L238
	movaps	xmm0,  [sse_PCS_NRNR]
L288:
	mov	eax, DWORD  [ebp-208]
	fld	st1
	mov	edx, DWORD  [ebp+8]
	fadd	st0, st2
	fld	DWORD  [edx+8+eax*4]
	fld	st1
	fld	DWORD  [edx+12+eax*4]
	fxch	st3
	fmul	st0, st2
	fxch	st1
	mov	eax, DWORD  [ebp-88]
	mov	edx, DWORD  [ebp-96]
	fmul	st0, st3
	fxch	st2
	add	eax, edx
	fst	DWORD  [ebp-56]
	fxch	st1
	mov	ecx, eax
	fsub	st0, st3
	fxch	st3
	movss	xmm1,[ebp-56]
	fstp	DWORD  [ebp-60]
	add	ecx, edi
	fsubrp	st1, st0
	fxch	st3
	fstp	DWORD  [ebp-64]
	fxch	st1
	cmp	ecx, eax
	fstp	DWORD  [ebp-68]
	fxch	st1
	shufps	xmm1, xmm1, 0
	movaps	  [ebp-120], xmm1
	movss	xmm1,[ebp-60]
	fstp	DWORD  [ebp-72]
	fstp	DWORD  [ebp-76]
	movss	xmm7,[ebp-76]
	shufps	xmm1, xmm1, 0
	movaps	  [ebp-136], xmm1
	movss	xmm1,[ebp-64]
	shufps	xmm7, xmm7, 0
	xorps	xmm7, xmm0
	mov	DWORD  [ebp-84], eax
	shufps	xmm1, xmm1, 0
	movaps	  [ebp-152], xmm1
	movss	xmm1,[ebp-68]
	mov	DWORD  [ebp-192], ecx
	shufps	xmm1, xmm1, 0
	movaps	  [ebp-168], xmm1
	movss	xmm1,[ebp-72]
	shufps	xmm1, xmm1, 0
	movaps	  [ebp-184], xmm1
	movaps	xmm1,   [ebp-136]
	xorps	xmm1, xmm0
	movaps	  [ebp-136], xmm1
	movaps	xmm1,   [ebp-152]
	xorps	xmm1, xmm0
	movaps	  [ebp-152], xmm1
	jg	L279
	jmp	L290
align 8
L291:
	mov	edx, DWORD  [ebp-84]
	lea	ecx, [edx+edi]
L279:
	movaps	xmm0,   [esi+ecx*4]
	lea	ebx, [ecx+edi]
	mov	eax, DWORD  [ebp-84]
	mov	DWORD  [ebp-204], ebx
	add	ebx, edi
	lea	edx, [esi+eax*4]
	movaps	xmm3,   [edx]
	mov	eax, DWORD  [ebp-204]
	lea	eax, [esi+eax*4]
	mov	DWORD  [ebp-204], eax
	movaps	xmm5, xmm3
	addps	xmm5, xmm0
	movaps	xmm4,   [eax]
	subps	xmm3, xmm0
	movaps	xmm1, xmm5
	movaps	xmm0,   [esi+ebx*4]
	movaps	xmm2, xmm4
	addps	xmm2, xmm0
	subps	xmm4, xmm0
	movaps	xmm6, xmm2
	movaps	xmm0, xmm5
	shufps	xmm0, xmm5, 177
	shufps	xmm6, xmm2, 177
	subps	xmm1, xmm2
	mulps	xmm1,   [ebp-168]
	subps	xmm0, xmm6
	addps	xmm5, xmm2
	mulps	xmm0,   [ebp-152]
	movaps	  [edx], xmm5
	movaps	xmm5, xmm3
	shufps	xmm5, xmm3, 177
	subps	xmm0, xmm1
	movaps	xmm1, xmm4
	shufps	xmm1, xmm4, 177
	movaps	  [eax], xmm0
	movaps	xmm2, xmm5
	movaps	xmm0,  [sse_PCS_NRNR]
	add	DWORD  [ebp-84], 4
	xorps	xmm1, xmm0
	xorps	xmm4, xmm0
	movaps	xmm0, xmm3
	subps	xmm2, xmm4
	mov	edx, DWORD  [ebp-84]
	mulps	xmm2,   [ebp-136]
	addps	xmm0, xmm1
	subps	xmm3, xmm1
	mulps	xmm0,   [ebp-120]
	addps	xmm5, xmm4
	mulps	xmm5, xmm7
	mulps	xmm3,   [ebp-184]
	cmp	DWORD  [ebp-192], edx
	addps	xmm0, xmm2
	movaps	  [esi+ecx*4], xmm0
	addps	xmm3, xmm5
	movaps	  [esi+ebx*4], xmm3
	jg	L291
L290:
	mov	ecx, DWORD  [ebp-188]
	mov	ebx, DWORD  [ebp-80]
	add	DWORD  [ebp-88], ecx
	add	DWORD  [ebp-200], ecx
	cmp	DWORD  [ebp-88], ebx
	jl	L280
L286:
	add	esp, 204
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
	push	ebx
	sub	esp, 64
	cmp	eax, 8
	mov	DWORD  [ebp-20], ecx
	mov	ecx, eax
	mov	DWORD  [ebp-16], edx
	mov	DWORD  [edx], 0
	mov	DWORD  [ebp-32], 1
	jle	L343
align 16
L299:
	mov	DWORD  [ebp-24], 0
	mov	eax, DWORD  [ebp-32]
	sar	ecx, 1
	cmp	DWORD  [ebp-24], eax
	jge	L345
	mov	edx, DWORD  [ebp-16]
align 16
L298:
	inc	DWORD  [ebp-24]
	mov	eax, DWORD  [edx]
	mov	ebx, DWORD  [ebp-32]
	add	eax, ecx
	mov	DWORD  [edx+ebx*4], eax
	add	edx, 4
	cmp	DWORD  [ebp-24], ebx
	jl	L298
L345:
	sal	DWORD  [ebp-32], 1
	mov	eax, DWORD  [ebp-32]
	sal	eax, 3
	cmp	eax, ecx
	jl	L299
L343:
	mov	eax, DWORD  [ebp-32]
	mov	esi, DWORD  [ebp-32]
	sal	eax, 3
	add	esi, esi
	cmp	eax, ecx
	mov	DWORD  [ebp-36], esi
	je	L354
	mov	DWORD  [ebp-28], 1
	mov	ecx, DWORD  [ebp-32]
	cmp	DWORD  [ebp-28], ecx
	jge	L357
align 16
L358:
	mov	DWORD  [ebp-24], 0
	mov	ebx, DWORD  [ebp-28]
	cmp	DWORD  [ebp-24], ebx
	jge	L352
	add	ebx, ebx
	mov	DWORD  [ebp-68], ebx
align 16
L340:
	mov	esi, DWORD  [ebp-28]
	mov	edi, DWORD  [ebp-16]
	mov	edx, DWORD  [ebp-24]
	mov	ecx, DWORD  [ebp-16]
	mov	eax, DWORD  [edi+esi*4]
	mov	esi, DWORD  [ebp-68]
	mov	ebx, DWORD  [ecx+edx*4]
	lea	edi, [eax+edx*2]
	mov	eax, DWORD  [ebp-20]
	add	esi, ebx
	lea	ecx, [eax+esi*4]
	lea	ebx, [eax+edi*4]
	mov	eax, DWORD  [ebp-36]
	movlps	xmm0,   [ebx]
	movlps	xmm1,   [ecx]
	lea	edx, [edi+eax]
	mov	edi, DWORD  [ebp-20]
	lea	eax, [esi+eax]
	lea	edx, [edi+edx*4]
	movhps	xmm0,   [edx]
	lea	eax, [edi+eax*4]
	movhps	xmm1,   [eax]
	movlps	  [ecx], xmm0
	movlps	  [ebx], xmm1
	movhps	  [eax], xmm0
	movhps	  [edx], xmm1
	mov	eax, DWORD  [ebp-28]
	inc	DWORD  [ebp-24]
	cmp	DWORD  [ebp-24], eax
	jl	L340
L352:
	inc	DWORD  [ebp-28]
	mov	edx, DWORD  [ebp-32]
	cmp	DWORD  [ebp-28], edx
	jl	L358
L357:
	add	esp, 64
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
L354:
	mov	DWORD  [ebp-28], 0
	mov	edi, DWORD  [ebp-32]
	cmp	DWORD  [ebp-28], edi
	jge	L357
	mov	DWORD  [ebp-76], esi
	sal	edi, 2
	mov	DWORD  [ebp-72], edi
L324:
	mov	DWORD  [ebp-24], 0
	mov	eax, DWORD  [ebp-28]
	cmp	DWORD  [ebp-24], eax
	jge	L349
	mov	edx, DWORD  [ebp-72]
	add	eax, eax
	mov	ecx, DWORD  [ebp-36]
	mov	DWORD  [ebp-40], eax
	add	edx, ecx
	mov	DWORD  [ebp-64], edx
align 16
L323:
	mov	ebx, DWORD  [ebp-16]
	mov	ecx, DWORD  [ebp-28]
	mov	esi, DWORD  [ebp-24]
	mov	eax, DWORD  [ebx+ecx*4]
	mov	ecx, DWORD  [ebp-20]
	lea	edi, [eax+esi*2]
	mov	eax, DWORD  [ebp-24]
	mov	esi, DWORD  [ebp-40]
	mov	edx, DWORD  [ebx+eax*4]
	mov	eax, DWORD  [ebp-72]
	mov	ebx, DWORD  [ebp-20]
	add	esi, edx
	mov	edx, DWORD  [ebp-20]
	lea	ecx, [ecx+esi*4]
	add	eax, edi
	lea	eax, [ebx+eax*4]
	lea	edx, [edx+edi*4]
	mov	DWORD  [ebp-44], edx
	movlps	xmm0,   [edx]
	mov	edx, DWORD  [ebp-20]
	mov	DWORD  [ebp-48], ecx
	movlps	xmm1,   [ecx]
	mov	DWORD  [ebp-52], eax
	movlps	xmm2,   [eax]
	mov	eax, DWORD  [ebp-36]
	lea	ebx, [esi+eax]
	add	eax, edi
	lea	ebx, [edx+ebx*4]
	lea	ecx, [edx+eax*4]
	movlps	xmm3,   [ebx]
	mov	eax, DWORD  [ebp-64]
	mov	DWORD  [ebp-56], ecx
	movhps	xmm0,   [ecx]
	mov	ecx, DWORD  [ebp-72]
	add	eax, esi
	add	ecx, esi
	lea	ecx, [edx+ecx*4]
	mov	edx, DWORD  [ebp-64]
	movhps	xmm1,   [ecx]
	mov	esi, DWORD  [ebp-48]
	add	edx, edi
	mov	edi, DWORD  [ebp-20]
	lea	edx, [edi+edx*4]
	lea	eax, [edi+eax*4]
	movhps	xmm2,   [edx]
	movhps	xmm3,   [eax]
	movlps	  [esi], xmm0
	mov	edi, DWORD  [ebp-44]
	movlps	  [edi], xmm1
	movlps	  [ebx], xmm2
	mov	ebx, DWORD  [ebp-52]
	movlps	  [ebx], xmm3
	movhps	  [ecx], xmm0
	mov	esi, DWORD  [ebp-56]
	movhps	  [esi], xmm1
	movhps	  [eax], xmm2
	movhps	  [edx], xmm3
	mov	edi, DWORD  [ebp-28]
	inc	DWORD  [ebp-24]
	cmp	DWORD  [ebp-24], edi
	jl	L323
L349:
	mov	eax, DWORD  [ebp-28]
	mov	edx, DWORD  [ebp-16]
	mov	edi, DWORD  [ebp-76]
	inc	DWORD  [ebp-28]
	mov	esi, DWORD  [edx+eax*4]
	mov	ebx, DWORD  [ebp-20]
	add	DWORD  [ebp-76], 2
	mov	ecx, DWORD  [ebp-36]
	add	edi, esi
	mov	eax, ebx
	lea	esi, [edi+ecx]
	mov	edx, DWORD  [eax+4+esi*4]
	fld	DWORD  [eax+esi*4]
	mov	ecx, DWORD  [ebx+edi*4]
	mov	ebx, DWORD  [ebx+4+edi*4]
	mov	DWORD  [eax+4+edi*4], edx
	mov	edx, DWORD  [ebp-32]
	cmp	DWORD  [ebp-28], edx
	fstp	DWORD  [eax+edi*4]
	mov	DWORD  [eax+esi*4], ecx
	mov	DWORD  [eax+4+esi*4], ebx
	jl	L324
	add	esp, 64
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
align 2
align 16
_Z7cftfsubiPfS_:
	push	ebp
	mov	ebp, esp
	push	edi
	mov	edi, edx
	push	esi
	mov	esi, eax
	push	ebx
	sub	esp, 12
	cmp	eax, 8
	mov	DWORD  [ebp-16], ecx
	mov	ebx, 2
	jg	L404
L360:
	lea	eax, [0+ebx*4]
	cmp	eax, esi
	je	L405
	xor	ecx, ecx
	cmp	ecx, ebx
	jge	L359
	mov	edx, edi
align 16
L398:
	movaps	xmm0,   [edx]
	add	ecx, 8
	movaps	xmm1,   [edx+16]
	movaps	xmm3,   [edx+ebx*4]
	movaps	xmm2,   [edx+16+ebx*4]
	movaps	xmm5, xmm0
	movaps	xmm4, xmm1
	addps	xmm5, xmm3
	subps	xmm0, xmm3
	addps	xmm4, xmm2
	movaps	  [edx], xmm5
	subps	xmm1, xmm2
	movaps	  [edx+16], xmm4
	movaps	  [edx+ebx*4], xmm0
	movaps	  [edx+16+ebx*4], xmm1
	add	edx, 32
	cmp	ecx, ebx
	jl	L398
L359:
	add	esp, 12
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
L404:
	call	_Z6cft1stiPfS_
	cmp	esi, 32
	mov	ebx, 8
	jle	L360
align 16
L363:
	mov	eax, DWORD  [ebp-16]
	mov	edx, ebx
	mov	ecx, edi
	sal	ebx, 2
	mov	DWORD  [esp], eax
	mov	eax, esi
	call	_Z6cftmdliiPfS_
	lea	eax, [0+ebx*4]
	cmp	eax, esi
	jge	L360
	mov	eax, DWORD  [ebp-16]
	mov	edx, ebx
	mov	ecx, edi
	sal	ebx, 2
	mov	DWORD  [esp], eax
	mov	eax, esi
	call	_Z6cftmdliiPfS_
	lea	eax, [0+ebx*4]
	cmp	eax, esi
	jl	L363
	jmp	L360
L405:
	xor	ecx, ecx
	cmp	ecx, ebx
	jge	L359
	mov	eax, edi
	lea	edx, [ebx+ebx*2]
align 16
L383:
	movaps	xmm2,   [eax]
	add	ecx, 4
	movaps	xmm1,   [eax+ebx*4]
	movaps	xmm0,   [eax+ebx*8]
	movaps	xmm3, xmm2
	addps	xmm3, xmm1
	subps	xmm2, xmm1
	movaps	xmm1,   [edi+edx*4]
	movaps	xmm4, xmm0
	subps	xmm0, xmm1
	shufps	xmm0, xmm0, 177
	addps	xmm4, xmm1
	movaps	xmm1, xmm3
	xorps	xmm0,  [sse_PCS_NRNR]
	addps	xmm1, xmm4
	subps	xmm3, xmm4
	movaps	  [eax], xmm1
	movaps	xmm1, xmm2
	addps	xmm1, xmm0
	movaps	  [eax+ebx*8], xmm3
	subps	xmm2, xmm0
	movaps	  [eax+ebx*4], xmm1
	add	eax, 16
	movaps	  [edi+edx*4], xmm2
	add	edx, 4
	cmp	ecx, ebx
	jl	L383
	add	esp, 12
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
segment_data_aligned
align 16
align 8
LC3:
	DD 1413754136
	DD 1072243195
align 16
segment_code
align 2
align 16
sse__Z6makewtiPiPf:
	push	ebp
	mov	ebp, esp
	push	edi
	push	esi
	push	ebx
	sub	esp, 76
	mov	edx, DWORD  [ebp+8]
	mov	eax, DWORD  [ebp+12]
	mov	edi, DWORD  [ebp+16]
	cmp	edx, 2
	mov	DWORD  [eax], edx
	mov	DWORD  [eax+4], 1
	jle	L406
	sar	edx, 1
	mov	eax, 0x3f800000
	cvtsi2ss	xmm0, edx
	mov	DWORD  [ebp-32], edx
	fild	DWORD  [ebp-32]
	fdivr	QWORD [LC3]
	mov	DWORD  [edi], eax
	mov	eax, 0x00000000
	mov	DWORD  [edi+4], eax
	movss	[ebp-60],xmm0
	fstp	DWORD  [ebp-36]
	fld	DWORD  [ebp-60]
	fmul	DWORD  [ebp-36]
	fstp	QWORD  [esp]
	call	cos
	fstp	DWORD  [ebp-28]
	mov	edx, DWORD  [ebp-32]
	mov	eax, DWORD  [ebp-28]
	cmp	edx, 2
	mov	DWORD  [edi+edx*4], eax
	mov	DWORD  [edi+4+edx*4], eax
	jle	L406
	mov	esi, 2
	cmp	esi, edx
	jmp	L415
align 8
L416:
	cvtsi2ss	xmm0, esi
	movss	[ebp-60],xmm0
	fld	DWORD  [ebp-60]
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
L415:
	jl	L416
	mov	edx, DWORD  [ebp+12]
	mov	ecx, edi
	mov	eax, DWORD  [ebp+8]
	add	edx, 8
	call	_Z6bitrv2iPiPf
align 16
L406:
	add	esp, 76
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
segment_data_aligned
align 16
align 8
LC7:
	DD 1413754136
	DD 1072243195
align 4
LC8:
	DD 1056964608
align 8
LC9:
	DD 0
	DD 1071644672
align 16
segment_code
align 2
align 16
sse__Z6makectiPiPf:
	push	ebp
	mov	ebp, esp
	push	edi
	push	esi
	push	ebx
	sub	esp, 76
	mov	ebx, DWORD  [ebp+8]
	mov	eax, DWORD  [ebp+12]
	cmp	ebx, 1
	mov	DWORD  [eax+4], ebx
	jle	L417
	mov	edi, ebx
	mov	esi, 1
	sar	edi, 1
	push	edi
	cvtsi2ss	xmm0, edi
	fild	DWORD  [esp]
	add	esp, 4
	fdivr	QWORD [LC7]
	movss	[ebp-60],xmm0
	fstp	DWORD  [ebp-32]
	fld	DWORD  [ebp-60]
	fmul	DWORD  [ebp-32]
	fstp	QWORD  [esp]
	call	cos
	fstp	DWORD  [ebp-28]
	mov	eax, DWORD  [ebp+16]
	cmp	esi, edi
	fld	DWORD  [ebp-28]
	fst	DWORD  [eax]
	fmul	DWORD [LC8]
	fstp	DWORD  [eax+edi*4]
	jge	L417
	lea	ebx, [eax-4+ebx*4]
align 16
L422:
	cvtsi2ss	xmm0, esi
	movss	[ebp-60],xmm0
	fld	DWORD  [ebp-60]
	fmul	DWORD  [ebp-32]
	fst	QWORD  [esp]
	fstp	DWORD  [ebp-56]
	call	cos
	fmul	QWORD [LC9]
	mov	eax, DWORD  [ebp+16]
	fld	DWORD  [ebp-56]
	fxch	st1
	fstp	DWORD  [eax+esi*4]
	inc	esi
	fstp	QWORD  [esp]
	call	sin
	fmul	QWORD [LC9]
	fstp	DWORD  [ebx]
	sub	ebx, 4
	cmp	esi, edi
	jl	L422
align 16
L417:
	add	esp, 76
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
segment_data_aligned
align 16
align 4
LC11:
	DD 1056964608
align 16
segment_code
align 2
align 16
sse__Z6rdft_riiPfPiS_:
	push	ebp
	mov	ebp, esp
	push	edi
	push	esi
	push	ebx
	sub	esp, 156
	mov	edx, DWORD  [ebp+20]
	mov	edi, DWORD  [ebp+24]
	mov	eax, DWORD  [ebp+12]
	mov	esi, DWORD  [ebp+8]
	mov	DWORD  [ebp-36], edi
	mov	edi, DWORD  [edx]
	mov	ebx, DWORD  [ebp+16]
	mov	DWORD  [ebp-28], eax
	lea	eax, [0+edi*4]
	cmp	eax, esi
	mov	DWORD  [ebp-32], edx
	jl	L511
L425:
	mov	edx, DWORD  [ebp-32]
	mov	edx, DWORD  [edx+4]
	mov	eax, edx
	mov	DWORD  [ebp-40], edx
	sal	eax, 2
	cmp	eax, esi
	jl	L512
	mov	eax, DWORD  [ebp-28]
	test	eax, eax
	js	L427
L521:
	cmp	esi, 4
	jg	L513
	je	L514
L456:
	fld	DWORD  [ebx]
	fld	DWORD  [ebx+4]
	fld	st1
	fsub	st0, st1
	fxch	st2
	faddp	st1, st0
	fxch	st1
	fstp	DWORD  [ebx+4]
	fstp	DWORD  [ebx]
L520:
	add	esp, 156
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
align 8
L512:
	mov	edx, DWORD  [ebp-36]
	mov	eax, esi
	sar	eax, 2
	mov	DWORD  [ebp-40], eax
	lea	eax, [edx+edi*4]
	mov	DWORD  [esp+8], eax
	mov	eax, DWORD  [ebp-32]
	mov	DWORD  [esp+4], eax
	mov	edx, DWORD  [ebp-40]
	mov	DWORD  [esp], edx
	call	sse__Z6makectiPiPf
	mov	eax, DWORD  [ebp-28]
	test	eax, eax
	jns	L521
align 16
L427:
	fld	DWORD  [ebx]
	cmp	esi, 4
	fld	st0
	fsub	DWORD  [ebx+4]
	fmul	DWORD [LC11]
	fsub	st1, st0
	fxch	st1
	fstp	DWORD  [ebx]
	jle	L459
	fchs
	mov	edx, esi
	mov	eax, DWORD  [ebp-36]
	sar	edx, 1
	mov	ecx, 2
	mov	DWORD  [ebp-92], edx
	mov	edx, DWORD  [ebp-40]
	lea	edi, [eax+edi*4]
	mov	DWORD  [ebp-88], edi
	fstp	DWORD  [ebx+4]
	add	edx, edx
	mov	eax, edx
	cdq
	idiv	DWORD  [ebp-92]
	mov	DWORD  [ebp-96], eax
	xor	eax, eax
	cmp	ecx, DWORD  [ebp-92]
	mov	DWORD  [ebp-140], eax
	jge	L503
	lea	edx, [ebx-8+esi*4]
align 16
L463:
	fld	DWORD  [ebx+ecx*4]
	mov	edi, DWORD  [ebp-96]
	mov	eax, DWORD  [ebp-40]
	add	DWORD  [ebp-140], edi
	fld	st0
	fld	DWORD  [ebx+4+ecx*4]
	fld	DWORD [LC11]
	mov	edi, DWORD  [ebp-140]
	fld	st1
	fxch	st3
	fsub	DWORD  [edx]
	fxch	st3
	sub	eax, edi
	mov	edi, DWORD  [ebp-88]
	mov	DWORD  [ebp-144], eax
	fadd	DWORD  [edx+4]
	fxch	st1
	fsub	DWORD  [edi+eax*4]
	mov	eax, DWORD  [ebp-140]
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
	cmp	ecx, DWORD  [ebp-92]
	jl	L463
L503:
	mov	edx, DWORD  [ebp-92]
	mov	ecx, ebx
	mov	eax, esi
	xor	BYTE  [ebx+7+edx*4], -128
	mov	edx, DWORD  [ebp-32]
	add	edx, 8
	call	_Z6bitrv2iPiPf
	mov	DWORD  [ebp-100], 2
	cmp	esi, 8
	jg	L516
L465:
	mov	eax, DWORD  [ebp-100]
	sal	eax, 2
	cmp	eax, esi
	je	L517
	xor	ecx, ecx
	cmp	ecx, DWORD  [ebp-100]
	jmp	L509
align 8
L518:
	fld	DWORD  [ebx+ecx*4]
	mov	edx, DWORD  [ebp-100]
	fld	DWORD  [ebx+4+ecx*4]
	fld	st1
	lea	eax, [ecx+edx]
	fld	DWORD  [ebx+eax*4]
	fld	DWORD  [ebx+4+eax*4]
	fxch	st4
	fadd	st0, st1
	fxch	st4
	fsub	st0, st3
	fxch	st3
	fchs
	fxch	st2
	fsubrp	st1, st0
	fxch	st3
	fstp	DWORD  [ebx+ecx*4]
	fsub	DWORD  [ebx+4+eax*4]
	fstp	DWORD  [ebx+4+ecx*4]
	fxch	st1
	add	ecx, 2
	cmp	ecx, edx
	fstp	DWORD  [ebx+eax*4]
	fstp	DWORD  [ebx+4+eax*4]
L509:
	jl	L518
	add	esp, 156
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
align 8
L511:
	mov	eax, DWORD  [ebp-36]
	mov	edi, esi
	sar	edi, 2
	mov	DWORD  [esp+4], edx
	mov	DWORD  [esp+8], eax
	mov	DWORD  [esp], edi
	call	sse__Z6makewtiPiPf
	jmp	L425
align 8
L513:
	mov	edx, DWORD  [ebp-32]
	mov	eax, esi
	mov	ecx, ebx
	add	edx, 8
	call	_Z6bitrv2iPiPf
	mov	ecx, DWORD  [ebp-36]
	mov	eax, esi
	mov	edx, ebx
	call	_Z7cftfsubiPfS_
	fld	DWORD  [ebx+8]
	mov	edx, esi
	mov	DWORD  [ebp-44], 4
	sar	edx, 1
	mov	eax, DWORD  [ebp-36]
	mov	DWORD  [ebp-48], edx
	mov	edx, DWORD  [ebp-40]
	fld	st0
	fld	DWORD  [ebx+12]
	lea	ecx, [eax+edi*4]
	add	edx, edx
	fld	DWORD [LC11]
	mov	eax, edx
	cdq
	idiv	DWORD  [ebp-48]
	lea	edx, [esi-2]
	fld	st1
	fxch	st3
	mov	DWORD  [ebp-140], eax
	mov	esi, DWORD  [ebp-140]
	mov	DWORD  [ebp-52], eax
	mov	eax, DWORD  [ebp-40]
	mov	edi, DWORD  [ebp-140]
	fsub	DWORD  [ebx+edx*4]
	fxch	st1
	sub	eax, esi
	fsub	DWORD  [ecx+eax*4]
	mov	eax, DWORD  [ebp-48]
	fld	DWORD  [ecx+edi*4]
	add	edi, edi
	fld	st1
	fxch	st5
	fadd	DWORD  [ebx+4+edx*4]
	mov	DWORD  [ebp-140], edi
	fld	st1
	fxch	st6
	cmp	DWORD  [ebp-44], eax
	fmul	st0, st4
	fxch	st6
	fmul	st0, st1
	fxch	st3
	fmulp	st1, st0
	fxch	st1
	fmulp	st3, st0
	fxch	st4
	fsubrp	st1, st0
	fxch	st3
	faddp	st1, st0
	fxch	st3
	fsub	st0, st2
	fxch	st1
	fsub	st0, st3
	fxch	st1
	fstp	DWORD  [ebx+8]
	fstp	DWORD  [ebx+12]
	fadd	DWORD  [ebx+edx*4]
	fxch	st1
	fsubr	DWORD  [ebx+4+edx*4]
	fxch	st1
	fstp	DWORD  [ebx+edx*4]
	fstp	DWORD  [ebx+4+edx*4]
	jge	L456
	mov	edi, DWORD  [ebp-40]
	lea	esi, [ebx+16]
	mov	eax, DWORD  [ebp-52]
	lea	edx, [ebx-16+edx*4]
	lea	edi, [ecx+edi*4]
	sal	eax, 2
	mov	DWORD  [ebp-76], edi
	mov	edi, DWORD  [ebp-52]
	mov	DWORD  [ebp-80], eax
	add	edi, edi
	lea	ecx, [ecx+edi*4]
	mov	DWORD  [ebp-84], edi
align 16
L454:
	movss	xmm1,[sse_PFV_0P5]
	mov	eax, DWORD  [ebp-52]
	mov	edi, DWORD  [ebp-140]
	movaps	xmm2,   [ebp-72]
	movaps	xmm4, xmm1
	sal	edi, 2
	movss	xmm7,[ecx+eax*4]
	mov	eax, DWORD  [ebp-76]
	movlps	xmm2,   [edx+8]
	movaps	xmm5,   [esi]
	movhps	xmm2,   [edx]
	movss	xmm6,[ecx]
	sub	eax, edi
	mov	edi, DWORD  [ebp-80]
	movss	xmm3,[eax]
	shufps	xmm6, xmm7, 0
	sub	eax, edi
	movss	xmm0,[eax]
	subss	xmm4, xmm3
	movaps	xmm3, xmm4
	movaps	xmm4,  [sse_PCS_NRNR]
	subss	xmm1, xmm0
	movaps	xmm0, xmm2
	shufps	xmm3, xmm1, 0
	xorps	xmm0, xmm4
	addps	xmm0, xmm5
	movaps	xmm1, xmm0
	shufps	xmm1, xmm0, 177
	mulps	xmm1, xmm6
	xorps	xmm1, xmm4
	mulps	xmm0, xmm3
	addps	xmm0, xmm1
	movaps	  [ebp-72], xmm0
	subps	xmm5, xmm0
	xorps	xmm4, xmm0
	movaps	  [esi], xmm5
	subps	xmm2, xmm4
	add	esi, 16
	movlps	  [edx+8], xmm2
	movhps	  [edx], xmm2
	sub	edx, 16
	mov	eax, DWORD  [ebp-84]
	add	DWORD  [ebp-44], 4
	mov	edi, DWORD  [ebp-48]
	lea	ecx, [ecx+eax*4]
	add	DWORD  [ebp-140], eax
	cmp	DWORD  [ebp-44], edi
	jl	L454
	jmp	L456
L459:
	fstp	DWORD  [ebx+4]
	jne	L520
	mov	ecx, DWORD  [ebp-36]
	add	esp, 156
	mov	edx, ebx
	pop	ebx
	mov	eax, 4
	pop	esi
	pop	edi
	pop	ebp
	jmp	_Z7cftfsubiPfS_
L514:
	mov	ecx, DWORD  [ebp-36]
	mov	edx, ebx
	mov	eax, 4
	call	_Z7cftfsubiPfS_
	jmp	L456
L516:
	mov	ecx, DWORD  [ebp-36]
	mov	edx, ebx
	mov	eax, esi
	call	_Z6cft1stiPfS_
	mov	DWORD  [ebp-100], 8
	cmp	esi, 32
	jle	L465
L468:
	mov	edi, DWORD  [ebp-36]
	mov	eax, esi
	mov	ecx, ebx
	mov	DWORD  [esp], edi
	mov	edx, DWORD  [ebp-100]
	call	_Z6cftmdliiPfS_
	sal	DWORD  [ebp-100], 2
	mov	eax, DWORD  [ebp-100]
	sal	eax, 2
	cmp	eax, esi
	jl	L468
	jmp	L465
L517:
	xor	ecx, ecx
	cmp	ecx, DWORD  [ebp-100]
	jge	L520
	mov	edx, DWORD  [ebp-100]
	lea	eax, [edx+edx*2]
	lea	edi, [ebx+eax*4]
	lea	eax, [esi+ebx]
	lea	esi, [ebx+edx*8]
	mov	edx, ebx
align 16
L490:
	movaps	xmm0,   [ebp-120]
	add	ecx, 2
	movlps	xmm0,   [edx]
	movhps	xmm0,   [esi]
	movaps	  [ebp-120], xmm0
	movaps	xmm0,   [ebp-136]
	movaps	xmm2,   [ebp-120]
	movaps	xmm1,   [ebp-120]
	movlps	xmm0,   [eax]
	movhps	xmm0,   [edi]
	movaps	  [ebp-136], xmm0
	shufps	xmm2, xmm2, 68
	shufps	xmm0, xmm0, 68
	xorps	xmm0,  [sse_PCS_NRRN]
	shufps	xmm1, xmm1, 238
	xorps	xmm2,  [sse_PCS_RNRN]
	addps	xmm2, xmm0
	movaps	xmm3, xmm2
	movaps	xmm0,   [ebp-136]
	shufps	xmm0, xmm0, 238
	xorps	xmm0,  [sse_PCS_RRNN]
	addps	xmm1, xmm0
	shufps	xmm1, xmm1, 180
	movaps	xmm0, xmm1
	xorps	xmm0,  [sse_PCS_NNNR]
	subps	xmm3, xmm0
	movlps	  [edx], xmm3
	add	edx, 8
	movhps	  [eax], xmm3
	add	eax, 8
	xorps	xmm1,  [sse_PCS_NNNR]
	addps	xmm2, xmm1
	movlps	  [esi], xmm2
	add	esi, 8
	movhps	  [edi], xmm2
	add	edi, 8
	cmp	ecx, DWORD  [ebp-100]
	jl	L490
	jmp	L520
align 2
align 16
sse__Z4rdftiiPfPiS_:
	push	ebp
	mov	ebp, esp
	sub	esp, 40
	and esp, -16
	mov	eax, DWORD  [ebp+24]
	mov	DWORD  [esp+16], eax
	mov	eax, DWORD  [ebp+20]
	mov	DWORD  [esp+12], eax
	mov	eax, DWORD  [ebp+16]
	mov	DWORD  [esp+8], eax
	mov	eax, DWORD  [ebp+12]
	mov	DWORD  [esp+4], eax
	mov	eax, DWORD  [ebp+8]
	mov	DWORD  [esp], eax
	call	sse__Z6rdft_riiPfPiS_
	leave
	ret
align 2
align 16
sse__ZN35tRisaPhaseVocoderDSP_SSE_Trampoline13__ProcessCoreEi:
	push	ebp
	mov	ebp, esp
	push	edi
	push	esi
	push	ebx
	sub	esp, 380
	mov	eax, DWORD  [ebp+8]
	mov	DWORD  [ebp-124], eax
	mov	edx, DWORD  [eax+32]
	mov	ecx, eax
	mov	eax, DWORD  [eax]
	shr	edx, 1
	mov	DWORD  [ebp-128], edx
	mov	edx, DWORD  [ebp+12]
	mov	esi, DWORD  [eax+edx*4]
	mov	eax, DWORD  [ecx+4]
	mov	eax, DWORD  [eax+edx*4]
	mov	DWORD  [ebp-132], eax
	stmxcsr	DWORD  [ebp-28]
	and	DWORD  [ebp-28], -24577
	ldmxcsr	DWORD  [ebp-28]
	mov	eax, DWORD  [ecx+20]
	mov	DWORD  [esp+16], eax
	mov	eax, DWORD  [ecx+16]
	mov	DWORD  [esp+8], esi
	mov	DWORD  [esp+12], eax
	mov	eax, 1
	mov	DWORD  [esp+4], eax
	mov	eax, DWORD  [ecx+32]
	mov	DWORD  [esp], eax
	call	sse__Z4rdftiiPfPiS_
	mov	ecx, DWORD  [ebp-124]
	fld1
	movss	xmm0,[ecx+80]
	fld	DWORD  [ecx+60]
	shufps	xmm0, xmm0, 0
	fucomi	st0, st1
	fstp	st1
	movaps	  [ebp-152], xmm0
	movss	xmm1,[ecx+64]
	shufps	xmm1, xmm1, 0
	movaps	  [ebp-168], xmm1
	jp	L665
	je	L530
L665:
	movss	xmm0,[ecx+68]
	xor	edx, edx
	cmp	edx, DWORD  [ebp-128]
	shufps	xmm0, xmm0, 0
	movaps	  [ebp-184], xmm0
	movss	xmm1,[ecx+72]
	shufps	xmm1, xmm1, 0
	movaps	  [ebp-200], xmm1
	movss	xmm0,[ecx+76]
	shufps	xmm0, xmm0, 0
	movaps	  [ebp-216], xmm0
	jae	L658
	fstp	st0
	movq	mm1, QWORD [sse_RISA_V_I32_1]
	pxor	mm0, mm0
	mov	eax, DWORD  [ecx+8]
	movq	QWORD  [ebp-256], mm0
	mov	ecx, DWORD  [ebp+12]
	movq	QWORD  [ebp-328], mm1
	mov	eax, DWORD  [eax+ecx*4]
align 16
L587:
	movaps	xmm0,   [esi+16+edx*8]
	movaps	xmm4,   [esi+edx*8]
	movaps	xmm6,  [sse_PCS_RRRR]
	movq	mm1, QWORD  [ebp-328]
	movaps	xmm1, xmm4
	shufps	xmm4, xmm0, 221
	movaps	xmm3, xmm4
	shufps	xmm1, xmm0, 136
	movaps	xmm2, xmm1
	andps	xmm3,  [sse_PABSMASK]
	movaps	xmm0, xmm4
	mulps	xmm0, xmm4
	mulps	xmm2, xmm1
	movaps	xmm7, xmm1
	addps	xmm2, xmm0
	addps	xmm3,  [sse_RISA_VFASTATAN2_E]
	andps	xmm7, xmm6
	sqrtps	xmm2, xmm2
	movaps	xmm0,  [sse_PFV_0]
	movaps	  [ebp-232], xmm2
	movaps	xmm2, xmm1
	andps	xmm4, xmm6
	xorps	xmm3, xmm7
	cmpltps	xmm0, xmm1
	movaps	xmm5, xmm0
	movaps	xmm0, xmm3
	addps	xmm1, xmm3
	xorps	xmm0, xmm6
	addps	xmm2, xmm0
	divps	xmm2, xmm1
	movaps	xmm1,  [sse_RISA_VFASTATAN2_C1]
	movaps	xmm0, xmm5
	andnps	xmm5,  [sse_RISA_VFASTATAN2_C2]
	xorps	xmm2, xmm7
	andps	xmm0, xmm1
	mulps	xmm1, xmm2
	movaps	xmm2,   [eax+edx*4]
	orps	xmm0, xmm5
	subps	xmm0, xmm1
	xorps	xmm0, xmm4
	subps	xmm2, xmm0
	movaps	  [eax+edx*4], xmm0
	movaps	xmm0,   [ebp-248]
	cvtsi2ss	xmm0, edx
	shufps	xmm0, xmm0, 0
	addps	xmm0,  [sse_PFV_INIT]
	movaps	  [ebp-248], xmm0
	mulps	xmm0,   [ebp-168]
	subps	xmm2, xmm0
	movaps	xmm0, xmm2
	mulps	xmm0,  [sse_RISA_V_R_PI]
	movaps	xmm1, xmm2
	cvttps2pi	mm4, xmm0
	movq	mm0, mm4
	movhlps	xmm0, xmm0
	pand	mm0, mm1
	movq	mm3, mm0
	movq	mm0, QWORD  [ebp-328]
	movq	mm1, mm4
	cvttps2pi	mm2, xmm0
	psrad	mm1, 31
	movaps	xmm0, xmm2
	por	mm1, mm0
	movq	mm0, QWORD  [ebp-256]
	psubd	mm0, mm3
	pand	mm0, mm1
	movq	mm1, QWORD  [ebp-328]
	paddd	mm4, mm0
	movq	mm0, mm2
	cvtpi2ps	xmm0, mm4
	pand	mm0, mm1
	movq	mm3, mm0
	movq	mm0, QWORD  [ebp-328]
	movq	mm1, mm2
	psrad	mm1, 31
	por	mm1, mm0
	movq	mm0, QWORD  [ebp-256]
	psubd	mm0, mm3
	pand	mm0, mm1
	paddd	mm2, mm0
	cvtpi2ps	xmm1, mm2
	movlhps	xmm0, xmm1
	mulps	xmm0,  [sse_RISA_V_PI]
	subps	xmm2, xmm0
	emms
	mulps	xmm2,   [ebp-184]
	movaps	xmm0,   [ebp-232]
	movaps	xmm1,   [ebp-232]
	addps	xmm2,   [ebp-248]
	mulps	xmm2,   [ebp-200]
	movlhps	xmm0, xmm2
	movhlps	xmm2, xmm1
	shufps	xmm0, xmm0, 216
	shufps	xmm2, xmm2, 216
	movaps	  [esi+edx*8], xmm0
	movaps	  [esi+16+edx*8], xmm2
	add	edx, 4
	cmp	edx, DWORD  [ebp-128]
	jb	L587
	mov	eax, DWORD  [ebp-124]
	fld	DWORD  [eax+60]
L658:
	xor	edi, edi
	fld1
	fdivrp	st1, st0
	cmp	edi, DWORD  [ebp-128]
	jae	L670
	fnstcw	WORD  [ebp-106]
	mov	ebx, DWORD  [ebp-132]
	movzx	eax, WORD  [ebp-106]
	or	ax, 3072
	mov	WORD  [ebp-108], ax
	jmp	L595
align 8
L673:
	fstp	st0
	cmp	ecx, DWORD  [ebp-128]
	jae	L593
	mov	eax, DWORD  [esi+ecx*8]
	inc	edi
	mov	edx, DWORD  [ebp-124]
	mov	DWORD  [ebx], eax
	fld	DWORD  [edx+60]
	fmul	DWORD  [esi+4+ecx*8]
	fstp	DWORD  [ebx+4]
	add	ebx, 8
	cmp	edi, DWORD  [ebp-128]
	jae	L670
align 16
L595:
	xor	edx, edx
	push	edx
	xor	edx, edx
	push	edi
	fild	QWORD  [esp]
	add	esp, 8
	push	edx
	fmul	st0, st1
	fld	st0
	fldcw	WORD  [ebp-108]
	fistp	QWORD  [ebp-120]
	fldcw	WORD  [ebp-106]
	mov	eax, DWORD  [ebp-120]
	push	eax
	mov	ecx, eax
	lea	eax, [ecx+1]
	fild	QWORD  [esp]
	add	esp, 8
	cmp	eax, DWORD  [ebp-128]
	fsubp	st1, st0
	jae	L673
	fld	DWORD  [esi+ecx*8]
	mov	eax, DWORD  [ebp-124]
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
L675:
	add	ebx, 8
	cmp	edi, DWORD  [ebp-128]
	jb	L595
L670:
	fstp	st0
	xor	ebx, ebx
	cmp	ebx, DWORD  [ebp-128]
	jae	L603
align 16
L674:
	mov	ecx, DWORD  [ebp-132]
	mov	edx, DWORD  [ebp-124]
	movaps	xmm0,   [ecx+ebx*8]
	mov	eax, DWORD  [edx+12]
	lea	edx, [ebp-56]
	movaps	xmm1,   [ecx+16+ebx*8]
	movaps	xmm2, xmm0
	mov	ecx, DWORD  [ebp+12]
	shufps	xmm2, xmm1, 136
	shufps	xmm0, xmm1, 221
	mov	eax, DWORD  [eax+ecx*4]
	movaps	xmm1,   [ebp-280]
	mulps	xmm0,   [ebp-216]
	movaps	  [ebp-344], xmm2
	cvtsi2ss	xmm1, ebx
	shufps	xmm1, xmm1, 0
	addps	xmm1,  [sse_PFV_INIT]
	subps	xmm0, xmm1
	movaps	  [ebp-280], xmm1
	mulps	xmm0,   [ebp-168]
	mulps	xmm1,   [ebp-168]
	addps	xmm0, xmm1
	mulps	xmm0,   [ebp-152]
	movaps	xmm1,   [eax+ebx*4]
	subps	xmm1, xmm0
	movaps	xmm0, xmm1
	movaps	  [eax+ebx*4], xmm1
	lea	eax, [ebp-72]
	call	_Z23RisaVFast_sincos_F4_SSEU8__vectorfRS_S0_
	movaps	xmm2,   [ebp-344]
	mov	eax, DWORD  [ebp-132]
	movaps	xmm1, xmm2
	mulps	xmm1,   [ebp-56]
	mulps	xmm2,   [ebp-72]
	movaps	xmm0, xmm1
	movlhps	xmm0, xmm2
	movhlps	xmm2, xmm1
	shufps	xmm0, xmm0, 216
	shufps	xmm2, xmm2, 216
	movaps	  [eax+ebx*8], xmm0
	movaps	  [eax+16+ebx*8], xmm2
	add	ebx, 4
	cmp	ebx, DWORD  [ebp-128]
	jb	L674
L603:
	mov	edx, DWORD  [ebp-124]
	mov	ecx, DWORD  [ebp+12]
	mov	eax, DWORD  [edx+20]
	mov	DWORD  [esp+16], eax
	mov	eax, DWORD  [edx+16]
	mov	DWORD  [esp+12], eax
	mov	eax, DWORD  [edx+4]
	mov	eax, DWORD  [eax+ecx*4]
	mov	DWORD  [esp+8], eax
	mov	eax, -1
	mov	DWORD  [esp+4], eax
	mov	eax, DWORD  [edx+32]
	mov	DWORD  [esp], eax
	call	sse__Z4rdftiiPfPiS_
	add	esp, 380
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
align 8
L530:
	fstp	st0
	xor	ebx, ebx
	cmp	ebx, DWORD  [ebp-128]
	jae	L603
	pxor	mm0, mm0
	movq	QWORD  [ebp-320], mm0
align 16
L656:
	movaps	xmm0,   [esi+16+ebx*8]
	mov	edx, DWORD  [ebp-124]
	mov	ecx, DWORD  [ebp+12]
	movaps	xmm1,   [esi+ebx*8]
	mov	eax, DWORD  [edx+8]
	movaps	xmm4,  [sse_PCS_RRRR]
	movq	mm2, QWORD [sse_RISA_V_I32_1]
	movaps	xmm7, xmm1
	shufps	xmm1, xmm0, 221
	movaps	  [ebp-296], xmm1
	shufps	xmm7, xmm0, 136
	mov	eax, DWORD  [eax+ecx*4]
	movaps	xmm0, xmm1
	movaps	xmm6, xmm7
	mulps	xmm0, xmm1
	andps	xmm1,  [sse_PABSMASK]
	mulps	xmm6, xmm7
	addps	xmm6, xmm0
	movaps	xmm0,  [sse_PFV_0]
	movaps	xmm5, xmm7
	andps	xmm5, xmm4
	addps	xmm1,  [sse_RISA_VFASTATAN2_E]
	movaps	xmm2, xmm7
	sqrtps	xmm6, xmm6
	cmpltps	xmm0, xmm7
	movaps	xmm3, xmm0
	xorps	xmm1, xmm5
	movaps	xmm0, xmm1
	addps	xmm7, xmm1
	xorps	xmm0, xmm4
	andps	xmm4,   [ebp-296]
	addps	xmm2, xmm0
	movaps	xmm1, xmm3
	movaps	xmm0,  [sse_RISA_VFASTATAN2_C1]
	divps	xmm2, xmm7
	xorps	xmm2, xmm5
	andnps	xmm3,  [sse_RISA_VFASTATAN2_C2]
	movaps	  [ebp-296], xmm4
	andps	xmm1, xmm0
	mulps	xmm0, xmm2
	orps	xmm1, xmm3
	subps	xmm1, xmm0
	xorps	xmm1, xmm4
	movaps	xmm0,   [eax+ebx*4]
	movaps	  [eax+ebx*4], xmm1
	subps	xmm0, xmm1
	movaps	xmm1,   [ebp-312]
	cvtsi2ss	xmm1, ebx
	shufps	xmm1, xmm1, 0
	addps	xmm1,  [sse_PFV_INIT]
	movaps	xmm3, xmm1
	mulps	xmm3,   [ebp-168]
	movaps	  [ebp-312], xmm1
	subps	xmm0, xmm3
	movaps	xmm1, xmm0
	mulps	xmm1,  [sse_RISA_V_R_PI]
	movaps	xmm2, xmm0
	cvttps2pi	mm5, xmm1
	movq	mm0, mm5
	movq	mm1, mm5
	pand	mm0, mm2
	movq	mm3, mm0
	movq	mm0, QWORD  [ebp-320]
	psrad	mm1, 31
	movhlps	xmm1, xmm1
	por	mm1, mm2
	cvttps2pi	mm4, xmm1
	movaps	xmm1, xmm0
	psubd	mm0, mm3
	pand	mm0, mm1
	paddd	mm5, mm0
	movq	mm0, mm4
	movq	mm1, mm4
	pand	mm0, mm2
	movq	mm3, mm0
	movq	mm0, QWORD  [ebp-320]
	psrad	mm1, 31
	cvtpi2ps	xmm1, mm5
	por	mm1, mm2
	psubd	mm0, mm3
	pand	mm0, mm1
	paddd	mm4, mm0
	cvtpi2ps	xmm2, mm4
	movlhps	xmm1, xmm2
	mulps	xmm1,  [sse_RISA_V_PI]
	subps	xmm0, xmm1
	emms
	mov	eax, DWORD  [edx+12]
	addps	xmm0, xmm3
	mulps	xmm0,   [ebp-152]
	lea	edx, [ebp-88]
	movaps	  [ebp-360], xmm6
	mov	eax, DWORD  [eax+ecx*4]
	movaps	xmm1,   [eax+ebx*4]
	subps	xmm1, xmm0
	movaps	xmm0, xmm1
	movaps	  [eax+ebx*4], xmm1
	lea	eax, [ebp-104]
	call	_Z23RisaVFast_sincos_F4_SSEU8__vectorfRS_S0_
	movaps	xmm6,   [ebp-360]
	mov	eax, DWORD  [ebp-132]
	movaps	xmm7, xmm6
	mulps	xmm7,   [ebp-88]
	mulps	xmm6,   [ebp-104]
	movaps	xmm0, xmm7
	movaps	xmm1, xmm6
	movhlps	xmm1, xmm7
	movlhps	xmm0, xmm6
	shufps	xmm0, xmm0, 216
	movaps	  [ebp-296], xmm1
	shufps	xmm1, xmm1, 216
	movaps	  [eax+ebx*8], xmm0
	movaps	  [eax+16+ebx*8], xmm1
	add	ebx, 4
	cmp	ebx, DWORD  [ebp-128]
	jb	L656
	jmp	L603
align 8
L593:
	mov	DWORD  [ebx], 0x00000000
	mov	DWORD  [ebx+4], 0x00000000
	inc	edi
	jmp	L675
align 16
segment_code
align 2
align 16
sse__ZN35tRisaPhaseVocoderDSP_SSE_Trampoline14_ProcessCore_rEi:
	push	ebp
	mov	ebp, esp
	pop	ebp
	jmp	sse__ZN35tRisaPhaseVocoderDSP_SSE_Trampoline13__ProcessCoreEi
align 16
segment_code
align 2
align 16
sse__ZN35tRisaPhaseVocoderDSP_SSE_Trampoline12_ProcessCoreEi:
	push	ebp
	mov	ebp, esp
	sub	esp, 24
	and esp, -16
	mov	eax, DWORD  [ebp+12]
	mov	DWORD  [esp+4], eax
	mov	eax, DWORD  [ebp+8]
	mov	DWORD  [esp], eax
	call	sse__ZN35tRisaPhaseVocoderDSP_SSE_Trampoline14_ProcessCore_rEi
	leave
	ret
align 16
segment_code
align 2
align 16
sse__ZN20tRisaPhaseVocoderDSP11ProcessCoreEi:
	push	ebp
	mov	ebp, esp
	pop	ebp
	jmp	sse__ZN35tRisaPhaseVocoderDSP_SSE_Trampoline12_ProcessCoreEi
align 2
align 16
sse__Z32RisaDeinterleaveApplyingWindow_rPPfPKfS_ijj:
	push	ebp
	mov	ebp, esp
	push	edi
	push	esi
	push	ebx
	sub	esp, 8
	mov	esi, DWORD  [ebp+20]
	mov	ecx, DWORD  [ebp+12]
	cmp	esi, 1
	je	L681
	cmp	esi, 2
	je	L802
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	cmp	DWORD  [ebp-16], edx
	jae	L974
align 16
L975:
	xor	edx, edx
	cmp	edx, esi
	jge	L962
	mov	ebx, DWORD  [ebp+24]
	mov	edi, DWORD  [ebp-16]
	add	ebx, edi
	mov	DWORD  [ebp-20], ebx
align 16
L923:
	mov	ebx, DWORD  [ebp+8]
	mov	edi, DWORD  [ebp-16]
	mov	eax, DWORD  [ebx+edx*4]
	mov	ebx, DWORD  [ebp+16]
	inc	edx
	fld	DWORD  [ebx+edi*4]
	mov	edi, DWORD  [ebp-20]
	fmul	DWORD  [ecx]
	add	ecx, 4
	cmp	edx, esi
	fstp	DWORD  [eax+edi*4]
	jl	L923
L962:
	inc	DWORD  [ebp-16]
	mov	eax, DWORD  [ebp+28]
	cmp	DWORD  [ebp-16], eax
	jb	L975
L974:
	add	esp, 8
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
L681:
	mov	edx, DWORD  [ebp+8]
	mov	edi, DWORD  [ebp+24]
	mov	eax, DWORD  [edx]
	lea	ebx, [eax+edi*4]
	test	bl, 15
	sete	al
	cmp	al, 1
	sbb	eax, eax
	and	eax, 4
	test	cl, 15
	lea	edx, [eax+2]
	sete	cl
	test	cl, cl
	cmovne	edx, eax
	lea	ecx, [edx+1]
	test	BYTE  [ebp+16], 15
	sete	al
	test	al, al
	mov	eax, edx
	cmove	eax, ecx
	cmp	DWORD  [ebp+28], 7
	ja	L969
L691:
	mov	eax, DWORD  [ebp+28]
	cmp	DWORD  [ebp-16], eax
	jmp	L963
L970:
	mov	edx, DWORD  [ebp-16]
	mov	ecx, DWORD  [ebp+16]
	mov	edi, DWORD  [ebp+12]
	mov	eax, DWORD  [ebp+28]
	fld	DWORD  [ecx+edx*4]
	fmul	DWORD  [edi+edx*4]
	fadd	DWORD  [ebx+edx*4]
	fstp	DWORD  [ebx+edx*4]
	inc	edx
	cmp	edx, eax
	mov	DWORD  [ebp-16], edx
L963:
	jb	L970
	add	esp, 8
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
L802:
	mov	edx, DWORD  [ebp+8]
	mov	eax, DWORD  [ebp+24]
	mov	esi, DWORD  [edx]
	sal	eax, 2
	mov	ebx, eax
	add	esi, eax
	mov	eax, DWORD  [edx+4]
	add	ebx, eax
	test	esi, 15
	je	L971
L803:
	mov	ecx, 4
L804:
	test	BYTE  [ebp+12], 15
	lea	eax, [ecx+2]
	sete	dl
	test	dl, dl
	cmovne	eax, ecx
	lea	ecx, [eax+1]
	test	BYTE  [ebp+16], 15
	sete	dl
	test	dl, dl
	cmove	eax, ecx
	cmp	DWORD  [ebp+28], 3
	jbe	L813
	cmp	eax, 7
	ja	L813
	jmp	DWORD [L911+eax*4]
segment_data_aligned
align 16
align 4
L911:
	DD L815
	DD L827
	DD L839
	DD L851
	DD L863
	DD L875
	DD L887
	DD L899
align 16
segment_code
L969:
	cmp	eax, 7
	ja	L691
	jmp	DWORD [L797+eax*4]
segment_data_aligned
align 16
align 4
L797:
	DD L693
	DD L706
	DD L719
	DD L732
	DD L745
	DD L758
	DD L771
	DD L784
align 16
segment_code
L971:
	xor	ecx, ecx
	test	bl, 15
	je	L804
	jmp	L803
L815:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 3
	cmp	edx, 0
	jbe	L813
	xor	eax, eax
L826:
	mov	ecx, DWORD  [ebp+16]
	mov	edi, DWORD  [ebp+12]
	movaps	xmm1,   [eax+ecx]
	movaps	xmm2,   [edi+eax*2]
	movaps	xmm0,   [edi+16+eax*2]
	movaps	xmm3, xmm1
	shufps	xmm3, xmm1, 80
	mulps	xmm2, xmm3
	shufps	xmm1, xmm1, 250
	mulps	xmm0, xmm1
	movaps	xmm1, xmm2
	shufps	xmm1, xmm0, 136
	shufps	xmm2, xmm0, 221
	movaps	  [eax+esi], xmm1
	movaps	  [eax+ebx], xmm2
	add	eax, 16
	add	DWORD  [ebp-16], 4
	cmp	edx, DWORD  [ebp-16]
	ja	L826
L813:
	mov	eax, DWORD  [ebp+28]
	cmp	DWORD  [ebp-16], eax
	jmp	L964
L972:
	mov	edx, DWORD  [ebp-16]
	mov	ecx, DWORD  [ebp+16]
	mov	edi, DWORD  [ebp+12]
	mov	eax, DWORD  [ebp+28]
	fld	DWORD  [ecx+edx*4]
	fmul	DWORD  [edi+edx*8]
	fstp	DWORD  [esi+edx*4]
	fld	DWORD  [ecx+edx*4]
	fmul	DWORD  [edi+4+edx*8]
	fstp	DWORD  [ebx+edx*4]
	inc	edx
	cmp	edx, eax
	mov	DWORD  [ebp-16], edx
L964:
	jb	L972
	add	esp, 8
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
L784:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 7
	cmp	edx, 0
	jbe	L691
	xor	eax, eax
L796:
	mov	ecx, DWORD  [ebp+12]
	mov	edi, DWORD  [ebp+16]
	movups	xmm0,   [eax+ecx]
	movups	xmm1,   [eax+edi]
	mulps	xmm0, xmm1
	movups	  [eax+ebx], xmm0
	movups	xmm0,   [eax+16+ecx]
	movups	xmm1,   [eax+16+edi]
	mulps	xmm0, xmm1
	movups	  [eax+16+ebx], xmm0
	add	eax, 32
	add	DWORD  [ebp-16], 8
	cmp	edx, DWORD  [ebp-16]
	ja	L796
	jmp	L691
L771:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 7
	cmp	edx, 0
	jbe	L691
	xor	eax, eax
L783:
	mov	ecx, DWORD  [ebp+12]
	mov	edi, DWORD  [ebp+16]
	movups	xmm0,   [eax+ecx]
	movaps	xmm1,   [eax+edi]
	mulps	xmm0, xmm1
	movups	  [eax+ebx], xmm0
	movups	xmm0,   [eax+16+ecx]
	movaps	xmm1,   [eax+16+edi]
	mulps	xmm0, xmm1
	movups	  [eax+16+ebx], xmm0
	add	eax, 32
	add	DWORD  [ebp-16], 8
	cmp	edx, DWORD  [ebp-16]
	ja	L783
	jmp	L691
L758:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 7
	cmp	edx, 0
	jbe	L691
	xor	eax, eax
L770:
	mov	ecx, DWORD  [ebp+12]
	mov	edi, DWORD  [ebp+16]
	movaps	xmm0,   [eax+ecx]
	movups	xmm1,   [eax+edi]
	mulps	xmm0, xmm1
	movups	  [eax+ebx], xmm0
	movaps	xmm0,   [eax+16+ecx]
	movups	xmm1,   [eax+16+edi]
	mulps	xmm0, xmm1
	movups	  [eax+16+ebx], xmm0
	add	eax, 32
	add	DWORD  [ebp-16], 8
	cmp	edx, DWORD  [ebp-16]
	ja	L770
	jmp	L691
L745:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 7
	cmp	edx, 0
	jbe	L691
	xor	eax, eax
L757:
	mov	ecx, DWORD  [ebp+12]
	mov	edi, DWORD  [ebp+16]
	movaps	xmm0,   [eax+ecx]
	movaps	xmm1,   [eax+edi]
	mulps	xmm0, xmm1
	movups	  [eax+ebx], xmm0
	movaps	xmm0,   [eax+16+ecx]
	movaps	xmm1,   [eax+16+edi]
	mulps	xmm0, xmm1
	movups	  [eax+16+ebx], xmm0
	add	eax, 32
	add	DWORD  [ebp-16], 8
	cmp	edx, DWORD  [ebp-16]
	ja	L757
	jmp	L691
L732:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 7
	cmp	edx, 0
	jbe	L691
	xor	eax, eax
L744:
	mov	ecx, DWORD  [ebp+12]
	mov	edi, DWORD  [ebp+16]
	movups	xmm0,   [eax+ecx]
	movups	xmm1,   [eax+edi]
	mulps	xmm0, xmm1
	movaps	  [eax+ebx], xmm0
	movups	xmm0,   [eax+16+ecx]
	movups	xmm1,   [eax+16+edi]
	mulps	xmm0, xmm1
	movaps	  [eax+16+ebx], xmm0
	add	eax, 32
	add	DWORD  [ebp-16], 8
	cmp	edx, DWORD  [ebp-16]
	ja	L744
	jmp	L691
L719:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 7
	cmp	edx, 0
	jbe	L691
	xor	eax, eax
L731:
	mov	ecx, DWORD  [ebp+12]
	mov	edi, DWORD  [ebp+16]
	movups	xmm0,   [eax+ecx]
	movaps	xmm1,   [eax+edi]
	mulps	xmm0, xmm1
	movaps	  [eax+ebx], xmm0
	movups	xmm0,   [eax+16+ecx]
	movaps	xmm1,   [eax+16+edi]
	mulps	xmm0, xmm1
	movaps	  [eax+16+ebx], xmm0
	add	eax, 32
	add	DWORD  [ebp-16], 8
	cmp	edx, DWORD  [ebp-16]
	ja	L731
	jmp	L691
L706:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 7
	cmp	edx, 0
	jbe	L691
	xor	eax, eax
L718:
	mov	ecx, DWORD  [ebp+12]
	mov	edi, DWORD  [ebp+16]
	movaps	xmm0,   [eax+ecx]
	movups	xmm1,   [eax+edi]
	mulps	xmm0, xmm1
	movaps	  [eax+ebx], xmm0
	movaps	xmm0,   [eax+16+ecx]
	movups	xmm1,   [eax+16+edi]
	mulps	xmm0, xmm1
	movaps	  [eax+16+ebx], xmm0
	add	eax, 32
	add	DWORD  [ebp-16], 8
	cmp	edx, DWORD  [ebp-16]
	ja	L718
	jmp	L691
L693:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 7
	cmp	edx, 0
	jbe	L691
	xor	eax, eax
L705:
	mov	ecx, DWORD  [ebp+12]
	mov	edi, DWORD  [ebp+16]
	movaps	xmm0,   [eax+ecx]
	movaps	xmm1,   [eax+edi]
	mulps	xmm0, xmm1
	movaps	  [eax+ebx], xmm0
	movaps	xmm0,   [eax+16+ecx]
	movaps	xmm1,   [eax+16+edi]
	mulps	xmm0, xmm1
	movaps	  [eax+16+ebx], xmm0
	add	eax, 32
	add	DWORD  [ebp-16], 8
	cmp	edx, DWORD  [ebp-16]
	ja	L705
	jmp	L691
L899:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 3
	cmp	edx, 0
	jbe	L813
	xor	eax, eax
L910:
	mov	ecx, DWORD  [ebp+16]
	mov	edi, DWORD  [ebp+12]
	movups	xmm1,   [eax+ecx]
	movups	xmm2,   [edi+eax*2]
	movups	xmm0,   [edi+16+eax*2]
	movaps	xmm3, xmm1
	shufps	xmm3, xmm1, 80
	mulps	xmm2, xmm3
	shufps	xmm1, xmm1, 250
	mulps	xmm0, xmm1
	movaps	xmm1, xmm2
	shufps	xmm1, xmm0, 136
	shufps	xmm2, xmm0, 221
	movups	  [eax+esi], xmm1
	movups	  [eax+ebx], xmm2
	add	eax, 16
	add	DWORD  [ebp-16], 4
	cmp	edx, DWORD  [ebp-16]
	ja	L910
	jmp	L813
L887:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 3
	cmp	edx, 0
	jbe	L813
	xor	eax, eax
L898:
	mov	ecx, DWORD  [ebp+16]
	mov	edi, DWORD  [ebp+12]
	movaps	xmm1,   [eax+ecx]
	movups	xmm2,   [edi+eax*2]
	movups	xmm0,   [edi+16+eax*2]
	movaps	xmm3, xmm1
	shufps	xmm3, xmm1, 80
	mulps	xmm2, xmm3
	shufps	xmm1, xmm1, 250
	mulps	xmm0, xmm1
	movaps	xmm1, xmm2
	shufps	xmm1, xmm0, 136
	shufps	xmm2, xmm0, 221
	movups	  [eax+esi], xmm1
	movups	  [eax+ebx], xmm2
	add	eax, 16
	add	DWORD  [ebp-16], 4
	cmp	edx, DWORD  [ebp-16]
	ja	L898
	jmp	L813
L875:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 3
	cmp	edx, 0
	jbe	L813
	xor	eax, eax
L886:
	mov	ecx, DWORD  [ebp+16]
	mov	edi, DWORD  [ebp+12]
	movups	xmm1,   [eax+ecx]
	movaps	xmm2,   [edi+eax*2]
	movaps	xmm0,   [edi+16+eax*2]
	movaps	xmm3, xmm1
	shufps	xmm3, xmm1, 80
	mulps	xmm2, xmm3
	shufps	xmm1, xmm1, 250
	mulps	xmm0, xmm1
	movaps	xmm1, xmm2
	shufps	xmm1, xmm0, 136
	shufps	xmm2, xmm0, 221
	movups	  [eax+esi], xmm1
	movups	  [eax+ebx], xmm2
	add	eax, 16
	add	DWORD  [ebp-16], 4
	cmp	edx, DWORD  [ebp-16]
	ja	L886
	jmp	L813
L863:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 3
	cmp	edx, 0
	jbe	L813
	xor	eax, eax
L874:
	mov	ecx, DWORD  [ebp+16]
	mov	edi, DWORD  [ebp+12]
	movaps	xmm1,   [eax+ecx]
	movaps	xmm2,   [edi+eax*2]
	movaps	xmm0,   [edi+16+eax*2]
	movaps	xmm3, xmm1
	shufps	xmm3, xmm1, 80
	mulps	xmm2, xmm3
	shufps	xmm1, xmm1, 250
	mulps	xmm0, xmm1
	movaps	xmm1, xmm2
	shufps	xmm1, xmm0, 136
	shufps	xmm2, xmm0, 221
	movups	  [eax+esi], xmm1
	movups	  [eax+ebx], xmm2
	add	eax, 16
	add	DWORD  [ebp-16], 4
	cmp	edx, DWORD  [ebp-16]
	ja	L874
	jmp	L813
L851:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 3
	cmp	edx, 0
	jbe	L813
	xor	eax, eax
L862:
	mov	ecx, DWORD  [ebp+16]
	mov	edi, DWORD  [ebp+12]
	movups	xmm1,   [eax+ecx]
	movups	xmm2,   [edi+eax*2]
	movups	xmm0,   [edi+16+eax*2]
	movaps	xmm3, xmm1
	shufps	xmm3, xmm1, 80
	mulps	xmm2, xmm3
	shufps	xmm1, xmm1, 250
	mulps	xmm0, xmm1
	movaps	xmm1, xmm2
	shufps	xmm1, xmm0, 136
	shufps	xmm2, xmm0, 221
	movaps	  [eax+esi], xmm1
	movaps	  [eax+ebx], xmm2
	add	eax, 16
	add	DWORD  [ebp-16], 4
	cmp	edx, DWORD  [ebp-16]
	ja	L862
	jmp	L813
L839:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 3
	cmp	edx, 0
	jbe	L813
	xor	eax, eax
L850:
	mov	ecx, DWORD  [ebp+16]
	mov	edi, DWORD  [ebp+12]
	movaps	xmm1,   [eax+ecx]
	movups	xmm2,   [edi+eax*2]
	movups	xmm0,   [edi+16+eax*2]
	movaps	xmm3, xmm1
	shufps	xmm3, xmm1, 80
	mulps	xmm2, xmm3
	shufps	xmm1, xmm1, 250
	mulps	xmm0, xmm1
	movaps	xmm1, xmm2
	shufps	xmm1, xmm0, 136
	shufps	xmm2, xmm0, 221
	movaps	  [eax+esi], xmm1
	movaps	  [eax+ebx], xmm2
	add	eax, 16
	add	DWORD  [ebp-16], 4
	cmp	edx, DWORD  [ebp-16]
	ja	L850
	jmp	L813
L827:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 3
	cmp	edx, 0
	jbe	L813
	xor	eax, eax
L838:
	mov	ecx, DWORD  [ebp+16]
	mov	edi, DWORD  [ebp+12]
	movups	xmm1,   [eax+ecx]
	movaps	xmm2,   [edi+eax*2]
	movaps	xmm0,   [edi+16+eax*2]
	movaps	xmm3, xmm1
	shufps	xmm3, xmm1, 80
	mulps	xmm2, xmm3
	shufps	xmm1, xmm1, 250
	mulps	xmm0, xmm1
	movaps	xmm1, xmm2
	shufps	xmm1, xmm0, 136
	shufps	xmm2, xmm0, 221
	movaps	  [eax+esi], xmm1
	movaps	  [eax+ebx], xmm2
	add	eax, 16
	add	DWORD  [ebp-16], 4
	cmp	edx, DWORD  [ebp-16]
	ja	L838
	jmp	L813
align 2
align 16
sse__Z30RisaDeinterleaveApplyingWindowPPfPKfS_ijj:
	push	ebp
	mov	ebp, esp
	sub	esp, 40
	and esp, -16
	mov	eax, DWORD  [ebp+28]
	mov	DWORD  [esp+20], eax
	mov	eax, DWORD  [ebp+24]
	mov	DWORD  [esp+16], eax
	mov	eax, DWORD  [ebp+20]
	mov	DWORD  [esp+12], eax
	mov	eax, DWORD  [ebp+16]
	mov	DWORD  [esp+8], eax
	mov	eax, DWORD  [ebp+12]
	mov	DWORD  [esp+4], eax
	mov	eax, DWORD  [ebp+8]
	mov	DWORD  [esp], eax
	call	sse__Z32RisaDeinterleaveApplyingWindow_rPPfPKfS_ijj
	leave
	ret
align 2
align 16
sse__Z33RisaInterleaveOverlappingWindow_rPfPKPKfS_ijj:
	push	ebp
	mov	ebp, esp
	push	edi
	push	esi
	push	ebx
	sub	esp, 12
	mov	esi, DWORD  [ebp+20]
	mov	ebx, DWORD  [ebp+8]
	cmp	esi, 1
	mov	ecx, ebx
	je	L979
	cmp	esi, 2
	je	L1132
	mov	DWORD  [ebp-16], 0
	mov	eax, DWORD  [ebp+28]
	cmp	DWORD  [ebp-16], eax
	jae	L1336
align 16
L1337:
	xor	edx, edx
	cmp	edx, esi
	jge	L1324
	mov	ebx, DWORD  [ebp+24]
	mov	edi, DWORD  [ebp-16]
	add	ebx, edi
	mov	DWORD  [ebp-24], ebx
align 16
L1285:
	mov	ebx, DWORD  [ebp+12]
	mov	edi, DWORD  [ebp-16]
	mov	eax, DWORD  [ebx+edx*4]
	mov	ebx, DWORD  [ebp+16]
	inc	edx
	fld	DWORD  [ebx+edi*4]
	mov	edi, DWORD  [ebp-24]
	fmul	DWORD  [eax+edi*4]
	fadd	DWORD  [ecx]
	fstp	DWORD  [ecx]
	add	ecx, 4
	cmp	edx, esi
	jl	L1285
L1324:
	inc	DWORD  [ebp-16]
	mov	eax, DWORD  [ebp+28]
	cmp	DWORD  [ebp-16], eax
	jb	L1337
L1336:
	add	esp, 12
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
L979:
	mov	edx, DWORD  [ebp+12]
	test	bl, 15
	mov	edi, DWORD  [ebp+24]
	mov	eax, DWORD  [edx]
	lea	esi, [eax+edi*4]
	sete	al
	cmp	al, 1
	sbb	eax, eax
	and	eax, 4
	test	esi, 15
	lea	edx, [eax+2]
	sete	cl
	test	cl, cl
	cmovne	edx, eax
	lea	ecx, [edx+1]
	test	BYTE  [ebp+16], 15
	sete	al
	test	al, al
	mov	eax, edx
	cmove	eax, ecx
	cmp	DWORD  [ebp+28], 7
	ja	L1331
L989:
	mov	eax, DWORD  [ebp+28]
	cmp	DWORD  [ebp-16], eax
	jmp	L1325
L1332:
	mov	edx, DWORD  [ebp-16]
	mov	ecx, DWORD  [ebp+16]
	mov	edi, DWORD  [ebp+28]
	fld	DWORD  [ecx+edx*4]
	fmul	DWORD  [esi+edx*4]
	fadd	DWORD  [ebx+edx*4]
	fstp	DWORD  [ebx+edx*4]
	inc	edx
	cmp	edx, edi
	mov	DWORD  [ebp-16], edx
L1325:
	jb	L1332
	add	esp, 12
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
L1132:
	mov	edx, DWORD  [ebp+12]
	mov	eax, DWORD  [ebp+24]
	mov	esi, DWORD  [edx]
	sal	eax, 2
	add	esi, eax
	mov	DWORD  [ebp-20], esi
	mov	esi, eax
	mov	eax, DWORD  [edx+4]
	add	esi, eax
	test	bl, 15
	sete	al
	cmp	al, 1
	sbb	eax, eax
	and	eax, 4
	test	BYTE  [ebp-20], 15
	je	L1333
L1136:
	lea	ecx, [eax+2]
L1137:
	test	BYTE  [ebp+16], 15
	lea	edx, [ecx+1]
	sete	al
	test	al, al
	mov	eax, ecx
	cmove	eax, edx
	cmp	DWORD  [ebp+28], 3
	jbe	L1143
	cmp	eax, 7
	ja	L1143
	jmp	DWORD [L1273+eax*4]
segment_data_aligned
align 16
align 4
L1273:
	DD L1145
	DD L1161
	DD L1177
	DD L1193
	DD L1209
	DD L1225
	DD L1241
	DD L1257
align 16
segment_code
L1331:
	cmp	eax, 7
	ja	L989
	jmp	DWORD [L1127+eax*4]
segment_data_aligned
align 16
align 4
L1127:
	DD L991
	DD L1008
	DD L1025
	DD L1042
	DD L1059
	DD L1076
	DD L1093
	DD L1110
align 16
segment_code
L1333:
	test	esi, 15
	mov	ecx, eax
	je	L1137
	jmp	L1136
L1145:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 3
	cmp	edx, 0
	jbe	L1143
	xor	eax, eax
L1160:
	movaps	xmm0,   [eax+esi]
	mov	edi, DWORD  [ebp-20]
	mov	ecx, DWORD  [ebp+16]
	movaps	xmm1,   [eax+edi]
	movaps	xmm2,   [eax+ecx]
	mulps	xmm1, xmm2
	mulps	xmm0, xmm2
	movaps	xmm2, xmm1
	movlhps	xmm2, xmm0
	movhlps	xmm1, xmm0
	movaps	xmm0,   [ebx+eax*2]
	shufps	xmm2, xmm2, 216
	shufps	xmm1, xmm1, 114
	addps	xmm0, xmm2
	movaps	  [ebx+eax*2], xmm0
	movaps	xmm0,   [ebx+16+eax*2]
	addps	xmm0, xmm1
	movaps	  [ebx+16+eax*2], xmm0
	add	eax, 16
	add	DWORD  [ebp-16], 4
	cmp	edx, DWORD  [ebp-16]
	ja	L1160
L1143:
	mov	edi, DWORD  [ebp+28]
	cmp	DWORD  [ebp-16], edi
	jmp	L1326
L1334:
	mov	eax, DWORD  [ebp-16]
	mov	edx, DWORD  [ebp+16]
	mov	ecx, DWORD  [ebp-20]
	mov	edi, DWORD  [ebp+28]
	fld	DWORD  [edx+eax*4]
	fmul	DWORD  [ecx+eax*4]
	fadd	DWORD  [ebx+eax*8]
	fstp	DWORD  [ebx+eax*8]
	fld	DWORD  [edx+eax*4]
	fmul	DWORD  [esi+eax*4]
	fadd	DWORD  [ebx+4+eax*8]
	fstp	DWORD  [ebx+4+eax*8]
	inc	eax
	cmp	eax, edi
	mov	DWORD  [ebp-16], eax
L1326:
	jb	L1334
	add	esp, 12
	pop	ebx
	pop	esi
	pop	edi
	pop	ebp
	ret
L1110:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 7
	cmp	edx, 0
	jbe	L989
	xor	eax, eax
L1126:
	movups	xmm0,   [eax+esi]
	mov	edi, DWORD  [ebp+16]
	movups	xmm1,   [eax+edi]
	mulps	xmm0, xmm1
	movups	xmm1,   [eax+ebx]
	addps	xmm0, xmm1
	movups	  [eax+ebx], xmm0
	movups	xmm1,   [eax+16+edi]
	movups	xmm0,   [eax+16+esi]
	mulps	xmm0, xmm1
	movups	xmm1,   [eax+16+ebx]
	addps	xmm0, xmm1
	movups	  [eax+16+ebx], xmm0
	add	eax, 32
	add	DWORD  [ebp-16], 8
	cmp	edx, DWORD  [ebp-16]
	ja	L1126
	jmp	L989
L1093:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 7
	cmp	edx, 0
	jbe	L989
	xor	eax, eax
L1109:
	movups	xmm0,   [eax+esi]
	mov	ecx, DWORD  [ebp+16]
	movaps	xmm1,   [eax+ecx]
	mulps	xmm0, xmm1
	movups	xmm1,   [eax+ebx]
	addps	xmm0, xmm1
	movups	  [eax+ebx], xmm0
	movaps	xmm1,   [eax+16+ecx]
	movups	xmm0,   [eax+16+esi]
	mulps	xmm0, xmm1
	movups	xmm1,   [eax+16+ebx]
	addps	xmm0, xmm1
	movups	  [eax+16+ebx], xmm0
	add	eax, 32
	add	DWORD  [ebp-16], 8
	cmp	edx, DWORD  [ebp-16]
	ja	L1109
	jmp	L989
L1076:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 7
	cmp	edx, 0
	jbe	L989
	xor	eax, eax
L1092:
	movaps	xmm0,   [eax+esi]
	mov	edi, DWORD  [ebp+16]
	movups	xmm1,   [eax+edi]
	mulps	xmm0, xmm1
	movups	xmm1,   [eax+ebx]
	addps	xmm0, xmm1
	movups	  [eax+ebx], xmm0
	movups	xmm1,   [eax+16+edi]
	movaps	xmm0,   [eax+16+esi]
	mulps	xmm0, xmm1
	movups	xmm1,   [eax+16+ebx]
	addps	xmm0, xmm1
	movups	  [eax+16+ebx], xmm0
	add	eax, 32
	add	DWORD  [ebp-16], 8
	cmp	edx, DWORD  [ebp-16]
	ja	L1092
	jmp	L989
L1059:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 7
	cmp	edx, 0
	jbe	L989
	xor	eax, eax
L1075:
	movaps	xmm0,   [eax+esi]
	mov	ecx, DWORD  [ebp+16]
	movaps	xmm1,   [eax+ecx]
	mulps	xmm0, xmm1
	movups	xmm1,   [eax+ebx]
	addps	xmm0, xmm1
	movups	  [eax+ebx], xmm0
	movaps	xmm1,   [eax+16+ecx]
	movaps	xmm0,   [eax+16+esi]
	mulps	xmm0, xmm1
	movups	xmm1,   [eax+16+ebx]
	addps	xmm0, xmm1
	movups	  [eax+16+ebx], xmm0
	add	eax, 32
	add	DWORD  [ebp-16], 8
	cmp	edx, DWORD  [ebp-16]
	ja	L1075
	jmp	L989
L1042:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 7
	cmp	edx, 0
	jbe	L989
	xor	eax, eax
L1058:
	movups	xmm0,   [eax+esi]
	mov	edi, DWORD  [ebp+16]
	movups	xmm1,   [eax+edi]
	mulps	xmm0, xmm1
	movaps	xmm1,   [eax+ebx]
	addps	xmm0, xmm1
	movaps	  [eax+ebx], xmm0
	movups	xmm1,   [eax+16+edi]
	movups	xmm0,   [eax+16+esi]
	mulps	xmm0, xmm1
	movaps	xmm1,   [eax+16+ebx]
	addps	xmm0, xmm1
	movaps	  [eax+16+ebx], xmm0
	add	eax, 32
	add	DWORD  [ebp-16], 8
	cmp	edx, DWORD  [ebp-16]
	ja	L1058
	jmp	L989
L1025:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 7
	cmp	edx, 0
	jbe	L989
	xor	eax, eax
L1041:
	movups	xmm0,   [eax+esi]
	mov	ecx, DWORD  [ebp+16]
	movaps	xmm1,   [eax+ecx]
	mulps	xmm0, xmm1
	movaps	xmm1,   [eax+ebx]
	addps	xmm0, xmm1
	movaps	  [eax+ebx], xmm0
	movaps	xmm1,   [eax+16+ecx]
	movups	xmm0,   [eax+16+esi]
	mulps	xmm0, xmm1
	movaps	xmm1,   [eax+16+ebx]
	addps	xmm0, xmm1
	movaps	  [eax+16+ebx], xmm0
	add	eax, 32
	add	DWORD  [ebp-16], 8
	cmp	edx, DWORD  [ebp-16]
	ja	L1041
	jmp	L989
L1008:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 7
	cmp	edx, 0
	jbe	L989
	xor	eax, eax
L1024:
	movaps	xmm0,   [eax+esi]
	mov	edi, DWORD  [ebp+16]
	movups	xmm1,   [eax+edi]
	mulps	xmm0, xmm1
	movaps	xmm1,   [eax+ebx]
	addps	xmm0, xmm1
	movaps	  [eax+ebx], xmm0
	movups	xmm1,   [eax+16+edi]
	movaps	xmm0,   [eax+16+esi]
	mulps	xmm0, xmm1
	movaps	xmm1,   [eax+16+ebx]
	addps	xmm0, xmm1
	movaps	  [eax+16+ebx], xmm0
	add	eax, 32
	add	DWORD  [ebp-16], 8
	cmp	edx, DWORD  [ebp-16]
	ja	L1024
	jmp	L989
L991:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 7
	cmp	edx, 0
	jbe	L989
	xor	eax, eax
L1007:
	movaps	xmm0,   [eax+esi]
	mov	ecx, DWORD  [ebp+16]
	movaps	xmm1,   [eax+ecx]
	mulps	xmm0, xmm1
	movaps	xmm1,   [eax+ebx]
	addps	xmm0, xmm1
	movaps	  [eax+ebx], xmm0
	movaps	xmm1,   [eax+16+ecx]
	movaps	xmm0,   [eax+16+esi]
	mulps	xmm0, xmm1
	movaps	xmm1,   [eax+16+ebx]
	addps	xmm0, xmm1
	movaps	  [eax+16+ebx], xmm0
	add	eax, 32
	add	DWORD  [ebp-16], 8
	cmp	edx, DWORD  [ebp-16]
	ja	L1007
	jmp	L989
L1257:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 3
	cmp	edx, 0
	jbe	L1143
	xor	eax, eax
L1272:
	movups	xmm0,   [eax+esi]
	mov	edi, DWORD  [ebp-20]
	mov	ecx, DWORD  [ebp+16]
	movups	xmm1,   [eax+edi]
	movups	xmm2,   [eax+ecx]
	mulps	xmm1, xmm2
	mulps	xmm0, xmm2
	movaps	xmm2, xmm1
	movlhps	xmm2, xmm0
	movhlps	xmm1, xmm0
	movups	xmm0,   [ebx+eax*2]
	shufps	xmm2, xmm2, 216
	shufps	xmm1, xmm1, 114
	addps	xmm0, xmm2
	movups	  [ebx+eax*2], xmm0
	movups	xmm0,   [ebx+16+eax*2]
	addps	xmm0, xmm1
	movups	  [ebx+16+eax*2], xmm0
	add	eax, 16
	add	DWORD  [ebp-16], 4
	cmp	edx, DWORD  [ebp-16]
	ja	L1272
	jmp	L1143
L1241:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 3
	cmp	edx, 0
	jbe	L1143
	xor	eax, eax
L1256:
	movups	xmm0,   [eax+esi]
	mov	edi, DWORD  [ebp-20]
	mov	ecx, DWORD  [ebp+16]
	movups	xmm1,   [eax+edi]
	movaps	xmm2,   [eax+ecx]
	mulps	xmm1, xmm2
	mulps	xmm0, xmm2
	movaps	xmm2, xmm1
	movlhps	xmm2, xmm0
	movhlps	xmm1, xmm0
	movups	xmm0,   [ebx+eax*2]
	shufps	xmm2, xmm2, 216
	shufps	xmm1, xmm1, 114
	addps	xmm0, xmm2
	movups	  [ebx+eax*2], xmm0
	movups	xmm0,   [ebx+16+eax*2]
	addps	xmm0, xmm1
	movups	  [ebx+16+eax*2], xmm0
	add	eax, 16
	add	DWORD  [ebp-16], 4
	cmp	edx, DWORD  [ebp-16]
	ja	L1256
	jmp	L1143
L1225:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 3
	cmp	edx, 0
	jbe	L1143
	xor	eax, eax
L1240:
	movaps	xmm0,   [eax+esi]
	mov	edi, DWORD  [ebp-20]
	mov	ecx, DWORD  [ebp+16]
	movaps	xmm1,   [eax+edi]
	movups	xmm2,   [eax+ecx]
	mulps	xmm1, xmm2
	mulps	xmm0, xmm2
	movaps	xmm2, xmm1
	movlhps	xmm2, xmm0
	movhlps	xmm1, xmm0
	movups	xmm0,   [ebx+eax*2]
	shufps	xmm2, xmm2, 216
	shufps	xmm1, xmm1, 114
	addps	xmm0, xmm2
	movups	  [ebx+eax*2], xmm0
	movups	xmm0,   [ebx+16+eax*2]
	addps	xmm0, xmm1
	movups	  [ebx+16+eax*2], xmm0
	add	eax, 16
	add	DWORD  [ebp-16], 4
	cmp	edx, DWORD  [ebp-16]
	ja	L1240
	jmp	L1143
L1209:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 3
	cmp	edx, 0
	jbe	L1143
	xor	eax, eax
L1224:
	movaps	xmm0,   [eax+esi]
	mov	edi, DWORD  [ebp-20]
	mov	ecx, DWORD  [ebp+16]
	movaps	xmm1,   [eax+edi]
	movaps	xmm2,   [eax+ecx]
	mulps	xmm1, xmm2
	mulps	xmm0, xmm2
	movaps	xmm2, xmm1
	movlhps	xmm2, xmm0
	movhlps	xmm1, xmm0
	movups	xmm0,   [ebx+eax*2]
	shufps	xmm2, xmm2, 216
	shufps	xmm1, xmm1, 114
	addps	xmm0, xmm2
	movups	  [ebx+eax*2], xmm0
	movups	xmm0,   [ebx+16+eax*2]
	addps	xmm0, xmm1
	movups	  [ebx+16+eax*2], xmm0
	add	eax, 16
	add	DWORD  [ebp-16], 4
	cmp	edx, DWORD  [ebp-16]
	ja	L1224
	jmp	L1143
L1193:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 3
	cmp	edx, 0
	jbe	L1143
	xor	eax, eax
L1208:
	movups	xmm0,   [eax+esi]
	mov	edi, DWORD  [ebp-20]
	mov	ecx, DWORD  [ebp+16]
	movups	xmm1,   [eax+edi]
	movups	xmm2,   [eax+ecx]
	mulps	xmm1, xmm2
	mulps	xmm0, xmm2
	movaps	xmm2, xmm1
	movlhps	xmm2, xmm0
	movhlps	xmm1, xmm0
	movaps	xmm0,   [ebx+eax*2]
	shufps	xmm2, xmm2, 216
	shufps	xmm1, xmm1, 114
	addps	xmm0, xmm2
	movaps	  [ebx+eax*2], xmm0
	movaps	xmm0,   [ebx+16+eax*2]
	addps	xmm0, xmm1
	movaps	  [ebx+16+eax*2], xmm0
	add	eax, 16
	add	DWORD  [ebp-16], 4
	cmp	edx, DWORD  [ebp-16]
	ja	L1208
	jmp	L1143
L1177:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 3
	cmp	edx, 0
	jbe	L1143
	xor	eax, eax
L1192:
	movups	xmm0,   [eax+esi]
	mov	edi, DWORD  [ebp-20]
	mov	ecx, DWORD  [ebp+16]
	movups	xmm1,   [eax+edi]
	movaps	xmm2,   [eax+ecx]
	mulps	xmm1, xmm2
	mulps	xmm0, xmm2
	movaps	xmm2, xmm1
	movlhps	xmm2, xmm0
	movhlps	xmm1, xmm0
	movaps	xmm0,   [ebx+eax*2]
	shufps	xmm2, xmm2, 216
	shufps	xmm1, xmm1, 114
	addps	xmm0, xmm2
	movaps	  [ebx+eax*2], xmm0
	movaps	xmm0,   [ebx+16+eax*2]
	addps	xmm0, xmm1
	movaps	  [ebx+16+eax*2], xmm0
	add	eax, 16
	add	DWORD  [ebp-16], 4
	cmp	edx, DWORD  [ebp-16]
	ja	L1192
	jmp	L1143
L1161:
	mov	DWORD  [ebp-16], 0
	mov	edx, DWORD  [ebp+28]
	sub	edx, 3
	cmp	edx, 0
	jbe	L1143
	xor	eax, eax
L1176:
	movaps	xmm0,   [eax+esi]
	mov	edi, DWORD  [ebp-20]
	mov	ecx, DWORD  [ebp+16]
	movaps	xmm1,   [eax+edi]
	movups	xmm2,   [eax+ecx]
	mulps	xmm1, xmm2
	mulps	xmm0, xmm2
	movaps	xmm2, xmm1
	movlhps	xmm2, xmm0
	movhlps	xmm1, xmm0
	movaps	xmm0,   [ebx+eax*2]
	shufps	xmm2, xmm2, 216
	shufps	xmm1, xmm1, 114
	addps	xmm0, xmm2
	movaps	  [ebx+eax*2], xmm0
	movaps	xmm0,   [ebx+16+eax*2]
	addps	xmm0, xmm1
	movaps	  [ebx+16+eax*2], xmm0
	add	eax, 16
	add	DWORD  [ebp-16], 4
	cmp	edx, DWORD  [ebp-16]
	ja	L1176
	jmp	L1143
align 2
align 16
sse__Z31RisaInterleaveOverlappingWindowPfPKPKfS_ijj:
	push	ebp
	mov	ebp, esp
	sub	esp, 40
	and esp, -16
	mov	eax, DWORD  [ebp+28]
	mov	DWORD  [esp+20], eax
	mov	eax, DWORD  [ebp+24]
	mov	DWORD  [esp+16], eax
	mov	eax, DWORD  [ebp+20]
	mov	DWORD  [esp+12], eax
	mov	eax, DWORD  [ebp+16]
	mov	DWORD  [esp+8], eax
	mov	eax, DWORD  [ebp+12]
	mov	DWORD  [esp+4], eax
	mov	eax, DWORD  [ebp+8]
	mov	DWORD  [esp], eax
	call	sse__Z33RisaInterleaveOverlappingWindow_rPfPKPKfS_ijj
	leave
	ret
align 16
