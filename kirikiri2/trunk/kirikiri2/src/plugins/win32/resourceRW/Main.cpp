#include <windows.h>
#include "tp_stub.h"
#include "simplebinder.hpp"


using SimpleBinder::BindUtil;

static iTJSDispatch2 *LangIdTable = NULL;

class ResourceUtil {
protected:
	WORD lang_;

	static bool getResTypeAndName(tTJSVariant *type, tTJSVariant *name, LPCWSTR &lpType, LPCWSTR &lpName) {
		lpType = lpName = NULL;
		switch (type->Type()) {
		case tvtInteger: lpType = MAKEINTRESOURCEW((int)type->AsInteger()); break;
		case tvtString:  lpType = type->GetString(); break;
		default: return false;
		}
		if (name) switch (name->Type()) {
		case tvtInteger: lpName = MAKEINTRESOURCEW((int)name->AsInteger()); break;
		case tvtString:  lpName = name->GetString(); break;
		default: return false;
		}
		return true;
	}

public:
	ResourceUtil() : lang_(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL)) {}
	virtual ~ResourceUtil() {}

	static tjs_int getLang(ttstr name, const tjs_char *prefix = NULL) {
		if (LangIdTable && !name.IsEmpty()) {
			tTJSVariant var;
			name.ToUppserCase();
			if (TJS_SUCCEEDED(LangIdTable->PropGet(0, name.c_str(), name.GetHint(), &var, LangIdTable)) && var.Type() == tvtInteger) {
				return (tjs_int)(var.AsInteger());
			}
			var.Clear();
			name = ttstr(prefix) + name;
			if (TJS_SUCCEEDED(LangIdTable->PropGet(0, name.c_str(), name.GetHint(), &var, LangIdTable)) && var.Type() == tvtInteger) {
				return (tjs_int)(var.AsInteger());
			}
		}
		return -1;
	}

	static void ThrowLastError(const tjs_char *format) {
		LPVOID lpMessageBuffer;
		::FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
						 NULL, ::GetLastError(),
						 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						 (LPWSTR)&lpMessageBuffer, 0, NULL);
		ttstr message((tjs_char*)lpMessageBuffer);
		::LocalFree(lpMessageBuffer);

		TVPThrowExceptionMessage(format, message);
	}

	/**
	 * function setLang(prim, sub);
	 * @param prim 主言語(int, string)
	 * @param sub 副言語(int, string)
	 */
	tjs_error setLang(tTJSVariant *r, tTJSVariant *arg, tjs_int optnum, tTJSVariant **optargs) {
		tjs_int primlang = 0, sublang = 0;
		switch (arg->Type()) {
		case tvtInteger: primlang = (tjs_int)arg->AsInteger(); break;
		case tvtString:  primlang = getLang(arg->GetString(), TJS_W("LANG_")); break;
		default: return TJS_E_INVALIDPARAM;
		}
		if (optnum > 0) {
			switch (optargs[0]->Type()) {
			case tvtInteger: sublang = (tjs_int)optargs[0]->AsInteger(); break;
			case tvtString:  sublang = getLang(optargs[0]->GetString(), TJS_W("SUBLANG_")); break;
			default: return TJS_E_INVALIDPARAM;
			}
		}
		if (primlang >= 0 && sublang >= 0) {
			lang_ = MAKELANGID((primlang & 0xFFFF), (sublang & 0xFFFF));
		} else {
			if (primlang < 0) TVPThrowExceptionMessage(TJS_W("unknown primary language %1"), ttstr(arg->GetString()));
			if (sublang  < 0) TVPThrowExceptionMessage(TJS_W("unknown sub language %1"), ttstr(optargs[0]->GetString()));
		}
		if (r) *r = (tjs_int)lang_;
		return TJS_S_OK;
	}
};

class ResourceWriter : public ResourceUtil {
	HANDLE handle_;
	bool changed_;
public:
	ResourceWriter() : handle_(NULL), changed_(false) {}
	ResourceWriter(const tjs_char *file, bool clean = false) : handle_(NULL), changed_(false) { open_(file, clean); }
	virtual ~ResourceWriter() { close_(false); }

protected:
	void open_(const tjs_char *file, bool clean = false) {
		if (handle_) close_(false);
		ttstr local(file);
		TVPGetLocalName(local);
		handle_ = ::BeginUpdateResourceW(local.c_str(), clean ? TRUE : FALSE);
		if (!handle_) ThrowLastError(TJS_W("BeginUpdateResource: %1"));
		changed_ = clean;
	}

