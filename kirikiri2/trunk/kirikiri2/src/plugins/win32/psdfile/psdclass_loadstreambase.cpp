#ifndef LOAD_MEMORY

#include "psdclass.h"

#define PSD_READ_BUFFERSIZE (16*1024)

void
PSD::clearStream()
{
	if (pStream) {
		pStream->Release();
		pStream = 0;
	}
	mStreamSize = 0;
	mBufferPos = -1;
	mBufferSize = 0;
	delete[] mBuffer;
	mBuffer = 0;
}


unsigned char &
PSD::getStreamValue(const tTVInteger &pos)
{
	static unsigned char eof = 0;
	if (pos >=0 && pos < mStreamSize) {
		if (mBuffer) {
			if (pos >= mBufferPos && pos < mBufferPos + mBufferSize) {
				return mBuffer[pos - mBufferPos];
			}
		} else {
			mBuffer = new unsigned char[PSD_READ_BUFFERSIZE];
		}
		mBufferPos = pos;
		LARGE_INTEGER n;
		n.QuadPart = pos;
		pStream->Seek(n, STREAM_SEEK_SET, 0);
		pStream->Read(mBuffer, PSD_READ_BUFFERSIZE, &mBufferSize);
		return mBuffer[0];
	}
	return eof;
}

void
PSD::copyToBuffer(char *buf, tTVInteger pos, int size)
{
	if (pos >=0 && pos < mStreamSize) {
		if (size < 32) {
			if (mBuffer) {
				if (pos >= mBufferPos && pos+size < mBufferPos + mBufferSize) {
					memcpy(buf, &mBuffer[pos - mBufferPos], size);
					return;
				}
			} else {
				mBuffer = new unsigned char[PSD_READ_BUFFERSIZE];
			}
			mBufferPos = pos;
			LARGE_INTEGER n;
			n.QuadPart = pos;
			pStream->Seek(n, STREAM_SEEK_SET, 0);
			pStream->Read(mBuffer, PSD_READ_BUFFERSIZE, &mBufferSize);
			memcpy(buf, mBuffer, size);
			return;
		}
		// ’¼Ú“Ç‚Ýž‚Þ
		LARGE_INTEGER n;
		n.QuadPart = pos;
		pStream->Seek(n, STREAM_SEEK_SET, 0);
		ULONG rsize;
		pStream->Read(buf, size, &rsize);
	}
}

#endif
