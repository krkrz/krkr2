/**
 * squirrel 用基底オブジェクト＋疑似スレッド
 */
#include "sqobject.h"

#include <ctype.h>
#include <sqstdio.h>
#include <string>
#include <new>

// ログ出力用
#define PRINT(v,msg) {\
	SQPRINTFUNCTION print = sq_getprintfunc(v);\
	if (print) {\
		print(v,msg);\
	}\
}

/**
 * ループ実行開始
 * スレッドが投入されたのでメインループを開始する処理を呼び出す
 * ユーザ側で定義する必要があります
 */
extern void sqobject_start();

namespace sqobject {

/**
 * パラメータ数の取得
 * @param v VM
 * @return パラメータ数
 */
static int getParamCount(HSQUIRRELVM v)
{
	return sq_gettop(v) - 1;
}

/**
 * 文字列取得用
 * @param v VM
 * @param idx インデックス
 * @return 文字列
 */
static const SQChar *getString(HSQUIRRELVM v, int idx) {
	const SQChar *x = NULL;
	sq_getstring(v, idx, &x);
	return x;
};

SQRESULT ERROR_CREATE(HSQUIRRELVM v) {
	return sq_throwerror(v, _SC("can't create native instance"));
}

SQRESULT ERROR_NOMEMBER(HSQUIRRELVM v) {
	return sq_throwerror(v, _SC("no such member"));
}

SQRESULT ERROR_BADINSTANCE(HSQUIRRELVM v) {
	return sq_throwerror(v, _SC("bad instance"));
}

void getSetterName(tstring &store, const SQChar *name)
{
	store = _SC("set");
	store += toupper(*name);
	store += (name + 1);
}

void getGetterName(tstring &store, const SQChar *name)
{
	store = _SC("get");
	store += toupper(*name);
	store += (name + 1);
}

static const SQUserPointer OBJTYPETAG = (SQUserPointer)"OBJTYPETAG";
static const SQUserPointer THREADTYPETAG = (SQUserPointer)"THREADTYPETAG";

// ---------------------------------------------------------
// ObjectInfo
// ---------------------------------------------------------

/**
 * squirrel オブジェクト保持用クラス
 */
class ObjectInfo {

protected:
	HSQUIRRELVM v; // オブジェクトの属していたVM
	HSQOBJECT obj; // オブジェクト参照情報

public:
	// 内容消去
	void clear() {
		sq_release(v,&obj);
		v = NULL;
		sq_resetobject(&obj);
	}

	// スタックから取得
	void getStack(HSQUIRRELVM v, int idx) {
		clear();
		this->v = v;
		sq_getstackobj(v, idx, &obj);
		sq_addref(v, &obj);
	}

	// コンストラクタ
	ObjectInfo() : v(NULL) {
		sq_resetobject(&obj);
	}

	// コンストラクタ
	ObjectInfo(HSQUIRRELVM v, int idx) : v(v) {
		sq_resetobject(&obj);
		sq_getstackobj(v, idx, &obj);
		sq_addref(v, &obj);
	}

	// コピーコンストラクタ
	ObjectInfo(const ObjectInfo &orig) {
		v   = orig.v;
		obj = orig.obj;
		sq_addref(v, &obj);
	}

	// 代入
	ObjectInfo & operator=(const ObjectInfo &orig) {
		clear();
		v = orig.v;
		obj = orig.obj;
		sq_addref(v, &obj);
		return *this;
	}

	// デストラクタ
	virtual ~ObjectInfo() {
		clear();
	}

	// スレッドか？
	bool isThread() const {
		return sq_isthread(obj);
	}

	// 同じスレッドか？
	bool isSameThread(const HSQUIRRELVM v) const {
		return sq_isthread(obj) && obj._unVal.pThread == v;
	}

	// スレッドを取得
	operator HSQUIRRELVM() const {
		HSQUIRRELVM vm = sq_isthread(obj) ? obj._unVal.pThread : NULL;
		return vm;
	}
	