	void close_(bool write = true) {
		if (!handle_) return;
		if (!::EndUpdateResourceW(handle_, (write && changed_) ? FALSE : TRUE)) {
			handle_ = NULL;
			ThrowLastError(TJS_W("EndUpdateResource: %1"));
		}
		handle_ = NULL;
	}


public:
	/**
	 * function ResorceWriter(file, clean) { if (file !== void) open(file, clean); }
	 * @param optional file 操作対象ファイル
	 * @param optional clean 既存リソースを全クリアするか
	 */
	static tjs_error CreateNew(ResourceWriter* &inst, tjs_int optnum, tTJSVariant **optargs) {
		if (optnum > 0) {
			inst = new ResourceWriter(optargs[0]->GetString(), optnum>1 && optargs[1]->operator bool());
		} else {
			inst = new ResourceWriter();
		}
		return TJS_S_OK;
	}

	/**
	 * function open(file, clean = false);
	 * @param file 操作対象ファイル
	 * @param optional clean 既存リソースを全クリアするか
	 */
	tjs_error open(tTJSVariant *r, tTJSVariant *filename, tjs_int optnum, tTJSVariant **optargs) {
		open_(filename->GetString(), optnum>0 && optargs[0]->operator bool());
		if (r) *r = (tjs_int)handle_;
		return TJS_S_OK;
	}
	/**
	 * function close(write = true);
	 * @param optional write 結果を書き出すかどうか
	 */
	tjs_error close(tTJSVariant *r, tjs_int optnum, tTJSVariant **optargs) {
		close_(optnum==0 || optargs[0]->operator bool());
		if (r) r->Clear();
		return TJS_S_OK;
	}

	/**
	 * function clear(type, name)
	 * @param type リソースタイプ(int, string) -> rt*定数
	 * @param name リソース名(int, string)
	 */
	tjs_error clear(tTJSVariant *r, tTJSVariant *type, tTJSVariant *name) {
		if (!handle_) return TJS_E_FAIL;

		LPCWSTR lpType, lpName;
		if (!getResTypeAndName(type, name, lpType, lpName)) return TJS_E_INVALIDPARAM;

		if (::UpdateResourceW(handle_, lpType, lpName, lang_, NULL, 0) == FALSE) {
			ThrowLastError(TJS_W("UpdateResource: %1"));
		} else {
			changed_ = true;
		}
		if (r) r->Clear();
		return TJS_S_OK;
	}

	/**
	 * function writeFromText(type, name, text, utf8=false);
	 * @param type リソースタイプ(int, string) -> rt*定数
	 * @param name リソース名(int, string)
	 * @param text リソース内容(string)
	 */
	tjs_error writeFromText(tTJSVariant *r, tTJSVariant *type, tTJSVariant *name, tTJSVariant *text, tjs_int optnum, tTJSVariant **optargs) {
		if (!handle_) return TJS_E_FAIL;

		LPCWSTR lpType, lpName;
		if (!getResTypeAndName(type, name, lpType, lpName)) return TJS_E_INVALIDPARAM;

		LPVOID lpData = 0;
		DWORD  cbData = 0;
		ttstr tmp(text->GetString());
		bool utf8 = optnum>0 && optargs[0]->operator bool();
		if (utf8) {
			const tjs_char *error = 0;
			int maxlen = TVPWideCharToUtf8String(text->GetString(), NULL);
			if (maxlen < 0) error = TJS_W("invalid character in %1");
			else {
				char *out = (char*)tmp.AllocBuffer(maxlen + 1);
				cbData = TVPWideCharToUtf8String(text->GetString(), out);
				if (cbData != (DWORD)maxlen) error = TJS_W("mismatch length %1");
				out[maxlen] = 0;
				lpData = (LPVOID)out;
			}
			if (error) {
				tTJSVariant strname(*name);
				strname.ToString();
				TVPThrowExceptionMessage(error, strname.GetString());
			}
		} else {
			lpData = (void*)tmp.c_str();
			cbData = (tmp.length()+1) * sizeof(tjs_char);
		}
		if (::UpdateResourceW(handle_, lpType, lpName, lang_, lpData, cbData) == FALSE) {
			ThrowLastError(TJS_W("UpdateResource: %1"));
		} else {
			changed_ = true;
		}
		tmp.Clear();
		if (r) r->Clear();
		return TJS_S_OK;
	}

