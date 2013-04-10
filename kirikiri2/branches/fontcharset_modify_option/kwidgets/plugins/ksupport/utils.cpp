//
// $Id$
//


#include <windows.h>
#undef max
#undef min

#include <vector>
#include <algorithm>
#include "ncbind.hpp"


//----------------------------------------------------------------------
// 各種ユーティリティ関数群
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// 変数
tjs_uint32 countHint;

//----------------------------------------------------------------------
// 宣言
bool equalStruct(tTJSVariant v1, tTJSVariant v2);
bool equalStructNumericLoose(tTJSVariant v1, tTJSVariant v2);

//----------------------------------------------------------------------
// 辞書を作成
tTJSVariant createDictionary(void)
{
  iTJSDispatch2 *obj = TJSCreateDictionaryObject();
  tTJSVariant result(obj, obj);
  obj->Release();
  return result;
}

//----------------------------------------------------------------------
// 配列を作成
tTJSVariant createArray(void)
{
  iTJSDispatch2 *obj = TJSCreateArrayObject();
  tTJSVariant result(obj, obj);
  obj->Release();
  return result;
}

//----------------------------------------------------------------------
// 辞書の要素を全比較するCaller
class DictMemberCompareCaller : public tTJSDispatch
{
public:
  tTJSVariantClosure &target;
  bool match;

  DictMemberCompareCaller(tTJSVariantClosure &_target)
    : target(_target)
    , match(true) {
  }

  virtual tjs_error TJS_INTF_METHOD FuncCall( // function invocation
                                             tjs_uint32 flag,			// calling flag
                                             const tjs_char * membername,// member name ( NULL for a default member )
                                             tjs_uint32 *hint,			// hint for the member name (in/out)
                                             tTJSVariant *result,		// result
                                             tjs_int numparams,			// number of parameters
                                             tTJSVariant **param,		// parameters
                                             iTJSDispatch2 *objthis		// object as "this"
                                             ) {
    if (result)
      *result = true;
    if (numparams > 1) {
      if ((int)*param[1] != TJS_HIDDENMEMBER) {
	const tjs_char *key = param[0]->GetString();
	tTJSVariant value = *param[2];
	tTJSVariant targetValue;
	if (target.PropGet(0, key, NULL, &targetValue, NULL)
	    == TJS_S_OK) {
	  match = match && equalStruct(value, targetValue);
	  if (result)
	    *result = match;
	}
      }
    }
    return TJS_S_OK;
  }
};  

//----------------------------------------------------------------------
// 構造体比較関数
bool equalStruct(tTJSVariant v1, tTJSVariant v2)
{
  // タイプがオブジェクトなら特殊判定
  if (v1.Type() == tvtObject
      && v2.Type() == tvtObject) {
    if (v1.AsObjectNoAddRef() == v2.AsObjectNoAddRef())
      return true;

    tTJSVariantClosure &o1 = v1.AsObjectClosureNoAddRef();
    tTJSVariantClosure &o2 = v2.AsObjectClosureNoAddRef();
    
    // 関数どうしなら特別扱いで関数比較
    if (o1.IsInstanceOf(0, NULL, NULL, L"Function", NULL)== TJS_S_TRUE
	&& o2.IsInstanceOf(0, NULL, NULL, L"Function", NULL)== TJS_S_TRUE)
      return v1.DiscernCompare(v2);

    // Arrayどうしなら全項目を比較
    if (o1.IsInstanceOf(0, NULL, NULL, L"Array", NULL)== TJS_S_TRUE
	&& o2.IsInstanceOf(0, NULL, NULL, L"Array", NULL)== TJS_S_TRUE) {
      // 長さが一致していなければ比較失敗
      tTJSVariant o1Count, o2Count;
      (void)o1.PropGet(0, L"count", &countHint, &o1Count, NULL);
      (void)o2.PropGet(0, L"count", &countHint, &o2Count, NULL);
      if (! o1Count.DiscernCompare(o2Count))
	return false;
      // 全項目を順番に比較
      tjs_int count = o1Count;
      tTJSVariant o1Val, o2Val;
      for (tjs_int i = 0; i < count; i++) {
	(void)o1.PropGetByNum(TJS_IGNOREPROP, i, &o1Val, NULL);
	(void)o2.PropGetByNum(TJS_IGNOREPROP, i, &o2Val, NULL);
	if (! equalStruct(o1Val, o2Val))
	  return false;
      }
      return true;
    }

    // Dictionaryどうしなら全項目を比較
    if (o1.IsInstanceOf(0, NULL, NULL, L"Dictionary", NULL)== TJS_S_TRUE
	&& o2.IsInstanceOf(0, NULL, NULL, L"Dictionary", NULL)== TJS_S_TRUE) {
      // 項目数が一致していなければ比較失敗
      tjs_int o1Count, o2Count;
      (void)o1.GetCount(&o1Count, NULL, NULL, NULL);
      (void)o2.GetCount(&o2Count, NULL, NULL, NULL);
      if (o1Count != o2Count)
	return false;
      // 全項目を順番に比較
      DictMemberCompareCaller *caller = new DictMemberCompareCaller(o2);
      tTJSVariantClosure closure(caller);
      tTJSVariant(o1.EnumMembers(TJS_IGNOREPROP, &closure, NULL));
      bool result = caller->match;
      caller->Release();
      return result;
    }      
  }

  return v1.DiscernCompare(v2);
}

