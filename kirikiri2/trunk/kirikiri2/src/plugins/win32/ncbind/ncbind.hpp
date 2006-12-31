#ifndef _ncbind_hpp_
#define _ncbind_hpp_

#include <windows.h>
#include "tp_stub.h"
#include "ncb_invoke.hpp"


#if (defined(DEBUG) || defined(_DEBUG))
#define NCB_LOG(n)     TVPAddLog(ttstr(n))
#define NCB_LOG_W(log) NCB_LOG(TJS_W(log))
#define NCB_LOG_2(a,b) TVPAddLog(ttstr(a) + ttstr(b))
#else
#define NCB_LOG_VOID   ((void)0)
#define NCB_LOG(n)     NCB_LOG_VOID
#define NCB_LOG_W(log) NCB_LOG_VOID
#define NCB_LOG_2(a,b) NCB_LOG_VOID
#endif
////////////////////////////////////////
/// NativeClass 名前/ID/クラスオブジェクト保持用
template <class T>
struct ncbClassInfo {
	typedef T NativeClassT;

	typedef tjs_char const*           NameT;
	typedef tjs_int32                 IdentT;
	typedef tTJSNativeClassForPlugin  ObjectT;

	/// プロパティ取得
	static inline NameT    GetName()        { return _info.name; }
	static inline IdentT   GetID()          { return _info.id; }
	static inline ObjectT *GetClassObject() { return _info.obj; }

	/// イニシャライザ
	static inline bool Set(NameT name, IdentT id, ObjectT *obj) {
		if (_info.initialized) return false;
		_info.name = name;
		_info.id   = id;
		_info.obj  = obj;
		return (_info.initialized = true);
	}
	/// 再初期化
	static inline void Clear() {
		_info.name = 0;
		_info.id   = 0;
		_info.obj  = 0;
		_info.initialized = false;
	}
private:
	typedef struct info {
		info() : initialized(false), name(0), id(0), obj(0) {}

		bool initialized;
		NameT name;
		IdentT id;
		ObjectT *obj;
	} InfoT;
	static InfoT _info;
};
template <> struct ncbClassInfo<void> {};


////////////////////////////////////////
/// インスタンスアダプタ
template <class T>
struct ncbInstanceAdaptor : public tTJSNativeInstance {
	typedef T NativeClassT;
	typedef ncbInstanceAdaptor<NativeClassT> AdaptorT;
	typedef ncbClassInfo<NativeClassT>       ClassInfoT;

	/*constructor*/ ncbInstanceAdaptor() : _instance(0) {}
	/*destructor*/ ~ncbInstanceAdaptor() { _deleteInstance(); }

	// TJS2 オブジェクトが作成されるときに呼ばれる
	//tjs_error TJS_INTF_METHOD Construct(tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *tjs_obj);
	// …のだが，TJS_BEGIN_NATIVE_CONSTRUCTOR マクロから呼ばれるので
	// 上記マクロを使用せずに独自実装しているここでは使用しない(⇒ncbNativeClassConstructor)

	/// オブジェクトが無効化されるときに呼ばれる
	void TJS_INTF_METHOD Invalidate() { _deleteInstance(); }

private:
	/// 実インスタンスへのポインタ
	NativeClassT *_instance;

	/// 実インスタンス破棄
	void _deleteInstance() {
		if (_instance) delete _instance;
		_instance = 0;
	}

	//--------------------------------------
	// staticヘルパ関数
public:

	/// iTJSDispatch2 から Adaptor を取得
	static AdaptorT *GetAdaptor(iTJSDispatch2 *obj) {
		iTJSNativeInstance* adp = 0;
		if (!obj) {
			TVPThrowExceptionMessage(TJS_W("No instance."));
			return 0;
		}
		if (TJS_FAILED(obj->NativeInstanceSupport(TJS_NIS_GETINSTANCE, ClassInfoT::GetID(), &adp))) {
			TVPThrowExceptionMessage(TJS_W("Invalid instance type."));
			return 0;
		}
		return static_cast<AdaptorT*>(adp);
	}

	/// iTJSDispatch2 から NativeClassインスタンスを取得
	static NativeClassT *GetNativeInstance(iTJSDispatch2 *obj) {
		AdaptorT *adp = GetAdaptor(obj);
		return adp ? adp->_instance : 0;
	}

	/// NativeClassインスタンスを設定
	static bool SetNativeInstance(iTJSDispatch2 *obj, NativeClassT *instance) {
		AdaptorT *adp = GetAdaptor(obj);
		if (!adp) return false;
		adp->_instance = instance;
		return true;
	}

	/// クラスオブジェクトからAdaptorインスタンスを生成してinstanceを代入
	static iTJSDispatch2* CreateAdaptor(NativeClassT *inst) {
		typename ClassInfoT::ObjectT *clsobj = ClassInfoT::GetClassObject();
		if (!clsobj) {
			TVPThrowExceptionMessage(TJS_W("No class object."));
			return 0;
		}

		iTJSDispatch2 *global = TVPGetScriptDispatch(), *obj = 0;
		tTJSVariant dummy, *param = &dummy;
		// 引数が1つでかつvoidであれば実インスタンスをnewしない動作になる
		tjs_error r = clsobj->CreateNew(0, NULL, NULL, &obj, 1, &param, global);
		if (global) global->Release();

		if (TJS_FAILED(r) || !obj) {
			TVPThrowExceptionMessage(TJS_W("Can't create instance"));
			return 0;
		}
		AdaptorT *adp = GetAdaptor(obj);
		if (adp) adp->_instance = inst;
		return obj;
	}

	/// 空の Adaptor を生成する (tTJSNativeClassForPlugin型の関数)
	static iTJSNativeInstance* TJS_INTF_METHOD CreateEmptyAdaptor() {
		return static_cast<iTJSNativeInstance*>(new AdaptorT());
	}
};

////////////////////////////////////////
/// 型変換用ヘルパテンプレート
struct ncbTypeConvertor {

