#include <stdio.h>
#include <process.h>
#include <string>
#include "ncbind/ncbind.hpp"
#include "sqlite3.h"
#include "sqlite3_xp3_vfs/xp3_vfs.h"


// メッセージコード
#define	WM_SQLITE       (WM_APP+8)
#define	SQLITE_START    0x0100
#define	SQLITE_PROGRESS 0x0200
#define	SQLITE_END      0x0400

/**
 * ステートメント tTJSVariant をバインド
 * @param stmt ステートメント
 * @param variant バインドするデータ
 * @param num バインド位置
 */
static int
bind(sqlite3_stmt *stmt, tTJSVariant &variant, int pos)
{
	pos++;
	switch (variant.Type()) {
	case tvtInteger:
		return sqlite3_bind_int64(stmt, pos, variant);
	case tvtReal:
		return sqlite3_bind_double(stmt, pos, variant);
	case tvtString:
		{
			tTJSVariantString *str = variant.AsStringNoAddRef();
			return sqlite3_bind_text16(stmt, pos, *str, str->GetLength() * sizeof tjs_char, SQLITE_TRANSIENT);
		}
	case tvtOctet:
		{
			tTJSVariantOctet *octet = variant.AsOctetNoAddRef();
			return sqlite3_bind_blob(stmt, pos, octet->GetData(), octet->GetLength(), SQLITE_STATIC);
		}
	default:
		return sqlite3_bind_null(stmt, pos);
	}
}

/**
 * ステートメントから tTJSVariant を取得
 * @param stmt ステートメント
 * @param variant 格納先
 * @param num カラム番号
 */
static void
getColumnData(sqlite3_stmt *stmt, tTJSVariant &variant, int num)
{
	switch (sqlite3_column_type(stmt, num)) {
	case SQLITE_INTEGER:
		variant = sqlite3_column_int64(stmt, num);
		break;
	case SQLITE_FLOAT:
		variant = sqlite3_column_double(stmt, num);
		break;
	case SQLITE_TEXT:
		variant = (const tjs_char *)sqlite3_column_text16(stmt, num);
		break;
	case SQLITE_BLOB:
		variant = tTJSVariant((const tjs_uint8 *) sqlite3_column_blob(stmt, num), sqlite3_column_bytes(stmt, num));
		break;
	default:
		variant.Clear();
		break;
	}
}

/**
 * Sqliteクラス
 */
class Sqlite {

public:
	/**
	 * コンストラクタ
	 * @param database データベースファイル名
	 * @param readonly 読み込み専用
	 */
	Sqlite(const tjs_char *database, bool readonly=false) : db(NULL) {
		if (readonly) {
			// 読み込み専用時は吉里吉里の処理系を使う
			int	len = ::WideCharToMultiByte(CP_UTF8, 0, database, -1, NULL, 0, NULL, NULL);
			if (len > 0) {
				char *buf = new char[len + 1];
				::WideCharToMultiByte(CP_UTF8, 0, database, -1, buf, len, NULL, NULL);
				buf[len] = '\0';
				sqlite3_open_v2(buf, &db, SQLITE_OPEN_READONLY, "xp3");
				delete[] buf;
			}
		} else {
			// そうでない場合はOSの処理系を使う
			ttstr filename = database;
			if (*database == '\0' || *database == ':') {
				// 空文字または ':' で始まる場合は sqlite の特殊な扱い
				sqlite3_open16(database, &db);
			} else {
				filename = TVPNormalizeStorageName(filename);
				if (filename.length() && !wcschr(filename.c_str(), '>')) {
					TVPGetLocalName(filename);
					sqlite3_open16(filename.c_str(), &db);
				}
			}
		}
	}

	/**
	 * デストラクタ
	 */
	~Sqlite() {
		if (db) {
			sqlite3_close(db);
		}
	}

