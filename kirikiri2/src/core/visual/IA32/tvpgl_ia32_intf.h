/*
	this is a part of TVP (KIRIKIRI) software source.
	see other sources for license.
	(C)2001-2004 W.Dee <dee@kikyou.info>
*/

/* C-language interface to tvpgl_ia32.lib */
/* this file is always generated by summary.pl */

#ifndef __TVPGL_IA32_H__
#define __TVPGL_IA32_H__


#include "tjsTypes.h"
#include "tvpgl.h"

#ifdef _WIN32
#define TVP_GL_IA32_FUNC_DECL(rettype, funcname, arg)  rettype __cdecl funcname arg
#define TVP_GL_IA32_FUNC_EXTERN_DECL(rettype, funcname, arg)  extern rettype __cdecl funcname arg
#define TVP_GL_IA32_FUNC_PTR_DECL(rettype, funcname, arg) rettype __cdecl (*funcname) arg
#define TVP_GL_IA32_FUNC_PTR_EXTERN_DECL(rettype, funcname, arg) extern rettype __cdecl (*funcname) arg
#endif

#ifdef __cplusplus
 extern "C" {
#endif

extern void TVPGL_IA32_Init();

/*[*/
//---------------------------------------------------------------------------
// CPU Types
//---------------------------------------------------------------------------
/*]*/
/*[*/
#define TVP_CPU_HAS_FPU 0x000010000
#define TVP_CPU_HAS_MMX 0x000020000
#define TVP_CPU_HAS_3DN 0x000040000
#define TVP_CPU_HAS_SSE 0x000080000
#define TVP_CPU_HAS_CMOV 0x000100000
#define TVP_CPU_HAS_E3DN 0x000200000
#define TVP_CPU_HAS_EMMX 0x000400000
#define TVP_CPU_HAS_SSE2 0x000800000
#define TVP_CPU_HAS_TSC 0x001000000
#define TVP_CPU_FEATURE_MASK 0x0ffff0000
#define TVP_CPU_IS_INTEL 0x000000010
#define TVP_CPU_IS_AMD 0x000000020
#define TVP_CPU_IS_IDT 0x000000030
#define TVP_CPU_IS_CYRIX 0x000000040
#define TVP_CPU_IS_NEXGEN 0x000000050
#define TVP_CPU_IS_RISE 0x000000060
#define TVP_CPU_IS_UMC 0x000000070
#define TVP_CPU_IS_TRANSMETA 0x000000080
#define TVP_CPU_IS_UNKNOWN 0x000000000
#define TVP_CPU_VENDOR_MASK 0x000000ff0
#define TVP_CPU_FAMILY_MASK 0x00000000f
/*]*/
/*[*/
/*]*/
/*[*/
/*]*/
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPAdditiveAlphaBlend_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPAdditiveAlphaBlend_o_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len, tjs_int opa));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPAdditiveAlphaBlend_HDA_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPAdditiveAlphaBlend_a_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPAdditiveAlphaBlend_emmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPAdditiveAlphaBlend_o_emmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len, tjs_int opa));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPAdditiveAlphaBlend_HDA_emmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPAdditiveAlphaBlend_a_emmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len));
/*[*/
/*]*/
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPAddBlend_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPAddBlend_HDA_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPAddBlend_o_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len, tjs_int opa));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPAddBlend_HDA_o_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len, tjs_int opa));
/*[*/
/*]*/
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPAdjustGamma_a_mmx_a,  (tjs_uint32 *dest, tjs_int len, tTVPGLGammaAdjustTempData *temp));
/*[*/
/*]*/
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPAlphaBlend_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPAlphaBlend_o_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len, tjs_int opa));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPAlphaBlend_HDA_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPAlphaBlend_d_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPConstAlphaBlend_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len, tjs_int opa));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPConstAlphaBlend_SD_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src1, const tjs_uint32 *src2, tjs_int len, tjs_int opa));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPAlphaBlend_emmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPAlphaBlend_o_emmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len, tjs_int opa));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPAlphaBlend_HDA_emmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPAlphaBlend_d_emmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPConstAlphaBlend_emmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len, tjs_int opa));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPConstAlphaBlend_SD_emmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src1, const tjs_uint32 *src2, tjs_int len, tjs_int opa));
/*[*/
/*]*/
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPFillARGB_mmx_a,  (tjs_uint32 *dest, tjs_int len, tjs_uint32 value));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPFillARGB_sse_a,  (tjs_uint32 *dest, tjs_int len, tjs_uint32 value));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPFillARGB_NC_sse_a,  (tjs_uint32 *dest, tjs_int len, tjs_uint32 value));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPFillColor_mmx_a,  (tjs_uint32 *dest, tjs_int len, tjs_uint32 color));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPConstColorAlphaBlend_mmx_a,   (tjs_uint32 *dest, tjs_int len, tjs_uint32 color, tjs_int opa));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPConstColorAlphaBlend_d_mmx_a,   (tjs_uint32 *dest, tjs_int len, tjs_uint32 color, tjs_int opa));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPConstColorAlphaBlend_a_mmx_a,   (tjs_uint32 *dest, tjs_int len, tjs_uint32 color, tjs_int opa));
/*[*/
/*]*/
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPApplyColorMap65_mmx_a,  (tjs_uint32 *dest, const tjs_uint8 *src, tjs_int len, tjs_uint32 color));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPApplyColorMap65_d_mmx_a,  (tjs_uint32 *dest, const tjs_uint8 *src, tjs_int len, tjs_uint32 color));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPApplyColorMap65_a_mmx_a,  (tjs_uint32 *dest, const tjs_uint8 *src, tjs_int len, tjs_uint32 color));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPApplyColorMap65_emmx_a,  (tjs_uint32 *dest, const tjs_uint8 *src, tjs_int len, tjs_uint32 color));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPApplyColorMap65_d_emmx_a,  (tjs_uint32 *dest, const tjs_uint8 *src, tjs_int len, tjs_uint32 color));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPApplyColorMap65_a_emmx_a,  (tjs_uint32 *dest, const tjs_uint8 *src, tjs_int len, tjs_uint32 color));
/*[*/
/*]*/
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPDarkenBlend_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPDarkenBlend_HDA_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len));
 extern tjs_uint32 TVPCPUFeatures;
 extern tjs_uint32 TVPCPUID1_EAX;
 extern tjs_uint32 TVPCPUID1_EBX;
 extern tjs_nchar TVPCPUVendor[16];
 extern tjs_nchar TVPCPUName[52];