	template <typename FROM, typename TO>
	struct Conversion {
	private:
		typedef char OK;
		typedef struct { char ng[2]; } NG;
		static OK check(TO);
		static NG check(...);
		static FROM wrap();
	public:
		enum {
			Exists = (sizeof(check(wrap())) == sizeof(OK)),
			Same   = false
		};
	};
	template <typename T> struct Conversion<T, T>       { enum { Exists = true,  Same = true  }; };
	template <typename T> struct Conversion<T, void>    { enum { Exists = false, Same = false }; };
	template <typename T> struct Conversion<void, T>    { enum { Exists = false, Same = false }; };
//	template <>           struct Conversion<void, void> { enum { Exists = false, Same = true  }; };
#define NCB_INNER_CONVERSION_SPECIALIZATION \
	template <> struct ncbTypeConvertor::Conversion<void, void> { enum { Exists = false, Same = true  }; };

	/// 修飾子取り外し(ポインタ，参照，const等を外した素の型が typedef される)
	template <typename T> struct Stripper             { typedef T Type; };
	template <typename T> struct Stripper<T*>         { typedef typename Stripper<T>::Type Type; };
	template <typename T> struct Stripper<T&>         { typedef typename Stripper<T>::Type Type; };
	template <typename T> struct Stripper<const    T> { typedef typename Stripper<T>::Type Type; };
//	template <typename T> struct Stripper<volatile T> { typedef typename Stripper<T>::Type Type; };

	/// ポインタ取得
	template <typename T> struct ToPointer            { static T* Get(T &t) { return &t; } };
	template <typename T> struct ToPointer<T&>        { static T* Get(T &t) { return &t; } };
	template <typename T> struct ToPointer<T*>        { static T* Get(T* t) { return  t; } };
	template <typename T> struct ToPointer<T const&>  { static T* Get(T const &t) { return const_cast<T*>(&t); } };
	template <typename T> struct ToPointer<T const*>  { static T* Get(T const *t) { return const_cast<T*>( t); } };

	/// ポインタ⇒修飾子変換
	template <typename T> struct ToTarget             { static T  Get(T *t) { return T(*t); } };
	template <typename T> struct ToTarget<T&>         { static T& Get(T *t) { return   *t; } };
	template <typename T> struct ToTarget<T*>         { static T* Get(T *t) { return    t; } };


	// 直コピー動作
	struct DirectCopy {
		template <typename DST, typename SRC>
		inline void operator()(DST &dst, SRC const &src) const { dst = src; }
	};

	// キャスト動作
	template <typename CAST>
	struct CastCopy {
		template <typename DST, typename SRC>
		inline void operator()(DST &dst, SRC const &src) const { dst = static_cast<DST>(static_cast<CAST>(src)); }
	};

	// 型特殊化が存在するかマップ
	template <typename T, bool SRCF>
	struct SpecialMap {
		enum { Exists = false, Modifier = false, IsSource = SRCF };
		typedef T Type;
	};

	/// 動作未定（コンパイルエラー用）
	struct NoImplement;

	// コンバータ動作選択
	struct SelectConvertorTypeBase {
	protected:
		/// ３項演算子
		template <bool EXP, class THEN, class ELSE> struct ifelse                   { typedef ELSE Type; };
		template <          class THEN, class ELSE> struct ifelse<true, THEN, ELSE> { typedef THEN Type; };

		template <typename T, bool IsSrcF>
		struct hasSpecial {
			typedef typename Stripper<T>::Type StripT;
			typedef SpecialMap<T,      IsSrcF> ThisMapT;
			typedef SpecialMap<StripT, IsSrcF> StripMapT;
			enum {
				exthis  = ThisMapT::Exists,
				exstrip = StripMapT::Exists && StripMapT::Modifier,
				Exists = exthis | exstrip,
			};
			typedef typename ifelse<exthis,  typename ThisMapT::Type,
			/*    */typename ifelse<exstrip, typename StripMapT::Type, void>::Type
				/*                */>::Type Type;
		};
		template <typename T> struct wrap { typedef T Type; };

		template <typename SRC, typename DST>
		struct directSelect {
			typedef typename ifelse<Conversion<SRC, DST>::Exists, DirectCopy, NoImplement>::Type Type;
		};
	};
	template <typename SRC, typename DST>
	struct SelectConvertorType : public SelectConvertorTypeBase {
	private:
		typedef hasSpecial<SRC, true > SrcSpecialT;
		typedef hasSpecial<DST, false> DstSpecialT;

		struct specialSelect {
			typedef typename ifelse<DstSpecialT::Exists, typename DstSpecialT::Type, typename SrcSpecialT::Type>::Type Type;
		};
		typedef typename ifelse<
			SrcSpecialT::Exists || DstSpecialT::Exists, specialSelect, directSelect<SRC, DST>
				>::Type select;
	public:
		typedef typename select::Type Type;
	};
};
       NCB_INNER_CONVERSION_SPECIALIZATION
#undef NCB_INNER_CONVERSION_SPECIALIZATION

#define NCB_TYPECONV_MAPSET(mapsel, type, conv, mod) \
	template <> struct ncbTypeConvertor::SpecialMap<type, mapsel> { \
		enum { Exists = true, Modifier = mod, IsSource = mapsel }; \
		typedef conv Type; \
	}

#define NCB_TYPECONV_SRCMAP_SET(type, conv, mod) NCB_TYPECONV_MAPSET(true,  type, conv, mod)
#define NCB_TYPECONV_DSTMAP_SET(type, conv, mod) NCB_TYPECONV_MAPSET(false, type, conv, mod)

#define NCB_TYPECONV_CAST(type, cast) \
	NCB_TYPECONV_SRCMAP_SET(type, ncbTypeConvertor::CastCopy<cast>, false); \
	NCB_TYPECONV_DSTMAP_SET(type, ncbTypeConvertor::CastCopy<cast>, false)

#define NCB_TYPECONV_CAST_INTEGER(type)    NCB_TYPECONV_CAST(type, tTVInteger)
#define NCB_TYPECONV_CAST_REAL(type)       NCB_TYPECONV_CAST(type, tTVReal)

/// 数値はキャストで変換する
NCB_TYPECONV_CAST_INTEGER(   signed char);
NCB_TYPECONV_CAST_INTEGER( unsigned char);
NCB_TYPECONV_CAST_INTEGER(    signed int);
NCB_TYPECONV_CAST_INTEGER(  unsigned int);
NCB_TYPECONV_CAST_INTEGER(  signed short);
NCB_TYPECONV_CAST_INTEGER(unsigned short);
NCB_TYPECONV_CAST_INTEGER(   signed long);
NCB_TYPECONV_CAST_INTEGER( unsigned long);
NCB_TYPECONV_CAST_REAL(            float);
NCB_TYPECONV_CAST_REAL(           double);
NCB_TYPECONV_CAST(            bool, bool);

