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
	//@param object_size_ 動かすものの大きさ
	//@param page_size_ 物を表示する大きさ(バー含む)
	//@param is_horizontal_ trueで横向きバー
	ScroolBar(int32_t object_size_, int32_t page_size_, bool is_horizontal_ = false) :ScroolBar() { set(object_size_, page_size_, is_horizontal_); }
	//@param object_size_ 動かすものの大きさ
	//@param page_size_ 物を表示する大きさ(バー含む)
	//@param is_horizontal_ trueで横向きバー
	void set(int32_t object_size_, int32_t page_size_, bool is_horizontal_ = false);

	struct keyboard_input_mask{
		static const uint32_t up        = 0b0001;
		static const uint32_t down      = 0b0010;
		static const uint32_t page_up   = 0b0100;
		static const uint32_t page_down = 0b1000;
	};
	//@param bar_size バーの描画y(横の時はx)サイズ
	//@param mouse_relative バーの左上を原点としたマウスの相対座標
	//@param wheel マウスのホイールの回転量
	//@param mouse_left_input マウスの「掴む」ボタンの入力
	//@param keyboard_input keyboard_input_maskで構成されたキーの入力状況
	//@param arrow_value 矢印ボタンが押されたときの移動量
	void update(uint32_t bar_size, dxle::point_c<int32_t> mouse_relative, int32_t wheel, bool mouse_left_input, uint32_t keyboard_input, uint32_t arrow_value = 2);
	void draw(dxle::pointi32 bar_pos, uint32_t bar_size)const;
	
	//どれだけずらすべきか取得
	int32_t get_value()const { return now_pos; }
private:
	int32_t object_size;//ずらしたいオブジェクトのyサイズ(pix)
	int32_t page_size;//スクロールバー部分を除いたオブジェクトの描画範囲(pix)
	DEBUG_NOTE;//↑スクロールバーを不要なときに消す動作を実装したい
	int32_t now_pos;//ずらすピクセル [0,object_size-page_size) (pix)
	int32_t grip_start_mousepos;//グリップが始まったときのマウスの位置
	int32_t grip_start_nowpos;//グリップが始まったときのnow_pos
	bool is_horizontal;
	bool last_mouse_input;
	bool is_holded;//マウスに掴まれているか
	enum class mouse_pos{up_arrow, up_space, grip, down_space, down_arrow, out}on_mouse_pos;
	static const int32_t bar_width = 17;
	static const int32_t arrow_size = 15;
};
