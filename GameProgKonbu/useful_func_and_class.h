#ifndef USEFUL_FUNC_AND_CLASS_H_201503222223_1223_
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

//!���F����������������(x:��,y:����,size:�c�̒����̔���)
inline void DrawToLeftArrow(int x, int y, int size_half, unsigned color)throw()
{
	DrawTriangle(x, y, x + (int)(sqrt3 * size_half), y - size_half, x + (int)(sqrt3 * size_half), y + size_half, color, TRUE);
}
//!���F���E������������(x:�E,y:����,size:�c�̒����̔���)
inline void DrawToRightArrow(int x, int y, int size_half, unsigned color)throw()
{
	DrawTriangle(x, y, x - (int)(sqrt3 * size_half), y - size_half, x - (int)(sqrt3 * size_half), y + size_half, color, TRUE);
}
//!���F����������������(x:��,y:����,size:���̒���)
inline void DrawToLeftArrow2(int x, int y, int width, dxle::dx_color_param color)throw()
{
	DrawTriangle(x, y, x + width, y - (int)(width / sqrt3), x + width, y + (int)(width / sqrt3), color.get(), TRUE);
}
//!���F���E������������(x:�E,y:����,size:�c�̒����̔���)
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