/// 一時的にバッファを確保してそこに NarrowStr として書き込む
struct ncbVariatToNChar {
	ncbVariatToNChar() : _nstr(0) {}
	~ncbVariatToNChar() {
		if (_nstr) {
			NCB_LOG_W("~ncbVariatToNChar > delete[]");
			delete[] _nstr;
		}
	}
	template <typename DST>
	inline void operator()(DST &dst, tTJSVariant const &src) {
		// なぜ tTJSVariant::Type() は const メソッドでないのか…
		if ((const_cast<tTJSVariant*>(&src))->Type() == tvtString) {
			tTJSString s(src.AsStringNoAddRef());
			tjs_int len = s.GetNarrowStrLen();

			NCB_LOG_W("ncbVariatToNChar::operator() > new tjs_nchar[]");
			_nstr = new tjs_nchar[len+1];
			s.ToNarrowStr(_nstr, len+1);
		}
		dst = static_cast<DST>(_nstr);
	}
private:
	tjs_nchar *_nstr;
};

// Narrow文字列として登録するマクロ
#define NCB_TYPECONV_NARROW_STRING(type) \
	NCB_TYPECONV_SRCMAP_SET(type, ncbTypeConvertor::CastCopy<tjs_nchar const*>, false); \
	NCB_TYPECONV_DSTMAP_SET(type, ncbVariatToNChar, false)

/// signed char と char って別物なのかすら？
NCB_TYPECONV_NARROW_STRING(         char const*);
NCB_TYPECONV_NARROW_STRING(  signed char const*);
NCB_TYPECONV_NARROW_STRING(unsigned char const*);



/// Boxing/Unboxing
struct ncbNativeObjectBoxing {
	typedef tTJSVariant VarT;
	typedef ncbTypeConvertor ConvT;
	/// Boxing
	struct Boxing { enum { HasWork = false };
		template <typename SRC>
		inline void operator ()(VarT &dst, SRC const &src) const {
			typedef SRC                                     TargetT;
			typedef typename ConvT::Stripper<TargetT>::Type ClassT;
			typedef ncbInstanceAdaptor<ClassT>              AdaptorT;

			ClassT *p = ConvT::ToPointer<TargetT>::Get(src);		//< 実インスタンスポインタ
			iTJSDispatch2 *adpobj = AdaptorT::CreateAdaptor(p);		//< アダプタTJSオブジェクト生成
			dst = adpobj;											//< Variantにコピー
			adpobj->Release();										//< コピー済みなのでadaptorは不要
		}
	};

	/// Unboxing
	struct Unboxing { enum { HasWork = false };
		template <typename DST>
		inline void operator ()(DST &dst, VarT const &src) const {
			typedef DST                                     TargetT;
			typedef typename ConvT::Stripper<TargetT>::Type ClassT;
			typedef ncbInstanceAdaptor<ClassT>              AdaptorT;

			iTJSDispatch2 *obj = src.AsObjectNoAddRef();			//< 参照カウンタ増加なしでDispatchオブジェクト取得
			ClassT *p = AdaptorT::GetNativeInstance(obj);			//< 実インスタンスのポインタを取得
			dst = ConvT::ToTarget<TargetT>::Get(p);					//< 必要とされる型に変換して返す
		}
	};
};

#define NCB_TYPECONV_BOXING(type) \
	NCB_TYPECONV_SRCMAP_SET(type, ncbNativeObjectBoxing::Boxing,   true); \
	NCB_TYPECONV_DSTMAP_SET(type, ncbNativeObjectBoxing::Unboxing, true)



/// 特殊化用のマクロ (自動変換でなく直で指定する場合はこのマクロを使用する)
#define NCB_SET_TOVARIANT_CONVERTOR(type, convertor) \
	template <> struct ncbTypeConvertor::SelectConvertorType<type, tTJSVariant> { typedef convertor Type; }

#define NCB_SET_TOVALUE_CONVERTOR(type, convertor) \
	template <> struct ncbTypeConvertor::SelectConvertorType<tTJSVariant, type> { typedef convertor Type; }

#define NCB_SET_CONVERTOR(type, convertor) \
	NCB_SET_TOVARIANT_CONVERTOR(type, convertor); \
	NCB_SET_TOVALUE_CONVERTOR(  type, convertor) \

/// 返り値なしの場合のダミーの TOVARIANT を登録
NCB_SET_TOVARIANT_CONVERTOR(void, struct {});


/*
	型変換を直で書きたい時は

	struct CustomConvertor {
		void operator ()(tTJSVariant &dst, CustomType const &src);
		void operator ()(CustomType const &src, tTJSVariant &dst);
	};
	NCB_SET_CONVERTOR(CustomType, CustomConvertor);

	といったような感じで適当に
 */



////////////////////////////////////////
/// メソッド呼び出し用ベースクラス
struct ncbNativeClassMethodBase : public tTJSDispatch {
	typedef tTJSDispatch    BaseT;
	typedef const tjs_char *NameT;
	typedef tjs_uint32      FlagsT;

	/// constructor
	ncbNativeClassMethodBase(NameT n) : _name(n) {} //< クラス名を_nameに保持 (Function or Property)
	~ncbNativeClassMethodBase() {}

	/// IsInstanceOf 実装
	tjs_error TJS_INTF_METHOD IsInstanceOf(
		tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint, 
		const tjs_char *classname, iTJSDispatch2 *objthis)
	{
		// 自分自身(membername==0)で比較クラス名が_nameならTRUE，それ以外は丸投げ
		return (!membername && !TJS_stricmp(classname, _name)) ? TJS_S_TRUE
			: BaseT::IsInstanceOf(flag, membername, hint, classname, objthis);
	}

	/// 空のメソッド(finalize Callback用) ここに置いてよいのか微妙だが
	static tjs_error TJS_INTF_METHOD EmptyCallback(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) { return TJS_S_OK; }

private:
	NameT _name;

	// privateで隠蔽してみる
	typedef MethodCaller    CallerT;


	//--------------------------------------
protected:
	/// tMethodTraitsラッパー
	template <typename T>
	struct traits {
		typedef CallerT::tMethodTraits<T> TraitsT;
		typedef typename TraitsT::ResultType ResultT;
		typedef typename TraitsT::ClassType ClassT;
		typedef typename TraitsT::ArgsType  ArgsT;
		enum { ArgsCount = TraitsT::ArgsCount };
	};


