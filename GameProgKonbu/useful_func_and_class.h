﻿#ifndef USEFUL_FUNC_AND_CLASS_H_201503222223_1223_
#define USEFUL_FUNC_AND_CLASS_H_201503222223_1223_

using dxColor = decltype(GetColor(255, 255, 255));
using tifstream = std::basic_ifstream<TCHAR>;
using tofstream = std::basic_ofstream<TCHAR>;

static constexpr double sqrt3 = (1.7320508075688772935274463415059);

template<typename RandomAccessIterator>
inline typename std::enable_if<
	std::is_base_of<std::random_access_iterator_tag, typename std::iterator_traits<RandomAccessIterator>::iterator_category>::value
>::type shuffle_use_DxLib(RandomAccessIterator first, RandomAccessIterator last)
{
	auto num(std::distance(first, last) - 1);
	while (num > 0)
	{
		std::iter_swap(first, first + DxLib::GetRand(num));
		++first;
		--num;
	}
}


inline void DrawBoxWithFrame(dxle::pointi pos1, dxle::pointi pos2, dxle::dx_color_param color, dxle::dx_color_param edge_color) {
	dxle::DrawBox(pos1, pos2, color, true);
	dxle::DrawBox(pos1, pos2, edge_color, false);
};
template<typename... Args>
inline void DrawStringRight(dxle::pointi pos, const TCHAR*const str, dxle::dx_color_param color, int font, dxle::sizei area, Args&&... args)
{
	dxle::sizei temp_size;
	DxLib::GetDrawFormatStringSizeToHandle(&temp_size.width, &temp_size.height, nullptr,
		font, str, args...);//@todo dxlibex
	auto diff = (area - temp_size); diff.height /= 2;
	DrawFormatStringToHandle(pos.x + diff.width, pos.y + diff.height, color.get(), font, str, std::forward<Args>(args)...);
}
inline void DrawStringRight(dxle::pointi pos, const TCHAR*const str, dxle::dx_color_param color, int font, dxle::sizei area)
{
	dxle::sizei temp_size;
	DxLib::GetDrawStringSizeToHandle(&temp_size.width, &temp_size.height, nullptr,
		str, -1, font);//@todo dxlibex
	auto diff = (area - temp_size); diff.height /= 2;
	DrawStringToHandle(pos.x + diff.width, pos.y + diff.height, str, color.get(), font);
}
inline void DrawStringCenter2(dxle::pointi pos, const TCHAR*const str, dxle::dx_color_param color, int font, dxle::sizei area)
{
	dxle::sizei temp_size;
	DxLib::GetDrawStringSizeToHandle(&temp_size.width, &temp_size.height, nullptr,
		str, -1, font);//@todo dxlibex
	auto diff = (area - temp_size) / 2;
	DrawStringToHandle(pos.x + diff.width, pos.y + diff.height, str, color.get(), font);
}
template<typename... Args>
inline void DrawStringCenter2(dxle::pointi pos, const TCHAR*const str, dxle::dx_color_param color, int font, dxle::sizei area, Args&&... args)
{
	dxle::sizei temp_size;
	DxLib::GetDrawFormatStringSizeToHandle(&temp_size.width, &temp_size.height, nullptr,
		font, str, args...);//@todo dxlibex
	auto diff = (area - temp_size) / 2;
	DrawFormatStringToHandle(pos.x + diff.width, pos.y + diff.height, color.get(), font, str, std::forward<Args>(args)...);
}
inline void DrawStringCenter(dxle::pointi pos, const TCHAR*const str, dxle::dx_color_param color, int font, int area_width)
{
	auto str_width = DxLib::GetDrawStringWidthToHandle(str, -1, font);
	DrawStringToHandle(pos.x + (area_width - str_width)/2, pos.y, str, color.get(), font);
}
template<typename... Args>
inline void DrawStringCenter(dxle::pointi pos, const TCHAR*const str, dxle::dx_color_param color, int font, int area_width, Args&&... args)
{
	auto str_width = DxLib::GetDrawFormatStringWidthToHandle(font, str, args...);
	DrawFormatStringToHandle(pos.x + (area_width - str_width) / 2, pos.y, color.get(), font, str, std::forward<Args>(args)...);
}

//!黄色い左向き矢印を書く(x:左,y:中央,size:縦の長さの半分)
inline void DrawToLeftArrow(int x, int y, int size_half, unsigned color)throw()
{
	DrawTriangle(x, y, x + (int)(sqrt3 * size_half), y - size_half, x + (int)(sqrt3 * size_half), y + size_half, color, TRUE);
}
//!黄色い右向き矢印を書く(x:右,y:中央,size:縦の長さの半分)
inline void DrawToRightArrow(int x, int y, int size_half, unsigned color)throw()
{
	DrawTriangle(x, y, x - (int)(sqrt3 * size_half), y - size_half, x - (int)(sqrt3 * size_half), y + size_half, color, TRUE);
}
//!黄色い左向き矢印を書く(x:左,y:中央,size:横の長さ)
inline void DrawToLeftArrow2(int x, int y, int width, dxle::dx_color_param color)throw()
{
	DrawTriangle(x, y, x + width, y - (int)(width / sqrt3), x + width, y + (int)(width / sqrt3), color.get(), TRUE);
}
//!黄色い右向き矢印を書く(x:右,y:中央,size:縦の長さの半分)
inline void DrawToRightArrow2(int x, int y, int width, dxle::dx_color_param color)throw()
{
	DrawTriangle(x, y, x - width, y - (int)(width / sqrt3), x - width, y + (int)(width / sqrt3), color.get(), TRUE);
}

inline dxle::tstring operator"" _ts(const TCHAR* s, std::size_t l)
{
	return{ s,l };
}

template<typename FUNC>
struct Finally_C {
	Finally_C(FUNC&& func_):func(std::forward<FUNC>(func_)){}
	~Finally_C() { func(); }
	FUNC func;
};
template<typename FUNC>
Finally_C<FUNC> Finally(FUNC&& func_) {
	return Finally_C<FUNC>{ std::forward<FUNC>(func_) };
}
#define FINALLY_2(FUNC,N) \
	auto&& FINALLY_ ## N ## _FINALLY_ = Finally(FUNC);(void*)(&FINALLY_ ## N ## _FINALLY_)
#define FINALLY_1(FUNC,N) FINALLY_2(FUNC, N)
#define FINALLY(FUNC) FINALLY_1(FUNC, __COUNTER__)

inline bool IsInArea(dxle::pointi pos, dxle::pointi area_pos1, dxle::sizei area_size)
{
	return ((area_pos1.x <= pos.x && pos.x < area_pos1.x + area_size.width) &&
		(area_pos1.y <= pos.y && pos.y < area_pos1.y + area_size.height));
}

#endif
