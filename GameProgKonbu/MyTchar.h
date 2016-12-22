#pragma once

using tifstream = std::basic_ifstream<TCHAR>;
using tofstream = std::basic_ofstream<TCHAR>;

namespace tstring_literal {
	inline dxle::tstring operator"" _ts(const TCHAR* s, std::size_t l)
	{
		return{ s,l };
	}
}
using namespace tstring_literal;
