// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#ifndef _WIN32_WINNT		// Windows XP 以降のバージョンに固有の機能の使用を許可します。                   
#define _WIN32_WINNT 0x0501	// これを Windows の他のバージョン向けに適切な値に変更してください。
#endif				

#include <stdio.h>
#include <tchar.h>

// TODO: プログラムに必要な追加ヘッダーをここで参照してください。
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <stack>
#include <algorithm>

#if 1
#include <boost/detail/endian.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#endif