	/**
	 * SQLを実行する
	 */
	static tjs_error exec(tTJSVariant *result, tjs_int numparams, tTJSVariant **params, Sqlite *self) {
		if (numparams < 2) {
			return TJS_E_BADPARAMCOUNT;
		}
		sqlite3_stmt *stmt = NULL;
		int ret;
		if ((ret = sqlite3_prepare16_v2(self->db, params[1]->GetString(), -1, &stmt, NULL)) == SQLITE_OK) {
			sqlite3_reset(stmt);
			// パラメータをバインド
			for (int n=2;n<numparams;n++) {
				::bind(stmt, *params[n], n-2);
			}
			if (params[0]->Type() == tvtObject) {
				tTJSVariantClosure &callback = params[0]->AsObjectClosureNoAddRef();
				// カラム数
				int argc = sqlite3_column_count(stmt);
				// 引数初期化
				tTJSVariant **args = new tTJSVariant*[argc];
				for (int i=0;i<argc;i++) {
					args[i] = new tTJSVariant();
				}
				while ((ret = sqlite3_step(stmt)) == SQLITE_ROW) {
					// 引数に展開
					for (int i=0;i<argc;i++) {
						::getColumnData(stmt, *args[i], i);
					}
					callback.FuncCall(0, NULL, NULL, NULL, argc, args, NULL);
				}
				// 引数破棄
				for (int i=0;i<argc;i++) {
					delete args[i];
				}
				delete[] args;
			} else {
				while ((ret = sqlite3_step(stmt)) == SQLITE_ROW);
			}
			sqlite3_finalize(stmt);
		}
		if (result) {
			*result = ret == SQLITE_OK || ret == SQLITE_DONE;
		}
		return TJS_S_OK;
	}

	/**
	 * 値取得用にSQLを実行する。
	 */
	static tjs_error execValue(tTJSVariant *result, tjs_int numparams, tTJSVariant **params, Sqlite *self) {
		if (numparams < 1) {
			return TJS_E_BADPARAMCOUNT;
		}
		sqlite3_stmt *stmt = NULL;
		if (sqlite3_prepare16_v2(self->db, params[0]->GetString(), -1, &stmt, NULL) == SQLITE_OK) {
			sqlite3_reset(stmt);
			for (int n=1;n<numparams;n++) {
				::bind(stmt, *params[n], n-2);
			}
			int argc = sqlite3_column_count(stmt);
			if (sqlite3_step(stmt) == SQLITE_ROW && argc > 0) {
				if (result) {
					::getColumnData(stmt, *result, 0);
				}
			}
			sqlite3_finalize(stmt);
		}
		return TJS_S_OK;
	}
	
	bool begin() {
		return sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL) == SQLITE_OK;
	}
	bool commit() {
		return sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL) == SQLITE_OK;
	}
	bool rollback() {
		return sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL) == SQLITE_OK;
	}

	int getErrorCode() const {
		return db ? sqlite3_errcode(db) : -1;
	}
	
	ttstr getErrorMessage() const {
		return db ? ttstr((const tjs_char*)sqlite3_errmsg16(db)) : ttstr("database open failed");
	}
	
	/**
	 * インスタンス生成ファクトリ
	 */
	static tjs_error factory(Sqlite **result, tjs_int numparams, tTJSVariant **params, iTJSDispatch2 *objthis) {
		if (numparams < 1) {
			return TJS_E_BADPARAMCOUNT;
		}
		*result = new Sqlite(params[0]->GetString(), numparams > 1 ? (int)*params[1] != 0 : false);
		return TJS_S_OK;
	}
	
	sqlite3 *getDatabase() const {
		return db;
	}
	
private:
	sqlite3 *db;
};

NCB_REGISTER_CLASS(Sqlite) {
	Factory(&ClassT::factory);
	RawCallback(TJS_W("exec"), &Class::exec, 0);
	RawCallback(TJS_W("execValue"), &Class::execValue, 0);
	NCB_METHOD(begin);
	NCB_METHOD(commit);
	NCB_METHOD(rollback);
	NCB_PROPERTY_RO(errorCode, getErrorCode);
	NCB_PROPERTY_RO(errorMessage, getErrorMessage);
};

/**
 * Sqliteのステートメントを扱うクラス
 */
class SqliteStatement {

public:
	/**
	 * コンストラクタ
	 */
	SqliteStatement(tTJSVariant &sqlite) : sqlite(sqlite), db(NULL), stmt(NULL) {
		Sqlite *sq = ncbInstanceAdaptor<Sqlite>::GetNativeInstance(sqlite.AsObjectNoAddRef());
		if (sq) {
			db = sq->getDatabase();
		}
	}

