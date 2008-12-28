/**
 * squirrel 用基底オブジェクト＋疑似スレッド
 */
#include <ctype.h>
#include <squirrel.h>
#include <sqstdio.h>
#include <string>
#include <new>
#include <vector>

// ログ出力用
extern void printFunc(HSQUIRRELVM v, const SQChar* format, ...);

#include <tchar.h>
#if _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

//#define USEUD

// ループ実行開始
extern void sqobject_start();
// ループ実行終了
extern void sqobject_stop();

// 型名
#define OBJECTNAME _SC("Object")
#define THREADNAME _SC("Thread")

// メソッド登録
#define REGISTMETHOD(name) \
	sq_pushstring(v, _SC(#name), -1);\
	sq_newclosure(v, name, 0);\
	sq_createslot(v, -3);

// メソッド登録（名前つき）
#define REGISTMETHODNAME(name, method) \
	sq_pushstring(v, _SC(#name), -1);\
	sq_newclosure(v, method, 0);\
	sq_createslot(v, -3);

// メソッド登録
#define REGISTENUM(name) \
	sq_pushstring(v, _SC(#name), -1);\
	sq_pushinteger(v, name);\
	sq_createslot(v, -3);

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
 * 数値取得用
 * @param v VM
 * @param idx インデックス
 * @param defValue デフォルト値
 * @return 整数値
 */
static int getInt(HSQUIRRELVM v, int idx, int defValue) {
	if (sq_gettop(v) > idx) {
		switch (sq_gettype(v, idx)) {
		case OT_INTEGER:
		case OT_FLOAT:
			{
				int ret;
				sq_getinteger(v, idx, &ret);
				return ret;
			}
		}
	}
	return defValue;
};

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

static const SQUserPointer OBJTYPETAG = (SQUserPointer)"OBJTYPETAG";
static const SQUserPointer THREADTYPETAG = (SQUserPointer)"THREADTYPETAG";

class MyObject;
class MyThread;

/**
 * squirrel オブジェクト保持用クラス
 */
class SQObjectInfo {

protected:
	HSQUIRRELVM v; // オブジェクトの属していたVM
	HSQOBJECT obj; // オブジェクト参照情報

public:
	// 内容消去
	void clear() {
		if (v) {
			sq_release(v,&obj);
			sq_resetobject(&obj);
			v = NULL;
		}
	}

	// スタックから取得
	void getStack(HSQUIRRELVM v, int idx) {
		clear();
		this->v = v;
		sq_getstackobj(v, idx, &obj);
		sq_addref(v, &obj);
	}
	
	// コンストラクタ
	SQObjectInfo() : v(NULL) {
		sq_resetobject(&obj);
	}

	// コンストラクタ
	SQObjectInfo(HSQUIRRELVM v, int idx) : v(v) {
		sq_resetobject(&obj);
		sq_getstackobj(v, idx, &obj);
		sq_addref(v, &obj);
	}

	// コピーコンストラクタ
	SQObjectInfo(const SQObjectInfo &orig) {
		v   = orig.v;
		obj = orig.obj;
		if (v) {
			sq_addref(v, &obj);
		}
	}

	// 代入
	SQObjectInfo & operator=(const SQObjectInfo &orig) {
		clear();
		v = orig.v;
		obj = orig.obj;
		if (v) {
			sq_addref(v, &obj);
		}
		return *this;
	}
	
	// デストラクタ
	virtual ~SQObjectInfo() {
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
	
	// インスタンスユーザポインタを取得
	SQUserPointer getInstanceUserPointer(const SQUserPointer tag) {
		SQUserPointer up = NULL;
		if (v && sq_isinstance(obj)) {
			sq_pushobject(v, obj);
			if (!SQ_SUCCEEDED(sq_getinstanceup(v, -1, &up, tag))) {
				up = NULL;
			}
			sq_pop(v, 1);
		}
		return up;
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
	MyThread *getMyThread() {
		SQUserPointer up = NULL;
		if (v && sq_isinstance(obj)) {
			sq_pushobject(v, obj);
			if (!SQ_SUCCEEDED(sq_getinstanceup(v, -1, &up, THREADTYPETAG))) {
				up = NULL;
			}
			sq_pop(v, 1);
		}
		return (MyThread*)up;
	}

	// インスタンスユーザポインタを取得
	MyObject *getMyObject() {
		SQUserPointer up = NULL;
		if (v && sq_isinstance(obj)) {
			sq_pushobject(v, obj);
			if (!SQ_SUCCEEDED(sq_getinstanceup(v, -1, &up, OBJTYPETAG)) ||
				!SQ_SUCCEEDED(sq_getinstanceup(v, -1, &up, THREADTYPETAG))) {
				up = NULL;
			}
			sq_pop(v, 1);
		}
		return (MyObject*)up;
	}
	
	// ---------------------------------------------------
	// wait処理用メソッド
	// ---------------------------------------------------

	// MyObjectか？
	bool isMyObject() {
		bool ret = false;
		if (sq_isinstance(obj)) {
			SQUserPointer typetag;
			sq_pushobject(v, obj);
			if (SQ_SUCCEEDED(sq_gettypetag(v, -1, &typetag))) {
				ret = (typetag == OBJTYPETAG ||
					   typetag == THREADTYPETAG);
			}
			sq_pop(v, 1);
		}
		return ret;
	}

	bool isString() const {
		return sq_isstring(obj);
	}

	bool isSameString(const SQChar *str) const {
		if (str && sq_isstring(obj)) {
			const SQChar *mystr;
			sq_pushobject(v, obj);
			sq_getstring(v, -1, &mystr);
			sq_pop(v, 1);
			return mystr && _tcscmp(str, mystr) == 0;
		}
		return false;
	}

	bool isNumeric() const {
		return sq_isnumeric(obj) != 0;
	}
};

/**
 * オブジェクト用
 */
class MyObject {

protected:
	// このオブジェクトを待ってるスレッドの一覧
	std::vector<MyThread*> _waitThreadList;
	// delegate
	SQObjectInfo delegate;

public:
	/**
	 * オブジェクト待ちの登録
	 * @param thread スレッド
	 */
	void addWait(MyThread *thread) {
		_waitThreadList.push_back(thread);
	}

	/**
	 * オブジェクト待ちの解除
	 * @param thread スレッド
	 */
	void removeWait(MyThread *thread) {
		std::vector<MyThread*>::iterator i = _waitThreadList.begin();
		while (i != _waitThreadList.end()) {
			if (*i == thread) {
				i = _waitThreadList.erase(i);
			} else {
				i++;	
			}
		}
	}

protected:

	/**
	 * コンストラクタ
	 */
	MyObject() {
	}

	/**
	 * デストラクタ
	 */
	virtual ~MyObject() {
		_notifyAll();
	}

	/**
	 * このオブジェクトを待っている１スレッドの待ちを解除
	 */
	void _notify();
	
	/**
	 * このオブジェクトを待っている全スレッドの待ちを解除
	 */
	void _notifyAll();

	// ------------------------------------------------------------------

	/**
	 * @return オブジェクト情報オブジェクト
	 */
	static MyObject *getObject(HSQUIRRELVM v, int idx) {
		SQUserPointer up;
#ifdef USEUD
		SQUserPointer typetag;
		if (SQ_SUCCEEDED(sq_getuserdata(v, idx, &up, &typetag)) &&
			(typetag == THREADTYPETAG || typetag == OBJTYPETAG)) {
			return (MyObject*)up;
		}
#else
		if (SQ_SUCCEEDED(sq_getinstanceup(v, idx, &up, THREADTYPETAG)) ||
			SQ_SUCCEEDED(sq_getinstanceup(v, idx, &up, OBJTYPETAG))) {
			return (MyObject*)up;
		}
#endif
		return NULL;
	}

	/**
	 * オブジェクトのリリーサ
	 */
	static SQInteger release(SQUserPointer up, SQInteger size) {
		MyObject *self = (MyObject*)up;
#ifdef USEUD
		self->~MyObject();
#else
		delete self;
#endif
		return SQ_OK;
	}

	static SQInteger ERROR_CREATE(HSQUIRRELVM v) {
		return sq_throwerror(v, _SC("can't create native instance"));
	}
	
	/**
	 * オブジェクトのコンストラクタ
	 */
	static SQInteger constructor(HSQUIRRELVM v) {
		SQInteger result = SQ_OK;
#ifdef USEUD
		MyObject *self = getObject(v, 1);
		if (self) {
			new (self) MyObject();
			result = sq_setreleasehook(v, 1, release);
		} else {
			result = ERROR_CREATE(v);
		}
#else
		MyObject *self = new MyObject();
		if (SQ_SUCCEEDED(result = sq_setinstanceup(v, 1, self))) {;
			sq_setreleasehook(v, 1, release);
		}
#endif
		if (SQ_SUCCEEDED(result)) {
			if (getParamCount(v) > 0) {
				self->delegate.getStack(v, 2);
			}
		}
		return result;
	}

	static SQInteger ERROR_NOMEMBER(HSQUIRRELVM v) {
		return sq_throwerror(v, _SC("no such member"));
	}

	static bool isClosure(SQObjectType type) {
		return type == OT_CLOSURE || type == OT_NATIVECLOSURE;
	}
	
	/**
	 * プロパティから値を取得
	 * @param name プロパティ名
	 * @return プロパティ値
	 */
	static SQInteger _get(HSQUIRRELVM v) {
		SQInteger result = SQ_OK;
		const SQChar *name = getString(v, 2);
		if (name && *name) {

			// delegateの参照
			MyObject *self = getObject(v, 1);
			if (self && self->delegate.isDelegate()) {
				self->delegate.push(v);
				sq_pushstring(v, name, -1);
				if (SQ_SUCCEEDED(result = sq_get(v,-2))) {
					// メソッドの場合は束縛処理
					if (isClosure(sq_gettype(v,-1)) && self->delegate.isBindDelegate()) {
						self->delegate.push(v);
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
			tstring name2 = _SC("get");
			name2 += toupper(*name);
			name2 += (name + 1);
			sq_push(v, 1); // self
			sq_pushstring(v, name2.c_str(), -1);
			if (SQ_SUCCEEDED(result = sq_get(v,-2))) {
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
	static SQInteger _set(HSQUIRRELVM v) {
		SQInteger result = SQ_OK;
		const SQChar *name = getString(v, 2);
		if (name && *name) {

			// delegateの参照
			MyObject *self = getObject(v, 1);
			if (self && self->delegate.isDelegate()) {
				self->delegate.push(v);
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
			tstring name2 = _SC("set");
			name2 += toupper(*name);
			name2 += (name + 1);
			sq_push(v, 1); // self
			sq_pushstring(v, name2.c_str(), -1);
			if (SQ_SUCCEEDED(result = sq_get(v,-2))) {
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
	static SQInteger hasSetProp(HSQUIRRELVM v) {
		SQInteger result = SQ_OK;
		SQBool ret = SQFalse;
		if (getParamCount(v) >= 2) {
			const SQChar *name = getString(v, 2);
			if (name && *name) {
				tstring name2 = _SC("set");
				name2 += toupper(*name);
				name2 += (name + 1);
				sq_push(v, 1); // object
				sq_pushstring(v, name2.c_str(), -1);
				if (SQ_SUCCEEDED(result = sq_get(v,-2))) {
					sq_pop(v,1);
					ret = SQTrue;
				} else {
					sq_pushstring(v, name, -1);
					if (SQ_SUCCEEDED(result = sq_get(v,-2))) {
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
	
	static SQInteger ERROR_BADINSTANCE(HSQUIRRELVM v) {
		return sq_throwerror(v, _SC("bad instance"));
	}

	/**
	 * 委譲の設定
	 */
	static SQInteger setDelegate(HSQUIRRELVM v) {
		MyObject *self = getObject(v, 1);
		if (self) {
			if (getParamCount(v) > 0) {
				self->delegate.getStack(v, 2);
			} else {
				self->delegate.clear();
			}
			return SQ_OK;
		} else {
			return ERROR_BADINSTANCE(v);
		}
	}
	
	/**
	 * 単一スレッドへのオブジェクト待ちの終了通知用
	 */
	static SQInteger notify(HSQUIRRELVM v) {
		MyObject *self = getObject(v, 1);
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
	static SQInteger notifyAll(HSQUIRRELVM v) {
		MyObject *self = getObject(v, 1);
		if (self) {
			self->_notifyAll();
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
		sq_pushroottable(v); // root
		sq_pushstring(v, OBJECTNAME, -1);
		sq_newclass(v, false);
		sq_settypetag(v, -1, OBJTYPETAG);
#ifdef USEUD
		sq_setclassudsize(v, -1, sizeof MyObject);
#endif
		REGISTMETHOD(constructor);
		REGISTMETHOD(_set);
		REGISTMETHOD(_get);
		REGISTMETHOD(hasSetProp);
		REGISTMETHOD(setDelegate);
		REGISTMETHOD(notify);
		REGISTMETHOD(notifyAll);
		sq_createslot(v, -3);
		sq_pop(v,1); // root
	};
};

class MyThread : public MyObject {

protected:
	long _currentTick; //< このスレッドの実行時間

	// スレッドデータ
	SQObjectInfo _thread;

	// 待ち対象
	std::vector<SQObjectInfo> _waitList;
	// 待ち時間
	SQInteger _waitTimeout;

	// 待ちがタイムアウトしたか
	SQObjectInfo _waitResult;

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
	bool notifyObject(MyObject *target) {
		bool find = false;
		std::vector<SQObjectInfo>::iterator i = _waitList.begin();
		while (i != _waitList.end()) {
			MyObject *obj = i->getMyObject();
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
		std::vector<SQObjectInfo>::iterator i = _waitList.begin();
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
	MyThread() : _currentTick(0), _waitTimeout(-1), _status(THREAD_STOP) {
	}
	
	// デストラクタ
	~MyThread() {
		_clear();
	}

	/**
	 * オブジェクトに対する待ち情報をクリアする
	 * @param status キャンセルの場合は true
	 */
	void _clearWait() {
		std::vector<SQObjectInfo>::iterator i = _waitList.begin();
		while (i != _waitList.end()) {
			MyObject *obj = i->getMyObject();
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
	SQInteger _wait(HSQUIRRELVM v) {
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
			case OT_INSTANCE:
				// 文字列またはインスタンスの場合は待ちリストに登録
				_waitList.push_back(SQObjectInfo(v,i));
				break;
			}
		}
		return sq_suspendvm(v);
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
	SQInteger _exec(HSQUIRRELVM v) {
		_clear();

		HSQUIRRELVM testVM = sq_newthread(v, 1024);
		_thread.getStack(v, -1);
		sq_pop(v, 1);
		
		// スレッド先頭にスクリプトをロード
		if (sq_gettype(v, 2) == OT_STRING) {
			SQInteger result;
			if (SQ_FAILED(result = sqstd_loadfile(_thread, getString(v, 2), SQTrue))) {
				_clear();
				return result;
			}
		} else {
			sq_move(_thread, v, 2);
		}
		_currentTick = 0;
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

		//dm("スレッド実行:" + this);
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
			SQInteger result;
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
					printFunc(_thread, str);
				} else {
					printFunc(_thread, _SC("failed to run by unknown reason"));
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
	static MyThread *getThread(HSQUIRRELVM v, int idx) {
		SQUserPointer up;
#ifdef USEUD
		SQUserPointer typetag;
		if (SQ_SUCCEEDED(sq_getuserdata(v, idx, &up, &typetag)) &&
			(typetag == THREADTYPETAG))) {
			return (MyObject*)up;
		}
#else
		if (SQ_SUCCEEDED(sq_getinstanceup(v, idx, &up, THREADTYPETAG))) {
			return (MyThread*)up;
		}
#endif
		return NULL;
	}

	/**
	 * オブジェクトのリリーサ
	 */
	static SQInteger release(SQUserPointer up, SQInteger size) {
		MyThread *self = (MyThread*)up;
#ifdef USEUD
		self->~MyThread();
#else
		delete self;
#endif
		return SQ_OK;
	}
	
	/**
	 * オブジェクトのコンストラクタ
	 */
	static SQInteger constructor(HSQUIRRELVM v) {
		SQInteger result = SQ_OK;
#ifdef USEUD
		MyThread *self = getThread(v, 1);
		if (self) {
			new (self) MyThread();
			result = sq_setreleasehook(v, 1, release);
		} else {
			result = ERROR_CREATE(v);
		}
#else
		MyThread *self = new MyThread();
		if (SQ_SUCCEEDED(result = sq_setinstanceup(v, 1, self))) {
			sq_setreleasehook(v, 1, release);
		}
#endif
		if (SQ_SUCCEEDED(result)) {
			if (getParamCount(v) > 1) {
				self->delegate.getStack(v, 3);
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
	
	static SQInteger getCurrentTick(HSQUIRRELVM v) {
		MyThread *self = getThread(v, 1);
		if (self) {
			sq_pushinteger(v, self->_currentTick);
			return 1;
		} else {
			return ERROR_BADINSTANCE(v);
		}
	}

	static SQInteger getStatus(HSQUIRRELVM v) {
		MyThread *self = getThread(v, 1);
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
	static SQInteger exec(HSQUIRRELVM v) {
		MyThread *self = getThread(v, 1);
		if (self) {
			return self->_exec(v);
		} else {
			return ERROR_BADINSTANCE(v);
		}
	}

	/**
	 * 処理を中断する
	 */
	static SQInteger exit(HSQUIRRELVM v) {
		MyThread *self = getThread(v, 1);
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
	static SQInteger stop(HSQUIRRELVM v) {
		MyThread *self = getThread(v, 1);
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
	static SQInteger run(HSQUIRRELVM v) {
		MyThread *self = getThread(v, 1);
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
	static SQInteger wait(HSQUIRRELVM v) {
		MyThread *self = getThread(v, 1);
		if (self) {
			return self->_wait(v);
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
		sq_pushroottable(v); // root
		sq_pushstring(v, THREADNAME, -1);
		sq_pushstring(v, OBJECTNAME, -1);
		sq_get(v,-3);
		sq_newclass(v, true); // 継承する
		sq_settypetag(v, -1, THREADTYPETAG);
#ifdef USEUD
		sq_setclassudsize(v, -1, sizeof MyThread);
#endif
		REGISTENUM(THREAD_STOP);
		REGISTENUM(THREAD_RUN);
		REGISTENUM(THREAD_WAIT);
		REGISTENUM(THREAD_END);
		REGISTMETHOD(constructor);
		REGISTMETHOD(getCurrentTick);
		REGISTMETHOD(getStatus);
		REGISTMETHOD(exec);
		REGISTMETHOD(exit);
		REGISTMETHOD(stop);
		REGISTMETHOD(run);
		REGISTMETHOD(wait);
		sq_createslot(v, -3);
		sq_pop(v, 1); // root
	};
	
	// -------------------------------------------------------------
	//
	// グローバルスレッド制御用諸機能
	//
	// -------------------------------------------------------------
	
protected:
	static std::vector<SQObjectInfo> threadList; //< スレッド一覧
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
		}
		threadList.push_back(SQObjectInfo(v,1));
		first = true;
	}

	// -------------------------------------------------------------
	// グローバルメソッド用
	// -------------------------------------------------------------

	/**
	 * 現在時刻の取得
	 */
	static SQInteger global_getCurrentTick(HSQUIRRELVM v) {
		sq_pushinteger(v, currentTick);
		return 1;
	}

	static SQInteger ERROR_NOTHREAD(HSQUIRRELVM v) {
		return sq_throwerror(v, _SC("no thread"));
	}
	
	/*
	 * @return 現在のスレッドを返す
	 */
	static SQInteger global_getCurrentThread(HSQUIRRELVM v) {
		std::vector<SQObjectInfo>::iterator i = threadList.begin();
		while (i != threadList.end()) {
			MyThread *th = i->getMyThread();
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
	static SQInteger global_getThreadList(HSQUIRRELVM v) {
		sq_newarray(v, 0);
		std::vector<SQObjectInfo>::const_iterator i = threadList.begin();
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
	static SQInteger global_fork(HSQUIRRELVM v) {
		//dm("スレッドを生成!");
		SQInteger result;
		sq_pushroottable(v); // root
		sq_pushstring(v, THREADNAME, -1);
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
	 * @return 現在実行中のスレッド情報オブジェクト(MyThread*)
	 */
	static MyThread *getCurrentThread(HSQUIRRELVM v) {
		std::vector<SQObjectInfo>::iterator i = threadList.begin();
		while (i != threadList.end()) {
			MyThread *th = i->getMyThread();
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
	static SQInteger global_exec(HSQUIRRELVM v) {
		MyThread *th = getCurrentThread(v);
		if (th) {
			SQInteger ret = th->_exec(v);
			if (ret) {
				return ret;
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
	static SQInteger global_exit(HSQUIRRELVM v) {
		MyThread *th = getCurrentThread(v);
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
	static SQInteger global_wait(HSQUIRRELVM v) {
		MyThread *th = getCurrentThread(v);
		if (th) {
			return th->_wait(v);
		} else {
			return ERROR_NOTHREAD(v);
		}
	}

	/**
	 * 全スレッドへのトリガ通知
	 * @param name 処理待ちトリガ名
	 */
	static SQInteger global_trigger(HSQUIRRELVM v) {
		const SQChar *name = getString(v, 2);
		std::vector<SQObjectInfo>::iterator i = threadList.begin();
		while (i != threadList.end()) {
			MyThread *th = i->getMyThread();
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
	 */
	static void main(long tick) {
		//dm("tick:" + tick + "\n");
		if (first) {
			prevTick = tick;
			first = false;
		}
		currentTick = tick;
		long diff = tick - prevTick;
		std::vector<SQObjectInfo>::iterator i = threadList.begin();
		while (i != threadList.end()) {
			MyThread *th = i->getMyThread();
			if (!th || th->_main(diff)) {
				i = threadList.erase(i);
			} else {
				i++;
			}
		}
		if (threadList.size() == 0) {
			sqobject_stop();
		}
		prevTick = tick;
	};

};

std::vector<SQObjectInfo> MyThread::threadList; //< スレッド一覧
bool MyThread::first;        //< 初回呼び出し扱い
long MyThread::prevTick;     //< 前回の呼び出し時間
long MyThread::currentTick;  //< 今回の呼び出し時間

/**
 * このオブジェクトを待っている１スレッドの待ちを解除
 */
void
MyObject::_notify() {
	std::vector <MyThread *>::iterator i = _waitThreadList.begin();
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
MyObject::_notifyAll()
{
	std::vector <MyThread *>::iterator i = _waitThreadList.begin();
	while (i != _waitThreadList.end()) {
		(*i)->notifyObject(this);
		i = _waitThreadList.erase(i);
	}
}

/**
 * オブジェクト/スレッド処理の初期化
 */
void
sqobject_init(HSQUIRRELVM v)
{
	MyObject::registClass(v);
	MyThread::registClass(v);
	MyThread::registGlobal(v);
}

/**
 * オブジェクト/スレッド処理メイン部分
 * @param tick tick値
 */
void
sqobject_main(int tick)
{
	MyThread::main(tick);
}