	// オブジェクトをPUSH
	void push(HSQUIRRELVM v) const {
		sq_pushobject(v, obj);
	}

	// ---------------------------------------------------
	// delegate 処理用
	// ---------------------------------------------------

	// delegate として機能するかどうか
	bool isDelegate() {
		return v && (sq_isinstance(obj) || sq_istable(obj));
	}

	// bindenv させるかどうか
	bool isBindDelegate() {
		return v && (sq_isinstance(obj));
	}

	// ---------------------------------------------------
	// オブジェクト取得
	// ---------------------------------------------------

	// インスタンスユーザポインタを取得
	Thread *getThread() {
		if (sq_isinstance(obj)) {
			sq_pushobject(v, obj);
			SQUserPointer up = NULL;
			if (SQ_SUCCEEDED(sq_getinstanceup(v, -1, &up, THREADTYPETAG))) {
				sq_pop(v,1);
				return (Thread*)up;
			}
			sq_pop(v, 1);
		}
		return NULL;
	}

	// インスタンスユーザポインタを取得
	Object *getObject() {
		if (sq_isinstance(obj)) {
			sq_pushobject(v, obj);
			Object *ret = Object::getObject(v, -1);
			sq_pop(v, 1);
			return ret;
		}
		return NULL;
	}
	
	// ---------------------------------------------------
	// wait処理用メソッド
	// ---------------------------------------------------