	/**
	 * function writeFromOctet(type, name, oct);
	 * @param type リソースタイプ(int, string) -> rt*定数
	 * @param name リソース名(int, string)
	 * @param oct  リソース内容(octet)
	 */
	tjs_error writeFromOctet(tTJSVariant *r, tTJSVariant *type, tTJSVariant *name, tTJSVariant *oct) {
		if (!handle_) return TJS_E_FAIL;

		LPCWSTR lpType, lpName;
		if (!getResTypeAndName(type, name, lpType, lpName)) return TJS_E_INVALIDPARAM;

		tTJSVariantOctet *octet = oct->AsOctetNoAddRef();
		if (octet) {
			LPVOID lpData = const_cast<tjs_uint8*>(octet->GetData());
			DWORD  cbData = octet->GetLength();
			if (::UpdateResourceW(handle_, lpType, lpName, lang_, lpData, cbData) == FALSE) {
				ThrowLastError(TJS_W("UpdateResource: %1"));
			} else {
				changed_ = true;
			}
		}
		if (r) r->Clear();
		return TJS_S_OK;
	}

	/**
	 * function writeFromFile(type, name, file);
	 * @param type リソースタイプ(int, string) -> rt*定数
	 * @param name リソース名(int, string)
	 * @param file リソース内容ファイル(string)
	 */
	tjs_error writeFromFile(tTJSVariant *r, tTJSVariant *type, tTJSVariant *name, tTJSVariant *file) {
		if (!handle_) return TJS_E_FAIL;

		LPCWSTR lpType, lpName;
		if (!getResTypeAndName(type, name, lpType, lpName)) return TJS_E_INVALIDPARAM;

		IStream *stream = TVPCreateIStream(file->GetString(), TJS_BS_READ);
		if (stream) {
			try {
				STATSTG stat;
				stream->Stat(&stat, STATFLAG_NONAME);
				tjs_uint64 qsize = (tjs_uint64)stat.cbSize.QuadPart;
				if (qsize > 0xFFFFFFFF) {
					TVPThrowExceptionMessage(TJS_W("too large file: %1"), file->GetString());
				}
				DWORD size = (DWORD)qsize;
				BYTE *p = new BYTE[size];
				try {
					DWORD read;
					stream->Read(p, size, &read);
					if (::UpdateResourceW(handle_, lpType, lpName, lang_, (LPVOID)p, size) == FALSE) {
						ThrowLastError(TJS_W("UpdateResource: %1"));
					} else {
						changed_ = true;
					}
				} catch (...) {
					delete[] p;
					throw;
				}
				delete[] p;
			} catch (...) {
				stream->Release();
				throw;
			}
			stream->Release();
		}
		if (r) r->Clear();
		return TJS_S_OK;
	}

	/**
	 * function setLang(primlang, sublang);
	 */
	tjs_error setLang(tTJSVariant *r, tTJSVariant *arg, tjs_int optnum, tTJSVariant **optargs) {
		return ResourceUtil::setLang(r, arg, optnum, optargs);
	}
};

class ResourceReader : public ResourceUtil {
	HMODULE handle_;
public:
	ResourceReader() : handle_(NULL) {}
	ResourceReader(const tjs_char *file) : handle_(NULL) { open_(file); }
	virtual ~ResourceReader() { close_(); }

protected:
	void open_(const tjs_char *file) {
		if (handle_) close_();
		ttstr local(file);
		TVPGetLocalName(local);
		handle_ = ::LoadLibraryExW(local.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE);
		if (!handle_) ThrowLastError(TJS_W("LoadLibraryEx: %1"));
	}

	void close_() {
		if (!handle_) return;
		if (!::FreeLibrary(handle_)) {
			handle_ = NULL;
			ThrowLastError(TJS_W("FreeLibrary: %1"));
		}
		handle_ = NULL;
	}

