#include "tjsbase.h"

void
TJSBase::wrap(Isolate *isolate, Handle<Object> obj)
{
	obj->SetHiddenValue(String::NewFromTwoByte(isolate, TJSINSTANCENAME), External::New(isolate, this));
}

// ƒpƒ‰ƒ[ƒ^æ“¾
bool
TJSBase::getVariant(Isolate *isolate, tTJSVariant &result, Handle<Object> obj)
{
	Local<Value> v = obj->GetHiddenValue(String::NewFromTwoByte(isolate, TJSINSTANCENAME));
	bool empty = v.IsEmpty();
	if (!v.IsEmpty()) {
		TJSBase *base = (TJSBase*)External::Cast(*v);
		result = base->variant;
		return true;
	}
	return false;
}
