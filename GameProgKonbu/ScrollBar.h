#pragma once
#include <cstdint>

#if !defined(__clang__) && 1
static_assert(false, "");
using int32_t = int;
using uint32_t = unsigned;
#endif
class ScroolBar
{
public:
	ScroolBar();
	//@param bar_size_ スクロールバーのサイズ
	//@param object_size_ 動かすものの大きさ
	//@param display_area_size_ 物を表示する大きさ
	//@param is_horizontal_ trueで横向きバー
	ScroolBar(int32_t bar_size_, int32_t object_size_, int32_t display_area_size_, bool is_horizontal_ = false) :ScroolBar() { set(bar_size_, object_size_, display_area_size_, is_horizontal_); }
	//@param bar_size スクロールバーのサイズ
	//@param object_size 動かすものの大きさ
	//@param display_area_size 物を表示する大きさ
	//@param is_horizontal trueで横向きバー
	void set(int32_t bar_size, int32_t object_size, int32_t display_area_size, bool is_horizontal = false);

	struct keyboard_input_mask{
		static const uint32_t up        = 0b0001;
		static const uint32_t down      = 0b0010;
		static const uint32_t page_up   = 0b0100;
		static const uint32_t page_down = 0b1000;
	};
	//@param mouse_relative バーの左上を原点としたマウスの相対座標
	//@param mouse_left_input マウスの「掴む」ボタンの入力
	void update(dxle::point_c<int32_t> mouse_relative, bool mouse_left_input, uint32_t keyboard_input);
	void draw()const;
	
	//どれだけずらすべきか取得
	int32_t get_value()const { return now_pos; }
private:
	int32_t bar_height;//スクロールバー自体の描画サイズ
	int32_t object_size;//ずらしたいオブジェクトのyサイズ
	int32_t display_area_size;//スクロールバー部分を除いたオブジェクトの描画範囲
	DEBUG_NOTE;//↑スクロールバーを不要なときに消す動作を実装したい
	int32_t now_pos;//ずらすピクセル [0,display_area_size-object_size) 描画座標スケール
	int32_t hold_pos_correction;//マウスに掴まれているとき grip_y = mouse_y + hold_pos_correction になる値 (old_grip_y-old_mouse_y) バー上座標スケール
	bool is_horizontal;
	bool last_mouse_input;
	bool is_holded;//マウスに掴まれているか
	bool is_mouse_on_grip;//マウスがバーの上にいるか
	static const int32_t bar_width = 17;
	static const int32_t arrow_size = 15;

	inline int32_t to_display_scale(int32_t bar_pos)const { return     bar_pos * object_size / bar_height; }//バー上座標=>描画座標の変換
	inline int32_t to_bar_scale(int32_t display_pos)const { return display_pos * bar_height / object_size; }//描画座標=>バー上座標の変換
};