	HRSRC findResource_(tTJSVariant *type, tTJSVariant *name, DWORD *size = NULL, bool raiseerr = false) {
		if (!handle_) {
			if (raiseerr) TVPThrowExceptionMessage(TJS_W("target not opened."));
			return NULL;
		}

		LPCWSTR lpType, lpName;
		if (!getResTypeAndName(type, name, lpType, lpName)) {
			if (raiseerr) TVPThrowExceptionMessage(TJS_W("invalid name or type."));
			return NULL;
		}

		HRSRC res = ::FindResourceExW(handle_, lpType, lpName, lang_);
		if (!res && raiseerr) ThrowLastError(TJS_W("FindResourceEx: %1"));
		if (res && size) {
			*size = ::SizeofResource(handle_, res);
		}
		return res;
	}
	LPVOID loadResource_(tTJSVariant *type, tTJSVariant *name, DWORD *size = NULL) {
		HRSRC res = findResource_(type, name, size, true);
		if (res) {
			HGLOBAL global = ::LoadResource(handle_, res);
			if (global) {
				LPVOID ptr = ::LockResource(global);
				if (ptr) return ptr;
				ThrowLastError(TJS_W("LockResource: %1"));
			} else {
				ThrowLastError(TJS_W("LoadResource: %1"));
			}
		}
		return NULL;
	}

	static BOOL CALLBACK EnumTypesProc(HMODULE hModule, LPWSTR lpszType, LONG_PTR lParam) {
		if (IS_INTRESOURCE(lpszType)) AddEnumResult(lParam, (tTVInteger)lpszType);
		else                          AddEnumResult(lParam,             lpszType);
		return TRUE;
	}
	static BOOL CALLBACK EnumNamesProc(HMODULE hModule, LPCWSTR lpszType, LPWSTR lpszName, LONG_PTR lParam) {
		if (IS_INTRESOURCE(lpszName)) AddEnumResult(lParam, (tTVInteger)lpszName);
		else                          AddEnumResult(lParam,             lpszName);
		return TRUE;
	}
	static BOOL CALLBACK EnumLangsProc(HMODULE hModule, LPCWSTR lpszType, LPCWSTR lpszName, WORD wIDLanguage, LONG_PTR lParam) {
		AddEnumResult(lParam, (tTVInteger)wIDLanguage);
		return TRUE;
	}
	static void AddEnumResult(LONG_PTR lParam, tTJSVariant v) {
		iTJSDispatch2 *arr = reinterpret_cast<iTJSDispatch2*>(lParam);
		if (arr) {
			static ttstr s_add(TJS_W("add"));
			tTJSVariant *param[] = { &v };
			Try_iTJSDispatch2_FuncCall(arr, 0, s_add.c_str(), s_add.GetHint(), NULL, 1, param, arr);
		}
	}

public:
	/**
	 * function ResorceReader(file) { if (file !== void) open(file); }
	 * @param optional file 操作対象ファイル
	 */
	static tjs_error CreateNew(ResourceReader* &inst, tjs_int optnum, tTJSVariant **optargs) {
		if (optnum > 0) {
			inst = new ResourceReader(optargs[0]->GetString());
		} else {
			inst = new ResourceReader();
		}
		return TJS_S_OK;
	}

	/**
	 * function open(file);
	 * @param file 操作対象ファイル
	 */
	tjs_error open(tTJSVariant *r, tTJSVariant *filename) {
		open_(filename->GetString());
		if (r) *r = (tjs_int)handle_;
		return TJS_S_OK;
	}
	/**
	 * function close();
	 */
	tjs_error close(tTJSVariant *r) {
		close_();
		if (r) r->Clear();
		return TJS_S_OK;
	}

	/**
	 * function isExistentResource(type, name);
	 */
	tjs_error isExistentResource(tTJSVariant *r, tTJSVariant *type, tTJSVariant *name) {
		HRSRC res = findResource_(type, name);
		if (r) *r = (res != NULL);
		return TJS_S_OK;
	}
	

