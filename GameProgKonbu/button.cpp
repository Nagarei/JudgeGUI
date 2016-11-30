#include "button.h"

namespace {

}
void Button::set_on_color(dxle::dx_color_param on_back_color_, dxle::dx_color_param on_edge_color_, dxle::dx_color_param on_string_color_)
{
	on_back_color = on_back_color_;
	on_edge_color = on_edge_color_;
	on_string_color = on_string_color_;
}
void Button::set_out_color(dxle::dx_color_param out_back_color_, dxle::dx_color_param out_edge_color_, dxle::dx_color_param out_string_color_)
{
	out_back_color   = out_back_color_;
	out_edge_color   = out_edge_color_;
	out_string_color = out_string_color_;

}
bool Button::update(dxle::pointi32 mouse_pos, bool mouse_left_input)
{
	FINALLY([&](){
		last_mouse_input = mouse_left_input;
	});
	is_in_area = ::IsInArea(mouse_pos, pos1, size);
	if (mouse_input_start_is_out) {
		mouse_input_start_is_out = last_mouse_input;
	}
	else {
		if ((!last_mouse_input) && mouse_left_input) {
			mouse_input_start_is_out = !is_in_area;
		}
		else if (last_mouse_input && !mouse_left_input) {
			return is_in_area;
		}
	}
	return false;
}
void Button::draw(int font)const
{
	draw_box(is_in_area || (last_mouse_input && !mouse_input_start_is_out));
	draw_str(is_in_area || (last_mouse_input && !mouse_input_start_is_out), Button_string_pos::center, font);
}

void Button::draw_box(bool is_on) const
{
	//DxLib::DrawRoundRect(pos1.x, pos1.y, pos1.x + size.width, pos1.y + size.height, 2, 2, color.get(), TRUE);
	dxle::dx_color_param color{ on_back_color }, edge_color{ on_edge_color };
	if (is_on) {
		//color = on_back_color;
		//edge_color = on_edge_color;
	}
	else {
		color = out_back_color;
		edge_color = out_edge_color;
	}
	DrawButton(pos1, size, color, edge_color);
}

void Button::draw_str(bool is_on, Button_string_pos str_pos_, int font) const
{
	if (size == 0) { return; }

	DrawButtonString(pos1, size, str, is_on ? on_string_color : out_string_color,
		str_pos_, font);
}

void DrawButton(dxle::pointi32 pos1, dxle::sizei32 size, dxle::dx_color_param color, dxle::dx_color_param edge_color)
{
	DxLib::DrawFillBox(pos1.x, pos1.y, pos1.x + size.width, pos1.y + size.height, color.get());
	DxLib::DrawLineBox(pos1.x, pos1.y, pos1.x + size.width, pos1.y + size.height, edge_color.get());
}

void DrawButtonString(dxle::pointi32 pos1, dxle::sizei32 size, const dxle::tstring& str, dxle::dx_color_param string_color, Button_string_pos str_pos_, int font, int32_t padding)
{
	RECT bef_draw_area;
	DxLib::GetDrawArea(&bef_draw_area);//@todo dxlibex
	FINALLY([&bef_draw_area]() {
		DxLib::SetDrawArea(bef_draw_area.left, bef_draw_area.top, bef_draw_area.right, bef_draw_area.bottom);
	});
	DxLib::SetDrawArea(pos1.x + padding, pos1.y + padding, pos1.x + size.width - padding, pos1.y + size.height - padding);
	dxle::sizei str_size;
	DxLib::GetDrawStringSizeToHandle(&str_size.width, &str_size.height, nullptr,
		str.c_str(), str.size(), font);//@todo dxlibex
	switch (str_pos_)
	{
	case Button_string_pos::center:
		DrawStringCenter2(pos1 + dxle::sizei32{ padding,padding }, str.c_str(), string_color, font, size - 2 * dxle::sizei32{ padding ,padding });
		//DrawStringToHandle(pos1.x + padding + (size.width - 2 * padding - str_size.width) / 2,
		//	pos1.y + padding + (size.height - 2 * padding - str_size.height) / 2,
		//	str.c_str(), string_color.get(), font);
		break;
	case Button_string_pos::left:
		DrawStringToHandle(pos1.x + padding, pos1.y + padding + (size.height - 2 * padding - str_size.height) / 2,
			str.c_str(), string_color.get(), font);
		break;
	default:
		break;
	}
}
