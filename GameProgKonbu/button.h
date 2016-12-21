#pragma once
#include "my_utility.h"

void DrawButton(dxle::pointi32 pos1, dxle::sizei32 size, dxle::dx_color_param color, dxle::dx_color_param edge_color);
enum class Button_string_pos { center, left };
void DrawButtonString(dxle::pointi32 pos1, dxle::sizei32 size, const dxle::tstring& str, dxle::dx_color_param string_color,
	Button_string_pos str_pos_, int font = DX_DEFAULT_FONT_HANDLE, int32_t padding = 2);
class Button final
{
private:
	dxle::dx_color_param out_back_color = dxle::color_tag::white;
	dxle::dx_color_param out_edge_color = dxle::color_tag::gray;
	dxle::dx_color_param out_string_color = dxle::color_tag::black;
	dxle::dx_color_param on_back_color = dxle::color_tag::black;
	dxle::dx_color_param on_edge_color = dxle::color_tag::gray;
	dxle::dx_color_param on_string_color = dxle::color_tag::white;
	dxle::pointi32 pos1;
	dxle::sizei32 size;
	dxle::tstring str;
	bool last_mouse_input = true;
	bool mouse_input_start_is_out = true;
	bool is_in_area = false;
public:
	Button() = default;
	Button(dxle::pointi32 pos1, dxle::sizei32 size, dxle::tstring str) :pos1(pos1), size(size), str(std::move(str)) {}
	void set_area(dxle::pointi pos1_, dxle::sizei size_) { pos1 = pos1_; size = size_; }
	template<typename... Args>
	void set_str(Args&&... args) { str.assign(std::forward<Args>(args)...); }
	void set_on_color(
		dxle::dx_color_param on_back_color,
		dxle::dx_color_param on_edge_color,
		dxle::dx_color_param on_string_color);
	void set_out_color(
		dxle::dx_color_param out_back_color,
		dxle::dx_color_param out_edge_color,
		dxle::dx_color_param out_string_color);

	//@return true: 入力アリ
	bool update(dxle::pointi32 mouse_pos, bool mouse_left_input);
	void draw(int font = DX_DEFAULT_FONT_HANDLE)const;

	bool IsInArea(dxle::pointi point)const { return ::IsInArea(point, pos1, size); }

	void draw_box(bool is_on)const;
	void draw_str(bool is_on, Button_string_pos str_pos_, int font = DX_DEFAULT_FONT_HANDLE)const;


	std::pair<dxle::pointi, dxle::sizei> get_area()const { return{ pos1, size }; }
	const dxle::tstring& get_str()const { return str; }
};