	/**
	 * function readToText(type, name, utf8=false);
	 */
	tjs_error readToText(tTJSVariant *r, tTJSVariant *type, tTJSVariant *name, tjs_int optnum, tTJSVariant **optargs) {
		DWORD size = 0;
		LPVOID ptr = loadResource_(type, name, &size);
		if (r) r->Clear();
		if (ptr) {
			bool utf8 = optnum>0 && optargs[0]->operator bool();
			if (utf8) {
				ttstr tmp;
				char *data = 0;
				tjs_int len = 0;
				try {
					data = new char[size+1];
					memcpy(data, ptr, size);
					data[size] = 0; // terminator
					len = TVPUtf8ToWideCharString(data, NULL);
					tjs_char *out = tmp.AllocBuffer((len+1) * sizeof(tjs_char)); // [XXX]
					TVPUtf8ToWideCharString(data, out);
					out[len] = 0;
				} catch (...) {
					delete [] data;
					throw;
				}
				delete [] data;
				tmp.FixLen();
				if (r) *r = tmp;
			} else {
				ttstr text((const tjs_char*)ptr, size/sizeof(tjs_char));
				if (r) *r = text;
			}
		}
		return TJS_S_OK;
	}

	/**
	 * function readToOctet(type, name);
	 */
	tjs_error readToOctet(tTJSVariant *r, tTJSVariant *type, tTJSVariant *name) {
		DWORD size = 0;
		LPVOID ptr = loadResource_(type, name, &size);
		if (r) r->Clear();
		if (ptr) {
			tTJSVariantOctet *oct = TJSAllocVariantOctet((const tjs_uint8*)ptr, (tjs_uint)size);
			if (r) *r = oct;
			oct->Release();
		}
		return TJS_S_OK;
	}

	/**
	 * function readToFile(type, name, file);
	 */
	tjs_error readToFile(tTJSVariant *r, tTJSVariant *type, tTJSVariant *name, tTJSVariant *file) {
		DWORD size = 0;
		LPVOID ptr = loadResource_(type, name, &size);
		if (r) r->Clear();
		if (ptr) {
			IStream *stream = TVPCreateIStream(file->GetString(), TJS_BS_WRITE);
			if (stream) {
				DWORD out = 0;
				try {
					if (stream->Write((BYTE*)ptr, size, &out) != S_OK) {
						TVPThrowExceptionMessage(TJS_W("output error: %1"), file->GetString());
					}
				} catch (...) {
					stream->Release();
					throw;
				}
				stream->Release();
				if (size != out) {
					TVPThrowExceptionMessage(TJS_W("write failed: %1"), file->GetString());
				}
				if (r) *r = (tTVInteger)size;
			}
		}
		return TJS_S_OK;
	}

	/**
	 * function setLang(primlang, sublang);
	 */
	tjs_error setLang(tTJSVariant *r, tTJSVariant *arg, tjs_int optnum, tTJSVariant **optargs) {
		return ResourceUtil::setLang(r, arg, optnum, optargs);
	}

	/**
	 * function enumTypes()
	 */
	tjs_error enumTypes(tTJSVariant *r) {
		if (r && handle_) {
			iTJSDispatch2 *arr = TJSCreateArrayObject();
			*r = tTJSVariant(arr, arr);
			arr->Release();
			if (!::EnumResourceTypesW(handle_, EnumTypesProc, (LONG_PTR)arr)) {
				ThrowLastError(TJS_W("EnumResourceTypes: %1"));
			}
		}
		return TJS_S_OK;
	}
	/**
	 * function enumTypes(type)
	 */
	tjs_error enumNames(tTJSVariant *r, tTJSVariant *type) {
		if (r && handle_) {
			iTJSDispatch2 *arr = TJSCreateArrayObject();
			*r = tTJSVariant(arr, arr);
			arr->Release();

			LPCWSTR lpType, lpName;
			if (getResTypeAndName(type, NULL, lpType, lpName)) {
				if (!::EnumResourceNamesW(handle_, lpType, EnumNamesProc, (LONG_PTR)arr)) {
					ThrowLastError(TJS_W("EnumResourceNames: %1"));
				}
			}
		}
		return TJS_S_OK;
	}
	/**
	 * function enumLangs(type, name)
	 */
	tjs_error enumLangs(tTJSVariant *r, tTJSVariant *type, tTJSVariant *name) {
		if (r && handle_) {
			iTJSDispatch2 *arr = TJSCreateArrayObject();
			*r = tTJSVariant(arr, arr);
			arr->Release();

			LPCWSTR lpType, lpName;
			if (getResTypeAndName(type, name, lpType, lpName)) {
				if (!::EnumResourceLanguagesW(handle_, lpType, lpName, EnumLangsProc, (LONG_PTR)arr)) {
					ThrowLastError(TJS_W("EnumResourceLanguages: %1"));
				}
			}
		}
		return TJS_S_OK;
	}

};

