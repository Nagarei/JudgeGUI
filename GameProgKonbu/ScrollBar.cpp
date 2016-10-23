#include "ScrollBar.h"
#include <utility>
#include "useful_func_and_class.h"

ScroolBar::ScroolBar()
	: bar_height(0)
	, object_size(1)
	, display_area_size(1)
	, now_pos(0)
	, hold_pos_correction(0)
	, is_horizontal(false)
	, last_mouse_input(false)
	, is_holded(false)
	, is_mouse_on_grip(false)
{
}

void ScroolBar::set(int32_t bar_size_, int32_t object_size_, int32_t display_area_size_, bool is_horizontal_)
{
	bar_height = bar_size_;
	object_size_ = object_size_;
	display_area_size = display_area_size_;
	now_pos = std::min(now_pos, std::max(0, object_size - display_area_size));
	is_horizontal = is_horizontal_;
}

void ScroolBar::update(dxle::point_c<int32_t> mouse_relative, bool mouse_left_input, uint32_t keyboard_input)
{
	if (bar_height == 0) { return; }
	if (object_size <= display_area_size) {
		now_pos = 0;
		return;
	}
	if (is_horizontal) {
		//x軸、y軸を入れ替えてやれば垂直と同じ
		std::swap(mouse_relative.x, mouse_relative.y);
	}

	FINALLY([&](){
		last_mouse_input = mouse_left_input; 
	});

	//マウス入力計算
	if (is_holded)
	{
		if (mouse_left_input) {
			int32_t grip_y = mouse_relative.y + hold_pos_correction;
			now_pos = to_display_scale(grip_y);
		}
		else {
			is_holded = false;
		}
	}
	else
	{
		if ((0 <= mouse_relative.x && mouse_relative.x < bar_width) &&
			(0 <= mouse_relative.y && mouse_relative.y < bar_height))
		{
			//マウスがバーの上
			if (mouse_relative.y < arrow_size) {
				//上矢印
				DEBUG_NOTE;
			}
			else if (to_bar_scale(now_pos) <= mouse_relative.y && mouse_relative.y < to_bar_scale(now_pos + object_size)) {
				DEBUG_NOTE;
			}
			else if (mouse_relative.y - arrow_size <= mouse_relative.y) {
				//下矢印
				DEBUG_NOTE;
			}
			is_mouse_on_bar = true;
			DEBUG_NOTE;
		}
	}

	//キーボード入力
	if (!is_holded)
	{
		DEBUG_NOTE;
	}

	//now_posの補正
	if (now_pos < 0) { now_pos = 0; }
	else if (std::max(0, object_size - display_area_size) < now_pos) {
		now_pos = std::max(0, object_size - display_area_size);
	}
}

void ScroolBar::draw() const
{
	if (bar_height == 0) { return; }
	DEBUG_NOTE;
}
