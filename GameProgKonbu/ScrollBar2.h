#pragma once
#include "ScrollBar.h"

//縦横スクロールバー
class ScrollBar2 final
{
private:
	dxle::pointi32 pos1;
	dxle::sizei32 bar_area;
	ScrollBar scrollbar_v;
	ScrollBar scrollbar_h;
	double extend_rate;//拡大/縮小率
public:
	ScrollBar2();
	//@param pos1_ バーの描画位置
	//@param bar_area_ バーの描画サイズ(バー含む物を表示する大きさ)
	//@param object_size 動かすものの大きさ
	ScrollBar2(const dxle::pointi32& pos1_, int32_t bar_width, dxle::sizei32 object_size) :ScrollBar2() {
		reset_Scroll(bar_area_, bar_width, object_size);
		set_pos(pos1_);
	}
	//@param bar_area_ バーの描画サイズ(バー含む物を表示する大きさ)
	//@param bar_width バーの太さ(描画サイズ)
	//@param object_size 動かすものの大きさ
	void reset_Scroll(dxle::sizei32 bar_area_, dxle::sizei32 object_size);

	void set_pos(const dxle::pointi32& pos1_)noexcept { pos1 = pos1_; }
	void update_Scroll();
	void draw_Scroll()const;
};
