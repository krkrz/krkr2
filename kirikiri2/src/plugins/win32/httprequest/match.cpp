#include <tchar.h>
#include <string>
#include <regex>

typedef std::basic_string<TCHAR> tstring;
typedef std::tr1::basic_regex<TCHAR> tregex;
typedef std::tr1::match_results<tstring::const_iterator> tmatch;

static tregex regctype(_T("<meta[ \\t]+http-equiv=(\\\"content-type\\\"|'content-type'|content-type)[ \\t]+content=(\\\"[^\\\"]*\\\"|'[^']*'|[^ \\t>]+).*>"), tregex::icase);

bool
matchContentType(tstring &text, tstring &ctype)
{
	bool ret = false;
	tmatch result;
	if (std::tr1::regex_search(text, result, regctype)) {
		tstring str = result.str(2);
		int len = str.size();
		const TCHAR *buf = str.c_str();
		if (len > 0) {
			if (buf[0] == '\'' || buf[0] == '"') {
				ctype = tstring(buf+1, len-2);
			} else {
				ctype = tstring(buf, len);
			}
			ret = true;
		}
	}
	return ret;
}
