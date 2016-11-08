#include "popup.h"
#include "useful_func_and_class.h"

namespace {
	constexpr int show_time = 1000;
}

popup::popup()
	: font(DxLib::CreateFontToHandle(_T("MS UI Gothic"), 16, 2))
{
}

void popup::update_()
{
	if (start_time == -1) {
		//�\�����Ă��Ȃ�
		//�V�K�\�����m�F
		if (!que.empty()) {
			start_time = myGetNowCount();
			draw_message();
		}
	}
	else {
		//�\����
		if (start_time + show_time < myGetNowCount()) {
			start_time = -1;
			que.pop();
		}
		else {
			draw_message();
		}
	}
}

void popup::draw_message() const
{
	if (que.empty()) { return; }

	RECT old_draw_area;
	DxLib::GetDrawArea(&old_draw_area);
	DxLib::SetDrawArea(pos.x, pos.y, pos.x + size.width, pos.y + size.height);
	FINALLY([&old_draw_area](){
		DxLib::SetDrawArea(old_draw_area.left, old_draw_area.top, old_draw_area.right, old_draw_area.bottom);
	});
	DxLib::DrawFillBox(pos.x, pos.y, pos.x + size.width, pos.y + size.height,
		dxle::dx_color(std::get<1>(que.front())).get());
	DxLib::DrawLineBox(pos.x, pos.y, pos.x + size.width, pos.y + size.height,
		dxle::dx_color(dxle::rgb{ 0,0,0 }).get());
	DrawStringCenter2(pos, std::get<0>(que.front()).c_str(), dxle::color_tag::black, font, size);
}