	bool isSameString(const SQChar *str) const {
		if (str && sq_isstring(obj)) {
			const SQChar *mystr;
			sq_pushobject(v, obj);
			sq_getstring(v, -1, &mystr);
			sq_pop(v, 1);
			return mystr && scstrcmp(str, mystr) == 0;
		}
		return false;
	}
};

// ---------------------------------------------------------
// Object
// ---------------------------------------------------------

/**
 * オブジェクト待ちの登録
 * @param thread スレッド
 */
void
Object::addWait(Thread *thread) {
	_waitThreadList.push_back(thread);
}

/**
 * オブジェクト待ちの解除
 * @param thread スレッド
 */
void
Object::removeWait(Thread *thread) {
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
Object::Object() : delegate(NULL) {
}

/**
 * デストラクタ
 */
Object::~Object() {
	_notifyAll();
	if (delegate) {
		delete delegate;
	}
}

// ------------------------------------------------------------------

/**
 * @return オブジェクト情報オブジェクト
 */
Object *
Object::getObject(HSQUIRRELVM v, int idx) {
	SQUserPointer up;
	std::vector<SQUserPointer>::const_iterator i = tags.begin();
	while (i != tags.end()) {
		if (SQ_SUCCEEDED(sq_getinstanceup(v, idx, &up, *i))) {
			return (Object*)up;
		}
		i++;
	}
	return NULL;
}

/**
 * オブジェクトのリリーサ
 */
SQRESULT
Object::release(SQUserPointer up, SQInteger size) {
	Object *self = (Object*)up;
	delete self;
	return SQ_OK;
}

/**
 * オブジェクトのコンストラクタ
 */
SQRESULT
Object::constructor(HSQUIRRELVM v) {
	SQRESULT result = SQ_OK;
	Object *self = new Object();
	if (SQ_SUCCEEDED(result = sq_setinstanceup(v, 1, self))) {;
		sq_setreleasehook(v, 1, release);
	} else {
		delete self;
	}
	if (SQ_SUCCEEDED(result)) {
		if (getParamCount(v) > 0) {
			self->delegate = new ObjectInfo(v, 2);
		}
	}
	return result;
}

bool
Object::isClosure(SQObjectType type) {
	return type == OT_CLOSURE || type == OT_NATIVECLOSURE;
}
	
/**
 * プロパティから値を取得
 * @param name プロパティ名
 * @return プロパティ値
 */
SQRESULT
Object::_get(HSQUIRRELVM v) {
	SQRESULT result = SQ_OK;
	const SQChar *name = getString(v, 2);
	if (name && *name) {
		
			// delegateの参照
		Object *self = getObject(v, 1);
		if (self && self->delegate &&  self->delegate->isDelegate()) {
			self->delegate->push(v);
			sq_pushstring(v, name, -1);
			if (SQ_SUCCEEDED(result = sq_get(v,-2))) {
				// メソッドの場合は束縛処理
				if (isClosure(sq_gettype(v,-1)) && self->delegate->isBindDelegate()) {
					self->delegate->push(v);
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
				//printf("呼び出し成功:%s\n", name);
				sq_remove(v, -2); // func
				sq_remove(v, -2); // self
				return 1;
			} else {
				sq_pop(v,2); // func, self
			}
		} else {
			sq_pop(v, 1); // self
		}
		
	}
	//return result;
	return ERROR_NOMEMBER(v);
}
	
/**
 * プロパティに値を設定
 * @param name プロパティ名
 * @param value プロパティ値
 */
SQRESULT
Object::_set(HSQUIRRELVM v) {
	SQRESULT result = SQ_OK;
	const SQChar *name = getString(v, 2);
	if (name && *name) {
		
		// delegateの参照
		Object *self = getObject(v, 1);
		if (self && self->delegate && self->delegate->isDelegate()) {
			self->delegate->push(v);
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
				//printf("呼び出し成功:%s\n", name);
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
Object::hasSetProp(HSQUIRRELVM v) {
	SQRESULT result = SQ_OK;
	SQBool ret = SQFalse;
	if (getParamCount(v) >= 2) {
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
Object::setDelegate(HSQUIRRELVM v) {
	Object *self = getObject(v, 1);
	if (self) {
		if (self->delegate) {
			delete self->delegate;
			self->delegate = NULL;
		}
		if (getParamCount(v) > 0) {
			self->delegate = new ObjectInfo(v,2);
		}
		return SQ_OK;
	} else {
		return ERROR_BADINSTANCE(v);
	}
}

/**
 * 委譲の設定
 */
SQRESULT
Object::getDelegate(HSQUIRRELVM v) {
	Object *self = getObject(v, 1);
	if (self) {
		if (self->delegate) {
			self->delegate->push(v);
		} else {
			sq_pushnull(v);
		}
		return 1;
	} else {
		return ERROR_BADINSTANCE(v);
	}
}

/**
 * 単一スレッドへのオブジェクト待ちの終了通知用
 */
SQRESULT
Object::notify(HSQUIRRELVM v) {
	Object *self = getObject(v, 1);
	if (self) {
		self->_notify();
		return SQ_OK;
	} else {
		return ERROR_BADINSTANCE(v);
	}
}

/**
 * 全スレッドへのオブジェクト待ちの終了通知
 */
SQRESULT
Object::notifyAll(HSQUIRRELVM v) {
	Object *self = getObject(v, 1);
	if (self) {
		self->_notifyAll();
		return SQ_OK;
	} else {
		return ERROR_BADINSTANCE(v);
	}
}

/**
 * クラスの登録
 * @param v squirrel VM
 */
void
Object::registClass(HSQUIRRELVM v) {

	Object::pushTag(OBJTYPETAG);

	sq_pushroottable(v); // root
	sq_pushstring(v, SQOBJECTNAME, -1);
	sq_newclass(v, false);
	sq_settypetag(v, -1, OBJTYPETAG);
	
	REGISTMETHOD(constructor);
	REGISTMETHOD(_set);
	REGISTMETHOD(_get);
	REGISTMETHOD(hasSetProp);
	REGISTMETHOD(setDelegate);
	REGISTMETHOD(getDelegate);
	REGISTMETHOD(notify);
	REGISTMETHOD(notifyAll);
	sq_createslot(v, -3);
	sq_pop(v,1); // root
};

std::vector<SQUserPointer> Object::tags;

class Thread : public Object {

protected:
	long _currentTick; //< このスレッドの実行時間

	// スレッドデータ
	ObjectInfo _thread;

	// 待ち対象
	std::vector<ObjectInfo> _waitList;
	// 待ち時間
	SQInteger _waitTimeout;

	// 待ちがタイムアウトしたか
	ObjectInfo _waitResult;

	/**
	 * スレッド状態
	 */
	enum ThreadStatus {
		THREAD_STOP,  // 停止
		THREAD_RUN,   // 動作中
		THREAD_WAIT,  // 待ち中
		THREAD_END,   // 終了
	} _status;

	/**
	 * @return 処理待ち中か
	 */
	bool isWait() {
		return _waitList.size() > 0 || _waitTimeout >= 0;
	}

public:

	/**
	 * @return 該当スレッドと現在管理中のスレッドが一致してれば true
	 */
	bool isSameThread(HSQUIRRELVM v) {
		return _thread.isSameThread(v);
	}

	/**
	 * オブジェクトに対する待ち情報を完了させる
	 * @param target 待ち対象
	 * @return 該当オブジェクトを待ってた場合は true
	 */
	bool notifyObject(Object *target) {
		bool find = false;
		std::vector<ObjectInfo>::iterator i = _waitList.begin();
		while (i != _waitList.end()) {
			Object *obj = i->getObject();
			if (obj && obj == target) {
				find = true;
				_waitResult = *i;
				i = _waitList.erase(i);
			} else {
				i++;
			}
		}
		if (find) {
			_clearWait();
		}
		return find;
	}

	/**
	 * トリガに対する待ち情報を完了させる
	 * @param name トリガ名
	 * @return 該当オブジェクトを待ってた場合は true
	 */
	bool notifyTrigger(const SQChar *name) {
		bool find = false;
		std::vector<ObjectInfo>::iterator i = _waitList.begin();
		while (i != _waitList.end()) {
			if (i->isSameString(name)) {
				find = true;
				_waitResult = *i;
				i = _waitList.erase(i);
			} else {
				i++;
			}
		}
		if (find) {
			_clearWait();
		}
		return find;
	}

protected:
	// コンストラクタ
	Thread() : _currentTick(0), _waitTimeout(-1), _status(THREAD_STOP) {
	}
	
	// デストラクタ
	~Thread() {
		_clear();
	}

	/**
	 * オブジェクトに対する待ち情報をクリアする
	 * @param status キャンセルの場合は true
	 */
	void _clearWait() {
		std::vector<ObjectInfo>::iterator i = _waitList.begin();
		while (i != _waitList.end()) {
			Object *obj = i->getObject();
			if (obj) {
				obj->removeWait(this);
			}
			i = _waitList.erase(i);
		}
		_waitTimeout = -1;
	}

	/**
	 * 待ち登録
	 */
	void _wait(HSQUIRRELVM v) {
		_clearWait();
		_waitResult.clear();
		int max = sq_gettop(v);
		for (int i=2;i<=max;i++) {
			switch (sq_gettype(v, 2)) {
			case OT_INTEGER:
			case OT_FLOAT:
				// 数値の場合はタイムアウト待ち
				{
					int timeout;
					sq_getinteger(v, i, &timeout);
					if (timeout >= 0) {
						if (_waitTimeout < 0  || _waitTimeout > timeout) {
							_waitResult.getStack(v, i);
							_waitTimeout = timeout;
						}
					}
				}
				break;
			case OT_STRING:
				// 待ちリストに登録
				_waitList.push_back(ObjectInfo(v,i));
				break;
			case OT_INSTANCE:
				// オブジェクトに待ち登録してから待ちリストに登録
				{
					ObjectInfo o(v,i);
					Object *obj = o.getObject();
					if (obj) {
						obj->addWait(this);
					}
					_waitList.push_back(o);
				}
				break;
			}
		}
	}

	/**
	 * waitのキャンセル
	 */
	void _cancelWait() {
		_clearWait();
		_waitResult.clear();
	}
	
	/**
	 * 情報破棄
	 */
	void _clear() {
		_clearWait();
		_thread.clear();
	}
	
	/**
	 * 実行開始
	 * @param func 実行対象ファンクション。文字列の場合該当スクリプトを読み込む
	 */
	SQRESULT _exec(HSQUIRRELVM v) {
		_clear();

		HSQUIRRELVM testVM = sq_newthread(v, 1024);
		_thread.getStack(v, -1);
		sq_pop(v, 1);
		
		// スレッド先頭にスクリプトをロード
		if (sq_gettype(v, 2) == OT_STRING) {
			SQRESULT result;
			if (SQ_FAILED(result = sqstd_loadfile(_thread, getString(v, 2), SQTrue))) {
				_clear();
				return result;
			}
		} else {
			sq_move(_thread, v, 2);
		}
		_status = THREAD_RUN;
		entryThread(v); // スタックの 1番に元のオブジェクトが入ってる
		return SQ_OK;
	}

	/**
	 * 実行終了
	 */
	void _exit() {
		_clear();
		_notifyAll();
		_status = THREAD_STOP;
	}

	/**
	 * 実行停止
	 */
	void _stop() {
		if (_thread.isThread()) {
			if (_status == THREAD_RUN) {
				_status = THREAD_STOP;
			}
		}
	}

	/**
	 * 実行再開
	 */
	void _run() {
		if (_thread.isThread()) {
			if (_status == THREAD_STOP) {
				_status = THREAD_RUN;
			}
		}
	}

	/**
	 * @return 実行ステータス
	 */
	int getStatus() {
		return !_thread.isThread() ? THREAD_END : isWait() ? THREAD_WAIT : _status;
	}

	/**
	 * スレッドのメイン処理
	 * @param diff 経過時間
	 * @return スレッド実行終了なら true
	 */
	bool _main(long diff) {

		_currentTick += diff;

		// タイムアウト処理
		if (_waitTimeout >= 0) {
			_waitTimeout -= diff;
			if (_waitTimeout < 0) {
				_clearWait();
			}
		}

		// スレッド実行
		if (getStatus() == THREAD_RUN) {
			SQRESULT result;
			if (sq_getvmstate(_thread) == SQ_VMSTATE_SUSPENDED) {
				_waitResult.push(_thread);
				_waitResult.clear();
				result = sq_wakeupvm(_thread, SQTrue, SQFalse, SQTrue);
			} else {
				sq_pushroottable(_thread);
				result = sq_call(_thread, 1, SQFalse, SQTrue);
			}
			if (SQ_FAILED(result)) {
				sq_getlasterror(_thread);
				const SQChar *str;
				if (SQ_SUCCEEDED(sq_getstring(_thread, -1, &str))) {
					PRINT(_thread, str);
				} else {
					PRINT(_thread, _SC("failed to run by unknown reason"));
				}
				sq_pop(_thread, 1);
				_exit();
			} else  if (sq_getvmstate(_thread) == SQ_VMSTATE_IDLE) {
				_exit();
			}
		}
		return getStatus() == THREAD_END;
	}

	// ------------------------------------------------------------------

	/**
	 * @return スレッド情報オブジェクト
	 */
	static Thread *getThread(HSQUIRRELVM v, int idx) {
		SQUserPointer up;
		if (SQ_SUCCEEDED(sq_getinstanceup(v, idx, &up, THREADTYPETAG))) {
			return (Thread*)up;
		}
		return NULL;
	}

	/**
	 * オブジェクトのリリーサ
	 */
	static SQRESULT release(SQUserPointer up, SQInteger size) {
		Thread *self = (Thread*)up;
		delete self;
		return SQ_OK;
	}
	
	/**
	 * オブジェクトのコンストラクタ
	 */
	static SQRESULT constructor(HSQUIRRELVM v) {
		SQRESULT result = SQ_OK;
		Thread *self = new Thread();
		if (SQ_SUCCEEDED(result = sq_setinstanceup(v, 1, self))) {
			sq_setreleasehook(v, 1, release);
		} else {
			delete self;
		}
		if (SQ_SUCCEEDED(result)) {
			if (getParamCount(v) > 1) {
				self->delegate = new ObjectInfo(v, 3);
			}
			if (getParamCount(v) > 0) {
				result = self->_exec(v);
			}
		}
		return result;
	}

	// ------------------------------------------------------
	// プロパティ
	// ------------------------------------------------------
	
	static SQRESULT getCurrentTick(HSQUIRRELVM v) {
		Thread *self = getThread(v, 1);
		if (self) {
			sq_pushinteger(v, self->_currentTick);
			return 1;
		} else {
			return ERROR_BADINSTANCE(v);
		}
	}

	static SQRESULT getStatus(HSQUIRRELVM v) {
		Thread *self = getThread(v, 1);
		if (self) {
			sq_pushinteger(v, self->getStatus());
			return 1;
		} else {
			return ERROR_BADINSTANCE(v);
		}
	}
	
	// ------------------------------------------------------
	// メソッド
	// ------------------------------------------------------

	/**
	 * 処理を開始する
	 * @param func スレッドで実行するファンクション
	 */
	static SQRESULT exec(HSQUIRRELVM v) {
		Thread *self = getThread(v, 1);
		if (self) {
			return self->_exec(v);
		} else {
			return ERROR_BADINSTANCE(v);
		}
	}

	/**
	 * 処理を中断する
	 */
	static SQRESULT exit(HSQUIRRELVM v) {
		Thread *self = getThread(v, 1);
		if (self) {
			self->_exit();
			return 0;
		} else {
			return ERROR_BADINSTANCE(v);
		}
	}
	
	/**
	 * 処理を一時停止する
	 */
	static SQRESULT stop(HSQUIRRELVM v) {
		Thread *self = getThread(v, 1);
		if (self) {
			self->_stop();
			return 0;
		} else {
			return ERROR_BADINSTANCE(v);
		}
	}

	/**
	 * 処理を再開する
	 */
	static SQRESULT run(HSQUIRRELVM v) {
		Thread *self = getThread(v, 1);
		if (self) {
			self->_run();
			return 0;
		} else {
			return ERROR_BADINSTANCE(v);
		}
	}

	/**
	 * 処理待ち
	 */
	static SQRESULT wait(HSQUIRRELVM v) {
		Thread *self = getThread(v, 1);
		if (self) {
			self->_wait(v);
			return SQ_OK;
		} else {
			return ERROR_BADINSTANCE(v);
		}
	}

	/**
	 * 処理待ち
	 */
	static SQRESULT cancelWait(HSQUIRRELVM v) {
		Thread *self = getThread(v, 1);
		if (self) {
			self->_cancelWait();
			return SQ_OK;
		} else {
			return ERROR_BADINSTANCE(v);
		}
	}

public:
	/**
	 * クラスの登録
	 * @param v squirrel VM
	 */
	static void registClass(HSQUIRRELVM v) {

		Object::pushTag(THREADTYPETAG);

		sq_pushroottable(v); // root
		sq_pushstring(v, SQTHREADNAME, -1);
		sq_pushstring(v, SQOBJECTNAME, -1);
		sq_get(v,-3);
		sq_newclass(v, true); // 継承する
		sq_settypetag(v, -1, THREADTYPETAG);

		REGISTENUM(STOP, THREAD_STOP);
		REGISTENUM(RUN, THREAD_RUN);
		REGISTENUM(WAIT, THREAD_WAIT);
		REGISTENUM(END, THREAD_END);
		REGISTMETHOD(constructor);
		REGISTMETHOD(getCurrentTick);
		REGISTMETHOD(getStatus);
		REGISTMETHOD(exec);
		REGISTMETHOD(exit);
		REGISTMETHOD(stop);
		REGISTMETHOD(run);
		REGISTMETHOD(wait);
		REGISTMETHOD(cancelWait);
		sq_createslot(v, -3);
		sq_pop(v, 1); // root
	};
	
	// -------------------------------------------------------------
	//
	// グローバルスレッド制御用諸機能
	//
	// -------------------------------------------------------------
	
protected:
	static std::vector<ObjectInfo> threadList; //< スレッド一覧
	static bool first;        //< 初回呼び出し扱い
	static long prevTick;     //< 前回の呼び出し時間
	static long currentTick;  //< 今回の呼び出し時間

	/**
	 * スレッド実行開始登録
	 * @param v Squirrelスタック 1番にスレッドオブジェクトがはいってるはず
	 */
	static void entryThread(HSQUIRRELVM v) {
		if (threadList.size() == 0) {
			sqobject_start();
			first = true;
		}
		threadList.push_back(ObjectInfo(v,1));
	}

	// -------------------------------------------------------------
	// グローバルメソッド用
	// -------------------------------------------------------------

	/**
	 * 現在時刻の取得
	 */
	static SQRESULT global_getCurrentTick(HSQUIRRELVM v) {
		sq_pushinteger(v, currentTick);
		return 1;
	}

	static SQRESULT ERROR_NOTHREAD(HSQUIRRELVM v) {
		return sq_throwerror(v, _SC("no thread"));
	}
	
	/*
	 * @return 現在のスレッドを返す
	 */
	static SQRESULT global_getCurrentThread(HSQUIRRELVM v) {
		std::vector<ObjectInfo>::iterator i = threadList.begin();
		while (i != threadList.end()) {
			Thread *th = i->getThread();
			if (th && th->isSameThread(v)) {
				i->push(v);
				return 1;
			}
			i++;
		}
		return ERROR_NOTHREAD(v);
	}
	
	/*
	 * @return 現在のスレッド一覧を返す
	 */
	static SQRESULT global_getThreadList(HSQUIRRELVM v) {
		sq_newarray(v, 0);
		std::vector<ObjectInfo>::const_iterator i = threadList.begin();
		while (i != threadList.end()) {
			i->push(v);
			sq_arrayappend(v, -2);
			i++;
		}
		return 1;
	}

	/*
	 * スクリプトを新しいスレッドとして実行する
	 * ※ return Thread(func); 相当
	 * @param func スレッドで実行するファンクション
	 * @return 新スレッド
	 */
	static SQRESULT global_fork(HSQUIRRELVM v) {
		//dm("スレッドを生成!");
		SQRESULT result;
		sq_pushroottable(v); // root
		sq_pushstring(v, SQTHREADNAME, -1);
		if (SQ_SUCCEEDED(result = sq_get(v,-2))) { // class
			sq_pushroottable(v); // 引数:self
			sq_push(v, 2);       // 引数:func
			if (SQ_SUCCEEDED(result = sq_call(v, 2, SQTrue, SQTrue))) { // コンストラクタ呼び出し
				sq_remove(v, -2); // class, root
				return 1;
			} else {
				sq_pop(v, 1); // class
			}
		}
		sq_pop(v,1); // root
		return result;
	}

	/**
	 * @return 現在実行中のスレッド情報オブジェクト(Thread*)
	 */
	static Thread *getCurrentThread(HSQUIRRELVM v) {
		std::vector<ObjectInfo>::iterator i = threadList.begin();
		while (i != threadList.end()) {
			Thread *th = i->getThread();
			if (th && th->isSameThread(v)) {
				return th;
			}
			i++;
		}
		return NULL;
	}
	
	/**
	 * スクリプトを切り替える
	 * @param func スレッドで実行するファンクション
	 */
	static SQRESULT global_exec(HSQUIRRELVM v) {
		Thread *th = getCurrentThread(v);
		if (th) {
			SQRESULT result;
			if (SQ_FAILED(result = th->_exec(v))) {
				return result;
			} else {
				return sq_suspendvm(v);
			}
		} else {
			return ERROR_NOTHREAD(v);
		}
	}

	/**
	 * 実行中スレッドの終了
	 */
	static SQRESULT global_exit(HSQUIRRELVM v) {
		Thread *th = getCurrentThread(v);
		if (th) {
			th->_exit();
			return sq_suspendvm(v);
		} else {
			return ERROR_NOTHREAD(v);
		}
	}

	/**
	 * 実行中スレッドの処理待ち
	 * @param target int:時間待ち(ms), string:トリガ待ち, obj:オブジェクト待ち
	 * @param timeout タイムアウト(省略時は無限に待つ)
	 * @return 待ちがキャンセルされたら true
	 */
	static SQRESULT global_wait(HSQUIRRELVM v) {
		Thread *th = getCurrentThread(v);
		if (th) {
			th->_wait(v);
			return sq_suspendvm(v);
		} else {
			return ERROR_NOTHREAD(v);
		}
	}

	/**
	 * 全スレッドへのトリガ通知
	 * @param name 処理待ちトリガ名
	 */
	static SQRESULT global_trigger(HSQUIRRELVM v) {
		const SQChar *name = getString(v, 2);
		std::vector<ObjectInfo>::iterator i = threadList.begin();
		while (i != threadList.end()) {
			Thread *th = i->getThread();
			if (th) {
				th->notifyTrigger(name);
			}
			i++;
		}
		return SQ_OK;
	}

public:
	/*
	 * グローバルメソッドの登録
	 * @param v squirrel VM
	 */
	static void registGlobal(HSQUIRRELVM v) {
		sq_pushroottable(v); // root
		REGISTMETHODNAME(getCurrentTick, global_getCurrentTick);
		REGISTMETHODNAME(getCurrentThread, global_getCurrentThread);
		REGISTMETHODNAME(getThreadList, global_getThreadList);
		REGISTMETHODNAME(fork, global_fork);
		REGISTMETHODNAME(exec, global_exec);
		REGISTMETHODNAME(exit, global_exit);
		REGISTMETHODNAME(wait, global_wait);
		REGISTMETHODNAME(notify, global_trigger);
		sq_pop(v, 1); // root
	}
	
public:

	/*
	 * 実行処理メインループ
	 * @param tick tick値
	 * 現在存在するスレッドを総なめで１度だけ実行する。
	 * システム本体のメインループ(イベント処理＋画像処理)
	 * から1度だけ呼び出すことで機能する。それぞれのスレッドは、
	 * 自分から明示的に suspend() または wait系のメソッドを呼び出して処理を
	 * 次のスレッドに委譲する必要がある。
	 * @return 
	 */
	static int main(long tick) {
		//dm("tick:" + tick + "\n");
		if (first) {
			prevTick = tick;
			first = false;
		}
		currentTick = tick;
		long diff = tick - prevTick;
		std::vector<ObjectInfo>::iterator i = threadList.begin();
		while (i != threadList.end()) {
			Thread *th = i->getThread();
			if (!th || th->_main(diff)) {
				i = threadList.erase(i);
			} else {
				i++;
			}
		}
		prevTick = tick;
		return threadList.size();
	};


	static void done() {
		threadList.clear();
	}
};

std::vector<ObjectInfo> Thread::threadList; //< スレッド一覧
bool Thread::first;        //< 初回呼び出し扱い
long Thread::prevTick;     //< 前回の呼び出し時間
long Thread::currentTick;  //< 今回の呼び出し時間

/**
 * このオブジェクトを待っている１スレッドの待ちを解除
 */
void
Object::_notify() {
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
Object::_notifyAll()
{
	std::vector <Thread *>::iterator i = _waitThreadList.begin();
	while (i != _waitThreadList.end()) {
		(*i)->notifyObject(this);
		i = _waitThreadList.erase(i);
	}
}

};

/**
 * オブジェクト/スレッド処理の初期化
 * @param v squirrel VM
 */
void
sqobject_init(HSQUIRRELVM v)
{
	sqobject::Object::registClass(v);
	sqobject::Thread::registClass(v);
	sqobject::Thread::registGlobal(v);
}

/**
 * オブジェクト/スレッド処理メイン部分
 * @param tick tick値
 */
int
sqobject_main(int tick)
{
	return sqobject::Thread::main(tick);
}

/**
 * オブジェクト/スレッド処理終了処理
 */
void
sqobject_done()
{
	sqobject::Thread::done();
}
