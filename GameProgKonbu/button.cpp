#include "button.h"

namespace {
	
}
void Button::draw_box(dxle::dx_color_param color) const
{
	DxLib::DrawRoundRect(pos1.x, pos1.y, pos1.x + size.width, pos1.y + size.height, 2, 2, color.get(), TRUE);
}

void Button::draw_str(dxle::dx_color_param string_color, str_pos str_pos_, int font) const
{
	if (size == 0) { return; }

	//ï`âÊîÕàÕí≤êÆ
	//@todo dxlibex
	RECT old_draw_area;
	DxLib::GetDrawArea(&old_draw_area);
	DxLib::SetDrawArea(
		std::max<int>(pos1.x, old_draw_area.left),
		std::max<int>(pos1.y, old_draw_area.top),
		std::min<int>(pos1.x + size.width, old_draw_area.right),
		std::min<int>(pos1.y + size.height, old_draw_area.bottom)
	);

	//ï∂éöï\é¶
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

	//ï`âÊîÕàÕí≤êÆ
	DxLib::SetDrawArea(
		(old_draw_area.left), (old_draw_area.top), (old_draw_area.right), (old_draw_area.bottom)
	);
}
