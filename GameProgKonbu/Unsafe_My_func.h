#pragma once


inline TCHAR* my_itoa(uint32_t n, TCHAR buf[]) {
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
	*iter = '\0';
	return buf;
}
template<size_t N>
inline void my_strcpy(TCHAR(&buf)[N], const dxle::tstring str) {
	auto iter = std::begin(buf), iter_end = std::end(buf);
	auto str_iter = std::begin(str), str_iter_end = std::end(str);
	while (str_iter != str_iter_end && iter != iter_end) {
		*iter++ = *str_iter++;
	}
}
