#pragma once
#include "FormatToString.h"
#include "Unsafe_My_func.h"
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

template<typename FUNC>
//f1.a f2.a ... の番号がどこまで続くか調べる
//なかったらreturn (unsigned)(-1)
inline uint32_t get_numobject_num(const dxle::tstring& file_name_before, const dxle::tstring& file_name_after, uint32_t index_start, uint32_t index_end, FUNC&& check_func)
{
	TCHAR buf[20];
	//二分探索
	uint32_t begin = index_start, end = index_end;//[begin,end)
	dxle::tstring file_name_temp;
	auto is_exists = [&file_name_temp, &file_name_before, &file_name_after, &check_func, &buf](uint32_t num) {
		file_name_temp = file_name_before;
		file_name_temp += my_itoa(num, buf);
		file_name_temp += file_name_after;
		return check_func(file_name_temp.c_str());
	};
	if (is_exists(begin) == false) { return (uint32_t)(-1); }
	while (is_exists(end)) {
		begin = end;
		assert(end < 0xffffffff / 2);
		end *= 2;
	}
	while (begin + 1 < end)
	{
		auto mid = begin + (end - begin) / 2;
		if (is_exists(mid)) {
			begin = mid;
		}
		else {
			end = mid;
		}
	}
	return begin;
}

//f1.a f2.a ... の番号がどこまで続くか調べる
//なかったらreturn (unsigned)(-1)
inline uint32_t get_numfile_num(const dxle::tstring& file_name_before, const dxle::tstring& file_name_after, uint32_t index_start, uint32_t index_end = 10000)
{
	return get_numobject_num(file_name_before, file_name_after, index_start, index_end, PathFileExists);
}
//f1_ f2_ ... の番号がどこまで続くか調べる
//なかったらreturn (unsigned)(-1)
//戻り値の番号まである（index_start==1で3が返ったら1,2,3がある）
inline uint32_t get_numdirectry_num(const dxle::tstring& file_name_before, const dxle::tstring& file_name_after, uint32_t index_start, uint32_t index_end = 10000)
{
	return get_numobject_num(file_name_before, file_name_after, index_start, index_end, PathIsDirectory);
}
