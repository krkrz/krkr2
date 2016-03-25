#include <windows.h>
#include "tp_stub.h"

#include "TextStreamHack.hpp"

#define FUNCSTUB_TEXTREAD "iTJSTextReadStream * ::TVPCreateTextStreamForRead(const ttstr &,const ttstr &)"
#define FUNCSTUB_R_ENCODE "iTJSTextReadStream * ::TVPCreateTextStreamForReadByEncoding(const ttstr &,const ttstr &,const ttstr &)" // only kirikir-z stub

//---------------------------------------------------------------------------
// DirtyHook - kirikiri2 ondemand patch (32bit only)

class DirtyHook
{
	enum { HOOK_ASMLEN = (5+2) };

	LPVOID target; // injection hook target address
	bool modified; // any modified
	HANDLE curproc; // current process handle
	BYTE backup[HOOK_ASMLEN]; // old backup codes
protected:
	void write(const BYTE *ptr, DWORD len) {
		DWORD prev;
		::VirtualProtect(target, len, PAGE_EXECUTE_READWRITE, &prev);
		::WriteProcessMemory(curproc, target, ptr, len, 0);
		::VirtualProtect(target, len, prev, &prev);
		::FlushInstructionCache(curproc, 0, 0);
	}
public:
	DirtyHook(LPVOID target) : target(target), modified(false)
	{
		curproc = ::GetCurrentProcess();
		::ReadProcessMemory(curproc, target, backup, HOOK_ASMLEN, 0);
	}
	~DirtyHook() {
		if (modified) write(backup, HOOK_ASMLEN);
	}
	void hook(LPVOID jumpaddr) {
		DWORD n = (DWORD)jumpaddr - (DWORD)target - HOOK_ASMLEN;
		BYTE asmjump[HOOK_ASMLEN] = {
			0x89, 0xc1,					// mov ecx,eax				[XXX] for borland to microsoft fastcall register convert
//			0x89, 0xc8,					// mov eax,ecx				[XXX] for microsoft to borland fastcall register convert
			0xe9,						// jmp rel32
			(n    ) & 0xFF,
			(n>>8 ) & 0xFF,
			(n>>16) & 0xFF,
			(n>>24) & 0xFF };
		write(asmjump, HOOK_ASMLEN);
		modified = true;
	}

	template <typename HOOK>
	static DirtyHook* MakeHook(const void *funcstub, const BYTE *bin, size_t len, HOOK hook) {
		if (memcmp(funcstub, bin, len)) return NULL; // asm unmatched

		// calc call address
		const BYTE *relative = (const BYTE*)funcstub + len;
		DWORD offset =
			((((DWORD)relative[0])    ) |
			 (((DWORD)relative[1])<<8 ) |
			 (((DWORD)relative[2])<<16) |
			 (((DWORD)relative[3])<<24));
		DWORD address = ((DWORD)relative) + 4 + offset;

		// set dirty-hook (HOOK: only supports microsoft fastcall function address, max 2 args)
		DirtyHook *ret = new DirtyHook((LPVOID)address);
		ret->hook((LPVOID)hook);
		return ret;
	}
} *TextReaderHook = NULL;

//---------------------------------------------------------------------------
bool onV2Link(iTVPFunctionExporter *exporter)
{
	const char *kzfunc = FUNCSTUB_R_ENCODE;
	void *ptr = NULL;
	if (exporter->QueryFunctionsByNarrowString(&kzfunc, &ptr, 1)) {
		TVPAddImportantLog(TJS_W("utf8hack: kirikiriZ not supported."));
	} else if ((ptr = TVPGetImportFuncPtr(FUNCSTUB_TEXTREAD)) != NULL) {
		const BYTE k2asm[] = {
			0x55,				// push ebp  
			0x8b, 0xec,			// mov  ebp,esp
			0x8b, 0x55, 0x0c,	// mov  edx,dword ptr [ebp+0Ch] 
			0x8b, 0x45, 0x08,	// mov  eax,dword ptr [ebp+8] 
			0xe8 };				// call xxxxxx

		TextReaderHook = DirtyHook::MakeHook(ptr, k2asm, sizeof(k2asm), ReplaceCreateTextStreamForRead);
		if (TextReaderHook) {
			tTJSVariant v;
			if (TVPGetCommandLine(TJS_W("-readencoding"), &v)) {
				ttstr encoding(v);
				if (!encoding.IsEmpty()) {
					SetDefaultReadCodePage(encoding.c_str());
				}
			}
			return TextEncodePropEntry(true);
		} else {
			TVPAddImportantLog(TJS_W("utf8hack: kirikiri2 binary mismatched."));
		}
	}
	return true;
}
bool onV2Unlink() {
	if (TextReaderHook) {
		delete TextReaderHook;
		return TextEncodePropEntry(false);
	}
	return true;
}
