#pragma once

#include "AnsiString.h"

//----------------------------------------------------------------------------
// TPersistent込み
namespace System {
	class TObject
	{
	public:
		TObject();
		virtual ~TObject(); // dynamic_castを使うのに、virtualな関数が欲しい。デストラクタ作るべ
		// 本来の戻り値はShortString。めんどいので、AnsiStringに。あと、typeid演算子を使う都合、staticにできなかった
		AnsiString ClassName() const;

		virtual void Dispatch(void*);
	};
};
typedef System::TObject TObject;
