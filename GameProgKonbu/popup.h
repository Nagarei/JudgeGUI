#pragma once
#include "fps.h"
class popup
{
private:
	dxle::pointi32 pos;
	dxle::sizei32 size;
	int font;
	std::queue<std::tuple<dxle::tstring, dxle::rgb, dxle::rgb, int>> que;
	int start_time = -1;
	popup();
	~popup()noexcept{
		//DxLib::DeleteFontToHandle(font);//先にDxLib_Endが呼ばれるはず
	}
	void set_(dxle::tstring&& str, dxle::rgb&& back_color, dxle::rgb&& str_color, int show_time) {
		que.emplace(std::move(str), std::move(back_color), std::move(str_color), show_time);
	}
	void update_();
	void draw_message()const;
public:
	void set_area(const dxle::pointi32& pos_, const dxle::sizei32& size_)noexcept {//non static
		pos = pos_; size = size_;
	}
public:
	static popup& GetIns() {
		static popup ins; return ins;
	}
	static void set(dxle::tstring str, dxle::rgb back_color = dxle::rgb{ 249,249,249 }, dxle::rgb str_color = dxle::color_tag::black, int show_time = 1000) {
		GetIns().set_(std::move(str), std::move(back_color), std::move(str_color), show_time);
	}
	static void update(){
		GetIns().update_();
	}
};
