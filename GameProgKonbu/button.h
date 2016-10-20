#pragma once
#include "useful_func_and_class.h"
class Button final
{
private:
	dxle::pointi pos1;
	dxle::sizei size;
	dxle::tstring str;
public:
	Button() = default;
	Button(dxle::pointi pos1, dxle::sizei size, dxle::tstring str):pos1(pos1),size(size), str(std::move(str)){}

	bool IsInArea(dxle::pointi point)const { return ::IsInArea(point, pos1, size); }

	void draw_box(dxle::dx_color_param color)const;
	enum class str_pos{center,left};
	void draw_str(dxle::dx_color_param string_color, str_pos str_pos_, int font = DX_DEFAULT_FONT_HANDLE)const;

	void set_area(dxle::pointi pos1_, dxle::sizei size_) { pos1 = pos1_; size = size_; }
	template<typename... Args>
	void set_str(Args&&... args) { str.assign(std::forward<Args>(args)...); }

	std::pair<dxle::pointi, dxle::sizei> get_area()const { return{ pos1, size }; }
	const dxle::tstring& get_str()const { return str; }
};