	//--------------------------------------
public:
	/// TJSNativeClassRegisterNCM に渡すフラグ
	virtual FlagsT GetFlags() const { return 0; }
	//// 以下，型からフラグを決定するためのヘルパテンプレート群
private:
	template <typename T> struct flagsSelector       { enum { Flags = 0 }; };
#define NCB_INNER_FLAGSELECTOR_SPECIALIZATION \
	template <>           struct ncbNativeClassMethodBase::flagsSelector<void> { enum { Flags = TJS_STATICMEMBER }; };

protected:
	template <typename T> 
	struct flags { enum { Flags = flagsSelector<typename traits<T>::ClassT>::Flags }; };


	//--------------------------------------
private:

	/// 引数/返り値の引き渡し用ファンクタ (ncb_invoke.hpp/MethodCallerに渡すために必要)
	template <typename RES, class ARGS, int ARGC>
	struct paramsFunctor {
		typedef tTJSVariant      VariantT;
		typedef ncbTypeConvertor ConvT;
		typedef typename     ConvT::SelectConvertorType<RES, VariantT>::Type ResultConvT;
		template <typename T> struct ArgsExtor { typedef typename ConvT::SelectConvertorType<VariantT, T>::Type Type; };
		typedef CallerT::tArgsExtract<ArgsExtor, ARGS, ARGC> ArgsConvT;
		/* ArgsConvT は以下のように展開される：
			struct ArgsConvT {
				ncbToValueConvertor<ARGS::Arg1Type> t1; // 一番目の引数の ncbToValueConvertor
				ncbToValueConvertor<ARGS::Arg2Type> t2; // 二番目の
				  :
				ncbToValueConvertor<ARGS::Arg[ARGC]Type> tARGC; // ARGC番目の
			}; */

		/// constructor
		paramsFunctor(VariantT *r, tjs_int n, VariantT const *const *p) : _result(r), _numparams(n), _param(p) {}

		/// 引数を NativeClassMethod へ引渡し
		template <int N, typename T>
		inline T operator ()(CallerT::tNumTag<N> const &index, CallerT::tTypeTag<T> const &type) {
			T ret;
			// N番目の ncbToValueConvertor を取り出して変換
			(CallerT::tArgsSelect<ArgsConvT, N>::Get(_aconv))(ret, (_numparams >= N) ? *(_param[N-1]) : VariantT());
			return ret;
		}

		/// NativeClassMethod の返り値をresultへ格納
		template <typename ResultT>
		inline void operator = (ResultT r) {
			if (_result) _rconv(*_result, r);
		}
	private:
		ArgsConvT      _aconv;
		ResultConvT    _rconv;

		tjs_int        _numparams;
		VariantT *  _result;
		VariantT const *const * _param;
	};
	template <typename METHOD>
	struct functor {
		typedef paramsFunctor<
			typename traits<METHOD>::ResultT,
		/**/typename traits<METHOD>::ArgsT,
		/**/traits<METHOD>::ArgsCount> FunctorT;
	};

	//--------------------------------------
protected:
	/// 型を引き渡すためのタグ
	template <typename T> struct factoryTag {};

	/// コンストラクタメイン実装
	template <typename METHOD>
	static inline tjs_error Factory(factoryTag<METHOD> const&, iTJSDispatch2 *objthis, tTJSVariant *result, tjs_int numparams, tTJSVariant const *const *param) {
		typedef typename traits<METHOD>::ClassT ClassT;
		typedef ncbInstanceAdaptor<ClassT> AdaptorT;

		if (result) result->Clear();

		// 引き数がひとつでかつvoidの場合はインスタンスを設定しない
		if (numparams == 1 && const_cast<tTJSVariant*>(param[0])->Type() == tvtVoid) {
			NCB_LOG_W("Constructor(void)");
			return TJS_S_OK;
		}
		// 引き数の個数が少ない場合はエラー
		if (numparams < traits<METHOD>::ArgsCount)
			return TJS_E_BADPARAMCOUNT;

		ClassT* instance;
		{
			// 引数受け渡しファンクタ
			typename functor<METHOD>::FunctorT fnct(result, numparams, param);
			// MethodCaller::Factory 呼び出し(ClassTのインスタンス作成)
			instance = CallerT::Factory(fnct, CallerT::tTypeTag<METHOD>());
			// paramsFunctor消滅させる
		}
		if (!instance) {
			TVPThrowExceptionMessage(TJS_W("NativeClassInstance creation faild."));
			return TJS_E_FAIL;
		}

		// エラー処理がどうなるのかよくわからんけど適当に（ぉ
		tjs_error r = TJS_S_OK;
		try {
			if (!AdaptorT::SetNativeInstance(objthis, instance)) {
				r = TJS_E_NATIVECLASSCRASH;
				delete instance;
			}
		} catch (...) {
			if (instance) delete instance;
			throw;
		}
		return r;
	}
	//--------------------------------------
private:
	/// static/非staticの場合わけInvokeのためのテンプレート
	template <class T>
	struct invokeImpl {
		/// 非staticの場合
		template <typename METHOD>
		static inline tjs_error Invoke(METHOD const &m, iTJSDispatch2 *objthis, tTJSVariant *result, tjs_int numparams, tTJSVariant const *const *param) {
			typedef T  ClassT;
			typedef ncbInstanceAdaptor<ClassT> AdaptorT;
			typedef typename functor<METHOD>::FunctorT FunctorT;

			// 実インスタンスのポインタ
			ClassT *obj = AdaptorT::GetNativeInstance(objthis);
			if (!obj) return TJS_E_NATIVECLASSCRASH;

			// インスタンスを渡してクラスメソッドを実行
			FunctorT fnct(result, numparams, param);
			return CallerT::Invoke(fnct, m, obj) ? TJS_S_OK : TJS_E_FAIL;
		}
	};
protected:
	/// メソッド呼び出し入り口
	template <typename METHOD>
	static inline tjs_error Invoke(METHOD const &m, iTJSDispatch2 *objthis, tTJSVariant *result, tjs_int numparams, tTJSVariant const *const *param) {
		// 引き数の個数が少ない場合はエラー
		if (numparams < traits<METHOD>::ArgsCount)
			return TJS_E_BADPARAMCOUNT;

		if (result) result->Clear();

		// MethodCaller::Invoke 呼び出し(static/非static で場合わけ)
		return invokeImpl<typename traits<METHOD>::ClassT>::Invoke(m, objthis, result, numparams, param);
	}
};
/// invokeImpl 特殊化（ClassType が void なら static メソッド (MethodCaller仕様)
template <>
struct ncbNativeClassMethodBase::invokeImpl<void> {
	/// staticメソッドをInvokeする場合
	template <typename METHOD>
	static tjs_error Invoke(METHOD const &m, iTJSDispatch2 *objthis, tTJSVariant *result, tjs_int numparams, tTJSVariant const *const *param) {
		typename functor<METHOD>::FunctorT fnct(result, numparams, param);
		return CallerT::Invoke(fnct, m) ? TJS_S_OK : TJS_E_FAIL; // 直呼びでOK
	}
};

