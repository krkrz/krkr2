#ifndef GolombTableH
#define GolombTableH

// Table for 'k' (predicted bit length) of golomb encoding
#define TVP_TLG6_GOLOMB_N_COUNT 4
extern char TVPTLG6GolombBitLengthTable[TVP_TLG6_GOLOMB_N_COUNT*2*128][TVP_TLG6_GOLOMB_N_COUNT];
void TVPTLG6InitGolombTable();

#endif
