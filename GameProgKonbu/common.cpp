#include "common.h"
#include "KeyInputData.h"
#include "Mouse.h"
#include "popup.h"

constexpr dxle::sizei32 popup_size = { 200, 75 };

Sequence_Commom::Sequence_Commom(int selecting_)
	: title_font(DxLib::CreateFontToHandle(_T("MS UI Gothic"), 30, 2))
	, score_font(DxLib::CreateFontToHandle(_T("MS UI Gothic"), 16, 2))
	, selecting(selecting_)
{
	SetWindowTitle();

	DxLib::GetWindowSize(&last_window_size.width, &last_window_size.height);//@todo dxlibex
	if (last_window_size.width < 320 || last_window_size.height < 240) {
		last_window_size.width = 320;
		last_window_size.height = 240;
	}

	popup::GetIns().set_area({ last_window_size.width - popup_size.width, title_space }, popup_size);

	arrow[0].set_area({ 0,0 }, { arrow_width, title_space });
	arrow[1].set_area({ last_window_size.width - arrow_width,0 }, { arrow_width , title_space });

	dxle::rgb out_back_color{ 154, 130, 0 };
	dxle::rgb on_back_color{ 0, 197, 30 };
	dxle::rgb out_edge_color = dxle::color_tag::white;
	dxle::rgb on_edge_color = dxle::color_tag::white;
	dxle::rgb on_string_color = dxle::color_tag::white;
	dxle::rgb out_string_color = dxle::color_tag::white;

	arrow[0].set_on_color(on_back_color, on_edge_color, on_string_color);
	arrow[1].set_on_color(on_back_color, on_edge_color, on_string_color);

	arrow[0].set_out_color(out_back_color, out_edge_color, out_string_color);
	arrow[1].set_out_color(out_back_color, out_edge_color, out_string_color);

}

void Sequence_Commom::draw_problem_state() const
{
	const auto& problems = Data::GetIns();

	DxLib::DrawFillBox(0, 0, last_window_size.width, title_space, dxle::dx_color(dxle::color_tag::black).get());//@todo dxlibex
	//タイトル表示
	DrawStringCenter({ 0,0 }, problems[selecting].GetName().c_str(), dxle::color_tag::white,
		title_font, last_window_size.width);
	//スコア表示
	DrawStringCenter({ 0,32 }, _T("%d/%d"), dxle::color_tag::white, score_font, last_window_size.width,
		problems[selecting].GetScore(), problems[selecting].GetMaxScore());
}

void Sequence_Commom::update_SelectProblem()
{
	auto& problems = Data::GetIns();
	auto& key = KeyInputData::GetIns();
	auto& mouse = Mouse::GetIns();

	//マウス入力チェック

	if (arrow[0].update(mouse.get_now_pos(), mouse.get_now_input() & MOUSE_INPUT_LEFT)) {
		//左
		if (selecting <= 0) {
			selecting = problems.size();
		}
		--selecting;
		SetWindowTitle();
	}
	if (arrow[1].update(mouse.get_now_pos(), mouse.get_now_input() & MOUSE_INPUT_LEFT)) {
		//右
		++selecting;
		if (problems.size() <= (unsigned)selecting) {
			selecting = 0;
		}
		SetWindowTitle();
	}

	//キーボード入力チェック
	if (key.GetKeyInput(KEY_INPUT_LCONTROL) || key.GetKeyInput(KEY_INPUT_RCONTROL))
	{
		if (key.GetDirectionKeyInput(key.KEY_LEFT)) {
			//左
			if (selecting <= 0) {
				selecting = problems.size();
			}
			--selecting;
			SetWindowTitle();
		}
		else if (key.GetDirectionKeyInput(key.KEY_RIGHT)) {
			//右
			++selecting;
			if (problems.size() <= (unsigned)selecting) {
				selecting = 0;
			}
			SetWindowTitle();
		}
	}
}

void Sequence_Commom::draw_SelectProblem() const
{
	arrow[0].draw();
	DrawToLeftArrow2(button_edge_size, title_space / 2, arrow_width - button_edge_size * 2, dxle::color_tag::yellow);
	arrow[1].draw();
	DrawToRightArrow2(last_window_size.width - button_edge_size, title_space / 2, arrow_width - button_edge_size * 2, dxle::color_tag::yellow);
}

void Sequence_Commom::reset_problemselect() {
	dxle::sizei32 window_size;
	DxLib::GetWindowSize(&window_size.width, &window_size.height);//@todo dxlibex
	//問題切り替え矢印
	//arrow[0].set_area({ 0,0 }, { arrow_width, title_space });
	arrow[1].set_area({ window_size.width - arrow_width,0 }, { arrow_width , title_space });

	popup::GetIns().set_area({ window_size.width - popup_size.width, title_space }, popup_size);
}
