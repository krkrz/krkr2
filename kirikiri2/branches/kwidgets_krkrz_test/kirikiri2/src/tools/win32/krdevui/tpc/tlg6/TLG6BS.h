//---------------------------------------------------------------------------
#ifndef TLG6BS_H
#define TLG6BS_H



#include <memory.h>

// TLG6.0 bitstream output implementation


class TLG6BitStream
{
	int BufferBitPos; // bit position of output buffer
	long BufferBytePos; // byte position of output buffer
	TStream * OutStream; // output stream
	unsigned char *Buffer; // output buffer
	long BufferCapacity; // output buffer capacity

public:
	TLG6BitStream(TStream * outstream) :
		OutStream(outstream),
		BufferBitPos(0),
		BufferBytePos(0),
		Buffer(NULL),
		BufferCapacity(0)
	{
	}

	~TLG6BitStream()
	{
		Flush();
	}

public:
	int GetBitPos() const { return BufferBitPos; }
	long GetBytePos() const { return BufferBytePos; }

	void Flush()
	{
		if(Buffer && (BufferBitPos || BufferBytePos))
		{
			if(BufferBitPos) BufferBytePos ++;
			OutStream->Write(Buffer, BufferBytePos);
			BufferBytePos = 0;
			BufferBitPos = 0;
		}
		free(Buffer);
		BufferCapacity = 0;
		Buffer = NULL;
	}

	long GetBitLength() const { return BufferBytePos * 8 + BufferBitPos; }

	void Put1Bit(bool b)
	{
		if(BufferBytePos == BufferCapacity)
		{
			// need more bytes
			long org_cap = BufferCapacity;
			BufferCapacity += 0x1000;
			if(Buffer)
				Buffer = (unsigned char *)realloc(Buffer, BufferCapacity);
			else
				Buffer = (unsigned char *)malloc(BufferCapacity);
			if(!Buffer) throw Exception("SaveTLG6: Insufficient memory");
			memset(Buffer + org_cap, 0, BufferCapacity - org_cap);
		}

		if(b) Buffer[BufferBytePos] |= 1 << BufferBitPos;
		BufferBitPos ++;
		if(BufferBitPos == 8)
		{
			BufferBitPos = 0;
			BufferBytePos ++;
		}
	}

	void PutGamma(int v)
	{
		// Put a gamma code.
		// v must be larger than 0.
		int t = v;
		t >>= 1;
		int cnt = 0;
		while(t)
		{
			Put1Bit(0);
			t >>= 1;
			cnt ++;
		}
		Put1Bit(1);
		while(cnt--)
		{
			Put1Bit(v&1);
			v >>= 1;
		}
	}

	void PutInterleavedGamma(int v)
	{
		// Put a gamma code, interleaved.
		// interleaved gamma codes are:
		//     1 :                   1
		//   <=3 :                 1x0
		//   <=7 :               1x0x0
		//  <=15 :             1x0x0x0
		//  <=31 :           1x0x0x0x0
		// and so on.
		// v must be larger than 0.
		
		v --;
		while(v)
		{
			v >>= 1;
			Put1Bit(0);
			Put1Bit(v&1);
		}
		Put1Bit(1);
	}

	static int GetGammaBitLengthGeneric(int v)
	{
		int needbits = 1;
		v >>= 1;
		while(v)
		{
			needbits += 2;
			v >>= 1;
		}
		return needbits;
	}

	static int GetGammaBitLength(int v)
	{
		// Get bit length where v is to be encoded as a gamma code.
		if(v<=1) return 1;    //                   1
		if(v<=3) return 3;    //                 x10
		if(v<=7) return 5;    //               xx100
		if(v<=15) return 7;   //             xxx1000
		if(v<=31) return 9;   //          x xxx10000
		if(v<=63) return 11;  //        xxx xx100000
		if(v<=127) return 13; //      xxxxx x1000000
		if(v<=255) return 15; //    xxxxxxx 10000000
		if(v<=511) return 17; //   xxxxxxx1 00000000
		return GetGammaBitLengthGeneric(v);
	}

	void PutNonzeroSigned(int v, int len)
	{
		// Put signed value into the bit pool, as length of "len".
		// v must not be zero. abs(v) must be less than 257.
		if(v > 0) v--;
		while(len --)
		{
			Put1Bit(v&1);
			v >>= 1;
		}
	}

	static int GetNonzeroSignedBitLength(int v)
	{
		// Get bit (minimum) length where v is to be encoded as a non-zero signed value.
		// v must not be zero. abs(v) must be less than 257.
		if(v == 0) return 0;
		if(v < 0) v = -v;
		if(v <= 1) return 1;
		if(v <= 2) return 2;
		if(v <= 4) return 3;
		if(v <= 8) return 4;
		if(v <= 16) return 5;
		if(v <= 32) return 6;
		if(v <= 64) return 7;
		if(v <= 128) return 8;
		if(v <= 256) return 9;
		return 10;
	}

	void PutValue(long v, int len)
	{
		// put value "v" as length of "len"
		while(len --)
		{
			Put1Bit(v&1);
			v >>= 1;
		}
	}

};


#endif
//---------------------------------------------------------------------------
