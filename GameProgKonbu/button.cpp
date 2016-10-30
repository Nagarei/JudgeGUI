#include "button.h"

namespace {

}
bool Button::update(dxle::pointi32 mouse_pos, bool mouse_left_input, uint32_t keyboard_input)
{
	FINALLY([&](){
		last_mouse_input = mouse_left_input;
	});
	is_in_area = ::IsInArea(mouse_pos, pos1, size);
	if (mouse_input_start_is_out) {
		mouse_input_start_is_out = last_mouse_input;
	}
	else {
		if ((!last_input) && mouse_left_input) {
			mouse_input_start_is_out = !is_in_area;
		}
		else if (last_input && !mouse_left_input) {
			return is_in_area;
		}
	}
	return false;
}
void Button::draw(dxle::pointi32 bar_pos)const
{
	dxle::dx_color color, edge_color;
	if (is_in_area) {
		color = on_back_color;
		edge_color = on_edge_color;
	}
	else {
		color = out_back_color;
		edge_color = out_edge_color;
	}
	DxLib::DrawFillBox(pos1.x, pos1.y, pos1.x + size.width, pos1.y + size.height, color.get());
	DxLib::DrawLineBox(pos1.x, pos1.y, pos1.x + size.width, pos1.y + size.height, edge_color.get());
}

void Button::draw_box(dxle::dx_color_param color) const
{
	DxLib::DrawRoundRect(pos1.x, pos1.y, pos1.x + size.width, pos1.y + size.height, 2, 2, color.get(), TRUE);
}

void Button::draw_str(dxle::dx_color_param string_color, str_pos str_pos_, int font) const
{
	if (size == 0) { return; }

	//描画範囲調整
	//@todo dxlibex
	RECT old_draw_area;
	DxLib::GetDrawArea(&old_draw_area);
	DxLib::SetDrawArea(
		std::max<int>(pos1.x, old_draw_area.left),
		std::max<int>(pos1.y, old_draw_area.top),
		std::min<int>(pos1.x + size.width, old_draw_area.right),
		std::min<int>(pos1.y + size.height, old_draw_area.bottom)
	);

	//文字表示
	switch (str_pos_)
	{
	case Button::str_pos::left:
		DxLib::DrawStringToHandle(pos1.x, pos1.y, str.c_str(), string_color.get(), font);
		break;
	default:
		assert(false);
		//fall
	case Button::str_pos::center: {
		int str_size_x, str_size_y;
		DxLib::GetDrawStringSizeToHandle(&str_size_x, &str_size_y, nullptr, str.c_str(), str.size(), font);
		DxLib::DrawStringToHandle(pos1.x + (size.width - str_size_x) / 2, pos1.y + (size.height - str_size_y) / 2,
			str.c_str(), string_color.get(), font);
	}
		break;
	}

	//描画範囲調整
	DxLib::SetDrawArea(
		(old_draw_area.left), (old_draw_area.top), (old_draw_area.right), (old_draw_area.bottom)
	);
}
