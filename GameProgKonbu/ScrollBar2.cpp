#include "ScrollBar2.h"
#include "Mouse.h"
#include "KeyInputData.h"
#include "fps.h"

namespace {
	constexpr int32_t bar_width = 17;
	constexpr int32_t wheel_coefficient = 25;//ホイール係数
}

ScrollBar2::ScrollBar2()
{}

void ScrollBar2::reset_Scroll(dxle::sizei32 bar_area_param, dxle::sizei32 object_size)
{
	dxle::sizei32 page_size = bar_area_param;
	if (bar_area_param != bar_area) {
		extend_rate = 1.0;
	}
	object_size *= extend_rate;

	//バーの有効無効でpageサイズが変わってくるのに注意
	scrollbar_v.set_bar_state(object_size.height, bar_area.height, false);
	bool bar_v_p_all_active = scrollbar_v.is_active();
	if (bar_v_p_all_active) {
		page_size.width -= bar_width;
	}
	scrollbar_h.set_bar_state(object_size.width, page_size.width, true);
	if (scrollbar_h.is_active()) {
		page_size.height -= bar_width;
		scrollbar_v.set_bar_state(object_size.height, page_size.height, false);
		if (!bar_v_p_all_active && scrollbar_v.is_active()) {
			//縦のバーが有効になった=>page_sizeの再計算発生
			page_size.width -= bar_width;
			scrollbar_h.set_bar_state(object_size.width, page_size.width, true);
		}
	}

	//バーの描画サイズセット
	scrollbar_v.set_bar_size({ bar_width, page_size.height });
	scrollbar_h.set_bar_size({ page_size.width , bar_width });

}

void ScrollBar2::update_Scroll()
{
	auto& key = KeyInputData::GetIns();
	auto& mouse = Mouse::GetIns();

	//拡大/縮小入力
	if (key.GetKeyInput(KEY_INPUT_LCONTROL) || key.GetKeyInput(KEY_INPUT_RCONTROL))
	{
		auto old_extend_rate = extend_rate;
		extend_rate += mouse.get_now_wheel() * 10.0 / 100.0;
		if (key.GetKeyInput(KEY_INPUT_0) || key.GetKeyInput(KEY_INPUT_NUMPAD0)) {
			extend_rate = 1.0;
		}
		if (old_extend_rate != extend_rate) {
			reset_Scroll();
			return;
		}
	}
	//スクロール
	uint32_t keyinput = 0;
	int32_t v_wheel = -mouse.get_now_wheel() * wheel_coefficient;
	int32_t h_wheel = -mouse.get_now_H_wheel() * wheel_coefficient;
	if (key.GetKeyInput(KEY_INPUT_LSHIFT) || key.GetKeyInput(KEY_INPUT_RSHIFT)) {
		std::swap(v_wheel, h_wheel);
	}
	if (key.GetKeyInput(KEY_INPUT_UP)  ) { keyinput |= ScrollBar::keyboard_input_mask::up; }
	if (key.GetKeyInput(KEY_INPUT_DOWN)) { keyinput |= ScrollBar::keyboard_input_mask::down; }
	if (key.GetKeyInput(KEY_INPUT_PGUP)) { keyinput |= ScrollBar::keyboard_input_mask::page_up; }
	if (key.GetKeyInput(KEY_INPUT_PGDN)) { keyinput |= ScrollBar::keyboard_input_mask::page_down; }
	scrollbar_v.update(GetFrameTime(),
		mouse.get_now_pos() - (pos1 + dxle::sizei32{ bar_area.width - bar_width, 0}),
		v_wheel, mouse.get_now_input() & MOUSE_INPUT_LEFT, keyinput);
	keyinput = 0;
	if (key.GetKeyInput(KEY_INPUT_LEFT) ) { keyinput |= ScrollBar::keyboard_input_mask::up; }
	if (key.GetKeyInput(KEY_INPUT_RIGHT)) { keyinput |= ScrollBar::keyboard_input_mask::down; }
	scrollbar_h.update(GetFrameTime(),
		mouse.get_now_pos() - (pos1 + dxle::sizei32{ 0, bar_area.height - bar_width }),
		h_wheel, mouse.get_now_input() & MOUSE_INPUT_LEFT, keyinput);

}

void ScrollBar2::draw_Scroll() const
{
	scrollbar_v.draw(pos1 + dxle::sizei32{ bar_area.width - bar_width, 0 });
	scrollbar_h.draw(pos1 + dxle::sizei32{ 0, bar_area.height - bar_width });
	if (scrollbar_v.is_active() && scrollbar_h.is_active()) {
		auto pos2 = pos1 + bar_area;
		DxLib::DrawFillBox(pos2.width - bar_width, pos2.height - bar_width
			, pos2.width, pos2.height, DxLib::GetColor(230, 231, 232));
	}
}
