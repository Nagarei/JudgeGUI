#pragma once
#include "FormatToString.h"

//f1.a f2.a ... の番号がどこまで続くか調べる
//なかったらreturn (unsigned)(-1)
inline uint32_t get_numfile_num(dxle::tstring file_name_before, dxle::tstring file_name_after, uint32_t index_start, uint32_t index_end = 10000)
{
	//二分探索
	uint32_t begin = index_start, end = index_end;//[begin,end)
	dxle::tstring file_name_temp;
	auto is_exisist = [&file_name_temp, &file_name_before, &file_name_after](uint32_t num) {
		file_name_temp = file_name_before;
		file_name_temp += ToStringEx(num);
		file_name_temp += file_name_after;
		return PathFileExists(file_name_temp.c_str());
	};
	if (is_exisist(begin) == false) { return (uint32_t)(-1); }
	while (is_exisist(end)) {
		begin = end;
		assert(end < 0xffffffff / 2);
		end *= 2;
	}
	while (begin + 1 < end)
	{
		auto mid = begin + (end - begin) / 2;
		if (is_exisist(mid)) {
			begin = mid;
		}
		else {
			end = mid;
		}
	}
	return begin;
}
