#include "ScrollBar.h"
#include <utility>
#include "useful_func_and_class.h"

ScroolBar::ScroolBar()
	: object_size(1)
	, page_size(1)
	, now_pos(0)
	, grip_start_mousepos(0)
	, grip_start_nowpos(0)
	, is_horizontal(false)
	, last_mouse_input(false)
	, is_holded(false)
	, on_mouse_pos(mouse_pos::out)
{
}

void ScroolBar::set(int32_t object_size_, int32_t page_size_, bool is_horizontal_)
{
	object_size = object_size_;
	page_size = page_size_;
	now_pos = std::min(now_pos, std::max(0, object_size - page_size) - 1);
	is_horizontal = is_horizontal_;
}

void ScroolBar::update(uint32_t bar_height, dxle::point_c<int32_t> mouse_relative, int32_t wheel, bool mouse_left_input, uint32_t keyboard_input, uint32_t arrow_value)
{
	assert(0 < bar_height);
	if (object_size <= page_size) {
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
	
	auto to_pix_scale = [object_size= object_size, bar_height](int32_t bar_v) {
		return bar_v * object_size / bar_height;
	};
	auto to_bar_scale = [object_size = object_size, bar_height](int32_t pix_v) {
		return pix_v * bar_height / object_size;
	};

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
				on_mouse_pos = mouse_pos::up_arrow;
			}
			else if (mouse_relative.y < arrow_size + to_bar_scale(now_pos)) {
				//上矢印とグリップの間
				on_mouse_pos = mouse_pos::up_space;
			}
			else if (mouse_relative.y < arrow_size + to_bar_scale(now_pos + page_size)) {
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
