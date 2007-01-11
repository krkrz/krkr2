#ifndef _ncbind_hpp_
#define _ncbind_hpp_

#include <windows.h>
#include "tp_stub.h"
#include "ncb_invoke.hpp"

////////////////////////////////////////
// ログ出力用マクロ

#define NCB_WARN(n)     TVPAddLog(ttstr(n))
#define NCB_WARN_2(a,b) TVPAddLog(ttstr(a) + ttstr(b))
#define NCB_WARN_W(str) NCB_WARN(TJS_W(str))

#if (defined(DEBUG) || defined(_DEBUG))
#define NCB_LOG(n)     NCB_WARN(n)
#define NCB_LOG_2(a,b) NCB_WARN_2(a,b)
#define NCB_LOG_W(str) NCB_WARN_W(str)
#else
#define NCB_LOG_VOID   ((void)0)
#define NCB_LOG(n)     NCB_LOG_VOID
#define NCB_LOG_2(a,b) NCB_LOG_VOID
#define NCB_LOG_W(str) NCB_LOG_VOID
#endif


////////////////////////////////////////
// 共通型定義
struct ncbTypedefs {
	typedef tjs_char const*           NameT;
	typedef tjs_uint32                FlagsT;
	typedef tjs_int32                 IdentT;

	typedef MethodCaller              CallerT;
	typedef tTJSNativeInstanceType    InstanceTypeT;
	typedef tTJSNativeClassForPlugin  ClassObjectT;

	/// 型の受け渡しで使用
	template <typename T> struct Tag {};
};

////////////////////////////////////////
/// NativeClass 名前/ID/クラスオブジェクト保持用
template <class T>
struct ncbClassInfo {
	typedef T NativeClassT;

	typedef ncbTypedefs::NameT        NameT;
	typedef ncbTypedefs::IdentT       IdentT;
	typedef ncbTypedefs::ClassObjectT ClassObjectT;

	/// プロパティ取得
	static inline NameT         GetName()        { return _info.name; }
	static inline IdentT        GetID()          { return _info.id; }
	static inline ClassObjectT *GetClassObject() { return _info.obj; }

