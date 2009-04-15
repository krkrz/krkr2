/*
 * copyright (c)2009 http://wamsoft.jp
 * zlib license
 */
#include "sqthread.h"

#include <string.h>
#include <ctype.h>
#include <string>
#include <new>

namespace sqobject {

/**
 * クロージャかどうか
 */
static bool
isClosure(SQObjectType type)
{
	return type == OT_CLOSURE || type == OT_NATIVECLOSURE;
}


/**
 * 文字列取得用
 * @param v VM
 * @param idx インデックス
 * @return 文字列
 */
const SQChar *getString(HSQUIRRELVM v, int idx) {
	const SQChar *x = NULL;
	sq_getstring(v, idx, &x);
	return x;
};

static SQRESULT ERROR_NOMEMBER(HSQUIRRELVM v) {
	return sq_throwerror(v, _SC("no such member"));
}

// setter名前決定
void getSetterName(tstring &store, const SQChar *name)
{
	store = _SC("set");
	store += toupper(*name);
	store += (name + 1);
}

// getter名前決定
void getGetterName(tstring &store, const SQChar *name)
{
	store = _SC("get");
	store += toupper(*name);
	store += (name + 1);
}

// ---------------------------------------------------------
// Object
// ---------------------------------------------------------

/**
 * オブジェクト待ちの登録
 * @param thread スレッド
 */
void
Object::addWait(Thread *thread)
{
	_waitThreadList.push_back(thread);
}

/**
 * オブジェクト待ちの解除
 * @param thread スレッド
 */
void
Object::removeWait(Thread *thread)
{
	std::vector<Thread*>::iterator i = _waitThreadList.begin();
	while (i != _waitThreadList.end()) {
		if (*i == thread) {
			i = _waitThreadList.erase(i);
		} else {
			i++;	
		}
	}
}

/**
 * コンストラクタ
 */
Object::Object()
{
}

/**
 * コンストラクタ
 */
Object::Object(HSQUIRRELVM v, int delegateIdx)
{
	if (sq_gettop(v) >= delegateIdx) {
		delegate.getStack(v, delegateIdx);
	}
}

/**
 * デストラクタ
 */
Object::~Object()
{
	notifyAll();
}

/**
 * このオブジェクトを待っている１スレッドの待ちを解除
 */
void
Object::notify()
{
	std::vector <Thread *>::iterator i = _waitThreadList.begin();
	while (i != _waitThreadList.end()) {
		if ((*i)->notifyObject(this)) {
			i = _waitThreadList.erase(i);
			return;
		} else {
			i++;
		}
	}
}
	
/**
 * このオブジェクトを待っている全スレッドの待ちを解除
 */
void
Object::notifyAll()
{
	std::vector <Thread *>::iterator i = _waitThreadList.begin();
	while (i != _waitThreadList.end()) {
		(*i)->notifyObject(this);
		i = _waitThreadList.erase(i);
	}
}

/**
 * プロパティから値を取得
 * @param name プロパティ名
 * @return プロパティ値
 */
SQRESULT
Object::_get(HSQUIRRELVM v)
{
	SQRESULT result = SQ_OK;
	const SQChar *name = getString(v, 2);
	if (name && *name) {
		// delegateの参照
		if (delegate.isDelegate()) {
			delegate.push(v);
			sq_pushstring(v, name, -1);
			if (SQ_SUCCEEDED(result = sq_get(v,-2))) {
				// メソッドの場合は束縛処理
				if (isClosure(sq_gettype(v,-1)) && delegate.isBindDelegate()) {
					delegate.push(v);
					if (SQ_SUCCEEDED(sq_bindenv(v, -2))) {
						sq_remove(v, -2); // 元のクロージャ
					}
				}
				sq_remove(v, -2);
				return 1;
			} else {
				sq_pop(v,1); // delegate
			}
		}
		
		// getter を探してアクセス
		tstring name2;
		getGetterName(name2, name);
		sq_push(v, 1); // self
		sq_pushstring(v, name2.c_str(), -1);
		if (SQ_SUCCEEDED(result = sq_rawget(v,-2))) {
			sq_push(v, 1); //  self;
			if (SQ_SUCCEEDED(result = sq_call(v,1,SQTrue,SQTrue))) {
				//sqprintf("呼び出し成功:%s\n", name);
				sq_remove(v, -2); // func
				sq_remove(v, -2); // self
				return 1;
			} else {
				sq_pop(v,2); // func, self
			}
		} else {
			sq_pop(v, 1); // self
#if 1
			// グローバル変数を参照
			sq_pushroottable(v);
			sq_pushstring(v, name, -1);
			if (SQ_SUCCEEDED(sq_rawget(v,-2))) {
				sq_remove(v, -2); // root
				return 1;
			} else {
				sq_pop(v,1);
			}
#endif
		}
	}
	return ERROR_NOMEMBER(v);
}

/**
 * プロパティに値を設定
 * @param name プロパティ名
 * @param value プロパティ値
 */
SQRESULT
Object::_set(HSQUIRRELVM v)
{
	SQRESULT result = SQ_OK;
	const SQChar *name = getString(v, 2);
	if (name && *name) {
		// delegateの参照
		if (delegate.isDelegate()) {
			delegate.push(v);
			sq_push(v, 2); // name
			sq_push(v, 3); // value
			if (SQ_SUCCEEDED(result = sq_set(v,-3))) {
				sq_pop(v,1); // delegate
				return SQ_OK;
			} else {
				sq_pop(v,1); // delegate
			}
		}
		
		// setter を探してアクセス
		tstring name2;
		getSetterName(name2, name);
		
		sq_push(v, 1); // self
		sq_pushstring(v, name2.c_str(), -1);
		if (SQ_SUCCEEDED(result = sq_rawget(v,-2))) {
			sq_push(v, 1); // self
			sq_push(v, 3); // value
			if (SQ_SUCCEEDED(result = sq_call(v,2,SQFalse,SQTrue))) {
				//sqprintf("呼び出し成功:%s\n", name);
				sq_pop(v,2); // func, self
				return SQ_OK;
			} else {
				sq_pop(v,2); // func, self
			}
		}
		
	}
	//return result;
	return ERROR_NOMEMBER(v);
}

/**
 * setプロパティの存在確認
 * @param name プロパティ名
 * @return setプロパティが存在したら true
 */
SQRESULT
Object::hasSetProp(HSQUIRRELVM v)
{
	SQRESULT result = SQ_OK;
	SQBool ret = SQFalse;
	if (sq_gettop(v) > 1) {
		const SQChar *name = getString(v, 2);
		if (name && *name) {
			tstring name2;
			getSetterName(name2, name);
			sq_push(v, 1); // object
			sq_pushstring(v, name2.c_str(), -1);
			if (SQ_SUCCEEDED(result = sq_rawget(v,-2))) {
				sq_pop(v,1);
				ret = SQTrue;
			} else {
				sq_pushstring(v, name, -1);
				if (SQ_SUCCEEDED(result = sq_rawget(v,-2))) {
					sq_pop(v,1);
					ret = SQTrue;
				}
			}
			sq_pop(v,1); // object
		}
	}
	if (SQ_SUCCEEDED(result)) {
		sq_pushbool(v, ret);
		return 1;
	} else {
		return result;
	}
}

/**
 * 委譲の設定
 */
SQRESULT
Object::setDelegate(HSQUIRRELVM v)
{
	if (sq_gettop(v) > 1) {
		delegate.getStack(v,2);
	} else {
		delegate.clear();
	}
	return SQ_OK;
}

/**
 * 委譲の設定
 */
SQRESULT
Object::getDelegate(HSQUIRRELVM v)
{
	delegate.push(v);
	return 1;
}

};