	/**
	 * デストラクタ
	 */
	~SqliteStatement() {
		close();
	}

	static tjs_error open(tTJSVariant *result, tjs_int numparams, tTJSVariant **params, SqliteStatement *self) {
		if (numparams < 1) {
			return TJS_E_BADPARAMCOUNT;
		}
		int ret;
		if ((ret = self->_open(params[0]->GetString())) == SQLITE_OK) {
			ret = self->_bind(params, 1, numparams-1);
		}
		if (result) {
			*result = ret;
		}
		return TJS_S_OK;
	}

	void close() {
		if (stmt) {
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
	}
	
	tTJSString getSql() const {
		ttstr ret;
		const char *sql = sqlite3_sql(stmt);
		if (sql) {
			int	len = ::MultiByteToWideChar(CP_UTF8, 0, sql, -1, NULL, 0);
			if (len > 0) {
				tjs_char *str = ret.AllocBuffer(len);
				::MultiByteToWideChar(CP_UTF8, 0, sql, -1, str, len);
			}
		}
		return ret;
	}

	int reset() {
		bindColumnNo = 0;
		return sqlite3_reset(stmt);
	}

	static tjs_error bind(tTJSVariant *result, tjs_int numparams, tTJSVariant **params, SqliteStatement *self) {
		if (numparams < 1) {
			return TJS_E_BADPARAMCOUNT;
		}
		int ret = self->_bind(params, 0, numparams);
		if (result) {
			*result = ret;
		}
		return TJS_S_OK;
	}
	
	static tjs_error bindAt(tTJSVariant *result, tjs_int numparams, tTJSVariant **params, SqliteStatement *self) {
		if (numparams < 1) {
			return TJS_E_BADPARAMCOUNT;
		}
		if (numparams > 1) {
			int col = self->getColumnNo(*params[1]);
			self->bindColumnNo = col >= 0 ? col : 0;
		}
		int ret = self->_bind(params, 0, 1);
		if (result) {
			*result = ret;
		}
		return TJS_S_OK;
	}

	// 単純実行
	int exec() {
		int ret = sqlite3_step(stmt);
		if (ret != SQLITE_ROW) {
			reset();
		}
		return ret;
	}

	// ステップ実行
	bool step() {
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			return true;
		}
		reset();
		return false;
	}

	int getDataCount() const {
		return sqlite3_data_count(stmt);
	}

	int getCount() const {
		return sqlite3_column_count(stmt);
	}

	bool isNull(tTJSVariant column) const {
		return sqlite3_column_type(stmt, getColumnNo(column)) == SQLITE_NULL;
	}

	int getType(tTJSVariant column) const {
		return sqlite3_column_type(stmt, getColumnNo(column));
	}

	ttstr getName(tTJSVariant column) const {
		return (const tjs_char *)sqlite3_column_name16(stmt, getColumnNo(column));
	}
	
	static tjs_error get(tTJSVariant *result, tjs_int numparams, tTJSVariant **params, SqliteStatement *self) {
		if (numparams < 1) {
			return TJS_E_BADPARAMCOUNT;
		}
		int col = self->getColumnNo(*params[0]);
		if (sqlite3_column_type(self->stmt, col) == SQLITE_NULL) {
			if (numparams > 1) {
				*result = *params[1];
			} else {
				result->Clear();
			}
		} else {
			::getColumnData(self->stmt, *result, col);
		}
		return TJS_S_OK;
	}

	// missing method
	static tjs_error missing(tTJSVariant *result, tjs_int numparams, tTJSVariant **params, SqliteStatement *self) {
		if (numparams < 3) {return TJS_E_BADPARAMCOUNT;};
		bool ret = false;
		if (!(int)*params[0]) {
			int col = self->getColumnNo(*params[1]);
			if (col >= 0) {
				tTJSVariant value;
				::getColumnData(self->stmt, value, col);
				params[2]->AsObjectClosureNoAddRef().PropSet(0, NULL, NULL, &value, NULL);
				ret = true;
			}
		}
		if (result) {
			*result = ret;
		}
		return TJS_S_OK;
	}
	
