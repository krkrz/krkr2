/****************************************************************************/
/*! @file
@brief DirectShowのHRESULTをメッセージに変える例外クラス

-----------------------------------------------------------------------------
	Copyright (C) 2004 T.Imoto <http://www.kaede-software.com>
-----------------------------------------------------------------------------
@author		T.Imoto
@date		2004/08/17
@note
			2004/08/17	T.Imoto		
*****************************************************************************/

#include "DShowException.h"


//----------------------------------------------------------------------------
//! @brief	  	DShowException constructor
//----------------------------------------------------------------------------
DShowException::DShowException()
: m_Hr(S_OK)
{
	AMGetErrorText( m_Hr, m_ErrorMes, MAX_ERROR_TEXT_LEN );
}
//----------------------------------------------------------------------------
//! @brief	  	DShowException constructor
//! @param 		hr : DirectShowの関数の返値
//----------------------------------------------------------------------------
DShowException::DShowException( HRESULT hr ) : m_Hr(hr)
{
	AMGetErrorText( m_Hr, m_ErrorMes, MAX_ERROR_TEXT_LEN );
}

//----------------------------------------------------------------------------
//! @brief	  	DShowException constructor
//! @param 		right : コピーもと
//----------------------------------------------------------------------------
DShowException::DShowException(const DShowException& right)
{
	*this = right;
}

//----------------------------------------------------------------------------
//! @brief	  	代入
//! @param 		right : コピーもと
//! @return		自身
//----------------------------------------------------------------------------
DShowException& DShowException::operator=(const DShowException& right)
{
	m_Hr = right.m_Hr;
	AMGetErrorText( m_Hr, m_ErrorMes, MAX_ERROR_TEXT_LEN );
	std::exception::operator=( right );
	return *this;
}


//----------------------------------------------------------------------------
//! @brief	  	エラーコードを代入し、メッセージを生成する
//! @param 		right : コピーもと
//! @return		自身
//----------------------------------------------------------------------------
void DShowException::SetHResult( HRESULT hr )
{
	m_Hr = hr;
	AMGetErrorText( m_Hr, m_ErrorMes, MAX_ERROR_TEXT_LEN );
}


//----------------------------------------------------------------------------
//! @brief	  	デストラクタでは特に何もしない
//----------------------------------------------------------------------------
DShowException::~DShowException()
{
}
//----------------------------------------------------------------------------
//! @brief	  	エラーの詳細を問い合わせる
//! @return		エラーメッセージ
//----------------------------------------------------------------------------
const char *DShowException::what( ) const
{
	return m_ErrorMes;
}



