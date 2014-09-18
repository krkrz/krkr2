#pragma once

#include <windows.h>
#include "tp_stub.h"

// [XXX]
struct eTJS {
	const ttstr message;
	eTJS(const ttstr& message) : message(message) {}
	const ttstr& GetMessage() const { return message; }
};
