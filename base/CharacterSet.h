#ifndef __CharacterSet_H__
#define __CharacterSet_H__

// various character conding conversion.
// currently only utf-8 related functions are implemented.
#include "tjsTypes.h"


extern tjs_int TVPWideCharToUtf8String(const tjs_char *in, char * out);
extern tjs_int TVPUtf8ToWideCharString(const char * in, tjs_char *out);



#endif
