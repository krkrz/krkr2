#include <windows.h>

#include <string.h>
#include "tvpext.h"


#pragma pack(push,1)

struct TTCWFHeader
{
	char mark[6];  // = "TCWF0\x1a"
	BYTE channels; // チャネル数
	BYTE reserved;
	LONG frequency; // サンプリング周波数
	LONG numblocks; // ブロック数
	LONG bytesperblock; // ブロックごとのバイト数
	LONG samplesperblock; // ブロックごとのサンプル数
};
struct TTCWUnexpectedPeak
{
	unsigned short int pos;
	short int revise;
};
struct TTCWBlockHeader  // ブロックヘッダ ( ステレオの場合はブロックが右・左の順に２つ続く)
{
	short int ms_sample0;
	short int ms_sample1;
	short int ms_idelta;
	byte ms_bpred;
	byte ima_stepindex;
	TTCWUnexpectedPeak peaks[6];
};
#pragma pack(pop)


static int AdaptationTable[]= 
{
	230, 230, 230, 230, 307, 409, 512, 614,
	768, 614, 512, 409, 307, 230, 230, 230 
};

static int AdaptCoeff1 [] = 
{	256, 512, 0, 192, 240, 460, 392 
} ;

static int AdaptCoeff2 [] = 
{	0, -256, 0, 64, 0, -208, -232
} ;

static int ima_index_adjust [16] =
{	-1, -1, -1, -1,		// +0 - +3, decrease the step size
	 2,  4,  6,  8,     // +4 - +7, increase the step size
	-1, -1, -1, -1,		// -0 - -3, decrease the step size
	 2,  4,  6,  8,		// -4 - -7, increase the step size
} ;

static int ima_step_size [89] = 
{	7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 
	50, 55, 60, 66, 73, 80, 88, 97, 107, 118, 130, 143, 157, 173, 190, 209, 230, 
	253, 279, 307, 337, 371, 408, 449, 494, 544, 598, 658, 724, 796, 876, 963, 
	1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024, 3327, 
	3660, 4026, 4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630, 9493, 10442,
	11487, 12635, 13899, 15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 
	32767
} ;

