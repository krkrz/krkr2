#include <windows.h>
#include "tp_stub.h"
#include <string>
#include <map>
#include <set>

#include <steam_api.h>

extern std::string convertTtstrToUtf8String(ttstr &buf);
extern ttstr convertUtf8StringToTtstr(const std::string &buf);

#define BASENAME L"steam"

/**
 * Steam ファイル情報保持用クラス
 * 参照がなくなった時点と Commit のタイミングで書き戻しを行う
 */
class SteamFile {
public:
	typedef std::map<std::string, SteamFile*> FileMap;

	// ファイルの存在確認
	static bool exist(const std::string &name) {
		FileMap::iterator it = fileMap.find(name);
		if (it != fileMap.end()) {
			return true;
		}
		ISteamRemoteStorage *storage = SteamRemoteStorage();
		if (storage) {
			return storage->FileExists(name.c_str());
		}
		return false;
	}

	// ファイル一覧取得
	static void getList(std::set<std::string> &names) {
		// アクセス中ファイル名
		FileMap::const_iterator it = fileMap.begin();
		while (it != fileMap.end()) {
			names.insert(it->first);
			it++;
		}
		// ストレージにあるファイル名一覧
		ISteamRemoteStorage *storage = SteamRemoteStorage();
		if (storage) {
			int count = storage->GetFileCount();
			for (int i=0;i<count;i++) {
				std::string n = storage->GetFileNameAndSize(i, 0);
				names.insert(n);
			}
		}
	}
	
	// ファイルを取得
	static SteamFile *getInstance(const std::string &name) {
		SteamFile *ret = 0;
		FileMap::iterator it = fileMap.find(name);
		if (it != fileMap.end()) {
			ret = it->second;
			ret->AddRef();
		} else {
			ret = new SteamFile(name.c_str());
		}
		return ret;
	}
	
	int AddRef(void) {
		refCount++;
		return refCount;
	}
	
	int Release(void) {
		int ret = --refCount;
		if (ret <= 0) {
			delete this;
			ret = 0;
		}
		return ret;
	}

	// IStream の取得
	IStream *getStream() {
		IStream *ret = 0;
		if (pStream) {
			pStream->Clone(&ret);
		}
		return ret;
	}

	// 更新通知
	void write() {
		writeFlag = true;
	}
	
	// コミット通知
	void commit() {
		if (hBuffer) {
			DWORD size = ::GlobalSize(hBuffer);
			if (initSize != size || writeFlag) {
				ISteamRemoteStorage *storage = SteamRemoteStorage();
				if (storage) {
					unsigned char* pBuffer = (unsigned char*)::GlobalLock(hBuffer);
					if (pBuffer) {
						storage->FileWrite(name.c_str(), pBuffer, size);
						::GlobalUnlock(hBuffer);
					}
				}
				initSize = size;
				writeFlag = false;
			}
		}
	}
	
protected:

	/**
	 * コンストラクタ
	 * @param name 名前
	 */
	SteamFile(const char *filename) : name(filename), refCount(1), hBuffer(0), pStream(0), initSize(0), writeFlag(false) {
		fileMap[name] = this;
		ISteamRemoteStorage *storage = SteamRemoteStorage();
		if (storage) {
			if (storage->FileExists(name.c_str())) {
				initSize = storage->GetFileSize(name.c_str());
				hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, initSize);
				unsigned char* pBuffer = (unsigned char*)::GlobalLock(hBuffer);
				if (pBuffer) {
					storage->FileRead(name.c_str(), pBuffer, initSize);
					::GlobalUnlock(hBuffer);
				}
			} else {
				hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, 0);
			}
			::CreateStreamOnHGlobal(hBuffer, FALSE, &pStream);
		}
	}

	/**
	 * デストラクタ
	 */
	~SteamFile() {
		if (pStream) {
			pStream->Release();
			pStream = 0;
		}
		commit();
		if (hBuffer) {
			::GlobalFree(hBuffer);
			hBuffer = 0;
		}
		fileMap.erase(name);
	}

	static FileMap fileMap;
	
	std::string name;
	int refCount;
	HGLOBAL hBuffer;
	IStream *pStream;
	DWORD initSize;
	bool writeFlag;
};

SteamFile::FileMap SteamFile::fileMap;

/**
 * Steam ファイル用の IStream
 * 更新の通知や終了を拾うために IStream をラッピングしてある
 */
class SteamStream : public IStream {

public:

	static SteamStream *create(const std::string &name) {
		SteamFile *file = SteamFile::getInstance(name);
		SteamStream *ret = new SteamStream(file);
		file->Release();
		return ret;
	}

	// IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) {
		if (riid == IID_IUnknown || riid == IID_ISequentialStream || riid == IID_IStream) {
			if (ppvObject == NULL)
				return E_POINTER;
			*ppvObject = this;
			AddRef();
			return S_OK;
		} else {
			*ppvObject = 0;
			return E_NOINTERFACE;
		}
	}

	ULONG STDMETHODCALLTYPE AddRef(void) {
		refCount++;
		return refCount;
	}
	
	ULONG STDMETHODCALLTYPE Release(void) {
		int ret = --refCount;
		if (ret <= 0) {
			delete this;
			ret = 0;
		}
		return ret;
	}

	// ISequentialStream
	HRESULT STDMETHODCALLTYPE Read(void *pv, ULONG cb, ULONG *pcbRead) {
		if (pStream) {
			return pStream->Read(pv, cb, pcbRead);
		}
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Write(const void *pv, ULONG cb, ULONG *pcbWritten) {
		if (pStream) {
			if (file) {
				file->write();
			}
			return pStream->Write(pv, cb, pcbWritten);
		}
		return E_NOTIMPL;
	}

	// IStream
	HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove,	DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition) {
		if (pStream) {
			return pStream->Seek(dlibMove, dwOrigin, plibNewPosition);
		}
		return E_NOTIMPL;
	}
	
	HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize) {
		if (pStream) {
			return pStream->SetSize(libNewSize);
		}
		return E_NOTIMPL;
	}
	
	HRESULT STDMETHODCALLTYPE CopyTo(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten) {
		if (pStream) {
			return pStream->CopyTo(pstm, cb, pcbRead, pcbWritten);
		}
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags) {
		if (pStream) {
			HRESULT ret = pStream->Commit(grfCommitFlags);
			if (file) {
				file->commit();
			}
			return ret;
		}
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Revert(void) {
		if (pStream) {
			HRESULT ret = pStream->Revert();
			if (file) {
				file->write();
				file->commit();
			}
			return ret;
		}
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) {
		if (pStream) {
			return pStream->LockRegion(libOffset, cb, dwLockType);
		}
		return E_NOTIMPL;
	}
	
	HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) {
		if (pStream) {
			return pStream->UnlockRegion(libOffset, cb, dwLockType);
		}
		return E_NOTIMPL;
	}
	
	HRESULT STDMETHODCALLTYPE Stat(STATSTG *pstatstg, DWORD grfStatFlag) {
		if (pStream) {
			return pStream->Stat(pstatstg, grfStatFlag);
		}
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Clone(IStream **ppstm) {
		*ppstm = (IStream*)new SteamStream(file);
		return S_OK;
	}

protected:

	SteamStream(SteamFile *file) : file(file), refCount(1), pStream(0) {
		if (file) {
			file->AddRef();
			pStream = file->getStream();
		}
	}
	
	~SteamStream() {
		if (pStream) {
			pStream->Release();
		}
		if (file) {
			file->commit();
			file->Release();
		}
	}

	SteamFile *file;
	int refCount;
	IStream *pStream;
};


/**
 * Steam ストレージ
 */
class SteamStorage : public iTVPStorageMedia
{
public:
	// -----------------------------------
	// iTVPStorageMedia Intefaces
	// -----------------------------------

	virtual void TJS_INTF_METHOD AddRef() {
		refCount++;
	};

	virtual void TJS_INTF_METHOD Release() {
		if (refCount == 1) {
			delete this;
		} else {
			refCount--;
		}
	};

	// returns media name like "file", "http" etc.
	virtual void TJS_INTF_METHOD GetName(ttstr &name) {
		name = BASENAME;
	}

	//	virtual ttstr TJS_INTF_METHOD IsCaseSensitive() = 0;
	// returns whether this media is case sensitive or not

	// normalize domain name according with the media's rule
	virtual void TJS_INTF_METHOD NormalizeDomainName(ttstr &name) {
		name.ToLowerCase();
	}

	// normalize path name according with the media's rule
	virtual void TJS_INTF_METHOD NormalizePathName(ttstr &name) {
		name.ToLowerCase();
	}

	// check file existence
	virtual bool TJS_INTF_METHOD CheckExistentStorage(const ttstr &name) {
		ttstr n = name;
		n.ToLowerCase();
		std::string filename = convertTtstrToUtf8String(n);
		return SteamFile::exist(filename);
	}

	// open a storage and return a tTJSBinaryStream instance.
	// name does not contain in-archive storage name but
	// is normalized.
	virtual tTJSBinaryStream * TJS_INTF_METHOD Open(const ttstr & name, tjs_uint32 flags) {
		ttstr n = name;
		n.ToLowerCase();
		std::string filename = convertTtstrToUtf8String(n);
		if (flags != TJS_BS_READ || SteamFile::exist(filename)) {
			SteamStream *stream = SteamStream::create(filename);
			if (stream) {
				if (flags == TJS_BS_WRITE) {
					ULARGE_INTEGER size = {0,0};
					stream->SetSize(size);
				} else if (flags == TJS_BS_APPEND) {
					LARGE_INTEGER pos = {0,0};
					stream->Seek(pos, STREAM_SEEK_END, 0);
				}
				tTJSBinaryStream *ret = TVPCreateBinaryStreamAdapter(stream);
				stream->Release();
				return ret;
			}
		}
		TVPThrowExceptionMessage(TJS_W("%1:cannot open steamfile"), name);
		return NULL;
	}

	// list files at given place
	virtual void TJS_INTF_METHOD GetListAt(const ttstr &name, iTVPStorageLister * lister) {
		if (name == "") {
			std::set<std::string> names;
			SteamFile::getList(names);
			std::set<std::string>::const_iterator it = names.begin();
			while (it != names.end()) {
				ttstr name = convertUtf8StringToTtstr(*it);
				lister->Add(name);
				it++;
			}
		}
	}

	// basically the same as above,
	// check wether given name is easily accessible from local OS filesystem.
	// if true, returns local OS native name. otherwise returns an empty string.
	virtual void TJS_INTF_METHOD GetLocallyAccessibleName(ttstr &name) {
		name = "";
	}

	/**
	 * コンストラクタ
	 */
	SteamStorage() : refCount(1) {
	}

	/**
	 * デストラクタ
	 */
	virtual ~SteamStorage() {
	}

protected:
	tjs_uint refCount; //< リファレンスカウント
};

static SteamStorage *instance = NULL;

void initStorage()
{
	if (instance == NULL) {
		instance = new SteamStorage();
		TVPRegisterStorageMedia(instance);
	}
}

void doneStorage()
{
	if (instance != NULL) {
		TVPUnregisterStorageMedia(instance);
		instance->Release();
		instance = NULL;
	}
}