/// ncbNativeClassMethodBase::flagsSelector特殊化(特殊化はクラス定義内には記述できないらしい)
       NCB_INNER_FLAGSELECTOR_SPECIALIZATION
#undef NCB_INNER_FLAGSELECTOR_SPECIALIZATION



////////////////////////////////////////
/// メソッド呼び出しクラステンプレート
// 本来は TJSCreateNativeClassMethod（及び吉里吉里内のtTJSNativeClassMethod）を使用するところを
// 自前で実装する(TJSCreateNativeClassMethodではstaticな関数しか呼べないのでメソッドへのポインタの保持が困難なため)
template <typename METHOD>
struct ncbNativeClassMethod : public ncbNativeClassMethodBase { 
	typedef METHOD MethodT;

	/// constructor
	ncbNativeClassMethod(MethodT m)
		: ncbNativeClassMethodBase(TJS_W("Function")), // TJSオブジェクト的には Function
		  _method(m)
	{
		if (!_method) TVPThrowExceptionMessage(TJS_W("No method pointer."));
	} 

	/// FuncCall実装
	tjs_error  TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint, 
		tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
	{
		// 自分自身が呼ばれたのではない場合は丸投げ
		if (membername) return BaseT::FuncCall(flag, membername, hint, result, numparams, param, objthis);

		// エラーチェック
		if (!objthis) return TJS_E_NATIVECLASSCRASH;

		// メソッド呼び出し
		return Invoke(_method, objthis, result, numparams, param);
	}
	/// TJSNativeClassRegisterNCMフラグ
	FlagsT GetFlags() const { return flags<MethodT>::Flags; }

private:
	MethodT const _method;
};

////////////////////////////////////////
/// コンストラクタ呼び出しクラステンプレート
template <typename METHOD>
struct ncbNativeClassConstructor : public ncbNativeClassMethodBase {
	typedef METHOD MethodT;

	/// constructor
	ncbNativeClassConstructor()
		: ncbNativeClassMethodBase(TJS_W("Function")) {} // TJSオブジェクト的には Function

	/// FuncCall実装
	tjs_error  TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint, 
		tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
	{
		// 自分自身が呼ばれたのではない場合は丸投げ
		if (membername) return BaseT::FuncCall(flag, membername, hint, result, numparams, param, objthis);

		// コンストラクタ呼び出し
		return Factory(factoryTag<MethodT>(), objthis, result, numparams, param); 
	}

	/// TJSNativeClassRegisterNCMフラグ
//	FlagsT GetFlags() const { return 0; }
}; 


////////////////////////////////////////

template <typename GETTER, typename SETTER>
struct ncbNativeClassProperty : public ncbNativeClassMethodBase {
	typedef GETTER GetterT;
	typedef SETTER SetterT;

	/// constructor
	ncbNativeClassProperty(GetterT get, SetterT set)
		: ncbNativeClassMethodBase(TJS_W("Property")), // TJSオブジェクト的には Property
		  _getter(get), _setter(set) {} 

	/// PropGet 実装
	tjs_error TJS_INTF_METHOD PropGet(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint, 
		tTJSVariant *result, iTJSDispatch2 *objthis)
	{
		// 自分自身が呼ばれたのではない場合は丸投げ
		if (membername) return BaseT::PropGet(flag, membername, hint, result, objthis);

		// エラーチェック
		if (!_getter) return TJS_E_ACCESSDENYED;
		if (!objthis) return TJS_E_NATIVECLASSCRASH;

		// メソッド呼び出し
		return Invoke(_getter, objthis, result, 0, 0);
	}

	/// PropSet 実装
	tjs_error TJS_INTF_METHOD PropSet(
		tjs_uint32 flag, const tjs_char *membername, tjs_uint32 *hint, 
		const tTJSVariant *param, iTJSDispatch2 *objthis)
	{
		// 自分自身が呼ばれたのではない場合は丸投げ
		if (membername) return BaseT::PropSet(flag, membername, hint, param, objthis);

		// エラーチェック
		if (!_setter) return TJS_E_ACCESSDENYED;
		if (!objthis) return TJS_E_NATIVECLASSCRASH;
		if (!param)   return TJS_E_FAIL;

		// メソッド呼び出し
		return Invoke(_setter, objthis, 0, 1, &param);
	}

	/// TJSNativeClassRegisterNCMフラグ
	FlagsT GetFlags() const { return flags<GetterT>::Flags; }

private:
	/// Getter と Setter が同じクラスのメソッドかどうかを判定（違う場合はコンパイルエラー）
	template <class T1, class T2> struct sameClassChecker;
	template <class T>            struct sameClassChecker<T, T> { enum { OK }; };
	enum { ClassCheck = sameClassChecker<
		typename traits<GetterT>::ClassT,
		typename traits<SetterT>::ClassT >::OK
	};

	/// プロパティメソッドへのポインタ
	GetterT const _getter;
	SetterT const _setter;
};


////////////////////////////////////////
/// 生コールバック用
template <class T>
struct ncbRawCallbackMethod : public ncbNativeClassMethodBase {
	typedef T NativeClassT;
	typedef ncbRawCallbackMethod<NativeClassT> ThisClassT;
	typedef ncbInstanceAdaptor<NativeClassT>     AdaptorT;

	typedef tjs_error (TJS_INTF_METHOD *CallbackT)(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, NativeClassT *nativeInstance);

