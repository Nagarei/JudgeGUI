#include "ScrollBar.h"
#include <utility>
#include "useful_func_and_class.h"
#include "other_usefuls.h"

ScrollBar::ScrollBar()
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

void ScrollBar::set_bar_state(int32_t object_size_, int32_t page_size_, bool is_horizontal_)
{
	object_size = object_size_;
	page_size = page_size_;
	set_now_pos_raw(now_pos);
	is_horizontal = is_horizontal_;

	reset_mouse_state();
}
void ScrollBar::set_bar_size(dxle::sizei32 bar_size_)
{
	bar_size = bar_size_;
	if (is_horizontal) {
		std::swap(bar_size.width, bar_size.height);
	}
	assert(0 < bar_size.width && bar_size.width*2 <= bar_size.height);
	reset_mouse_state();
}

bool ScrollBar::update(uint32_t frame_time, dxle::pointi32 mouse_relative, int32_t wheel, bool mouse_left_input, uint32_t keyboard_input, uint32_t arrow_value)
{
	const auto arrow_size = bar_size.width;
	assert(0 < bar_size.width && bar_size.width * 2 <= bar_size.height);
	if (object_size <= page_size) {
		now_pos = 0;
		return false;
	}
	if (is_horizontal) {
		//x軸、y軸を入れ替えてやれば垂直と同じ
		std::swap(mouse_relative.x, mouse_relative.y);
	}

	FINALLY([&]() {
		last_mouse_input = mouse_left_input;
	});

	auto old_nowpos = now_pos;

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
			if (!is_holded && !last_mouse_input && mouse_left_input)
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

	//mouse_input_start_is_outの計算;
	if (mouse_input_start_is_out) {
		mouse_input_start_is_out = mouse_left_input;
	}
	else  if (!last_mouse_input && mouse_left_input)
	{
		mouse_input_start_is_out = (on_mouse_pos == mouse_pos::out);
	}


	return old_nowpos != now_pos;
}

void ScrollBar::draw(dxle::pointi32 bar_pos) const
{
	assert(0 < bar_size.width && bar_size.width * 2 <= bar_size.height);
	if (object_size <= page_size) {
		return;
	}

	constexpr dxle::rgb back_color{ 230, 231, 232 };
	constexpr dxle::rgb arrow_neutral_color{ 80,80,80 };
	constexpr dxle::rgb arrow_on_color{ 80,80,80 };
	constexpr dxle::rgb arrow_on_back_color{ 210,210,210 };
	constexpr dxle::rgb arrow_push_color{ 255,255,255 };
	constexpr dxle::rgb arrow_push_back_color{ 120,120,120 };
	constexpr dxle::rgb grip_neutral_color{ 193,193,193 };
	constexpr dxle::rgb grip_on_color{ 168,168,168 };
	constexpr dxle::rgb grip_push_color{ 120,120,120 };

	auto draw_fill_box_custom = [is_horizontal = is_horizontal, pos_bace = bar_pos](int32_t barscale_pixdiff, dxle::sizei32 barscale_pixsize, dxle::dx_color_param color, int32_t side_space = 0)
	{
		auto pos = pos_bace;
		barscale_pixsize.width -= side_space * 2;
		if (is_horizontal) {
			std::swap(barscale_pixsize.width, barscale_pixsize.height);
			pos.x += barscale_pixdiff;
			pos.y += side_space;
		}
		else {
			pos.x += side_space;
			pos.y += barscale_pixdiff;
		}
		DxLib::DrawFillBox(pos.x, pos.y, pos.x + barscale_pixsize.width, pos.y + barscale_pixsize.height, color.get());
	};
	auto draw_triangle_custom = [is_horizontal = is_horizontal, pos_bace = bar_pos](int32_t barscale_pixdiff, int32_t area_size, dxle::dx_color_param color, bool is_up)
	{
		auto pos = pos_bace;
		dxle::pointi32 pos1{ area_size / 2, area_size *  3 / 8 };
		dxle::pointi32 pos2{ area_size / 4, area_size - pos1.y };
		dxle::pointi32 pos3{ area_size - pos2.x, area_size - pos1.y };
		if (!is_up) {
			//上下反転
			pos2.y = pos1.y;
			pos1.y = pos3.y;
			pos3.y = pos2.y;
		}
		if (is_horizontal) {
			//90度回転
			std::swap(pos1.x, pos1.y);
			std::swap(pos2.x, pos2.y);
			std::swap(pos3.x, pos3.y);
			pos.x += barscale_pixdiff;
		}
		else {
			pos.y += barscale_pixdiff;
		}
		pos1 += pos;
		pos2 += pos;
		pos3 += pos;
		DxLib::DrawTriangle(pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y, color.get(), TRUE);
	};

	//後ろ描画
	draw_fill_box_custom(0, bar_size, back_color);
	//上下矢印描画
	const auto arrow_size = bar_size.width;
	if (on_mouse_pos == mouse_pos::up_arrow) {
		draw_fill_box_custom(0, { arrow_size,arrow_size }, last_mouse_input ? arrow_push_back_color : arrow_on_back_color);
	}
	draw_triangle_custom(0, arrow_size,
		(on_mouse_pos == mouse_pos::up_arrow) ?
		(last_mouse_input ? arrow_push_color : arrow_on_color)
		: (arrow_neutral_color), true);

	if (on_mouse_pos == mouse_pos::down_arrow) {
		draw_fill_box_custom(bar_size.height-arrow_size, { arrow_size,arrow_size }, last_mouse_input ? arrow_push_back_color : arrow_on_back_color);
	}
	draw_triangle_custom(bar_size.height - arrow_size, arrow_size,
		(on_mouse_pos == mouse_pos::down_arrow) ?
		(last_mouse_input ? arrow_push_color : arrow_on_color)
		: (arrow_neutral_color), false);
	//グリップ描画
	draw_fill_box_custom(arrow_size + to_bar_scale(now_pos / 1000), { bar_size.width, to_bar_scale(now_pos / 1000 + page_size) - to_bar_scale(now_pos / 1000) },
		is_holded ? grip_push_color :
		((on_mouse_pos == mouse_pos::grip) ? grip_neutral_color : grip_on_color)
		, bar_size.width / 4);
}

void ScrollBar::reset_mouse_state()
{
	mouse_input_start_is_out = true;
	is_holded = false;
	on_mouse_pos = mouse_pos::out;
}