bool Entry(bool link) {

	if (link) {
		LangIdTable = TJSCreateDictionaryObject();
		BindUtil(LangIdTable, true)
#define _DEFLANG_(tag) .Variant(TJS_W( #tag), tag)
#include "lang.inc"
#undef  _DEFLANG_
			;
	} else {
		if (LangIdTable) LangIdTable->Release();
		/**/LangIdTable = NULL;
	}
	return
		(BindUtil(link)
		 .Class(TJS_W("ResourceWriter"), &ResourceWriter::CreateNew)
		 .Function(TJS_W("open"),  &ResourceWriter::open)
		 .Function(TJS_W("close"), &ResourceWriter::close)
		 .Function(TJS_W("setLang"), &ResourceWriter::setLang)
		 .Function(TJS_W("clear"), &ResourceWriter::clear)
		 .Function(TJS_W("writeFromText"), &ResourceWriter::writeFromText)
		 .Function(TJS_W("writeFromFile"), &ResourceWriter::writeFromFile)
		 .Function(TJS_W("writeFromOctet"), &ResourceWriter::writeFromOctet)
		 .IsValid()) &&
		(BindUtil(link)
		 .Class(TJS_W("ResourceReader"), &ResourceReader::CreateNew)
		 .Function(TJS_W("open"),  &ResourceReader::open)
		 .Function(TJS_W("close"), &ResourceReader::close)
		 .Function(TJS_W("setLang"), &ResourceReader::setLang)
		 .Function(TJS_W("isExistentResource"), &ResourceReader::isExistentResource)
		 .Function(TJS_W("readToText"), &ResourceReader::readToText)
		 .Function(TJS_W("readToFile"), &ResourceReader::readToFile)
		 .Function(TJS_W("readToOctet"), &ResourceReader::readToOctet)
		 .Function(TJS_W("enumTypes"), &ResourceReader::enumTypes)
		 .Function(TJS_W("enumNames"), &ResourceReader::enumNames)
		 .Function(TJS_W("enumLangs"), &ResourceReader::enumLangs)
		 .IsValid()) &&
		(BindUtil(link)
		 .Variant(TJS_W("rtAccelerator"), (tjs_int)RT_ACCELERATOR)
		 .Variant(TJS_W("rtAniCursor"), (tjs_int)RT_ANICURSOR)
		 .Variant(TJS_W("rtAniIcon"), (tjs_int)RT_ANIICON)
		 .Variant(TJS_W("rtBitmap"), (tjs_int)RT_BITMAP)
		 .Variant(TJS_W("rtCursor"), (tjs_int)RT_CURSOR)
		 .Variant(TJS_W("rtDialog"), (tjs_int)RT_DIALOG)
		 .Variant(TJS_W("rtFont"), (tjs_int)RT_FONT)
		 .Variant(TJS_W("rtFontDir"), (tjs_int)RT_FONTDIR)
		 .Variant(TJS_W("rtGroupCursor"), (tjs_int)RT_GROUP_CURSOR)
		 .Variant(TJS_W("rtGroupIcon"), (tjs_int)RT_GROUP_ICON)
		 .Variant(TJS_W("rtHtml"), (tjs_int)RT_HTML)
		 .Variant(TJS_W("rtIcon"), (tjs_int)RT_ICON)
		 .Variant(TJS_W("rtMenu"), (tjs_int)RT_MENU)
		 .Variant(TJS_W("rtMessageTable"), (tjs_int)RT_MESSAGETABLE)
		 .Variant(TJS_W("rtRcData"), (tjs_int)RT_RCDATA)
		 .Variant(TJS_W("rtString"), (tjs_int)RT_STRING)
		 .Variant(TJS_W("rtVersion"), (tjs_int)RT_VERSION)
		 .IsValid()) &&
			true;
}

bool onV2Link()   { return Entry(true);  }
bool onV2Unlink() { return Entry(false); }
