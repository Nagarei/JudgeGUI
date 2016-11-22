#pragma once
#include "ScrollBar.h"

//縦横スクロールバー
class ScrollBar2 final
{
private:
	dxle::pointi32 pos1;
	dxle::sizeui32 bace_object_size;
	dxle::sizeui32 bar_area;
	ScrollBar scrollbar_v;
	ScrollBar scrollbar_h;
	double extend_rate;//拡大/縮小率
public:
	ScrollBar2();
	//@param pos1_ バーの描画位置
	//@param bar_area_ バーの描画サイズ(バー含む物を表示する大きさ)
	//@param object_size_ 動かすものの大きさ
	ScrollBar2(const dxle::pointi32& pos1_, const dxle::sizeui32& bar_area_
		, const dxle::sizeui32& object_size_) :ScrollBar2() {
		reset(bar_area_, object_size_);
		set_pos(pos1_);
	}
	//@param bar_area_ バーの描画サイズ(バー含む物を表示する大きさ)
	//@param object_size_ 動かすものの大きさ
	void reset(const dxle::sizeui32& bar_area_, const dxle::sizeui32& object_size_);

	void set_pos(const dxle::pointi32& pos1_)noexcept { pos1 = pos1_; }
	void set_value(const dxle::sizei32& scroll_value) {
		scrollbar_v.set_now_pos(scroll_value.height);
		scrollbar_h.set_now_pos(scroll_value.width);
	}
	bool update();//@return true:スクロールされた
	void draw()const;

	dxle::sizei32 get_value()const {
		return{ scrollbar_h.get_value() , scrollbar_v.get_value() };
	}
	double get_extend_rate()const {
		return extend_rate;
	}
};
