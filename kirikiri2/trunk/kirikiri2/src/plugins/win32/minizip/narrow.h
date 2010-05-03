#ifndef __NARROW_STRING__
#define __NARROW_STRING__

/**
 * C•¶š—ñˆ——p
 */
class NarrowString {
private:
	tjs_nchar *_data;
public:
	NarrowString(const ttstr &str) : _data(NULL) {
		tjs_int len = str.GetNarrowStrLen();
		_data = new tjs_nchar[len+1];
		str.ToNarrowStr(_data, len+1);
	}
	~NarrowString() {
		delete[] _data;
	}

	const tjs_nchar *data() {
		return _data;
	}

	operator const char *() const
	{
		return (const char *)_data;
	}
};

#endif