	/// イニシャライザ
	static inline bool Set(NameT name, IdentT id, ClassObjectT *obj) {
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
		ClassObjectT *obj;
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
	static AdaptorT *GetAdaptor(iTJSDispatch2 *obj, bool err = false) {
		iTJSNativeInstance* adp = 0;
		if (!obj) {
			if (err) TVPThrowExceptionMessage(TJS_W("No instance."));
			return 0;
		}
		if (TJS_FAILED(obj->NativeInstanceSupport(TJS_NIS_GETINSTANCE, ClassInfoT::GetID(), &adp))) {
			if (err) TVPThrowExceptionMessage(TJS_W("Invalid instance type."));
			return 0;
		}
		return static_cast<AdaptorT*>(adp);
	}

	/// iTJSDispatch2 から NativeClassインスタンスを取得
	static NativeClassT *GetNativeInstance(iTJSDispatch2 *obj, bool err = false) {
		AdaptorT *adp = GetAdaptor(obj, err);
		return adp ? adp->_instance : 0;
	}

	/// NativeClassインスタンスを設定
	static bool SetNativeInstance(iTJSDispatch2 *obj, NativeClassT *instance, bool err = false) {
		AdaptorT *adp = GetAdaptor(obj, err);
		if (!adp) return false;
		adp->_instance = instance;
		return true;
	}

	/// アダプタを生成しつつNativeClassインスタンスを設定
	static bool SetAdaptorWithNativeInstance(iTJSDispatch2 *obj, NativeClassT *instance, bool err = false) {
		AdaptorT *adp = GetAdaptor(obj, false);
		if (adp) {
			if (adp->_instance) adp->_deleteInstance();
		} else if (!(adp = new AdaptorT())) {
			if (err) TVPThrowExceptionMessage(TJS_W("Create adaptor failed."));
			return false;
		}
		adp->_instance = instance;
		iTJSNativeInstance *ni = static_cast<iTJSNativeInstance*>(adp);
		if (TJS_FAILED(obj->NativeInstanceSupport(TJS_NIS_REGISTER, ClassInfoT::GetID(), &ni))) {
			if (err) TVPThrowExceptionMessage(TJS_W("Adaptor registration failed."));
			return false;
		}
		return true;
	}

	/// クラスオブジェクトからAdaptorインスタンスを生成してinstanceを代入
	static iTJSDispatch2* CreateAdaptor(NativeClassT *inst, bool err = false) {
		typename ClassInfoT::ClassObjectT *clsobj = ClassInfoT::GetClassObject();
		if (!clsobj) {
			if (err) TVPThrowExceptionMessage(TJS_W("No class object."));
			return 0;
		}

		iTJSDispatch2 *global = TVPGetScriptDispatch(), *obj = 0;
		tTJSVariant dummy, *param = &dummy;
		// 引数が1つでかつvoidであれば実インスタンスをnewしない動作になる
		tjs_error r = clsobj->CreateNew(0, 0, 0, &obj, 1, &param, global);
		if (global) global->Release();

		if (TJS_FAILED(r) || !obj) {
			if (err) TVPThrowExceptionMessage(TJS_W("Can't create instance"));
			return 0;
		}
		AdaptorT *adp = GetAdaptor(obj, err);
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

	/// FROM から TO へ変換できるか
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

	// 型特殊化が存在するかのマップ用
	template <typename T, bool SRCF>
	struct SpecialMap {
		enum { Exists = false, Modifier = false, IsSource = SRCF };
		typedef T Type;
	};

	/// 動作未定（コンパイルエラー用）
	struct NCB_COMPILE_ERROR_NoImplement;

	// コンバータ動作選択
	struct SelectConvertorTypeBase {
	protected:
		/// ３項演算子
		template <bool EXP, class THEN, class ELSE> struct ifelse                   { typedef ELSE Type; };
		template <          class THEN, class ELSE> struct ifelse<true, THEN, ELSE> { typedef THEN Type; };

		/// 特殊化が存在するか調べるテンプレート
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
			typedef typename ifelse<Conversion<SRC, DST>::Exists, DirectCopy, NCB_COMPILE_ERROR_NoImplement>::Type Type;
		};
	};

	/// コンバータのタイプを調べる
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
// ncbTypeConvertor::Conversion の特殊化
       NCB_INNER_CONVERSION_SPECIALIZATION
#undef NCB_INNER_CONVERSION_SPECIALIZATION

//--------------------------------------
// TypeConvertor関連の各種マクロ

/// SpecialMap に登録するマクロ
#define NCB_TYPECONV_MAPSET(mapsel, type, conv, mod) \
	template <> struct ncbTypeConvertor::SpecialMap<type, mapsel> { \
		enum { Exists = true, Modifier = mod, IsSource = mapsel }; \
		typedef conv Type; \
	}
#define NCB_TYPECONV_SRCMAP_SET(type, conv, mod) NCB_TYPECONV_MAPSET(true,  type, conv, mod)
#define NCB_TYPECONV_DSTMAP_SET(type, conv, mod) NCB_TYPECONV_MAPSET(false, type, conv, mod)

/// Cast動作としてマップに登録
#define NCB_TYPECONV_CAST(type, cast) \
	NCB_TYPECONV_SRCMAP_SET(type, ncbTypeConvertor::CastCopy<cast>, false); \
	NCB_TYPECONV_DSTMAP_SET(type, ncbTypeConvertor::CastCopy<cast>, false)

/// 数値キャストで登録
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
	/// Constructor (メソッドが呼ばれる前)
	ncbVariatToNChar() : _nstr(0) {}
	/// Destructor (メソッドが呼ばれた後)
	~ncbVariatToNChar() {
		if (_nstr) {
//			NCB_LOG_W("~ncbVariatToNChar > delete[]");
			delete[] _nstr;
		}
	}
	/// 引き数を受け渡すためのファンクタ
	template <typename DST>
	inline void operator()(DST &dst, tTJSVariant const &src) {
		// なぜ tTJSVariant::Type() は const メソッドでないのか…
		if ((const_cast<tTJSVariant*>(&src))->Type() == tvtString) {
			tTJSString s(src.AsStringNoAddRef());
			tjs_int len = s.GetNarrowStrLen();

//			NCB_LOG_W("ncbVariatToNChar::operator() > new tjs_nchar[]");
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



/// ネイティブインスタンスアダプタで Boxing/Unboxing する
struct ncbNativeObjectBoxing {
	typedef tTJSVariant VarT;
	typedef ncbTypeConvertor ConvT;
	/// Boxing
	struct Boxing {
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
	struct Unboxing {
		template <typename DST>
		inline void operator ()(DST &dst, VarT const &src) const {
			typedef DST                                     TargetT;
			typedef typename ConvT::Stripper<TargetT>::Type ClassT;
			typedef ncbInstanceAdaptor<ClassT>              AdaptorT;

			iTJSDispatch2 *obj = src.AsObjectNoAddRef();			//< 参照カウンタ増加なしでDispatchオブジェクト取得
			ClassT *p = AdaptorT::GetNativeInstance(obj, true);		//< 実インスタンスのポインタを取得
			dst = ConvT::ToTarget<TargetT>::Get(p);					//< 必要とされる型に変換して返す
		}
	};
};

/// ボックス化する型として登録するマクロ
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

	struct CustomType; // 変換する対象の型
	struct CustomConvertor { // コンバータ
		void operator ()(tTJSVariant &dst, CustomType const &src);
		void operator ()(CustomType const &src, tTJSVariant &dst);
	};
	NCB_SET_CONVERTOR(CustomType, CustomConvertor);
	といったような感じで適当に
 */

////////////////////////////////////////
/// メソッドオブジェクト（と，そのタイプとフラグ）を受け渡すためのインターフェース
struct ncbIMethodObject {
	typedef iTJSDispatch2*             DispatchT;
	typedef ncbTypedefs::FlagsT        FlagsT;
	typedef ncbTypedefs::InstanceTypeT TypesT;

	virtual DispatchT GetDispatch() const = 0;
	virtual FlagsT    GetFlags()    const = 0;
	virtual TypesT    GetType()     const = 0;
	virtual void      Release()     const = 0;
};

////////////////////////////////////////
/// メソッド呼び出し用ベースクラス

struct ncbNativeClassMethodBase : public tTJSDispatch {
	typedef tTJSDispatch             BaseT;
	typedef ncbNativeClassMethodBase ThisClassT;
	typedef ncbTypedefs              DefsT;
	typedef DefsT::NameT             NameT;
	typedef DefsT::FlagsT            FlagsT;
	typedef DefsT::InstanceTypeT     TypesT;
	typedef ncbIMethodObject const*  iMethodT;

	/// constructor
	ncbNativeClassMethodBase(NameT n) : _name(n) { _imethod = this; } //< クラス名を_nameに保持 (Function or Property)
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

private:
	NameT _name;

	// privateで隠蔽してみる
	typedef DefsT::CallerT CallerT;

	//--------------------------------------
	/// TJSNativeClassRegisterNCM に渡すタイプ
	virtual TypesT GetType() const { return nitMethod; }

	/// TJSNativeClassRegisterNCM に渡すフラグ
	virtual FlagsT GetFlags() const { return 0; }

	/// IMethod 実装
	struct iMethod : public ncbIMethodObject {
		typedef ncbNativeClassMethodBase MethodObjectT;
		void operator = (MethodObjectT *mo) { _this = mo; }
		DispatchT GetDispatch() const { return static_cast<DispatchT>(_this); }
		FlagsT    GetFlags()    const { return _this->GetFlags(); }
		TypesT    GetType()     const { return _this->GetType(); }
		void      Release()     const {}
	private:
		MethodObjectT *_this;
	} _imethod;

protected:
	/// IMethod 取得
	iMethodT GetIMethod() const { return &_imethod; }



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
	/// メソッド型からフラグを決定するためのヘルパテンプレート
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
			// ncbToVariantConvertor で返り値に変換
			if (_result) _rconv(*_result, r);
		}
	private:
		// 型変換用ワーク
		ArgsConvT   _aconv;
		ResultConvT _rconv;

		// 引数・返り値パラメータ
		tjs_int                _numparams;
		VariantT             * _result;
		VariantT const *const* _param;
	};
	/// メソッド型から paramsFunctor 型を決定するテンプレート
	template <typename METHOD>
	struct functor {
		typedef paramsFunctor<
			typename traits<METHOD>::ResultT,
		/**/typename traits<METHOD>::ArgsT,
		/**/traits<METHOD>::ArgsCount
			> FunctorT;
	};

	//--------------------------------------
protected:
	/// コンストラクタメイン実装
	template <typename METHOD>
	static inline tjs_error Constructor(DefsT::Tag<METHOD> const&, iTJSDispatch2 *objthis, tTJSVariant *result, tjs_int numparams, tTJSVariant const *const *param) {
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
	/// ネイティブインスタンスを取得するためのテンプレート（特殊化で上書きできるようにするため
	template <class T>
	struct nativeInstanceGetterBase {
		typedef T  ClassT;
		typedef ncbInstanceAdaptor<ClassT> AdaptorT;
		typedef iTJSDispatch2* DispatchT;
		typedef tjs_error      ErrorT;

		nativeInstanceGetterBase() : _error(TJS_S_OK) {}

		inline ClassT *GetNativeInstance(DispatchT objthis) {
			ClassT *r = AdaptorT::GetNativeInstance(objthis); // 実インスタンスのポインタ
			if (!r) SetError(TJS_E_NATIVECLASSCRASH);
			return r;
		}
		inline bool SetNativeInstance(DispatchT objthis, ClassT *obj) {
			SetError(TJS_S_OK);
			return AdaptorT::SetAdaptorWithNativeInstance(objthis, obj);
		}
		inline ErrorT  GetError() const   { return _error; }
		inline void    SetError(ErrorT e) { _error = e; }

		/// デフォルト動作
		inline ClassT* Get(DispatchT objthis) { return GetNativeInstance(objthis); }
	private:
		ErrorT _error;
	};

	/// 指定がない場合はデフォルト動作（BaseのGetが呼ばれる）
	template <class T>
	struct nativeInstanceGetter : public nativeInstanceGetterBase<T> {};


	/// static/非staticの場合わけInvokeのためのテンプレート
	template <class T>
	struct invokeImpl {
		/// 非staticの場合
		template <typename METHOD>
		static inline tjs_error Invoke(METHOD const &m, iTJSDispatch2 *objthis, tTJSVariant *result, tjs_int numparams, tTJSVariant const *const *param) {
			typedef T  ClassT;
			typedef typename functor<METHOD>::FunctorT FunctorT;
			typedef nativeInstanceGetter<ClassT> InstanceGetterT;

			InstanceGetterT g;
			ClassT *obj = g.Get(objthis);
			if (TJS_SUCCEEDED(g.GetError())) {
				// インスタンスを渡してクラスメソッドを実行
				FunctorT fnct(result, numparams, param);
				g.SetError(CallerT::Invoke(fnct, m, obj) ? TJS_S_OK : TJS_E_FAIL);
			}
			return g.GetError();
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
	typedef ncbNativeClassMethod ThisClassT;
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
	/// factory
	static iMethodT Create(MethodT m) { return (new ThisClassT(m))->GetIMethod(); }

protected:
	MethodT const _method;

private:
	/// TJSNativeClassRegisterNCMフラグ
	FlagsT GetFlags() const { return flags<MethodT>::Flags; }
};

////////////////////////////////////////
/// コンストラクタ呼び出しクラステンプレート
template <typename METHOD>
struct ncbNativeClassConstructor : public ncbNativeClassMethodBase {
	typedef ncbNativeClassConstructor ThisClassT;
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
		return Constructor(DefsT::Tag<MethodT>(), objthis, result, numparams, param); 
	}

	/// factory
	static iMethodT Create() { return (new ThisClassT())->GetIMethod(); }

	/// TJSNativeClassRegisterNCMフラグ
//	FlagsT GetFlags() const { return 0; }
}; 


////////////////////////////////////////

template <typename GETTER, typename SETTER>
struct ncbNativeClassProperty : public ncbNativeClassMethodBase {
	typedef ncbNativeClassProperty ThisClassT;
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

	/// factory
	static iMethodT Create(GetterT g, SetterT s) { return (new ThisClassT(g, s))->GetIMethod(); }

private:
	/// Getter と Setter が同じクラスのメソッドかどうかを判定（違う場合はコンパイルエラー）
	template <class T1, class T2> struct sameClassChecker;
	template <class T>            struct sameClassChecker<T, T> { enum { OK }; };
	enum { ClassCheck = sameClassChecker<
		typename traits<GetterT>::ClassT,
		typename traits<SetterT>::ClassT >::OK
	};

	/// TJSNativeClassRegisterNCMフラグ
	FlagsT GetFlags() const { return flags<GetterT>::Flags; }
	TypesT GetType()  const { return nitProperty; }

protected:
	/// プロパティメソッドへのポインタ
	GetterT const _getter;
	SetterT const _setter;
};


////////////////////////////////////////
/// 生コールバック用
template <typename T> struct ncbRawCallbackMethod;

// ネイティブインスタンスのポインタのみあらかじめ取得するコールバック
template <class T>
struct ncbRawCallbackMethod<
/*        */tjs_error (TJS_INTF_METHOD *         )(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, T *nativeInstance) > : public ncbNativeClassMethodBase {
	typedef tjs_error (TJS_INTF_METHOD *CallbackT)(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, T *nativeInstance);
	typedef T                                NativeClassT;
	typedef ncbRawCallbackMethod             ThisClassT;
	typedef ncbInstanceAdaptor<NativeClassT> AdaptorT;


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

	/// factory
	static iMethodT Create(CallbackT cb, FlagsT f) { return (new ThisClassT(cb, f))->GetIMethod(); }

protected:
	CallbackT const _callback;
	FlagsT    const _flag;
};

/// 従来の TJSCreateNativeClassMethod用 ncbIMethodObjectラッパ
template <>
struct ncbRawCallbackMethod<tTJSNativeClassMethodCallback> : public ncbIMethodObject {
	typedef ncbRawCallbackMethod ThisClassT;
	typedef tTJSNativeClassMethodCallback CallbackT;
	typedef ncbNativeClassMethodBase::iMethodT iMethodT;

	ncbRawCallbackMethod(CallbackT m, FlagsT f) : _dispatch(TJSCreateNativeClassMethod(m)), _flags(f) {}

	DispatchT GetDispatch() const { return _dispatch; }
	FlagsT    GetFlags()    const { return _flags; }
	TypesT    GetType()     const { return nitMethod; }
	void      Release()     const { delete this; }

	/// factory
	static iMethodT Create(CallbackT cb, FlagsT f) { return new ThisClassT(cb, f); }
private:
	DispatchT const _dispatch;
	FlagsT    const _flags;
};


////////////////////////////////////////
/// 単体のネイティブファンクション

// 場合わけ用ベース
template <typename MethodT>
struct ncbNativeFunctionBase : public ncbNativeClassMethodBase {
	ncbNativeFunctionBase() : ncbNativeClassMethodBase(TJS_W("Function")) {}
protected:
	static inline tjs_error invokeSelect(MethodT m, tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		return Invoke(m, objthis, result, numparams, param);
	}
};
// tTJSNativeClassMethodCallback なら直で呼ぶ
template <>
struct ncbNativeFunctionBase<tTJSNativeClassMethodCallback> : public ncbNativeClassMethodBase {
	ncbNativeFunctionBase() : ncbNativeClassMethodBase(TJS_W("Function")) {}
protected:
	static inline tjs_error invokeSelect(tTJSNativeClassMethodCallback m, tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) {
		return m(result, numparams, param,  objthis);
	}
};

// ネイティブファンクションオブジェクト本体
template <typename METHOD>
struct ncbNativeFunction : ncbNativeFunctionBase<METHOD> {
	typedef ncbNativeClassMethodBase BaseT;
	typedef METHOD MethodT;
	/// constructor
	ncbNativeFunction(MethodT m, bool hasMember = false) : _method(m), _hasMember(hasMember) {
		if (!_method) TVPThrowExceptionMessage(TJS_W("No function pointer."));
	}
	/// FuncCall実装
	tjs_error  TJS_INTF_METHOD FuncCall(
		tjs_uint32 flag, const tjs_char * membername, tjs_uint32 *hint, 
		tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis)
	{
		// 自分自身が呼ばれたのではない場合はエラーか丸投げ
		if (membername) return !_hasMember ? TJS_E_MEMBERNOTFOUND
			: BaseT::FuncCall(flag, membername, hint, result, numparams, param, objthis);

		// メソッド呼び出し
		return invokeSelect(_method, result, numparams, param, objthis);
	}

private:
	MethodT const _method;
	bool const _hasMember;
};




////////////////////////////////////////
/// NativeClass クラスオブジェクト登録ベース
struct ncbRegistBase {
	typedef ncbTypedefs::NameT        NameT;

	/**/ ncbRegistBase(NameT n, bool r) : _name(n), _isRegist(r) {}
	virtual ~ncbRegistBase() {}

	void Begin() { if (_isRegist) RegistBegin(); else UnregistBegin(); }
	void End()   { if (_isRegist) RegistEnd();   else UnregistEnd(); }

	virtual void   RegistBegin() {}
	virtual void UnregistBegin() {}
	virtual void   RegistEnd()   {}
	virtual void UnregistEnd()   {}

protected:
	bool  const _isRegist;
	NameT const _name;
	NameT GetName() const { return _name; }

	/// 空のメソッド(finalize Callback用)
	static tjs_error TJS_INTF_METHOD EmptyCallback(  tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) { return TJS_S_OK; }

	/// エラーを返すメソッド(empty Constructor用)
	static tjs_error TJS_INTF_METHOD NotImplCallback(tTJSVariant *result, tjs_int numparams, tTJSVariant **param, iTJSDispatch2 *objthis) { return TJS_E_NOTIMPL; }
};

template <typename T>
struct ncbRegistClassBase : public ncbRegistBase {
	typedef T                         SelectorT;
	typedef typename SelectorT::ItemT ItemT;
	typedef ncbTypedefs::FlagsT       FlagsT;

	virtual void   RegistItem(NameT, ItemT) {}
	virtual void UnregistItem(NameT)        {}

	ncbRegistClassBase(NameT n, bool r) : ncbRegistBase(n, r) {}

	/// メソッドを登録する
	template <typename MethodT>
	void Method(NameT name, MethodT m) {
		typedef typename SelectorT::template Method<MethodT> TargetT;
		if (_isRegist) RegistItem(name, TargetT::Type::Create(m));
		else         UnregistItem(name);
	}

	/// コンストラクタを登録する
	template <typename MethodT>
	void Constructor(ncbTypedefs::Tag<MethodT>) {
		typedef typename SelectorT::template Constructor<MethodT> TargetT;
		if (_isRegist) RegistItem(GetName(), TargetT::Type::Create());
		else         UnregistItem(GetName());
	}

	/// プロパティを登録する
	template <typename GetterT, typename SetterT>
	void Property(NameT name, GetterT g, SetterT s) {
		typedef typename SelectorT::template Property<GetterT, SetterT> TargetT;
		if (_isRegist) RegistItem(name, TargetT::Type::Create(g, s));
		else         UnregistItem(name);
	}
	// ReadOnly / WriteOnly Properties.
	template <typename GetterT> void Property(NameT name, GetterT g, int) { Property(name, g, static_cast<GetterT>(0)); }
	template <typename SetterT> void Property(NameT name, int, SetterT s) { Property(name, static_cast<SetterT>(0), s); }

	/// RawCallback
	template <typename MethodT>
	void MethodRawCallback(NameT name, MethodT m, ncbNativeClassMethodBase::FlagsT flags) {
		typedef typename SelectorT::template RawCallback<MethodT> TargetT;
		if (_isRegist) RegistItem(name, TargetT::Type::Create(m, flags));
		else         UnregistItem(name);
	}
};

////////////////////////////////////////

struct ncbRegistNativeClassSelector {
	typedef ncbNativeClassMethodBase::iMethodT ItemT;

	template <typename T>             struct Method      { typedef ncbNativeClassMethod<T>        Type; };
	template <typename T>             struct Constructor { typedef ncbNativeClassConstructor<T>   Type; };
	template <typename G, typename S> struct Property    { typedef ncbNativeClassProperty<G, S>   Type; };
	template <typename T>             struct RawCallback { typedef ncbRawCallbackMethod<T>        Type; };
};

template <class T>
struct ncbRegistNativeClass : public ncbRegistClassBase<ncbRegistNativeClassSelector> {
	typedef /*                     */ncbRegistClassBase<ncbRegistNativeClassSelector> BaseT;
	typedef T                                    NativeClassT;
	typedef ncbInstanceAdaptor<NativeClassT>     AdaptorT;

	typedef ncbClassInfo<NativeClassT>           ClassInfoT;
	typedef typename ClassInfoT::IdentT          IdentT;
	typedef typename ClassInfoT::ClassObjectT    ClassObjectT;

	ncbRegistNativeClass(NameT n, bool b) : BaseT(n, b), _hasCtor(false), _classobj(0) {}

	void RegistBegin() {
		NameT const className = GetName();

		NCB_LOG_2(TJS_W("BeginRegistClass: "), className);

		// クラスオブジェクトを生成
		_classobj = TJSCreateNativeClassForPlugin(className, AdaptorT::CreateEmptyAdaptor);

		// クラスIDを生成
		IdentT id  = TJSRegisterNativeClass(className);

		// ncbClassInfoに登録
		if (!ClassInfoT::Set(className, id, _classobj)) {
			TVPThrowExceptionMessage(TJS_W("Already registerd class."));
		}

		// クラスオブジェクトにIDを設定
		TJSNativeClassSetClassID(_classobj, id);

		// 空のfinalizeメソッドを追加
		TJSNativeClassRegisterNCM(_classobj, TJS_W("finalize"),
								  TJSCreateNativeClassMethod(EmptyCallback),
								  className, nitMethod);
	}

	void RegistItem(NameT name, typename SelectorT::ItemT item) {
		NCB_LOG_2(TJS_W("  RegistItem: "), name);
		NameT const className = GetName();
		if (name == className) {
			if (_hasCtor) TVPAddLog(ttstr(TJS_W("Multiple constructors.(")) + className + TJS_W(")"));
			_hasCtor = true;
		}
		if (item) {
			TJSNativeClassRegisterNCM(_classobj, name, item->GetDispatch(), className, item->GetType(), item->GetFlags());
			item->Release();
		}
	}

	void RegistEnd() {
		NameT const className = GetName();

		// コンストラクタがない場合はエラーを返すコンストラクタを追加する
		if (!_hasCtor) TJSNativeClassRegisterNCM(_classobj, className,
												 TJSCreateNativeClassMethod(NotImplCallback),
												 className, nitMethod);

		// TJS のグローバルオブジェクトを取得する
		iTJSDispatch2 *global = TVPGetScriptDispatch();
		if (!global) {
			NCB_WARN_W("No Global Dispatch, Regist failed.");
			return;
		}

		// 2 _classobj を tTJSVariant 型に変換
		tTJSVariant val(static_cast<iTJSDispatch2*>(_classobj));

		// 3 すでに val が _classobj を保持しているので、_classobj は Release する
		//_classobj->Release();
		// …としたいところだが，Boxing処理でインスタンスを作る関係でクラスオブジェクトが必要なので開放しない
		// (プラグイン開放時にリリースする⇒ncbCreateClass::Release)

		// 4 global の PropSet メソッドを用い、オブジェクトを登録する
		global->PropSet(
			TJS_MEMBERENSURE, // メンバがなかった場合には作成するようにするフラグ
			className, // メンバ名
			0, // ヒント ( 本来はメンバ名のハッシュ値だが、NULL でもよい )
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

		NCB_LOG_2(TJS_W("EndRegistClass: "), className);
	}

	/// プラグイン開放時にクラスオブジェクトをリリースする
	void UnregistBegin() {
		iTJSDispatch2 *global = TVPGetScriptDispatch();
		typename ClassInfoT::ClassObjectT *obj = ClassInfoT::GetClassObject();

		if (global) {
			global->DeleteMember(0, GetName(), 0, global);
			global->Release();
		}
		if (obj) obj->Release();

		ClassInfoT::Clear();
	}

protected:
	bool          _hasCtor;		//< コンストラクタを登録したか
	ClassObjectT *_classobj;	//< クラスオブジェクト
};

////////////////////////////////////////
/// 既存クラスオブジェクトを変更する

struct ncbAttachTJS2ClassSelector {
	typedef ncbNativeClassMethodBase::iMethodT ItemT;

	template <typename T>             struct Method      { typedef ncbNativeClassMethod<T>        Type; };
	template <typename G, typename S> struct Property    { typedef ncbNativeClassProperty<G, S>   Type; };
	template <typename T>             struct RawCallback { typedef ncbRawCallbackMethod<T>        Type; };

	struct NCB_COMPILE_ERROR_CantSetConstructor;
	template <typename T>             struct Constructor { typedef NCB_COMPILE_ERROR_CantSetConstructor Type; };
};
template <class T>
struct ncbAttachTJS2Class : public ncbRegistClassBase<ncbAttachTJS2ClassSelector> {
	typedef /*                   */ncbRegistClassBase<ncbAttachTJS2ClassSelector> BaseT;
	typedef T                                    NativeClassT;
	typedef ncbClassInfo<NativeClassT>           ClassInfoT;
	typedef typename ClassInfoT::IdentT          IdentT;
	typedef typename ClassInfoT::ClassObjectT    ClassObjectT;

	ncbAttachTJS2Class(NameT nativeClass, NameT tjs2Class, bool b) : BaseT(nativeClass, b), _tjs2ClassName(tjs2Class) {}

	void RegistBegin() {
		NameT const nativeClassName = GetName();

		NCB_LOG_2(TJS_W("BeginAttachTJS2Class: "), nativeClassName);

		// TJS のグローバルオブジェクトを取得する
		_global = TVPGetScriptDispatch();
		_tjs2ClassObj = GetGlobalObject(_tjs2ClassName, _global);

		// クラスIDを生成
		IdentT id  = TJSRegisterNativeClass(nativeClassName);
		NCB_LOG_2(TJS_W("  ID: "), (tjs_int)id);

		// ncbClassInfoに登録
		if (!ClassInfoT::Set(nativeClassName, id, 0)) {
			TVPThrowExceptionMessage(TJS_W("Already registerd class."));
		}
	}

	void RegistItem(NameT name, typename SelectorT::ItemT item) {
		NCB_LOG_2(TJS_W("  RegistItem: "), name);
		if (!item) return;
		iTJSDispatch2 *dsp = item->GetDispatch();
		tTJSVariant val(dsp);
		dsp->Release();
		FlagsT flg = item->GetFlags();
		_tjs2ClassObj->PropSet(TJS_MEMBERENSURE | flg, name, 0, &val, ((flg & TJS_STATICMEMBER) ? _global : _tjs2ClassObj));
		item->Release();
	}

	void RegistEnd() {
		NCB_LOG_2(TJS_W("EndAttachClass: "), GetName());
		if (_global) _global->Release();
		_global = 0;
		// _tjs2ClassObj は NoAddRef なので Release 不要
	}

	void UnregistBegin() {
		// クラスオブジェクトを取得
		iTJSDispatch2 *global = TVPGetScriptDispatch();
		_tjs2ClassObj = GetGlobalObject(_tjs2ClassName, global);
		if (global) global->Release();
	}
	void UnregistItem(NameT name) {
		_tjs2ClassObj->DeleteMember(0, name, 0, _tjs2ClassObj);
	}
	void UnregistEnd() {
		ClassInfoT::Clear();
		// _tjs2ClassObj は NoAddRef なので Release 不要
	}
protected:
	NameT const    _tjs2ClassName;
	iTJSDispatch2* _tjs2ClassObj;	//< クラスオブジェクト
	iTJSDispatch2* _global;

	// クラスオブジェクトを取得
	static iTJSDispatch2* GetGlobalObject(NameT name, iTJSDispatch2 *global) {
		tTJSVariant val;
		if (global) global->PropGet(0, name, 0, &val, global);
		else {
			NCB_WARN_W("No Global Dispatch.");
			TVPExecuteExpression(name, &val);
		}
		return val.AsObjectNoAddRef();
	}
};




////////////////////////////////////////
/// TJSクラス自動レジストクラス（スレッドアンセーフなので必要なら適当に修正のこと）
// 基本的に static const なインスタンスしか使用されないのでこれで十分な気はする
struct ncbAutoRegister {
	typedef ncbAutoRegister ThisClassT;
	typedef void (*CallBackT)();
	typedef ncbTypedefs::NameT NameT;

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

////////////////////////////////////////
template <class T>
struct  ncbNativeClassAutoRegister : public ncbAutoRegister {
	/**/ncbNativeClassAutoRegister(NameT n) : _name(n), ncbAutoRegister(ClassRegist)  {}
private:
	typedef T ClassT;
	typedef ncbRegistNativeClass<T> RegistT;

	NameT const _name;
	static void _Regist(RegistT &);
protected:
	void Regist()   const { RegistT r(_name, true);  r.Begin(); _Regist(r); r.End(); }
	void Unregist() const { RegistT r(_name, false); r.Begin(); _Regist(r); r.End(); }
};

#define NCB_REGISTER_CLASS_COMMON(cls, tmpl, init) \
	template    struct ncbClassInfo<cls>; \
	template <>        ncbClassInfo<cls>::InfoT ncbClassInfo<cls>::_info; \
	static tmpl<cls> tmpl ## _ ## cls init; \
	void   tmpl<cls>::_Regist (RegistT &_r_)

#define NCB_REGISTER_CLASS(cls) \
	NCB_TYPECONV_BOXING(cls); \
	NCB_REGISTER_CLASS_COMMON(cls, ncbNativeClassAutoRegister, (TJS_W(# cls)))

////////////////////////////////////////
template <class T>
struct  ncbAttachTJS2ClassAutoRegister : public ncbAutoRegister {
	/**/ncbAttachTJS2ClassAutoRegister(NameT ncn, NameT tjscn) : _nativeClassName(ncn), _tjs2ClassName(tjscn), ncbAutoRegister(ClassRegist) {}
protected:
	typedef T ClassT;
	typedef ncbAttachTJS2Class<T> RegistT;
	void Regist()   const { RegistT r(_nativeClassName, _tjs2ClassName, true);  r.Begin(); _Regist(r); r.End(); }
	void Unregist() const { RegistT r(_nativeClassName, _tjs2ClassName, false); r.Begin(); _Regist(r); r.End(); }
private:
	NameT const _nativeClassName;
	NameT const _tjs2ClassName;
	static void _Regist(RegistT &);
};

#define NCB_GET_INSTANCE_HOOK(cls) \
	template <> struct ncbNativeClassMethodBase::nativeInstanceGetter<cls> : public ncbNativeClassMethodBase::nativeInstanceGetterBase<cls>

#define NCB_GET_INSTANCE_HOOK_CLASS nativeInstanceGetter

#define NCB_INSTANCE_GETTER(objthis) \
	inline ClassT* Get(DispatchT objthis)

#define NCB_ATTACHED_INSTANCE_DELAY_CREATE(cls) \
	NCB_GET_INSTANCE_HOOK(cls) { NCB_INSTANCE_GETTER(objthis) { \
		ClassT* obj = GetNativeInstance(objthis); \
		if (!obj) { obj = new ClassT(); SetNativeInstance(objthis, obj); } \
		return obj; } }

#define NCB_ATTACH_CLASS_WITH_HOOK(cls, attach) \
	template <> struct ncbNativeClassMethodBase::nativeInstanceGetter<cls>; \
	NCB_REGISTER_CLASS_COMMON(cls, ncbAttachTJS2ClassAutoRegister, (TJS_W(# cls), TJS_W(# attach)))

#define NCB_ATTACH_CLASS(cls, attach) \
	NCB_ATTACHED_INSTANCE_DELAY_CREATE(cls); \
	NCB_ATTACH_CLASS_WITH_HOOK(cls, attach)

////////////////////////////////////////
#define NCB_CONSTRUCTOR(cargs) _r_.Constructor(ncbTypedefs::Tag<void (ClassT::*) cargs >())

#define NCB_METHOD_DIFFER(name, method) _r_.Method(TJS_W(# name), &ClassT::method)
#define NCB_METHOD(method) NCB_METHOD_DIFFER(method, method)

// tag = { Class, Const, Statc }
#define NCB_METHOD_DETAIL(name, tag, result, method, args) \
	_r_.Method(TJS_W(# name), static_cast<MethodType<Tag ## tag ## Method, result (ClassT::*) args >::Type >(&ClassT::method))

#define NCB_METHOD_RAW_CALLBACK(name, cb, flag) _r_.MethodRawCallback(TJS_W(# name), cb, flag)

#define NCB_PROPERTY(name,get,set) _r_.Property(TJS_W(# name), &ClassT::get, &ClassT::set)
#define NCB_PROPERTY_RO(name,get)  _r_.Property(TJS_W(# name), &ClassT::get, (int)0)
#define NCB_PROPERTY_WO(name,set)  _r_.Property(TJS_W(# name), (int)0, &ClassT::set)


////////////////////////////////////////
/// TJSファンクション自動レジストクラス

struct  ncbNativeFunctionAutoRegister : public ncbAutoRegister {
	ncbNativeFunctionAutoRegister() : ncbAutoRegister(ClassRegist) {}
protected:
	template <typename MethodT>
	static void RegistFunction(NameT name, NameT attach, MethodT m) {
		typedef ncbNativeFunction<MethodT> MethodObjectT;
		iTJSDispatch2 *dsp = GetDispatch(attach);
		if (!dsp) return;
		MethodObjectT *mobj = new MethodObjectT(m);
		if (mobj) {
			tTJSVariant val(mobj);
			mobj->Release();
			dsp->PropSet(TJS_MEMBERENSURE, name, 0, &val, dsp);
		}
		dsp->Release();
	}

	static void UnregistFunction(NameT name, NameT attach) {
		iTJSDispatch2 *dsp = GetDispatch(attach);
		if (!dsp) return;
		dsp->DeleteMember(0, name, 0, dsp);
		dsp->Release();
	}

private:
	static iTJSDispatch2* GetDispatch(NameT attach) {
		iTJSDispatch2 *ret, *global = TVPGetScriptDispatch();
		if (!global) return 0;

		if (!attach) ret = global;
		else {
			tTJSVariant val;
			global->PropGet(0, attach, 0, &val, global);
			global->Release();
			ret = val.AsObject();
		}
		return ret;
	}
};
template <typename DUMMY>
struct ncbNativeFunctionAutoRegisterTempl;

#define NCB_REGISTER_FUNCTION_COMMON(name, attach, function) \
	struct ncbFunctionTag_ ## name {}; \
	template <> struct ncbNativeFunctionAutoRegisterTempl<ncbFunctionTag_ ## name> : public ncbNativeFunctionAutoRegister \
	{	void Regist()   const { RegistFunction(TJS_W(# name), attach, &function); } \
		void Unregist() const { UnregistFunction(TJS_W(# name), attach); } }; \
	static ncbNativeFunctionAutoRegisterTempl<ncbFunctionTag_ ## name> ncbFunctionAutoRegister_ ## name

#define NCB_REGISTER_FUNCTION(name, function)       NCB_REGISTER_FUNCTION_COMMON(name, 0, function)
#define NCB_ATTACH_FUNCTION(name, attach, function) NCB_REGISTER_FUNCTION_COMMON(name, TJS_W(# attach), function)




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