	/// constructor
	ncbRawCallbackMethod(CallbackT m, FlagsT f)
		: ncbNativeClassMethodBase(TJS_W("Function")), // TJSオブジェクト的には Function
		  _callback(m), _flag(f)
	{
		if (!_callback) TVPThrowExceptionMessage(TJS_W("No callback pointer."));
	}

	/// FuncCall実装
	tjs_error  TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint, 
		tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
	{
		// 自分自身が呼ばれたのではない場合は丸投げ
		if (membername) return BaseT::FuncCall(flag, membername, hint, result, numparams, param, objthis);

		// エラーチェック
		if (!objthis) return TJS_E_NATIVECLASSCRASH;

		// 返り値クリア
		if (result) result->Clear();

		NativeClassT *obj = 0;
		if (!(_flag & TJS_STATICMEMBER)) {
			//< 実インスタンスのポインタ
			obj = AdaptorT::GetNativeInstance(objthis); 
			if (!obj) return TJS_E_NATIVECLASSCRASH;
		}
		// Callback呼び出し
		return _callback(result, numparams, param, obj);     //< Callback呼び出し
	}

	static ThisClassT *CreateObject(CallbackT cb, FlagsT flag) {
		return new ThisClassT(cb, flag);
	}
private:
	CallbackT const _callback;
	FlagsT    const _flag;
};

template <>
struct ncbRawCallbackMethod<void> {
	typedef tTJSNativeClassMethodCallback CallbackT;
	typedef tjs_uint32                     FlagsT;
	/// 
	static tTJSNativeClassMethod* CreateObject(CallbackT cb, FlagsT flag) {
		// 吉里吉里従来の呼び出し
		return TJSCreateNativeClassMethod(cb);
	}
};



////////////////////////////////////////
/// NativeClass クラスオブジェクト生成
template <class T>
struct ncbCreateClass {
//	typedef typename ncbModiferStripper<T>::Type NativeClassT;
	typedef T                                    NativeClassT;
	typedef ncbCreateClass<NativeClassT>         ThisClassT;

	typedef ncbClassInfo<NativeClassT>           ClassInfoT;
	typedef typename ClassInfoT::IdentT          IdentT;
	typedef typename ClassInfoT::NameT           NameT;
	typedef typename ClassInfoT::ObjectT         ClassObjectT;
	typedef ncbInstanceAdaptor<NativeClassT>     AdaptorT;
	typedef MethodCaller                         CallerT;

	/// コンストラクタ引数の受け渡しで使用
	template <class ARG> struct ConstructorArgs {};

	/// constructor
	ncbCreateClass(NameT name)
		: _hasCtor(false),
		  _classname(name), // クラス名を保持
		  _classobj(TJSCreateNativeClassForPlugin(name, AdaptorT::CreateEmptyAdaptor)), // クラスオブジェクトを生成(ID生成より前)
		  _classid(TJSRegisterNativeClass(name)) // クラスIDを生成
	{
		TVPAddLog(ttstr(TJS_W("BeginRegistClass: ")) + ttstr(_classname));

		// ncbClassInfoに登録
		if (!ClassInfoT::Set(_classname, _classid, _classobj)) {
			TVPThrowExceptionMessage(TJS_W("Already registerd class."));
		}

		// クラスオブジェクトにIDを設定
		TJSNativeClassSetClassID(_classobj, _classid);

		// 空のfinalizeメソッドを追加
		TJSNativeClassRegisterNCM(_classobj, TJS_W("finalize"),
								  TJSCreateNativeClassMethod(ncbNativeClassMethodBase::EmptyCallback),
								  _classname, nitMethod);
	}

	/// destructor
	~ncbCreateClass() {
		// TJS のグローバルオブジェクトを取得する
		iTJSDispatch2 *global = TVPGetScriptDispatch();
		if (!global) return;

		// 2 _classobj を tTJSVariant 型に変換
		tTJSVariant val(static_cast<iTJSDispatch2*>(_classobj));

		// 3 すでに val が _classobj を保持しているので、_classobj は Release する
		//_classobj->Release();
		// …としたいところだが，Boxing処理でインスタンスを作る関係でクラスオブジェクトが必要なので開放しない
		// (プラグイン開放時にリリースする⇒ncbCreateClass::Release)

		// 4 global の PropSet メソッドを用い、オブジェクトを登録する
		global->PropSet(
			TJS_MEMBERENSURE, // メンバがなかった場合には作成するようにするフラグ
			_classname, // メンバ名
			NULL, // ヒント ( 本来はメンバ名のハッシュ値だが、NULL でもよい )
			&val, // 登録する値
			global // コンテキスト ( global でよい )
			);

		// - global を Release する
		global->Release();

		// val をクリアする。
		// これは必ず行う。そうしないと val が保持しているオブジェクト
		// が Release されず、次に使う TVPPluginGlobalRefCount が正確にならない。
		//val.Clear();
		// …のだがローカルスコープで自動的に消去されるので必要ない
		TVPAddLog(ttstr(TJS_W("EndRegistClass: ")) + ttstr(_classname));
	}

	/// プラグイン開放時にクラスオブジェクトをリリースするためのstatic関数
	static void Release() {
		iTJSDispatch2 *global = TVPGetScriptDispatch();
		ClassObjectT *obj = ClassInfoT::GetClassObject();
		NameT name = ClassInfoT::GetName();

		if (global && name) global->DeleteMember(0, name, 0, global);
		if (obj) obj->Release();
		if (global) global->Release();

		ClassInfoT::Clear();
	}

	/// メソッドを登録する
	template <typename MethodT>
	ThisClassT &Method(NameT name, MethodT m) {
		typedef ncbNativeClassMethod<MethodT> MethodObjectT;
		MethodObjectT *mobj = new MethodObjectT(m);
		TJSNativeClassRegisterNCM(_classobj, name, mobj, _classname, nitMethod, mobj->GetFlags());
		return *this;
	}

	/// コンストラクタを登録する
	template <typename MethodT>
	ThisClassT &Constructor(ConstructorArgs<MethodT>) {
		typedef ncbNativeClassConstructor<MethodT> CtorObjectT;
		if (!_hasCtor) {
			CtorObjectT *ctor = new CtorObjectT();
			TJSNativeClassRegisterNCM(_classobj, _classname, ctor, _classname, nitMethod, ctor->GetFlags());
			_hasCtor = true;
		} else TVPAddLog(ttstr(TJS_W("Multiple constructors.(")) + _classname + TJS_W(")"));
		return *this;
	}