//----------------------------------------------------------------------
// 辞書の要素を全比較するCaller(数字の比較はゆるい)
class DictMemberCompareNumericLooseCaller : public tTJSDispatch
{
public:
  tTJSVariantClosure &target;
  bool match;

  DictMemberCompareNumericLooseCaller(tTJSVariantClosure &_target)
    : target(_target)
    , match(true) {
  }

  virtual tjs_error TJS_INTF_METHOD FuncCall( // function invocation
                                             tjs_uint32 flag,			// calling flag
                                             const tjs_char * membername,// member name ( NULL for a default member )
                                             tjs_uint32 *hint,			// hint for the member name (in/out)
                                             tTJSVariant *result,		// result
                                             tjs_int numparams,			// number of parameters
                                             tTJSVariant **param,		// parameters
                                             iTJSDispatch2 *objthis		// object as "this"
                                             ) {
    if (result)
      *result = true;
    if (numparams > 1) {
      if ((int)*param[1] != TJS_HIDDENMEMBER) {
	const tjs_char *key = param[0]->GetString();
	tTJSVariant value = *param[2];
	tTJSVariant targetValue;
	if (target.PropGet(0, key, NULL, &targetValue, NULL)
	    == TJS_S_OK) {
	  match = match && equalStructNumericLoose(value, targetValue);
	  if (result)
	    *result = match;
	}
      }
    }
    return TJS_S_OK;
  }
};  

//----------------------------------------------------------------------
// 構造体比較関数(数字の比較はゆるい)
bool equalStructNumericLoose(tTJSVariant v1, tTJSVariant v2)
{
  // タイプがオブジェクトなら特殊判定
  if (v1.Type() == tvtObject
      && v2.Type() == tvtObject) {
    if (v1.AsObjectNoAddRef() == v2.AsObjectNoAddRef())
      return true;

    tTJSVariantClosure &o1 = v1.AsObjectClosureNoAddRef();
    tTJSVariantClosure &o2 = v2.AsObjectClosureNoAddRef();
    
    // 関数どうしなら特別扱いで関数比較
    if (o1.IsInstanceOf(0, NULL, NULL, L"Function", NULL)== TJS_S_TRUE
	&& o2.IsInstanceOf(0, NULL, NULL, L"Function", NULL)== TJS_S_TRUE)
      return v1.DiscernCompare(v2);

    // Arrayどうしなら全項目を比較
    if (o1.IsInstanceOf(0, NULL, NULL, L"Array", NULL)== TJS_S_TRUE
	&& o2.IsInstanceOf(0, NULL, NULL, L"Array", NULL)== TJS_S_TRUE) {
      // 長さが一致していなければ比較失敗
      tTJSVariant o1Count, o2Count;
      (void)o1.PropGet(0, L"count", &countHint, &o1Count, NULL);
      (void)o2.PropGet(0, L"count", &countHint, &o2Count, NULL);
      if (! o1Count.DiscernCompare(o2Count))
	return false;
      // 全項目を順番に比較
      tjs_int count = o1Count;
      tTJSVariant o1Val, o2Val;
      for (tjs_int i = 0; i < count; i++) {
	(void)o1.PropGetByNum(TJS_IGNOREPROP, i, &o1Val, NULL);
	(void)o2.PropGetByNum(TJS_IGNOREPROP, i, &o2Val, NULL);
	if (! equalStructNumericLoose(o1Val, o2Val))
	  return false;
      }
      return true;
    }

    // Dictionaryどうしなら全項目を比較
    if (o1.IsInstanceOf(0, NULL, NULL, L"Dictionary", NULL)== TJS_S_TRUE
	&& o2.IsInstanceOf(0, NULL, NULL, L"Dictionary", NULL)== TJS_S_TRUE) {
      // 項目数が一致していなければ比較失敗
      tjs_int o1Count, o2Count;
      (void)o1.GetCount(&o1Count, NULL, NULL, NULL);
      (void)o2.GetCount(&o2Count, NULL, NULL, NULL);
      if (o1Count != o2Count)
	return false;
      // 全項目を順番に比較
      DictMemberCompareNumericLooseCaller *caller = new DictMemberCompareNumericLooseCaller(o2);
      tTJSVariantClosure closure(caller);
      tTJSVariant(o1.EnumMembers(TJS_IGNOREPROP, &closure, NULL));
      bool result = caller->match;
      caller->Release();
      return result;
    }      
  }

  // 数字の場合は
  if ((v1.Type() == tvtInteger || v1.Type() == tvtReal)
      && (v2.Type() == tvtInteger || v2.Type() == tvtReal))
      return v1.NormalCompare(v2);

  return v1.DiscernCompare(v2);
}

