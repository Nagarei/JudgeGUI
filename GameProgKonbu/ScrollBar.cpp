#include "ScrollBar.h"
#include <utility>
#include "useful_func_and_class.h"

ScroolBar::ScroolBar()
	: bar_height(0)
	, object_size(1)
	, display_area_size(1)
	, now_pos(0)
	, hold_start_pos(0)
	, is_horizontal(false)
	, last_mouse_input(false)
	, is_holded(false)
	, is_mouse_on_bar(false)
{
}

void ScroolBar::set(int32_t bar_size_, int32_t object_size_, int32_t display_area_size_, bool is_horizontal_)
{
	bar_height = bar_size_;
	object_size_ = object_size_;
	display_area_size = display_area_size_;
	now_pos = std::min(now_pos, std::max(0u, object_size - display_area_size));
	is_horizontal = is_horizontal_;
}

void ScroolBar::update(dxle::point_c<int32_t> mouse_relative, bool mouse_left_input, uint32_t keyboard_input)
{
	if (bar_height == 0) { return; }
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
			now_pos = grip_y * object_size / bar_height;
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
			is_mouse_on_bar = true;
			DEBUG_NOTE;
		}
	}

	//キーボード入力
	if (!is_holded)
	{
		DEBUG_NOTE;
	}
}

void ScroolBar::draw() const
{
	if (bar_height == 0) { return; }
	DEBUG_NOTE;
}