	/// プロパティを登録する
	template <typename GetterT, typename SetterT>
	ThisClassT &Property(NameT name, GetterT g, SetterT s) {
		typedef ncbNativeClassProperty<GetterT, SetterT> PropT;
		PropT *prop = new PropT(g, s);
		TJSNativeClassRegisterNCM(_classobj, name, prop, _classname, nitProperty, prop->GetFlags());
		return *this;
	}
	template <typename GetterT>
	ThisClassT &Property(NameT name, GetterT g, int) {
		typedef typename dummyProperty<GetterT>::SetterT SetterT;
		return Property(name, g, static_cast<SetterT>(0));
	}
	template <typename SetterT>
	ThisClassT &Property(NameT name, int, SetterT s) {
		typedef typename dummyProperty<SetterT>::GetterT GetterT;
		return Property(name, static_cast<SetterT>(0), s);
	}

	/// RawCallback
	template <typename MethodT>
	ThisClassT &MethodRawCallback(NameT name, MethodT m, ncbNativeClassMethodBase::FlagsT flag) {
		typedef typename rawCallbackSelect<NativeClassT, MethodT>::Type CallbackObjectT;
		TJSNativeClassRegisterNCM(_classobj, name, CallbackObjectT::CreateObject(m, flag), _classname, nitMethod, flag);
		return *this;
	}

private:
	bool          _hasCtor;		//< コンストラクタを登録したか
	NameT         _classname;	//< クラス名
	IdentT        _classid;		//< クラスID
	ClassObjectT *_classobj;	//< クラスオブジェクト

	template <typename A, typename B, typename C, typename D>
	struct typeSelector { typedef D Type; };

	template <typename A, typename C, typename D>
	struct typeSelector<A, A, C, D> { typedef C Type; };

	template <typename P>
	struct dummyProperty {
		typedef typename MethodCaller::tMethodTraits<P>::ClassType ClassT;
		typedef typename typeSelector<ClassT, void, void (*)(), void (ClassT::*)() >::Type MethodT;
		typedef MethodT GetterT;
		typedef MethodT SetterT;
	};

	template <class CLS, typename M> struct rawCallbackSelect                                     { typedef ncbRawCallbackMethod<CLS>  Type; };
	template <class CLS>             struct rawCallbackSelect<CLS, tTJSNativeClassMethodCallback> { typedef ncbRawCallbackMethod<void> Type; };
};


////////////////////////////////////////
/// TJSクラス自動レジストクラス（スレッドアンセーフなので必要なら適当に修正のこと）
// 基本的に static const なインスタンスしか使用されないのでこれで十分な気はする
struct ncbAutoRegister {
	typedef ncbAutoRegister ThisClassT;
	typedef void (*CallBackT)();

	enum LineT {
		PreRegist = 0,
		ClassRegist,
		PostRegist,
		LINE_COUNT };
#define NCB_INNER_AUTOREGISTER_LINES_INSTANCE { 0, 0, 0 }

	ncbAutoRegister(LineT line) : _next(_top[line]) { _top[line] = this; }

	static void AllRegist(  LineT line) { NCB_LOG_2(TJS_W("AllRegist:"),   line); for (ThisClassT const* p = _top[line]; p; p = p->_next) p->Regist();   }
	static void AllUnregist(LineT line) { NCB_LOG_2(TJS_W("AllUnregist:"), line); for (ThisClassT const* p = _top[line]; p; p = p->_next) p->Unregist(); }

	static void AllRegist()   { for (int line = 0; line < LINE_COUNT; line++) AllRegist(  static_cast<LineT>(line)); }
	static void AllUnregist() { for (int line = 0; line < LINE_COUNT; line++) AllUnregist(static_cast<LineT>(line)); }
protected:
	virtual void Regist()   const = 0;
	virtual void Unregist() const = 0;
private:
	ncbAutoRegister();
	/****/ ThisClassT const* _next;
	static ThisClassT const* _top[LINE_COUNT];

protected:
	/// NCB_METHOD_DETAILでメソッド詳細のタイプを決定するために使用
	struct TagClassMethod {};
	struct TagConstMethod {};
	struct TagStaticMethod {};
	template <class TAG, typename METHOD> struct MethodType;

	/// メソッド要素を分解して場合わけしつつ再構築する
#define NCB_INNER_CREATE_CLASS_METHODTYPE(tag, type) \
	template <typename METHOD> struct MethodType<tag, METHOD> { \
		typedef MethodCaller CallerT; \
		typedef CallerT::tMethodTraits<METHOD> TraitsT; \
		typedef typename CallerT::tMethodResolver< \
			typename TraitsT::ResultType, type, typename TraitsT::ArgsType >::Type Type; }
	NCB_INNER_CREATE_CLASS_METHODTYPE(TagClassMethod,  typename TraitsT::ClassType);
	NCB_INNER_CREATE_CLASS_METHODTYPE(TagConstMethod,  typename TraitsT::ClassType const);
	NCB_INNER_CREATE_CLASS_METHODTYPE(TagStaticMethod, void);
};

template <class T>
struct ncbClassAutoRegister : public ncbAutoRegister {
	typedef T ClassT;
	typedef ncbCreateClass<ClassT> RegistT;
	typedef ncbClassAutoRegister<ClassT> ThisClassT;
	typedef const tjs_char *NameT;

	ncbClassAutoRegister(NameT n) : _name(n), ncbAutoRegister(ClassRegist)  {}
protected:
	void Regist()   const { RegistT r(_name); _Regist(r); }
	void Unregist() const { ncbCreateClass<T>::Release(); }
private:
	NameT _name;
	static void _Regist(RegistT &);
};

#define NCB_REGISTER_CLASS(cls) \
	NCB_TYPECONV_BOXING(cls); \
	template    struct ncbClassInfo<cls>; \
	template <>        ncbClassInfo<cls>::InfoT ncbClassInfo<cls>::_info; \
	static ncbClassAutoRegister<cls> ncbClassAutoRegister_ ## cls(TJS_W(# cls)); \
	template <> void   ncbClassAutoRegister<cls>::_Regist(ncbClassAutoRegister<cls>::RegistT &_r_)

#define NCB_CONSTRUCTOR(cargs) _r_.Constructor(RegistT::ConstructorArgs<void (ClassT::*) cargs >())