//----------------------------------------------------------------------
// 辞書のキーを配列で返すCaller
class DictKeysCountCaller : public tTJSDispatch
{
public:
  tTJSVariantClosure &dst;
  tjs_int index;

  DictKeysCountCaller(tTJSVariantClosure &_dst)
    : dst(_dst)
    , index(0) {
  }

  virtual tjs_error TJS_INTF_METHOD FuncCall( // function invocation
                                             tjs_uint32 flag,			// calling flag
                                             const tjs_char * membername,// member name ( NULL for a default member )
                                             tjs_uint32 *hint,			// hint for the member name (in/out)
                                             tTJSVariant *result,		// result
                                             tjs_int numparams,			// number of parameters
                                             tTJSVariant **param,		// parameters
                                             iTJSDispatch2 *objthis		// object as "this"
                                             ) {
    if (numparams > 1) {
      if ((int)*param[1] != TJS_HIDDENMEMBER) {
	const tjs_char *key = param[0]->GetString();
	dst.PropSetByNum(0, index++, param[0], NULL);
      }
    }
    if (result)
      *result = true;
    return TJS_S_OK;
  }
};  

//----------------------------------------------------------------------
// 辞書のキーを配列にして返す
tTJSVariant dictionaryKeys(tTJSVariant dictionary)
{
  tTJSVariant result = createArray();
  tTJSVariantClosure &src = dictionary.AsObjectClosureNoAddRef();
  tTJSVariantClosure &dst = result.AsObjectClosureNoAddRef();
  DictKeysCountCaller *caller = new DictKeysCountCaller(dst);
  tTJSVariantClosure closure(caller);
  src.EnumMembers(TJS_IGNOREPROP, &closure, NULL);
  caller->Release();
  return result;
}

//----------------------------------------------------------------------
// 辞書の値を配列で返すCaller
class DictValuesCountCaller : public tTJSDispatch
{
public:
  tTJSVariantClosure &dst;
  tjs_int index;

  DictValuesCountCaller(tTJSVariantClosure &_dst)
    : dst(_dst)
    , index(0) {
  }

  virtual tjs_error TJS_INTF_METHOD FuncCall( // function invocation
                                             tjs_uint32 flag,			// calling flag
                                             const tjs_char * membername,// member name ( NULL for a default member )
                                             tjs_uint32 *hint,			// hint for the member name (in/out)
                                             tTJSVariant *result,		// result
                                             tjs_int numparams,			// number of parameters
                                             tTJSVariant **param,		// parameters
                                             iTJSDispatch2 *objthis		// object as "this"
                                             ) {
    if (numparams > 1) {
      if ((int)*param[1] != TJS_HIDDENMEMBER) {
	const tjs_char *key = param[0]->GetString();
	dst.PropSetByNum(0, index++, param[2], NULL);
      }
    }
    if (result)
      *result = true;
    return TJS_S_OK;
  }
};  

