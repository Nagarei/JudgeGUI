#pragma once
#include "useful_func_and_class.h"
class Button final
{
private:
				dxle::dx_color out_edge_color;
				dxle::dx_color out_back_color;
				dxle::dx_color on_edge_color;
				dxle::dx_color on_back_color;
				dxle::pointi32 pos1;
				dxle::sizei32 size;
				dxle::tstring str;
				bool last_mouse_input;
				bool mouse_input_start_is_out;
				bool is_in_area;
public:
  Button();
	Button(dxle::pointi32 pos1, dxle::sizei32 size, dxle::tstring str):pos1(pos1),size(size), str(std::move(str)){}
				
				void set(

dxle::dx_color out_edge_color,
    dxle::dx_color out_back_color,
				dxle::dx_color on_edge_color,
				dxle::dx_color on_back_color,
				dxle::pointi32 pos1,
	dxle::sizei32 size,
	dxle::tstring str);
				
				//@return true: にゆうりよくあり
				bool update(dxle::pointi32 mouse_pos, bool mouse_left_input, uint32_t keyboard_input = false);
				void draw(dxle::pointi32 bar_pos)const;

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

void DrawButton(dxle::pointi32 pos1, dxle::sizei32 size, dxle::dx_color_param color, dxle::dx_color_param back_color);
enum class str_pos{center,left};
void DrawButtonString(dxle::dx_color_param string_color, str_pos str_pos_, int font = DX_DEFAULT_FONT_HANDLE, int32_t padding = 2)
