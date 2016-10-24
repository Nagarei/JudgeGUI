#include "ScrollBar.h"
#include <utility>
#include "useful_func_and_class.h"

ScroolBar::ScroolBar()
	: object_size(1)
	, page_size(1)
	, now_pos(0)
	, grip_start_mousepos(0)
	, grip_start_nowpos(0)
	, bar_size()
	, is_horizontal(false)
	, mouse_input_start_is_out(false)
	, is_holded(false)
	, on_mouse_pos(mouse_pos::out)
{
}

void ScroolBar::set_bar_state(int32_t object_size_, int32_t page_size_, bool is_horizontal_)
{
	object_size = object_size_;
	page_size = page_size_;
	set_now_pos_raw(now_pos);
	is_horizontal = is_horizontal_;

	reset_mouse_state();
}

void ScroolBar::update(uint32_t frame_time, dxle::pointi32 mouse_relative, int32_t wheel, bool mouse_left_input, uint32_t keyboard_input, uint32_t arrow_value)
{
	assert(0 < bar_size.width && 0 < bar_size.height);
	if (object_size <= page_size) {
		now_pos = 0;
		return;
	}
	if (is_horizontal) {
		//x軸、y軸を入れ替えてやれば垂直と同じ
		std::swap(mouse_relative.x, mouse_relative.y);
	}
	
	auto to_pix_scale = [object_size= object_size, bar_size_height = bar_size.height](int32_t bar_v) {
		return bar_v * object_size / bar_size_height;
	};
	auto to_bar_scale = [object_size = object_size, bar_size_height = bar_size.height](int32_t pix_v) {
		return pix_v * bar_size_height / object_size;
	};


	//マウス入力計算
	if (is_holded)
	{
		if (mouse_left_input) {
			auto mouse_diff = mouse_relative.y - grip_start_mousepos;
			now_pos = grip_start_nowpos + to_pix_scale(mouse_diff) * 1000;
		}
		else {
			is_holded = false;
		}
	}
	on_mouse_pos = mouse_pos::out;
	if ((0 <= mouse_relative.x && mouse_relative.x < bar_size.width) &&
		(0 <= mouse_relative.y && mouse_relative.y < bar_size.height))
	{
		//マウスがバーの上

		if (mouse_relative.y < arrow_size) {
			//上矢印
			on_mouse_pos = mouse_pos::up_arrow;
			if(!is_holded && mouse_left_input){ now_pos -= arrow_value * frame_time; }
		}
		else if (mouse_relative.y < arrow_size + to_bar_scale(now_pos / 1000)) {
			//上矢印とグリップの間
			on_mouse_pos = mouse_pos::up_space;
			if (!is_holded && mouse_left_input) { now_pos -= page_size * frame_time * 2; }
		}
		else if (mouse_relative.y < arrow_size + to_bar_scale(now_pos / 1000 + page_size)) {
			//グリップ
			on_mouse_pos = mouse_pos::grip;
			if (!is_holded && mouse_left_input)
			{
				is_holded = true;
				grip_start_nowpos = now_pos;
				grip_start_mousepos = mouse_relative.y;
			}
		}
		else if (mouse_relative.y < bar_size.height - arrow_size) {
			//下矢印とグリップの間
			on_mouse_pos = mouse_pos::down_space;
			if (!is_holded && mouse_left_input) { now_pos += page_size * frame_time * 2; }
		}
		else{
			//下矢印
			on_mouse_pos = mouse_pos::down_arrow;
			if (!is_holded && mouse_left_input) { now_pos += arrow_value * frame_time; }
		}

	}
	if (!is_holded)
	{
		now_pos += wheel * 1000;
	}

	//キーボード入力
	if (!is_holded)
	{
		if (keyboard_input & keyboard_input_mask::up) {
			now_pos -= arrow_value * frame_time;
		}
		if (keyboard_input & keyboard_input_mask::page_up) {
			now_pos -= page_size * frame_time * 2;
		}
		if (keyboard_input & keyboard_input_mask::down) {
			now_pos += arrow_value * frame_time;
		}
		if (keyboard_input & keyboard_input_mask::page_down) {
			now_pos += page_size * frame_time * 2;
		}
	}

	//now_posの補正
	set_now_pos_raw(now_pos);

	mouse_input_start_is_outの計算;
}

void ScroolBar::draw(dxle::pointi32 bar_pos) const
{
	assert(0 < bar_size.width && 0 < bar_size.height);
	if (object_size <= page_size) {
		return;
	}
	DEBUG_NOTE;
}

void ScroolBar::reset_mouse_state()
{
	mouse_input_start_is_out = true;
	is_holded = false;
	on_mouse_pos = mouse_pos::out;
}