//----------------------------------------------------------------------
// 辞書の値を配列にして返す
tTJSVariant dictionaryValues(tTJSVariant dictionary)
{
  tTJSVariant result = createArray();
  tTJSVariantClosure &src = dictionary.AsObjectClosureNoAddRef();
  tTJSVariantClosure &dst = result.AsObjectClosureNoAddRef();
  DictValuesCountCaller *caller = new DictValuesCountCaller(dst);
  tTJSVariantClosure closure(caller);
  src.EnumMembers(TJS_IGNOREPROP, &closure, NULL);
  caller->Release();
  return result;
}

//----------------------------------------------------------------------
// 配列の値をキーに持つ辞書を返す。
tTJSVariant arrayHash(tTJSVariant array)
{
  tTJSVariant result = createDictionary();
  tTJSVariantClosure &src = array.AsObjectClosureNoAddRef();
  tTJSVariantClosure &dst = result.AsObjectClosureNoAddRef();

  tTJSVariant arrayCount;
  (void)src.PropGet(0, L"count", &countHint, &arrayCount, NULL);
  tjs_int count = arrayCount;
  tTJSVariant key;
  tTJSVariant value(true);
  for (tjs_int i = 0; i < count; i++) {
    (void)src.PropGetByNum(TJS_IGNOREPROP, i, &key, NULL);
    ttstr keyStr = key;
    (void)dst.PropSet(TJS_MEMBERENSURE, keyStr.c_str(), NULL, &value, NULL);
  }
  return result;
}

//----------------------------------------------------------------------
// 辞書を結合するcaller
class DictUnionCaller : public tTJSDispatch
{
public:
  tTJSVariantClosure &dst;

  DictUnionCaller(tTJSVariantClosure &_dst)
    : dst(_dst) {
  }

  virtual tjs_error TJS_INTF_METHOD FuncCall( // function invocation
                                             tjs_uint32 flag,			// calling flag
                                             const tjs_char * membername,// member name ( NULL for a default member )
                                             tjs_uint32 *hint,			// hint for the member name (in/out)
                                             tTJSVariant *result,		// result
                                             tjs_int numparams,			// number of parameters
                                             tTJSVariant **param,		// parameters
                                             iTJSDispatch2 *objthis		// object as "this"
                                             ) {
    if (numparams > 1) {
      if ((int)*param[1] != TJS_HIDDENMEMBER) {
	const tjs_char *key = param[0]->GetString();
	tTJSVariant value = *param[2];
	(void)dst.PropSet(TJS_MEMBERENSURE, key, NULL, &value, NULL);
      }
    }
    if (result)
      *result = true;
    return TJS_S_OK;
  }
};  

//----------------------------------------------------------------------
// 辞書を結合した辞書を返す
tTJSVariant unionDictionary(tTJSVariant v1, tTJSVariant v2)
{
  tTJSVariant result = createDictionary();
  tTJSVariantClosure &o1 = v1.AsObjectClosureNoAddRef();
  tTJSVariantClosure &o2 = v2.AsObjectClosureNoAddRef();
  tTJSVariantClosure &dst = result.AsObjectClosureNoAddRef();

  DictUnionCaller *caller = new DictUnionCaller(dst);
  tTJSVariantClosure closure(caller);
  o1.EnumMembers(TJS_IGNOREPROP, &closure, NULL);
  o2.EnumMembers(TJS_IGNOREPROP, &closure, NULL);
  caller->Release();

  return result;
}

//----------------------------------------------------------------------
// 配列の中身をvectorに展開
void array_to_vector(tTJSVariant &array, std::vector<tTJSVariant> &vec)
{
  tTJSVariantClosure &obj = array.AsObjectClosureNoAddRef();
  tTJSVariant objCount;
  (void)obj.PropGet(0, L"count", &countHint, &objCount, NULL);
  tjs_int count = objCount;
  vec.reserve(count);
  tTJSVariant value;
  for (tjs_int i = 0; i < count; i++) {
    (void)obj.PropGetByNum(TJS_IGNOREPROP, i, &value, NULL);
    vec.push_back(value);
  }
}