/*[*/
/*]*/
TVP_GL_IA32_FUNC_EXTERN_DECL( tjs_uint32, TVPCheckCPU, ());
TVP_GL_IA32_FUNC_EXTERN_DECL( tjs_uint64, TVPGetTSC, ());
/*[*/
/*]*/
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPLightenBlend_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPLightenBlend_HDA_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len));
/*[*/
/*]*/
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPMakeAlphaFromKey_cmovcc_a,  (tjs_uint32 *dest, tjs_int len, tjs_uint32 key));
/*[*/
/*]*/
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPMulBlend_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPMulBlend_HDA_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPMulBlend_o_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len, tjs_int opa));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPMulBlend_HDA_o_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len, tjs_int opa));
/*[*/
/*]*/
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPConvert24BitTo32Bit_mmx_a,  (tjs_uint32 *dest, const tjs_uint8 *buf, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPBLConvert24BitTo32Bit_mmx_a,  (tjs_uint32 *dest, const tjs_uint8 *buf, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPDither32BitTo16Bit565_mmx_a,  (tjs_uint16 *dest, const tjs_uint32 *src, tjs_int len, tjs_int xofs, tjs_int yofs));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPDither32BitTo16Bit555_mmx_a,  (tjs_uint16 *dest, const tjs_uint32 *src, tjs_int len, tjs_int xofs, tjs_int yofs));
/*[*/
/*]*/
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPScreenBlend_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPScreenBlend_HDA_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPScreenBlend_o_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len, tjs_int opa));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPScreenBlend_HDA_o_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len, tjs_int opa));
/*[*/
/*]*/
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPSubBlend_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPSubBlend_HDA_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPSubBlend_o_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len, tjs_int opa));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPSubBlend_HDA_o_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src, tjs_int len, tjs_int opa));
/*[*/
/*]*/
TVP_GL_IA32_FUNC_EXTERN_DECL(tjs_int,  TVPTLG5DecompressSlide_a,  (tjs_uint8 *out, const tjs_uint8 *in, tjs_int insize, tjs_uint8 *text, tjs_int initialr));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPTLG5ComposeColors3To4_mmx_a,  (tjs_uint8 *outp, const tjs_uint8 *upper, tjs_uint8 * const * buf, tjs_int width));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPTLG5ComposeColors4To4_mmx_a,  (tjs_uint8 *outp, const tjs_uint8 *upper, tjs_uint8 * const * buf, tjs_int width));
/*[*/
/*]*/
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPTLG6DecodeLine_mmx_a,  (tjs_uint32 *prevline, tjs_uint32 *curline, tjs_int width, tjs_int block_count, tjs_uint8 *filtertypes, tjs_int skipblockbytes, tjs_uint32 *input, tjs_uint32 initialp, tjs_int oddskip, tjs_int dir));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPTLG6DecodeLine_sse_a,  (tjs_uint32 *prevline, tjs_uint32 *curline, tjs_int width, tjs_int block_count, tjs_uint8 *filtertypes, tjs_int skipblockbytes, tjs_uint32 *input, tjs_uint32 initialp, tjs_int oddskip, tjs_int dir));
/*[*/
/*]*/
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPTLG6DecodeGolombValuesForFirst_a,  (tjs_int8 *pixelbuf, tjs_int pixel_count, tjs_uint8 *bit_pool));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPTLG6DecodeGolombValues_a,  (tjs_int8 *pixelbuf, tjs_int pixel_count, tjs_uint8 *bit_pool));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPTLG6DecodeGolombValuesForFirst_mmx_a,  (tjs_int8 *pixelbuf, tjs_int pixel_count, tjs_uint8 *bit_pool));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPTLG6DecodeGolombValues_mmx_a,  (tjs_int8 *pixelbuf, tjs_int pixel_count, tjs_uint8 *bit_pool));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPTLG6DecodeGolombValuesForFirst_emmx_a,  (tjs_int8 *pixelbuf, tjs_int pixel_count, tjs_uint8 *bit_pool));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPTLG6DecodeGolombValues_emmx_a,  (tjs_int8 *pixelbuf, tjs_int pixel_count, tjs_uint8 *bit_pool));
/*[*/
/*]*/
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPStretchCopy_mmx_a,  (tjs_uint32 *dest, tjs_int len, const tjs_uint32 *src, tjs_int srcstart, tjs_int srcstep));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPStretchConstAlphaBlend_mmx_a,  (tjs_uint32 *dest, tjs_int len, const tjs_uint32 *src, tjs_int srcstart, tjs_int srcstep, tjs_int opa));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPStretchAlphaBlend_mmx_a,  (tjs_uint32 *dest, tjs_int len, const tjs_uint32 *src, tjs_int srcstart, tjs_int srcstep));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPStretchAdditiveAlphaBlend_mmx_a,  (tjs_uint32 *dest, tjs_int len, const tjs_uint32 *src, tjs_int srcstart, tjs_int srcstep));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPFastLinearInterpV2_mmx_a,  (tjs_uint32 *dest, tjs_int len, const tjs_uint32 *src0, const tjs_uint32 *src1));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPFastLinearInterpH2F_mmx_a,  (tjs_uint32 *dest, tjs_int destlen, const tjs_uint32 *src));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPFastLinearInterpH2B_mmx_a,  (tjs_uint32 *dest, tjs_int destlen, const tjs_uint32 *src));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPLinTransCopy_mmx_a,  (tjs_uint32 *dest, tjs_int len, const tjs_uint32 *src,  tjs_int sx, tjs_int sy, tjs_int stepx, tjs_int stepy, tjs_int srcpitch));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPLinTransConstAlphaBlend_mmx_a,  (tjs_uint32 *dest, tjs_int len, const tjs_uint32 *src,  tjs_int sx, tjs_int sy, tjs_int stepx, tjs_int stepy, tjs_int srcpitch, tjs_int opa));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPInterpStretchCopy_mmx_a,  (tjs_uint32 *dest, tjs_int destlen, const tjs_uint32 *src1, const tjs_uint32 *src2, tjs_int blend_y, tjs_int srcstart, tjs_int srcstep));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPInterpStretchConstAlphaBlend_mmx_a,  (tjs_uint32 *dest, tjs_int destlen, const tjs_uint32 *src1, const tjs_uint32 *src2, tjs_int blend_y, tjs_int srcstart, tjs_int srcstep, tjs_int opa));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPInterpStretchAdditiveAlphaBlend_mmx_a,  (tjs_uint32 *dest, tjs_int destlen, const tjs_uint32 *src1, const tjs_uint32 *src2, tjs_int blend_y, tjs_int srcstart, tjs_int srcstep));
/*[*/
/*]*/
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPInitUnivTransBlendTable_mmx_c,  (tjs_uint32 *table, tjs_int phase, tjs_int vague));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPUnivTransBlend_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src1, const tjs_uint32 *src2, const tjs_uint8 *rule, const tjs_uint32 *table, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPUnivTransBlend_emmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src1, const tjs_uint32 *src2, const tjs_uint8 *rule, const tjs_uint32 *table, tjs_int len));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPUnivTransBlend_switch_mmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src1, const tjs_uint32 *src2, const tjs_uint8 *rule, const tjs_uint32 *table, tjs_int len, tjs_int src1lv, tjs_int src2lv));
TVP_GL_IA32_FUNC_EXTERN_DECL(void,  TVPUnivTransBlend_switch_emmx_a,  (tjs_uint32 *dest, const tjs_uint32 *src1, const tjs_uint32 *src2, const tjs_uint8 *rule, const tjs_uint32 *table, tjs_int len, tjs_int src1lv, tjs_int src2lv));

#ifdef __cplusplus
 }
#endif


#endif
