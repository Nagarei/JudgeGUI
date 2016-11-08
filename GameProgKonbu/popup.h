#pragma once
#include "fps.h"
class popup
{
private:
	dxle::pointi32 pos;
	dxle::sizei32 size;
	int font;
	std::queue<std::tuple<dxle::tstring, dxle::rgb>> que;
	int start_time = -1;
	popup();
	~popup()noexcept{
		//DxLib::DeleteFontToHandle(font);//先にDxLib_Endが呼ばれるはず
	}
	void set_(dxle::tstring&& str, dxle::rgb&& back_color) {
		que.emplace(std::move(str), std::move(back_color));
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
	static void set(dxle::tstring str, dxle::rgb back_color = dxle::rgb{ 249,249,249 }) {
		GetIns().set_(std::move(str), std::move(back_color));
	}
	static void update(){
		GetIns().update_();
	}
};