//----------------------------------------------------------------------
// TJSVariant比較
bool TJSVariantCompare(const tTJSVariant &v1, const tTJSVariant &v2)
{
  return v1.LittlerThan(v2);
}

//----------------------------------------------------------------------
// 和集合
tTJSVariant unionSet(tTJSVariant v1, tTJSVariant v2)
{
  std::vector<tTJSVariant> a1, a2, r;

  array_to_vector(v1, a1);
  array_to_vector(v2, a2);

  std::sort(a1.begin(), a1.end(), TJSVariantCompare);
  std::sort(a2.begin(), a2.end(), TJSVariantCompare);
  r.reserve(a1.size() + a2.size());
  std::set_union(a1.begin(), a1.end(),
		 a2.begin(), a2.end(),
		 std::back_inserter(r),
		 TJSVariantCompare);

  tTJSVariant result = createArray();
  tTJSVariantClosure &dst = result.AsObjectClosureNoAddRef();
  for (tjs_int i = 0; i < tjs_int(r.size()); i++) {
    (void)dst.PropSetByNum(0, i, &(r[i]), NULL);
  }

  return result;
}

//----------------------------------------------------------------------
// 碩集合
tTJSVariant intersectionSet(tTJSVariant v1, tTJSVariant v2)
{
  std::vector<tTJSVariant> a1, a2, r;

  array_to_vector(v1, a1);
  array_to_vector(v2, a2);

  std::sort(a1.begin(), a1.end(), TJSVariantCompare);
  std::sort(a2.begin(), a2.end(), TJSVariantCompare);
  r.reserve(std::min(a1.size(), a2.size()));
  std::set_intersection(a1.begin(), a1.end(),
			a2.begin(), a2.end(),
			std::back_inserter(r),
			TJSVariantCompare);

  tTJSVariant result = createArray();
  tTJSVariantClosure &dst = result.AsObjectClosureNoAddRef();
  for (tjs_int i = 0; i < tjs_int(r.size()); i++) {
    (void)dst.PropSetByNum(0, i, &(r[i]), NULL);
  }

  return result;
}

//----------------------------------------------------------------------
// 差集合
tTJSVariant differenceSet(tTJSVariant v1, tTJSVariant v2)
{
  std::vector<tTJSVariant> a1, a2, r;

  array_to_vector(v1, a1);
  array_to_vector(v2, a2);

  std::sort(a1.begin(), a1.end(), TJSVariantCompare);
  std::sort(a2.begin(), a2.end(), TJSVariantCompare);
  r.reserve(a1.size());
  std::set_difference(a1.begin(), a1.end(),
		      a2.begin(), a2.end(),
		      std::back_inserter(r),
		      TJSVariantCompare);

  tTJSVariant result = createArray();
  tTJSVariantClosure &dst = result.AsObjectClosureNoAddRef();
  for (tjs_int i = 0; i < tjs_int(r.size()); i++) {
    (void)dst.PropSetByNum(0, i, &(r[i]), NULL);
  }

  return result;
}

//----------------------------------------------------------------------
// 配列のスライス
tTJSVariant sliceArray(tTJSVariant v, tjs_int from, tjs_int size)
{
  tTJSVariantClosure &src = v.AsObjectClosureNoAddRef();
  tTJSVariant srcCount;
  (void)src.PropGet(0, L"count", &countHint, &srcCount, NULL);
  tjs_int count = srcCount;

  tTJSVariant result = createArray();
  tTJSVariantClosure &dst = result.AsObjectClosureNoAddRef();

  tTJSVariant value;
  for (tjs_int i = from; i < from + size; i++) {
    (void)src.PropGetByNum(TJS_IGNOREPROP, i, &value, NULL);
    (void)dst.PropSetByNum(0, i - from, &value, NULL);
  }

  return result;
}

