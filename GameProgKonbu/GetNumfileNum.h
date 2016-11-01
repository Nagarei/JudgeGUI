#pragma once
#include "FormatToString.h"

template<typename FUNC>
//f1.a f2.a ... の番号がどこまで続くか調べる
//なかったらreturn (unsigned)(-1)
inline uint32_t get_numobject_num(const dxle::tstring& file_name_before, const dxle::tstring& file_name_after, uint32_t index_start, uint32_t index_end, FUNC&& check_func)
{
	TCHAR buf[20];
	auto  my_itoa = [&buf](uint32_t n) {
		auto iter = buf;
		//uint32_t::max 4294967295
		uint32_t div = 1000000000;
		for (; 0 < div; div /= 10) {
			auto num = (n / div) % 10;
			if (iter != buf || num != 0) {
				*iter++ = static_cast<TCHAR>(num + _T('0'));
			}
		}
		if (iter == buf) {
			*iter++ = _T('0');
		}
		*iter = _T('\0');
		return buf;
	};
	//二分探索
	uint32_t begin = index_start, end = index_end;//[begin,end)
	dxle::tstring file_name_temp;
	auto is_exists = [&file_name_temp, &file_name_before, &file_name_after, &check_func, &my_itoa](uint32_t num) {
		file_name_temp = file_name_before;
		file_name_temp += my_itoa(num);
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
inline uint32_t get_numdirectry_num(const dxle::tstring& file_name_before, const dxle::tstring& file_name_after, uint32_t index_start, uint32_t index_end = 10000)
{
	return get_numobject_num(file_name_before, file_name_after, index_start, index_end, PathIsDirectory);
}