#define NCB_METHOD_DIFFER(name, method) _r_.Method(TJS_W(# name), &ClassT::method)
#define NCB_METHOD(method) NCB_METHOD_DIFFER(method, method)

// tag = { Class, Const, Statc }
#define NCB_METHOD_DETAIL(name, tag, result, method, args) \
	_r_.Method(TJS_W(# name), static_cast<MethodType<Tag ## tag ## Method, result (ClassT::*) args >::Type >(&ClassT::method))

#define NCB_METHOD_RAW_CALLBACK(name, cb, flag) _r_.MethodRawCallback(TJS_W(# name), cb, flag)

#define NCB_PROPERTY(name,get,set) _r_.Property(TJS_W(# name), &ClassT::get, &ClassT::set)
#define NCB_PROPERTY_RO(name,get)  _r_.Property(TJS_W(# name), &ClassT::get, (int)0)
#define NCB_PROPERTY_WO(name,set)  _r_.Property(TJS_W(# name), (int)0, &ClassT::set)



/*
	マクロ使用方法例

NCB_REGISTER_CLASS(クラス) {
	NCB_CLASS_DEF("クラス名", (コンストラクタ引数1,引数2, ...));
	NCB_METHOD(メソッド名1);
	NCB_METHOD(メソッド名2);
		:
	NCB_METHOD(メソッド名n);

	// メソッド名はそのまま"メソッド名"としてクラスに登録されます
	// 違う名前で登録したい時は
	NCB_METHOD(名前, メソッド名n);

	// 同名で引数の違うメソッドがある場合は以下のマクロを使ってください
	NCB_METHOD_DETAIL_M(返り値型, メソッド名, (引数1,引数2, ...)); // 普通のメソッドの場合
	NCB_DETAIL_C(返り値型, メソッド名, (引数1,引数2, ...)); // const メソッドの場合
	NCB_DETAIL_S(返り値型, メソッド名, (引数1,引数2, ...)); // staticメソッドの場合

	// プロパティを登録する場合は以下のマクロを使用します
	// setterとgetterメソッドにstaticと非staticが混在するとエラーになります
	// getterの返り値とsetterの引き数およびその個数のチェックはされないので注意してください
	NCB_PROPERTY   (プロパティ名, getterメソッド名, setterメソッド名);
	NCB_PROPERTY_RO(プロパティ名, getterメソッド名);
	NCB_PROPERTY_WO(プロパティ名, setterメソッド名);
	// (RO = Read Only, WO = Write Only)

}

 */


////////////////////////////////////////
/// TJSファンクション自動レジストクラス

template <typename METHOD>
struct ncbNativeFunction : public ncbNativeClassMethodBase {
	typedef METHOD MethodT;
	/// constructor
	ncbNativeFunction(MethodT m)
		: ncbNativeClassMethodBase(TJS_W("Function")), // TJSオブジェクト的には Function
		  _method(m)
	{
		if (!_method) TVPThrowExceptionMessage(TJS_W("No function pointer."));
	}
	/// FuncCall実装
	tjs_error  TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint, 
		tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
	{
		// 自分自身が呼ばれたのではない場合はエラー
		if (membername) return TJS_E_MEMBERNOTFOUND;
		// メソッド呼び出し
		return Invoke(_method, objthis, result, numparams, param);
	}
private:
	MethodT const _method;
};


struct ncbFunctionAutoRegister : public ncbAutoRegister {
	typedef tjs_char const*           NameT;

	ncbFunctionAutoRegister() : ncbAutoRegister(ClassRegist)  {}
protected:
	void Regist()   const {}
	void Unregist() const {}

	template <typename MethodT>
	static void RegistFunction(  NameT name, MethodT const &m) {
		typedef ncbNativeFunction<MethodT> MethodObjectT;

		iTJSDispatch2 * global = TVPGetScriptDispatch();
		if (!global) return;

		MethodObjectT *mobj = new MethodObjectT(m);
		if (mobj) {
			tTJSVariant val(mobj);
			mobj->Release();
			global->PropSet(TJS_MEMBERENSURE, name, 0, &val, global);
		}
		global->Release();
	}
	static void UnregistFunction(NameT name) {
		iTJSDispatch2 * global = TVPGetScriptDispatch();
		if (!global) return;
		global->DeleteMember(0, name, 0, global);
		global->Release();
	}
};

template <typename DUMMY>
struct ncbFunctionAutoRegisterTempl;

#define NCB_REGISTER_FUNCTION(name, function) \
	struct ncbFunctionTag_ ## name {}; \
	template <> struct ncbFunctionAutoRegisterTempl<ncbFunctionTag_ ## name> : public ncbFunctionAutoRegister \
	{	typedef        ncbFunctionAutoRegisterTempl<ncbFunctionTag_ ## name> ThisClassT; \
		void Regist()   const { RegistFunction(TJS_W(# name), function); } \
		void Unregist() const { UnregistFunction(TJS_W(# name)); } }; \
	static ncbFunctionAutoRegisterTempl<ncbFunctionTag_ ## name> ncbFunctionAutoRegister_ ## name




////////////////////////////////////////
/// レジスト前後のコールバック登録
struct ncbCallbackAutoRegister : public ncbAutoRegister {
	typedef void (*CallbackT)();

	ncbCallbackAutoRegister(LineT line, CallbackT init, CallbackT term)
		: _init(init), _term(term), ncbAutoRegister(line)  {}
protected:
	void Regist()   const { if (_init) _init(); }
	void Unregist() const { if (_term) _term(); }
private:
	CallbackT _init, _term;
};

#define NCB_REGISTER_CALLBACK(pos, init, term, tag) static ncbCallbackAutoRegister ncbCallbackAutoRegister_ ## pos ## _ ## tag (ncbAutoRegister::pos, init, term)
#define NCB_PRE_REGIST_CALLBACK(cb)    NCB_REGISTER_CALLBACK(PreRegist,  &cb, 0, cb ## _0)
#define NCB_POST_REGIST_CALLBACK(cb)   NCB_REGISTER_CALLBACK(PostRegist, &cb, 0, cb ## _0)
#define NCB_PRE_UNREGIST_CALLBACK(cb)  NCB_REGISTER_CALLBACK(PreRegist,  0, &cb, 0_ ## cb)
#define NCB_POST_UNREGIST_CALLBACK(cb) NCB_REGISTER_CALLBACK(PostRegist, 0, &cb, 0_ ## cb)


#endif