//----------------------------------------------------------------------
// 全配列巡回
tjs_error TJS_INTF_METHOD eachArray(tTJSVariant *result,
				     tjs_int numparams,
				     tTJSVariant **param,
				     iTJSDispatch2 *objthis) {
  if (numparams < 2) return TJS_E_BADPARAMCOUNT;
  tTJSVariantClosure &array = param[0]->AsObjectClosureNoAddRef();
  tTJSVariantClosure &func = param[1]->AsObjectClosureNoAddRef();

  tTJSVariant key, value;
  tTJSVariant **paramList = new tTJSVariant *[numparams];
  paramList[0] = &key;
  paramList[1] = &value;
  for (tjs_int i = 2; i < numparams; i++)
    paramList[i] = param[i];

  tTJSVariant arrayCount;
  (void)array.PropGet(0, L"count", &countHint, &arrayCount, NULL);
  tjs_int count = arrayCount;

  for (tjs_int i = 0; i < count; i++) {
    key = i;
    (void)array.PropGetByNum(TJS_IGNOREPROP, i, &value, NULL);
    (void)func.FuncCall(0, NULL, NULL, NULL, numparams, paramList, NULL);
  }

  delete[] paramList;

  return TJS_S_OK;
}

//----------------------------------------------------------------------
// 辞書を巡回するcaller
class DictIterateCaller : public tTJSDispatch
{
public:
  tTJSVariantClosure &dst;
  tTJSVariant **paramList;
  tjs_int paramCount;

  DictIterateCaller(tTJSVariantClosure &_dst,
		    tTJSVariant **_paramList,
		    tjs_int _paramCount)
    : dst(_dst)
    , paramList(_paramList) 
    , paramCount(_paramCount) {
  }

  virtual tjs_error TJS_INTF_METHOD FuncCall( // function invocation
                                             tjs_uint32 flag,			// calling flag
                                             const tjs_char * membername,// member name ( NULL for a default member )
                                             tjs_uint32 *hint,			// hint for the member name (in/out)
                                             tTJSVariant *result,		// result
                                             tjs_int numparams,			// number of parameters
                                             tTJSVariant **param,		// parameters
                                             iTJSDispatch2 *objthis		// object as "this"
                                             ) {
    if (numparams > 1) {
      if ((int)*param[1] != TJS_HIDDENMEMBER) {
	*paramList[0] = *param[0];
	*paramList[1] = *param[2];
	(void)dst.FuncCall(0, NULL, NULL, NULL, paramCount, paramList, NULL);
      }
    }
    if (result)
      *result = true;
    return TJS_S_OK;
  }
};  

//----------------------------------------------------------------------
// 全辞書巡回
tjs_error TJS_INTF_METHOD eachDictionary(tTJSVariant *result,
					 tjs_int numparams,
					 tTJSVariant **param,
					 iTJSDispatch2 *objthis) {
  if (numparams < 2) return TJS_E_BADPARAMCOUNT;
  tTJSVariantClosure &dictionary = param[0]->AsObjectClosureNoAddRef();
  tTJSVariantClosure &func = param[1]->AsObjectClosureNoAddRef();

  tTJSVariant key, value;
  tTJSVariant **paramList = new tTJSVariant *[numparams];
  paramList[0] = &key;
  paramList[1] = &value;
  for (tjs_int i = 2; i < numparams; i++)
    paramList[i] = param[i];

  DictIterateCaller *caller = new DictIterateCaller(func, paramList, numparams);
  tTJSVariantClosure closure(caller);
  dictionary.EnumMembers(TJS_IGNOREPROP, &closure, NULL);
  caller->Release();
  
  delete[] paramList;

  return TJS_S_OK;
}

NCB_REGISTER_FUNCTION(equalStruct, equalStruct);
NCB_REGISTER_FUNCTION(equalStructNumericLoose, equalStructNumericLoose);
NCB_REGISTER_FUNCTION(dictionaryKeys, dictionaryKeys);
NCB_REGISTER_FUNCTION(dictionaryValues, dictionaryValues);
NCB_REGISTER_FUNCTION(arrayHash, arrayHash);
NCB_REGISTER_FUNCTION(unionDictionary, unionDictionary);
NCB_REGISTER_FUNCTION(unionSet, unionSet);
NCB_REGISTER_FUNCTION(intersectionSet, intersectionSet);
NCB_REGISTER_FUNCTION(differenceSet, differenceSet);
NCB_REGISTER_FUNCTION(sliceArray, sliceArray);
NCB_REGISTER_FUNCTION(eachArray, eachArray);
NCB_REGISTER_FUNCTION(eachDictionary, eachDictionary);
