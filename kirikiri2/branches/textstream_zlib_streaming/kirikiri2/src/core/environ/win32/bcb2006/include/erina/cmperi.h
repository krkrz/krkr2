
/*****************************************************************************
                         E R I N A - L i b r a r y
                                                      最終更新 2001/05/23
 ----------------------------------------------------------------------------
         Copyright (C) 2000-2001 Leshade Entis. All rights reserved.
 *****************************************************************************/


#if	!defined(CMPERI_H_INCLUDE)
#define	CMPERI_H_INCLUDE

#include "experi.h"


/*****************************************************************************
                               ファイルオブジェクト
 *****************************************************************************/

class	EWriteFileObj
{
public:
	// 構築関数
	EWriteFileObj( void ) ;
	// 消滅関数
	virtual ~EWriteFileObj( void ) ;
	// データを書き出す
	virtual DWORD Write( const void * ptrBuf, DWORD dwByte ) = 0 ;
	// ファイルの長さを取得する
	virtual DWORD GetLength( void ) = 0 ;
	// ファイルポインタを取得する
	virtual DWORD GetPointer( void ) = 0 ;
	// ファイルポインタを移動する
	virtual void Seek( DWORD dwPointer ) = 0 ;

} ;

class	EWriteFile	: public	EWriteFileObj
{
protected:
	HANDLE	m_hFile ;
	bool	m_flagToClose ;

public:
	EWriteFile( void ) ;
	EWriteFile( HANDLE hFile ) ;
	virtual ~EWriteFile( void ) ;

	bool Open( const char * pszFile ) ;
	void Attach( HANDLE hFile ) ;
	void Close( void ) ;

	virtual DWORD Write( const void * ptrBuf, DWORD dwByte ) ;
	virtual DWORD GetLength( void ) ;
	virtual DWORD GetPointer( void ) ;
	virtual void Seek( DWORD dwPointer ) ;

} ;

class	ERIWriteFile	: public	EWriteFileObj
{
protected:
	EWriteFileObj *	m_pFile ;
	struct	RECORD_INFO
	{
		RECORD_INFO *	pParent ;
		DWORD			dwOffset ;
		DWORD			dwLimit ;
	} ;
	RECORD_INFO *	m_pRecInf ;

public:
	enum	FileID
	{
		fidImage,
		fidSound,
		fidMovie
	} ;

public:
	// 構築関数
	ERIWriteFile( void ) ;
	// 消滅関数
	virtual ~ERIWriteFile( void ) ;
	// 恵理ちゃんファイルを開く
	bool Open( EWriteFileObj * pFile, FileID fidType ) ;
	// 恵理ちゃんファイルを閉じる
	void Close( void ) ;
	// ヘッダレコードを書き出す
	bool WriteHeader
		( const ERI_FILE_HEADER & efh, const ERI_INFO_HEADER & eih ) ;
	// レコードを開く
	bool DescendRecord( const UINT64 * pRecID ) ;
	// レコードを閉じる
	void AscendRecord( void ) ;

	virtual DWORD Write( const void * ptrBuf, DWORD dwByte ) ;
	virtual DWORD GetLength( void ) ;
	virtual DWORD GetPointer( void ) ;
	virtual void Seek( DWORD dwPointer ) ;

} ;


/*****************************************************************************
                   ランレングス・ガンマ・コンテキスト
 *****************************************************************************/

class	RLHEncodeContext
{
protected:
	int		m_nIntBufCount ;	// 中間入力バッファに蓄積されているビット数
	DWORD	m_dwIntBuffer ;		// 中間入力バッファ
	ULONG	m_nBufferingSize ;	// バッファリングするバイト数
	ULONG	m_nBufCount ;		// バッファに蓄積されているバイト数
	PBYTE	m_ptrBuffer ;		// 出力バッファの先頭へのポインタ

	struct	MTF_TABLE
	{
		BYTE	iplt[0x100] ;

		inline void Initialize( void ) ;
		inline int Search( BYTE p ) ;
	} ;
	MTF_TABLE *	m_pMTFTable[0x101] ;	// MTFテーブル

	ERINA_HUFFMAN_TREE *	m_pLastHuffmanTree ;
	ERINA_HUFFMAN_TREE *	m_pHuffmanTree[0x101] ;

public:
	// 構築関数
	RLHEncodeContext( ULONG nBufferingSize ) ;
	// 消滅関数
	virtual ~RLHEncodeContext( void ) ;

