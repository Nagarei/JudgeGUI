#include "common.h"
#include "KeyInputData.h"
#include "Mouse.h"

Sequence_Commom::Sequence_Commom(int selecting_)
	: title_font(DxLib::CreateFontToHandle(_T("MS UI Gothic"), 30, 2))
	, score_font(DxLib::CreateFontToHandle(_T("MS UI Gothic"), 16, 2))
	, selecting(selecting_)
{
	SetWindowTitle();

	DxLib::GetWindowSize(&last_window_size.width, &last_window_size.height);//@todo dxlibex

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
	int window_x, window_y;
	DxLib::GetWindowSize(&window_x, &window_y);//@todo dxlibex

	//タイトル表示
	DrawStringCenter({ 0,0 }, problems[selecting].GetName().c_str(), dxle::color_tag::white,
		title_font, window_x);
	//スコア表示
	DrawStringCenter({ 0,32 }, _T("%d/%d"), dxle::color_tag::white, score_font, window_x,
		problems[selecting].GetScore(), problems[selecting].GetMaxScore());
}

void Sequence_Commom::update_SelectProblem()
{
	auto& problems = Data::GetIns();
	auto& key = KeyInputData::GetIns();
	auto& mouse = Mouse::GetIns();
	int window_x, window_y;
	DxLib::GetWindowSize(&window_x, &window_y);//@todo dxlibex

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
#if 0
	//@todo dxlibex
	while (!mouse.click_log_is_empty())
	{
		auto click_state = mouse.click_log_front(); mouse.click_log_pop();
		if ((click_state.type & MOUSE_INPUT_LEFT) && (click_state.pos.y < title_space))
		{
			if (0 <= click_state.pos.x && click_state.pos.x < side_space_size) {
				//左
				if (selecting <= 0) {
					selecting = problems.size();
				}
				--selecting;
				SetWindowTitle();
			}
			else if (window_x - side_space_size <= click_state.pos.x && click_state.pos.x < window_x) {
				//右
				++selecting;
				if (problems.size() <= (unsigned)selecting) {
					selecting = 0;
				}
				SetWindowTitle();
			}
		}
	}
#endif

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
	int window_x, window_y;
	DxLib::GetWindowSize(&window_x, &window_y);//@todo dxlibex
	arrow[0].draw();
	DrawToLeftArrow2(button_edge_size, title_space / 2, arrow_width - button_edge_size * 2, dxle::color_tag::yellow);
	arrow[1].draw();
	DrawToRightArrow2(window_x - button_edge_size, title_space / 2, arrow_width - button_edge_size * 2, dxle::color_tag::yellow);
}

