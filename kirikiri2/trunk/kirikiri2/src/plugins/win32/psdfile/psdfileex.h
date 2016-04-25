#ifndef __PSDFILEEX_H__
#define __PSDFILEEX_H__

#include "psdparse/psdfile.h"

// ŒÅ’èƒƒ‚ƒŠ‚©‚ç“Ç‚İ‚Şƒo[ƒWƒ‡ƒ“
class PSDFileEx : public psd::PSDFile
{
public:
	PSDFileEx() {}
	bool loadMemory(unsigned char *begin, unsigned char *end);
};

#endif

