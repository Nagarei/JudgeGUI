#pragma once

#include "babel/babel.h"

void erase_BOM(::babel::bbl_binary& str);
template<typename CharT>
std::basic_string<CharT> babel_translate(::babel::bbl_binary X, int to_base_encoding = ::babel::get_base_encoding())
{
	auto from_encoding = ::babel::analyze_base_encoding(X);
	erase_BOM(X);
	return ::babel::translate_to_string_engine<std::basic_string<CharT>>::
				ignite(X, from_encoding, to_base_encoding);
}
