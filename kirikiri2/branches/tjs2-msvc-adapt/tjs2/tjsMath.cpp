//---------------------------------------------------------------------------
/*
	TJS2 Script Engine
	Copyright (C) 2000-2005  W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// TJS Math class implementation
//---------------------------------------------------------------------------
#include "tjsCommHead.h"

#include "tjsMath.h"
#include "math.h"
#include "time.h"

#ifdef __WIN32__
#ifndef TJS_NO_MASK_MATHERR
	#include "float.h"
#endif
#endif

//---------------------------------------------------------------------------
// matherr and matherrl function
//---------------------------------------------------------------------------
// these functions invalidate the mathmarical error
// (other exceptions, like divide-by-zero error, are not to be caught here)
#if defined(__WIN32__) && !defined(__GNUC__) && !defined (_MSC_VER)
#ifndef TJS_NO_MASK_MATHERR
int _USERENTRY _matherr(struct _exception *e)
{
	return 1;
}
int _USERENTRY _matherrl(struct _exception *e)
{
	return 1;
}
#endif
#endif
//---------------------------------------------------------------------------



namespace TJS
{
//---------------------------------------------------------------------------
// tTJSNC_Math : TJS Native Class : Math
//---------------------------------------------------------------------------
tjs_uint32 tTJSNC_Math::ClassID = (tjs_uint32)-1;
tTJSNC_Math::tTJSNC_Math() :
	tTJSNativeClass(TJS_W("Math"))
{
	// constructor
	time_t time_num;
	time(&time_num);
	srand(time_num);

	/*
		TJS2 cannot promise that the sequence of generated random numbers are
		unique.
		Math.random uses old-style random generator from stdlib
		since Math.RandomGenerator provides Mersenne Twister high-quality random
		generator.
	*/

	TJSSetFPUE();

	TJS_BEGIN_NATIVE_MEMBERS(/*TJS class name*/Math)
	TJS_DECL_EMPTY_FINALIZE_METHOD
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_CONSTRUCTOR_DECL_NO_INSTANCE(/*TJS class name*/ Math)
{
	return TJS_S_OK;
}
TJS_END_NATIVE_CONSTRUCTOR_DECL(/*TJS class name*/Math)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/abs)
{
	if(numparams<1) return TJS_E_BADPARAMCOUNT;

	if(result)
	{
		TJSSetFPUE();
		*result=fabs(param[0]->AsReal());
	}

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/abs)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/acos)
{
	if(numparams<1) return TJS_E_BADPARAMCOUNT;

	if(result)
	{
		TJSSetFPUE();
		*result = acos(param[0]->AsReal());
	}

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/acos)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/asin)
{
	if(numparams<1) return TJS_E_BADPARAMCOUNT;

	if(result)
	{
		TJSSetFPUE();
		*result = asin(param[0]->AsReal());
	}

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/asin)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/atan)
{
	if(numparams<1) return TJS_E_BADPARAMCOUNT;

	if(result)
	{
		TJSSetFPUE();
		*result = atan(param[0]->AsReal());
	}

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/atan)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/atan2)
{
	if(numparams<2) return TJS_E_BADPARAMCOUNT;

	if(result)
	{
		TJSSetFPUE();
		*result = atan2(param[0]->AsReal(), param[1]->AsReal());
	}

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/atan2)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/ceil)
{
	if(numparams<1) return TJS_E_BADPARAMCOUNT;

	if(result)
	{
		TJSSetFPUE();
		*result = ceil(param[0]->AsReal());
	}

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/ceil)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/exp)
{
	if(numparams<1) return TJS_E_BADPARAMCOUNT;

	if(result)
	{
		TJSSetFPUE();
		*result = exp(param[0]->AsReal());
	}

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/exp)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/floor)
{
	if(numparams<1) return TJS_E_BADPARAMCOUNT;

	if(result)
	{
		TJSSetFPUE();
		*result = floor(param[0]->AsReal());
	}

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/floor)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/log)
{
	if(numparams<1) return TJS_E_BADPARAMCOUNT;

	if(result)
	{
		TJSSetFPUE();
		*result = log(param[0]->AsReal());
	}

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/log)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/pow)
{
	if(numparams<2) return TJS_E_BADPARAMCOUNT;

	if(result)
	{
		TJSSetFPUE();
		*result = pow(param[0]->AsReal(), param[1]->AsReal());
	}

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/pow)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/max)
{
	if(numparams<2) return TJS_E_BADPARAMCOUNT;

	TJSSetFPUE();
	tTVReal a = param[0]->AsReal();
	tTVReal b = param[1]->AsReal();

	if(result)
	{
		TJSSetFPUE();
		*result = std::max<tTVReal>(a,b);
	}

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/max)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/min)
{
	if(numparams<2) return TJS_E_BADPARAMCOUNT;

	TJSSetFPUE();

	tTVReal a = param[0]->AsReal();
	tTVReal b = param[1]->AsReal();

	if(result) *result = std::min<tTVReal>(a,b);

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/min)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/random)
{
	if(result)
	{
		TJSSetFPUE();
		*result = ((tTVReal)((tTVReal)TJS_rand()/(tTVReal)(TJS_RAND_MAX)));
	}
	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/random)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/round)
{
	if(numparams<1) return TJS_E_BADPARAMCOUNT;

	if(result)
	{
		TJSSetFPUE();
		*result = ::floor(param[0]->AsReal() + 0.5f);
	}

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/round)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/sin)
{
	if(numparams<1) return TJS_E_BADPARAMCOUNT;

	if(result)
	{
		TJSSetFPUE();
		*result = ::sin(param[0]->AsReal());
	}

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/sin)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/cos)
{
	if(numparams<1) return TJS_E_BADPARAMCOUNT;

	if(result)
	{
		TJSSetFPUE();
		*result = ::cos(param[0]->AsReal());
	}

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/cos)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/sqrt)
{
	if(numparams<1) return TJS_E_BADPARAMCOUNT;

	if(result)
	{
		TJSSetFPUE();
		*result = ::sqrt(param[0]->AsReal());
	}

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/sqrt)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_METHOD_DECL(/*func. name*/tan)
{
	if(numparams<1) return TJS_E_BADPARAMCOUNT;

	if(result)
	{
		TJSSetFPUE();
		*result = ::tan(param[0]->AsReal());
	}

	return TJS_S_OK;
}
TJS_END_NATIVE_METHOD_DECL(/*func. name*/tan)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(E)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		*result = M_E;
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_DENY_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(E)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(LOG2E)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		*result = M_LOG2E;
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_DENY_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(LOG2E)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(LOG10E)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		*result = M_LOG10E;
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_DENY_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(LOG10E)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(LN10)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		*result = M_LN10;
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_DENY_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(LN10)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(LN2)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		*result = M_LN2;
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_DENY_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(LN2)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(PI)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		*result = M_PI;
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_DENY_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(PI)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(SQRT1_2)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		*result = (M_SQRT2/2);
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_DENY_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(SQRT1_2)
//----------------------------------------------------------------------
TJS_BEGIN_NATIVE_PROP_DECL(SQRT2)
{
	TJS_BEGIN_NATIVE_PROP_GETTER
	{
		*result = M_SQRT2;
		return TJS_S_OK;
	}
	TJS_END_NATIVE_PROP_GETTER

	TJS_DENY_NATIVE_PROP_SETTER
}
TJS_END_NATIVE_PROP_DECL(SQRT2)
//---------------------------------------------------------------------------

	TJS_END_NATIVE_MEMBERS
} // tTJSNC_Math::tTJSNC_Math()
//---------------------------------------------------------------------------
}  // namespace TJS


