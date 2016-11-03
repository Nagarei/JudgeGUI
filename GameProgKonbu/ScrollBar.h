#pragma once
#include <cstdint>

class ScrollBar final
{
public:
	ScrollBar();
	//@param bar_size バーの描画サイズ
	//@param object_size_ 動かすものの大きさ
	//@param page_size_ 物を表示する大きさ(バー含む)
	//@param is_horizontal_ trueで横向きバー
	ScrollBar(dxle::sizei32 bar_size_, int32_t object_size_, int32_t page_size_, bool is_horizontal_ = false) :ScrollBar() { set_bar_size(bar_size_); set_bar_state(object_size_, page_size_, is_horizontal_); }
	//@param object_size_ 動かすものの大きさ
	//@param page_size_ 物を表示する大きさ(バー含む)
	//@param is_horizontal_ trueで横向きバー
	void set_bar_state(int32_t object_size_, int32_t page_size_, bool is_horizontal_ = false);
	//@param bar_size バーの描画サイズ
	void set_bar_size(dxle::sizei32 bar_size_);
	//@return 1 object_size_ 動かすものの大きさ
	//@return 2 page_size_ 物を表示する大きさ(バー含む)
	//@return 3 is_horizontal_ trueで横向きバー
	std::tuple<int32_t, int32_t,bool> get_bar_state();
	//@return bar_size バーの描画サイズ
	dxle::sizei32 get_bar_size();

	struct keyboard_input_mask{
		static const uint32_t up        = 0b0001;
		static const uint32_t down      = 0b0010;
		static const uint32_t page_up   = 0b0100;
		static const uint32_t page_down = 0b1000;
	};
	//@param frame_time 前回の呼び出しからの経過ミリ秒
	//@param mouse_relative バーの左上を原点としたマウスの相対座標
	//@param wheel マウスのホイールの回転量
	//@param mouse_left_input マウスの「掴む」ボタンの入力
	//@param keyboard_input keyboard_input_maskで構成されたキーの入力状況
	//@param arrow_value 矢印ボタンが押されたときの移動量(pix/sec)
	void update(uint32_t frame_time, dxle::pointi32 mouse_relative, int32_t wheel, bool mouse_left_input, uint32_t keyboard_input, uint32_t arrow_value = 200);
	void draw(dxle::pointi32 bar_pos)const;
	
	//どれだけずらすべきか取得
	int32_t get_value()const { return now_pos/1000; }
	bool is_active()const { return (page_size < object_size); }
	void set_now_pos(int32_t now_pos_) { set_now_pos_raw(now_pos_ * 1000); reset_mouse_state(); }
private:
	int32_t object_size;//ずらしたいオブジェクトのyサイズ(pix)
	int32_t page_size;//スクロールバー部分を除いたオブジェクトの描画範囲(pix)(is_visible==falseのときはバーの大きさが0の事に注意)
	int32_t now_pos;//ずらすピクセル*1000 [0,object_size-page_size*1000) (pix*1000)
	int32_t grip_start_mousepos;//グリップが始まったときのマウスの位置
	int32_t grip_start_nowpos;//グリップが始まったときのnow_pos(pix*1000)
	dxle::sizei32 bar_size;//縦でも横でも長いほうがheight
	bool is_horizontal;
	bool last_mouse_input;
	bool mouse_input_start_is_out;
	bool is_holded;//マウスに掴まれているか
	enum class mouse_pos{up_arrow, up_space, grip, down_space, down_arrow, out}on_mouse_pos;

	void set_now_pos_raw(int32_t now_pos_) {
		now_pos = std::max(0, std::min(now_pos_, (object_size - page_size - 1) * 1000));
	}
	void reset_mouse_state();

	int32_t to_pix_scale(int32_t bar_v)const {
		return bar_v * object_size / (bar_size.height - bar_size.width*2);
	};
	int32_t to_bar_scale(int32_t pix_v)const {
		return pix_v * (bar_size.height - bar_size.width * 2) / object_size;
	};
};
