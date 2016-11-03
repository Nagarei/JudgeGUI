#pragma once


class ScrollBar2 final
{
private:
	ScrollBar scrollbar_v;
	ScrollBar scrollbar_h;
	double extend_rate;//拡大/縮小率
public:
	ScrollBar2();
	//@param bar_area バーの描画サイズ
	//@param bar_width バーの太さ(描画サイズ)
	//@param object_size 動かすものの大きさ
	//@param page_size 物を表示する大きさ(バー含む)
	ScrollBar2(dxle::sizei32 bar_area, int32_t bar_width, dxle::sizei32 object_size, dxle::sizei32 page_size) :ScrollBar2() {
		reset_Scroll(bar_area, bar_width, object_size, page_size);
	}
	//@param bar_area バーの描画サイズ
	//@param bar_width バーの太さ(描画サイズ)
	//@param object_size 動かすものの大きさ
	//@param page_size 物を表示する大きさ(バー含む)
	void reset_Scroll(dxle::sizei32 bar_area, int32_t bar_width, dxle::sizei32 object_size, dxle::sizei32 page_size);
	void update_Scroll();
	void draw_Scroll()const;
};
