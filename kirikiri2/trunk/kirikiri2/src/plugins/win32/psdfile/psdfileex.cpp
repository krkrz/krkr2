#include "psdfileex.h"
#include "psdparse/psdparse.h"

bool
PSDFileEx::loadMemory(unsigned char *begin, unsigned char *end)
{
	isLoaded = false;
	psd::Parser<unsigned char*> parser(*this);
	bool r = parse(begin, end, parser);
	if (r && begin == end) {
		dprint("succeeded\n");
		isLoaded = processParsed();
	}
	return isLoaded;
}
