#ifndef __TJSBASE_H__
#define __TJSBASE_H__

#include <windows.h>
#include "tp_stub.h"
#include <v8.h>
using namespace v8;

class TJSBase {
public:
	TJSBase(const tTJSVariant &variant) : variant(variant) {}
	virtual ~TJSBase() {};
	static bool getVariant(tTJSVariant &result, Handle<Object> obj);
private:
	tTJSVariant variant;
};

#endif