	// ｎビット出力する
	int OutNBits( DWORD dwData, int n ) ;
	// ガンマコードに符号化した際のビット数を計算
	static inline int TryOutACode( int num ) ;
	// ガンマコードを出力する
	int OutACode( int num ) ;
	// バッファの内容を出力して空にする
	int Flushout( void ) ;
	// バッファの内容を出力する
	virtual ULONG WriteNextData( const BYTE * ptrBuffer, ULONG nBytes ) = 0 ;

	// ランレングスガンマ符号に符号化した際のサイズ（ビット数）を計算
	static int TryEncodeGammaCodes( const INT * ptrSrc, int nCount ) ;
	// ランレングスガンマ符号に符号化して出力する
	int EncodeGammaCodes( const INT * ptrSrc, int nCount ) ;

	// RL-MTF-G 符号の符号化の準備をする
	void PrepareToEncodeRLMTFGCode( void ) ;
	// RL-MTF-G 符号に符号化して出力する
	int EncodeRLMTFGCodes( PBYTE ptrSrc, int nCount ) ;

	// ERINA 符号の符号化の準備をする
	void PrepareToEncodeERINACode( void ) ;
	// ハフマン符号で出力する
	int OutAHuffmanCode( ERINA_HUFFMAN_TREE * tree, int num ) ;
	// 長さをハフマン符号で出力する
	int OutLengthHuffman( ERINA_HUFFMAN_TREE * tree, int length ) ;
	// ERINA 符号に符号化して出力する
	int EncodeERINACodes( const INT * ptrSrc, int nCount ) ;
	// ERINA 符号に符号化して出力する
	int EncodeERINACodesSBZL( const SBYTE * ptrSrc, int nCount ) ;
	// ERINA 符号（256色用）に符号化して出力する
	int EncodeERINACodes256( const BYTE * ptrSrc, int nCount ) ;

} ;

//
// MTF_TABLE を初期化する
//////////////////////////////////////////////////////////////////////////////
inline void RLHEncodeContext::MTF_TABLE::Initialize( void )
{
	for ( int i = 0; i < 0x100; i ++ )
		iplt[i] = (BYTE) i ;
}

//
// テーブルの中を検索して、Move To Front を実行
//////////////////////////////////////////////////////////////////////////////
inline int RLHEncodeContext::MTF_TABLE::Search( BYTE p )
{
	BYTE	lplt, cplt ;
	int		index = 0 ;
	lplt = p ;
	for ( ; ; )
	{
		cplt = iplt[index] ;
		iplt[index ++] = lplt ;
		if ( cplt == p )
			break ;
		lplt = cplt ;
	}
	return	index ;
}

//
// ガンマコードに符号化した際のビット数を計算
//////////////////////////////////////////////////////////////////////////////
inline int RLHEncodeContext::TryOutACode( int num )
{
	int	b = 0, c = 1 ;
	num -- ;
	while ( num >= c )
	{
		num -= c ;
		c <<= 1 ;
		b ++ ;
	}
	return	b * 2 + 1 ;
}


/*****************************************************************************
                               圧縮オブジェクト
 *****************************************************************************/

class	ERIEncoder
{
protected:
	ERI_INFO_HEADER		m_EriInfHdr ;			// 画像情報ヘッダ

	UINT				m_nBlockSize ;			// ブロッキングサイズ
	ULONG				m_nBlockArea ;			// ブロック面積
	ULONG				m_nBlockSamples ;		// ブロックのサンプル数
	UINT				m_nChannelCount ;		// チャネル数
	ULONG				m_nWidthBlocks ;		// 画像の幅（ブロック数）
	ULONG				m_nHeightBlocks ;		// 画像の高さ（ブロック数）

	UINT				m_nSrcBytesPerPixel ;	// 1ピクセルのバイト数

	PINT				m_ptrColumnBuf ;		// 列バッファ
	PINT				m_ptrLineBuf ;			// 行バッファ
	PINT				m_ptrBuffer1 ;			// 内部バッファ
	PINT				m_ptrBuffer2 ;
	int					m_nChannelDepth ;		// チャネルのビット深度

	PINT				m_pArrangeTable[4] ;	// サンプリングテーブル