	/**
	 * インスタンス生成ファクトリ
	 */
	static tjs_error factory(SqliteStatement **result, tjs_int numparams, tTJSVariant **params, iTJSDispatch2 *objthis) {
		if (numparams < 1) {
			return TJS_E_BADPARAMCOUNT;
		}
		if (params[0]->AsObjectClosureNoAddRef().IsInstanceOf(0, NULL, NULL, L"Sqlite", NULL) != TJS_S_TRUE) {
			TVPThrowExceptionMessage(L"InvalidObject");
		}
		SqliteStatement *state = new SqliteStatement(*params[0]);
		if (numparams > 1) {
			if (state->_open(params[1]->GetString()) != SQLITE_OK ||
				(numparams > 2 && state->_bind(params, 2, numparams-2) != SQLITE_OK)) {
				delete state;
				TVPThrowExceptionMessage(L"failed to open state");
				return TJS_E_FAIL;
			}
		}
		tTJSVariant name(TJS_W("missing"));
		objthis->ClassInstanceInfo(TJS_CII_SET_MISSING, 0, &name);
		*result = state;
		return TJS_S_OK;
	}
	
protected:

	/**
	 * カラム番号を取得
	 * @param column カラム指定
	 * @return カラム番号
	 */
	int getColumnNo(const tTJSVariant &column) const {
		switch (column.Type()) {
		case tvtInteger:
		case tvtReal:
			return column;
		case tvtString:
			{
				const tjs_char *col = column.GetString();
				int count = sqlite3_column_count(stmt);
				for (int i=0; i<count; i++) {
					if (_wcsicmp(col, (const tjs_char *)sqlite3_column_name16(stmt, i)) == 0)  {
						return i;
					}
				}
			}
		default:
			return -1;
		}
	}

	/**
	 * sql を開く
	 * @param sql ステートとして開くsql
	 * @return エラーコード
	 */
	int _open(const tjs_char *sql) {
		close();
		int ret;
		if ((ret = sqlite3_prepare16_v2(db, sql, -1, &stmt, NULL)) == SQLITE_OK) {
			reset();
		}
		return ret;
	}
	
	/**
	 * バインド処理
	 * @param params パラメータリスト
	 * @param start パラメータ開始位置
	 * @oaram count バインドする個数
	 * @return エラーコード
	 */
	int _bind(tTJSVariant **params, int start, int count) {
		int ret = SQLITE_OK;
		while (count > 0 && ret == SQLITE_OK) {
			ret = ::bind(stmt, *params[start++], bindColumnNo++);
			count--;
		}
		return ret;
	}
	
private:
	tTJSVariant sqlite;
	sqlite3 *db;
	sqlite3_stmt *stmt;
	int bindColumnNo;
};

#define ENUM(n) Variant(#n, (int)n)

NCB_REGISTER_CLASS(SqliteStatement) {
	ENUM(SQLITE_INTEGER);
	ENUM(SQLITE_FLOAT);
	ENUM(SQLITE_TEXT);
	ENUM(SQLITE_BLOB);
	ENUM(SQLITE_NULL);
	Factory(&ClassT::factory);
	RawCallback(TJS_W("open"), &Class::open, 0);
	NCB_METHOD(close);
	NCB_PROPERTY_RO(sql, getSql);
	NCB_METHOD(reset);
	RawCallback(TJS_W("bind"), &Class::bind, 0);
	RawCallback(TJS_W("bindAt"), &Class::bindAt, 0);
	NCB_METHOD(exec);
	NCB_METHOD(step);
	NCB_PROPERTY_RO(dataCount, getDataCount);
	NCB_PROPERTY_RO(count, getCount);
	NCB_METHOD(isNull);
	NCB_METHOD(getType);
	NCB_METHOD(getName);
	RawCallback(TJS_W("get"), &Class::get, 0);
	RawCallback(TJS_W("missing"), &Class::missing, 0);
};


// --------------------------------------------------------------------------

static void
initSqlite()
{
	sqlite3_vfs_register(getXp3Vfs(), 0);
}

static void
doneSqlite()
{
	sqlite3_vfs_unregister(getXp3Vfs());
}

NCB_PRE_REGIST_CALLBACK(initSqlite);
NCB_POST_UNREGIST_CALLBACK(doneSqlite);
