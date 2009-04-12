#include <windows.h>
#include <squirrel.h>
#include "tp_stub.h"

/**
 * ファイル読み込み用データ構造
 */
class SQFileInfo {

public:
	/// コンストラクタ
	SQFileInfo(const SQChar *filename) : is(NULL), buffer(NULL), size(0), readed(0) {
		is = TVPCreateIStream(filename, TJS_BS_READ);
		if (is) {
			STATSTG stat;
			is->Stat(&stat, STATFLAG_NONAME);
			size = (ULONG)stat.cbSize.QuadPart;
			buffer = new char[size];
		}
	}

	/// デストラクタ
	~SQFileInfo() {
		if (buffer) {
			delete [] buffer;
		}
		if (is) {
			is->Release();
		}
	}

	/// @return 読み込み完了したら true
	bool check() {
		if (buffer) {
			if (readed < size) {
				ULONG len;
				if (is->Read(buffer+readed,size-readed,&len) == S_OK) {
					readed += len;
				}
			}
			return readed >= size;
		} else {
			return true;
		}
	}

	/// @return バッファ
	const char *getBuffer() {
		return (const char*)buffer;
	}

	/// @return サイズ
	int getSize() {
		return (int)size;
	}

private:
	IStream *is;  ///< 入力ストリーム
	char *buffer; ///< 入力データのバッファ
	ULONG size;   ///< 読み込みサイズ
	ULONG readed; ///< 読み込み済みサイズ
};

/**
 * ファイルを非同期に開く
 * @param filename スクリプトファイル名
 * @return ファイルハンドラ
 */
void *sqobjOpenFile(const SQChar *filename)
{
	return (void*) new SQFileInfo(filename);
}

/**
 * ファイルが開かれたかどうかのチェック
 * @param handler ファイルハンドラ
 * @param dataPtr データ格納先アドレス(出力)
 * @param dataSize データサイズ(出力)
 * @return ロード完了していたら true
 */
bool sqobjCheckFile(void *handler, const char **dataAddr, int *dataSize)
{
	SQFileInfo *file = (SQFileInfo*)handler;
	if (file) {
		if (file->check()) {
			*dataAddr = file->getBuffer();
			*dataSize = file->getSize();
			return true;
		} else {
			return false;
		}
	} else {
		*dataAddr = NULL;
		*dataSize = 0;
		return true;
	}
}

/**
 * ファイルを閉じる
 * @param handler ファイルハンドラ
 */
void sqobjCloseFile(void *handler)
{
	SQFileInfo *file = (SQFileInfo*)handler;
	if (file) {
		delete file;
	}
}