	PBYTE				m_ptrOperations ;		// オペレーションテーブル
	PINT				m_ptrDstBuffer ;		// ディスティネーションバッファ

	ERINA_HUFFMAN_TREE *	m_pHuffmanTree ;	// ハフマン木

public:
	typedef	void (ERIEncoder::*PFUNC_COLOR_OPRATION)( void ) ;
	typedef	void (ERIEncoder::*PFUNC_SAMPLING)
		( PBYTE ptrSrc, LONG nLineBytes, int nWidth, int nHeight ) ;
	typedef	void (ERIEncoder::*PFUNC_SAMPLINGII)
		( REAL32 * ptrBuffer[], PBYTE ptrSrc,
			LONG nLineBytes, int nWidth, int nHeight ) ;
	typedef	void (ERIEncoder::*PFUNC_BLOCK_SCALING)( void ) ;

protected:
	UINT				m_nBlocksetCount ;		// 1ブロックセットのブロック数
	PFUNC_BLOCK_SCALING	m_pfnBlockScaling ;		// ブロックスケーリング関数

	REAL32 *			m_ptrBuffer3[4][4] ;	// 内部バッファ
	REAL32 *			m_ptrBuffer4[16] ;
	REAL32 *			m_ptrBuffer5[16] ;
	REAL32 *			m_ptrWorkDCT1 ;			// DCT 変換用作業領域
	REAL32 *			m_ptrWorkDCT2 ;
	INT					m_nDCDiffBuffer[16] ;	// DC 成分差分用バッファ

	PINT				m_ptrTableDC ;			// 直流成分

public:
	struct	PARAMETER
	{
		REAL32			m_rYScaleDC ;			// 輝度直流成分の量子化係数
		REAL32			m_rCScaleDC ;			// 色差直流成分の量子化係数
		REAL32			m_rYScaleLow ;			// 輝度低周波成分の量子化係数
		REAL32			m_rCScaleLow ;			// 色差低周波成分の量子化係数
		REAL32			m_rYScaleHigh ;			// 輝度高周波成分の量子化係数
		REAL32			m_rCScaleHigh ;			// 色差高周波成分の量子化係数
		int				m_nLowThreshold ;		// 輝度低周波成分の閾値
		int				m_nHighThreshold ;		// 輝度高周波成分の閾値

		PARAMETER( void ) ;
	} ;
protected:
	PARAMETER			m_parameter ;			// 圧縮パラメータ

	static PFUNC_COLOR_OPRATION	m_pfnColorOperation[0x10] ;

public:
	// 構築関数
	ERIEncoder( void ) ;
	// 消滅関数
	virtual ~ERIEncoder( void ) ;

	// 初期化（パラメータの設定）
	int Initialize( const ERI_INFO_HEADER & infhdr ) ;
	// 終了（メモリの解放など）
	void Delete( void ) ;
	// 画像を圧縮
	int EncodeImage
		( const RASTER_IMAGE_INFO & srcimginf,
			RLHEncodeContext & context, DWORD fdwFlags = 0x0021 ) ;
	// 圧縮オプションを設定
	void SetCompressionParameter( const PARAMETER & parameter ) ;

protected:
	// サンプリングテーブルの初期化
	void InitializeSamplingTable( void ) ;
	// フルカラー画像の圧縮
	int EncodeTrueColorImage
		( const RASTER_IMAGE_INFO & imginf,
			RLHEncodeContext & context, DWORD fdwFlags ) ;
	// フルカラー画像の圧縮
	int EncodeTrueColorImageII
		( const RASTER_IMAGE_INFO & imginf,
			RLHEncodeContext & context, DWORD fdwFlags ) ;
	// パレット画像の圧縮
	int EncodePaletteImage
		( const RASTER_IMAGE_INFO & imginf, RLHEncodeContext & context ) ;

	// 差分処理
	void DifferentialOperation( LONG nAllBlockLines, PINT * ptrNextLineBuf ) ;
	// オペレーションコードを取得
	DWORD DecideOperationCode
		( int nCompressMode, LONG nAllBlockLines, PINT * ptrNextLineBuf ) ;

