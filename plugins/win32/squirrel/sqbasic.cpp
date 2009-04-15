/**
 * squirrel 用基本メソッド用
 */
#include <squirrel.h>
#include <sqstdio.h>
#include <sqstdblob.h>
#include <sqstdaux.h>

#include <string>
typedef std::basic_string<SQChar> tstring;


/**
 * 文字列置換関数
 * @param str 置換対象
 * @param from 置換元
 * @param to 置換先
 */
static SQInteger
replace(HSQUIRRELVM v)
{
	const SQChar *str; // 置換対象
	const SQChar *from;  // 置換元
	const SQChar *to;  // 置換先
	if (SQ_SUCCEEDED(sq_getstring(v, 2, &str))) {
		if (SQ_SUCCEEDED(sq_getstring(v, 3, &from)) &&
			SQ_SUCCEEDED(sq_getstring(v, 4, &to))) {
			int nFromLen = scstrlen(from);
			const SQChar *start = str;
			const SQChar *p;
			tstring result;
			while ((p = scstrstr(start, from)) != NULL) {
				// 開始位置までコピー
				while (start < p) {
					result += *start++;
				}
				// 置換元サイズ分ずらす
				start = p + nFromLen;
				// 置換先をコピー
				result += to;
			}
			// 残りの文字列をコピー
			result += start;
			sq_pushstring(v, result.c_str(), -1);
			return 1;
		} else {
			sq_pushstring(v, str, -1);
			return 1;
		}
	}
	return 0;
}

/**
 * スタック情報の表示
 */
static SQInteger
printCallStack(HSQUIRRELVM v)
{
	sqstd_printcallstack(v);
	return 0;
}

/**
 * ガーベージコレクト起動
 */
static SQInteger
collectGarbage(HSQUIRRELVM v)
{
	sq_collectgarbage(v);
	return 0;
}

void
sqbasic_init(HSQUIRRELVM v)
{
	sq_pushroottable(v);

	// その他の基本ライブラリの登録
	sqstd_register_iolib(v);
	sqstd_register_bloblib(v);
	
	// replace の登録
	sq_pushstring(v, _SC("replace"), -1);
	sq_newclosure(v, replace, 0);
	sq_createslot(v, -3); 

	// printCallStack の登録
	sq_pushstring(v, _SC("printCallStack"), -1);
	sq_newclosure(v, printCallStack, 0);
	sq_createslot(v, -3); 
	
	// collectGarbage の登録
	sq_pushstring(v, _SC("collectGarbage"), -1);
	sq_newclosure(v, collectGarbage, 0);
	sq_createslot(v, -3); 
	
	sq_pop(v, 1);
}
