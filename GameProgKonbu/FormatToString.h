//printfのようなフォーマットをstd::stringに変換する
//(boost::formatの書式を受け取ってstd::stringに変換する)

// 2015/11/07 by nagata
//使用時はboostをインクルードディレクトリに追加してください
//あとはこのヘッダをインクルードするだけで使えます
//（現在Z:\library\boost）

#ifndef FORMAT_TO_STRING_2015_11_07_1500_415386410534036025
#define FORMAT_TO_STRING_2015_11_07_1500_415386410534036025
#include <boost/format.hpp>
#include <string>
#include <sstream>
#include <utility>

template<typename Format_T>
inline dxle::tstring FormatToString_impl(Format_T&& format){
	std::basic_stringstream<TCHAR> ss;
	ss << std::forward<Format_T>(format);
	return ss.str();
}
template<typename Format_T, typename Type1, typename... Args>
inline dxle::tstring FormatToString_impl(Format_T&& format, Type1&& param1, Args&&... args)
{
	return FormatToString_impl(std::forward<Format_T>(format) % std::forward<Type1>(param1), std::forward<Args>(args)...);
}


template<typename... Args>
//printfのようなフォーマットをstringに変換する
//(boost::formatの書式を受け取ってstringに変換する)
inline dxle::tstring FormatToString(dxle::tstring format_string, Args&&... args)
{
	return FormatToString_impl(boost::basic_format<TCHAR>(format_string), std::forward<Args>(args)...);
}


//受け取ったのを片っ端からostreamに突っ込む
inline void ToStringEx_impl(std::basic_ostream<TCHAR>&)
{}

template<typename Type1, typename... Args >
//受け取ったのを片っ端からostreamに突っ込む
inline void ToStringEx_impl(std::basic_ostream<TCHAR>& os, Type1&& param1, Args&&... args)
{
	ToStringEx_impl(os << std::forward<Type1>(param1), std::forward<Args>(args)...);
}

template<typename... Args>
//受け取ったのを片っ端からsstreamに突っ込む
inline dxle::tstring ToStringEx(Args&&... args)
{
	std::basic_stringstream<TCHAR> ss;
	ToStringEx_impl(ss, std::forward<Args>(args)...);
	return ss.str();
}

#endif