//---------------------------------------------------------------------------
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*)
{
	return 1;
}
//---------------------------------------------------------------------------
extern "C" HRESULT __stdcall Link(ITVP *tvp)
{
	return S_OK;
}
//---------------------------------------------------------------------------
extern "C" HRESULT __stdcall Unlink()
{
	return S_OK;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class TCWFUnpacker : public IWaveUnpacker
{
	ULONG RefCount;
	IStream *Storage;
	TTCWFHeader Header; // ヘッダ情報
	long DataStart; // データの開始位置
	long DataSize; // データサイズ
	long Pos; // 現在位置(サンプル数)
	long StreamPos; // Storage 上での位置
	long BufferRemain; // バッファの残りバイト数
	short int *SamplePos;
	BYTE *BlockBuffer; // ブロックバッファ
	short int *Samples;

public:
	// IUnknown 派生クラスではないので注意
	ULONG STDMETHODCALLTYPE AddRef(void)
	{
		RefCount++;
		return RefCount;
	}
	ULONG STDMETHODCALLTYPE Release(void)
	{
		ULONG c=RefCount--;
		if(!c) delete this;
		return c;
	}

// IWaveUnpacker
	HRESULT STDMETHODCALLTYPE GetTypeName(char *buf,long buflen)
	{
		if(buflen<16) return E_FAIL;
		strcpy(buf,"[*.wav] msadpcm");
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetWaveFormat(long *samplepersec,
		long *channels,long *bitspersample)
	{
		if(samplepersec) *samplepersec= Header.frequency;
		if(channels) *channels = Header.channels;
		if(bitspersample) *bitspersample=16;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE Render(void *buffer,long bufsize,
		long *numwrite); // 外部で定義
	
	HRESULT STDMETHODCALLTYPE GetLength(long *length)
	{
		if(!length) return E_FAIL;
		*length=(long) ( 1000.0*(double)Header.numblocks / (double)Header.channels *
			((double)Header.samplesperblock / (double)Header.frequency) );
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetCurrentPosition(long *pos)
	{
		if(!pos) return E_FAIL;
		*pos=(long) ( (double)Pos*1000.0 / (double)Header.frequency );
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SetCurrentPosition(long pos);
		// 外部で定義

	HRESULT STDMETHODCALLTYPE Invoke(wchar_t* name,DWORD flag,
		TTJSVariant_S &Result,TTJSVariant_S **ValArray,long NumVal,
		ITJSDispatch *ObjThis)
	{
		return E_NOTIMPL;
	}

//
	TCWFUnpacker(void)
	{
		RefCount=1;
		BlockBuffer=NULL;
		Samples=NULL;
	}

	~TCWFUnpacker(void)
	{
		if(BlockBuffer) delete [] BlockBuffer;
		if(Samples) delete [] Samples;
	}

	bool Open(IStream *storage); // 外部で定義
	bool ReadBlock(int, int); // 外部で定義
};
//---------------------------------------------------------------------------
bool TCWFUnpacker::Open(IStream *storage)
{
	ULONG read;
	LARGE_INTEGER p;
	p.QuadPart=0;
	storage->Seek(p,0,NULL);

	LARGE_INTEGER newpos;
	newpos.QuadPart=0;
	ULARGE_INTEGER result;
	storage->Seek(newpos,0,&result);
	if(result.QuadPart != (unsigned __int64) newpos.QuadPart) return false;

	// TCWF0 チェック
	storage->Read(&Header, sizeof(Header), &read);
	if(read!=sizeof(Header)) return false;
	if(memcmp(Header.mark,"TCWF0\x1a", 6)) return false; // マーク

	// 現在位置を取得
	newpos.QuadPart=0;
	storage->Seek(newpos,1,&result);
	StreamPos=(long)result.QuadPart;
	DataStart=StreamPos;

	// その他、初期化
	BufferRemain=0;
	Storage=storage;
	Pos=0;

	return true;
}
//---------------------------------------------------------------------------
bool TCWFUnpacker::ReadBlock(int numchans, int chan)
{
	// ブロックを読み込む

	// メモリ確保
	if(!BlockBuffer)
		BlockBuffer=new BYTE[Header.bytesperblock];
	if(!this->Samples)
		this->Samples=new short int[Header.samplesperblock * Header.channels];

	short int * Samples = this->Samples + chan;


	// シーク
	LARGE_INTEGER newpos;
	ULARGE_INTEGER result;
	newpos.QuadPart=0;
	Storage->Seek(newpos,1,&result);
	if((long)result.QuadPart!=StreamPos)
	{
		newpos.QuadPart=StreamPos;
		Storage->Seek(newpos,0,&result);
		if(result.QuadPart != (unsigned __int64) newpos.QuadPart) return false;
	}


	// ブロックヘッダ読み込み
	TTCWBlockHeader bheader;
	ULONG read;
	Storage->Read(&bheader,sizeof(bheader),&read);
	StreamPos+=read;
	if(sizeof(bheader)!=read) return false;
	Storage->Read(BlockBuffer, Header.bytesperblock - sizeof(bheader) , &read);
	StreamPos+=read;
	if((ULONG)Header.bytesperblock- sizeof(bheader)!=read) return false;

	// デコード
	Samples[0*numchans] = bheader.ms_sample0;
	Samples[1*numchans] = bheader.ms_sample1;
	int idelta = bheader.ms_idelta;
	int bpred = bheader.ms_bpred;
	if(bpred>=7) return false; // おそらく同期がとれていない

	int k;
	int p;

	//MS ADPCM デコード
	int predict;
	int bytecode;
	for (k = 2, p = 0 ; k < Header.samplesperblock ; k ++, p++)
	{
		bytecode = BlockBuffer[p] & 0xF ;

	    int idelta_save=idelta;
		idelta = (AdaptationTable [bytecode] * idelta) >> 8 ;
	    if (idelta < 16) idelta = 16;
	    if (bytecode & 0x8) bytecode -= 0x10 ;
	
    	predict = ((Samples [(k - 1)*numchans] * AdaptCoeff1 [bpred]) 
					+ (Samples [(k - 2)*numchans] * AdaptCoeff2 [bpred])) >> 8 ;
 
		int current = (bytecode * idelta_save) + predict;
    
	    if (current > 32767) 
			current = 32767 ;
	    else if (current < -32768) 
			current = -32768 ;
    
		Samples [k*numchans] = (short int) current ;
	};

	//IMA ADPCM デコード
	int step;
	int stepindex = bheader.ima_stepindex;
	int prev = 0;
	int diff;
	for (k = 2, p = 0 ; k < Header.samplesperblock ; k ++, p++)
	{
		bytecode= (BlockBuffer[p]>>4) & 0xF;
		
		step = ima_step_size [stepindex] ;
		int current = prev;
  

		diff = step >> 3 ;
		if (bytecode & 1) 
			diff += step >> 2 ;
		if (bytecode & 2) 
			diff += step >> 1 ;
		if (bytecode & 4) 
			diff += step ;
		if (bytecode & 8) 
			diff = -diff ;

		current += diff ;

		if (current > 32767) current = 32767;
		else if (current < -32768) current = -32768 ;

		stepindex+= ima_index_adjust [bytecode] ;
	
		if (stepindex< 0)  stepindex = 0 ;
		else if (stepindex > 88)	stepindex = 88 ;

		prev = current ;

		int n = Samples[k*numchans];
		n+=current;
		if (n > 32767) n = 32767;
		else if (n < -32768) n = -32768 ;
		Samples[k*numchans] =n;
	};

	// unexpected peak の修正
	int i;
	for(i=0; i<6; i++)
	{
		if(bheader.peaks[i].revise)
		{
			int pos = bheader.peaks[i].pos;
			int n = Samples[pos*numchans];
			n -= bheader.peaks[i].revise;
			if (n > 32767) n = 32767;
			else if (n < -32768) n = -32768 ;
			Samples[pos*numchans] = n;
		}
	}

	return true;
}
//---------------------------------------------------------------------------
HRESULT TCWFUnpacker::SetCurrentPosition(long pos)
{
	// pos (ms単位) に移動する
	// 現在位置を保存
	LARGE_INTEGER newpos;
	ULARGE_INTEGER result;
	newpos.QuadPart=0;
	Storage->Seek(newpos,1,&result);

	long bytepossave=(long)newpos.QuadPart;
	long samplepossave=Pos;

	// 新しい位置を特定
	long newbytepos= (long)( (double)Header.bytesperblock*(double)pos/1000.0/
		((double)Header.samplesperblock/
		(double)Header.channels/(double)Header.frequency));
	newbytepos/=Header.bytesperblock*Header.channels;
	Pos=newbytepos*(Header.samplesperblock/Header.channels);
	newbytepos*=Header.bytesperblock*Header.channels;

	// シーク
	newpos.QuadPart=DataStart+newbytepos;
	Storage->Seek(newpos,0,&result);
	if(result.QuadPart != (unsigned __int64) newpos.QuadPart)
	{
		// シーク失敗
		newpos.QuadPart=bytepossave;
		Storage->Seek(newpos,0,&result);
		Pos=samplepossave;
		return E_FAIL;
	}
	
	StreamPos=DataStart+newbytepos;

	return S_OK;
}
//---------------------------------------------------------------------------
HRESULT TCWFUnpacker::Render(void*buffer,long bufsize,long *numwrite)
{
	// 展開
	int n;
	short int *pbuf=(short int*)buffer;
	for(n=0;n<bufsize;n+=2)
	{
		if(BufferRemain<=0)
		{
			int i;
			for(i=0; i<Header.channels; i++)
			{
				if(!ReadBlock(Header.channels, i))
				{
					if(numwrite)
						*numwrite=n;		
					ZeroMemory((BYTE*)buffer+n,bufsize-n);
					Pos+=n/Header.channels/2;
					return S_OK;
				}
			}
			SamplePos = Samples;
			BufferRemain = Header.samplesperblock * Header.channels *2;
		}
		*(pbuf++)=*(SamplePos++);
		BufferRemain-=2;
	}

	if(numwrite) *numwrite=n;
	Pos+=n/Header.channels/2;
	return S_OK;
}
//---------------------------------------------------------------------------
extern "C" HRESULT __stdcall
	CreateWaveUnpacker(IStream *storage,
		long size,
		char *name,
		IWaveUnpacker **out)
{
	TCWFUnpacker *unp=new TCWFUnpacker();
	if(!unp->Open(storage))
	{
		delete unp;
		return E_FAIL;
	}

	*out=unp;
	return S_OK;
}
//---------------------------------------------------------------------------