	// チャネルのビット深度を取得する
	virtual int GetChannelDepth
		( DWORD fdwFormatType, DWORD dwBitsPerPixel ) ;
	// 指定のビット深度にデータを正規化する
	void NormalizeWithDepth( PINT ptrDst, PINT ptrSrc, int nCount ) ;
	// 画像をサンプリングする関数へのポインタを取得する
	virtual PFUNC_SAMPLING GetSamplingFunc
		( DWORD fdwFormatType, DWORD dwBitsPerPixel ) ;

	// カラーオペレーション関数群
	void ColorOperation0000( void ) ;
	void ColorOperation0101( void ) ;
	void ColorOperation0110( void ) ;
	void ColorOperation0111( void ) ;
	void ColorOperation1001( void ) ;
	void ColorOperation1010( void ) ;
	void ColorOperation1011( void ) ;
	void ColorOperation1101( void ) ;
	void ColorOperation1110( void ) ;
	void ColorOperation1111( void ) ;

	// グレイ画像のサンプリング
	void SamplingGray8
		( PBYTE ptrSrc, LONG nLineBytes, int nWidth, int nHeight ) ;
	// RGB画像(15ビット)のサンプリング
	void SamplingRGB16
		( PBYTE ptrSrc, LONG nLineBytes, int nWidth, int nHeight ) ;
	// RGB画像のサンプリング
	void SamplingRGB24
		( PBYTE ptrSrc, LONG nLineBytes, int nWidth, int nHeight ) ;
	// RGBA画像のサンプリング
	void SamplingRGBA32
		( PBYTE ptrSrc, LONG nLineBytes, int nWidth, int nHeight ) ;

protected:
	// サンプリングテーブルの初期化
	void InitializeZigZagTable( void ) ;
	// フルカラー画像の圧縮
	int EncodeTrueColorImageDCT
		( const RASTER_IMAGE_INFO & imginf,
			RLHEncodeContext & context, DWORD fdwFlags ) ;

	// 画像をサンプリングする関数へのポインタを取得する
	virtual PFUNC_SAMPLINGII GetSamplingFuncII
		( DWORD fdwFormatType, DWORD dwBitsPerPixel ) ;

	// RGB画像のサンプリング
	void SamplingRGB24II
		( REAL32 * ptrBuffer[], PBYTE ptrSrc,
			LONG nLineBytes, int nWidth, int nHeight ) ;
	// RGBA画像のサンプリング
	void SamplingRGBA32II
		( REAL32 * ptrBuffer[], PBYTE ptrSrc,
			LONG nLineBytes, int nWidth, int nHeight ) ;
	// 半端領域に指定値を設定
	void FillOddArea
		( REAL32 * ptrBuffer, int nWidth, int nHeight, REAL32 rFill ) ;

	// RGB -> YUV 変換関数
	void ConvertRGBtoYUV( void ) ;

	// 4:4:4 スケーリング
	void BlockScaling444( void ) ;
	// 4:2:2 スケーリング
	void BlockScaling422( void ) ;
	// 4:1:1 スケーリング
	void BlockScaling411( void ) ;

	// DCT 変換を施す
	void PerformDCT( void ) ;

	// 量子化を施す
	void ArrangeAndQuantumize( BYTE bytCoefficient[] ) ;

protected:
	// 展開進行状況通知関数
	virtual int OnEncodedBlock( LONG line, LONG column ) ;
	virtual int OnEncodedLine( LONG line ) ;

} ;

#define	ERI_ENCODE_INDEPENDENT_BLOCK	0x0001
#define	ERI_MAKE_COMPRESS_MODE(x)		(((x) & 0x03) << 4)
#define	ERI_GET_COMPRESS_MODE(x)		(((x) & 0x30) >> 4)



/*****************************************************************************
                     ファイルストリームコンテキスト
 *****************************************************************************/

class	EFileEncodeContext	: public	RLHEncodeContext
{
public:
	// 構築関数
	EFileEncodeContext( EWriteFileObj * pFileObj, ULONG nBufferingSize ) ;
	// 消滅関数
	virtual ~EFileEncodeContext( void ) ;

protected:
	EWriteFileObj *	m_pFileObj ;

public:
	// 次のデータを書き出す
	virtual ULONG WriteNextData( const BYTE * ptrBuffer, ULONG nBytes ) ;
	// ファイルオブジェクトを設定する
	void AttachFileObject( EWriteFileObj * pFileObj ) ;

} ;


#endif
