#include "tjsbase.h"

// ƒpƒ‰ƒ[ƒ^æ“¾
bool
TJSBase::getVariant(tTJSVariant &result, Handle<Object> obj)
{
	if (obj->InternalFieldCount() > 0) {
		TJSBase *base = (TJSBase*)obj->GetPointerFromInternalField(0);
		result = base->variant;
		return true;
	}
	return false;
}

