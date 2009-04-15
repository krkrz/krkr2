#include "sqcont.h"
#include "sqthread.h"

namespace sqobject {

static std::vector<ObjectInfo> continuousList; ///< スレッド一覧

/// ハンドラ登録
static SQRESULT addContinuousHandler(HSQUIRRELVM v)
{
	ObjectInfo info(v,-1);
	if (info.isClosure()) {
		std::vector<ObjectInfo>::iterator i = continuousList.begin();
		while (i != continuousList.end()) {
			if (*i == info) {
				return SQ_OK;
			}
			i++;
		}
		continuousList.push_back(info);
	}
	return SQ_OK;
}

/// ハンドラ削除
static SQRESULT removeContinuousHandler(HSQUIRRELVM v)
{
	ObjectInfo info(v,-1);
	if (info.isClosure()) {
		std::vector<ObjectInfo>::iterator i = continuousList.begin();
		while (i != continuousList.end()) {
			if (*i == info) {
				i = continuousList.erase(i);
			} else {
				i++;
			}
		}
	}
	return SQ_OK;
}

static SQRESULT clearContinuousHandler(HSQUIRRELVM v)
{
	continuousList.clear();
	return SQ_OK;
}


/// 機能登録
void registerContinuous()
{
#define REGISTERMETHOD(name) \
	sq_pushstring(v, _SC(#name), -1);\
	sq_newclosure(v, name, 0);\
	sq_createslot(v, -3);

	HSQUIRRELVM v = getGlobalVM();
	sq_pushroottable(v); // root
	REGISTERMETHOD(addContinuousHandler);
	REGISTERMETHOD(removeContinuousHandler);
	REGISTERMETHOD(clearContinuousHandler);
	sq_pop(v,1);
}

/// ハンドラ処理呼び出し。Thread::main の後で呼び出す必要がある
void mainContinuous()
{
	std::vector<ObjectInfo>::iterator i = continuousList.begin();
	while (i != continuousList.end()) {
		i->call(Thread::currentTick, Thread::diffTick);
		i++;
	}
}

/// 機能終了
void doneContinuous()
{
	continuousList.clear();
}

};

