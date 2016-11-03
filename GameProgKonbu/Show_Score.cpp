#include "Show_Score.h"
#include "other_usefuls.h"
#include "Mouse.h"
#include "KeyInputData.h"
#include "Contest.h"

namespace
{
	constexpr int32_t resuslt_size = 30;
}

Show_Score::Show_Score(int selecting_)
	: Sequence_Commom(selecting_)
	, menu_font(DxLib::CreateFontToHandle(_T("MS UI Gothic"), 16, 2))
	, result_font(DxLib::CreateFontToHandle(_T("MS UI Gothic"), 16, 2))
{
	to_problem.set_area({ 0, title_space }, { menu_space_size , menu_button_height });
	to_problem.set_str(_T("問題文"));

	dxle::rgb out_back_color{ 154, 130, 0 };
	dxle::rgb on_back_color{ 0, 197, 30 };
	dxle::rgb out_edge_color = dxle::color_tag::white;
	dxle::rgb on_edge_color = dxle::color_tag::white;
	dxle::rgb on_string_color = dxle::color_tag::white;
	dxle::rgb out_string_color = dxle::color_tag::white;

	to_problem.set_on_color(on_back_color, on_edge_color, on_string_color);
	to_problem.set_out_color(out_back_color, out_edge_color, out_string_color);

}

Show_Score::~Show_Score()
{
	DeleteFontToHandle(menu_font);
	DeleteFontToHandle(result_font);
}

std::unique_ptr<Sequence> Show_Score::update()
{
	std::unique_ptr<Sequence> next_sequence = nullptr;
	auto set_next = [&next_sequence](std::unique_ptr<Sequence>&& param) {
		if (param) { next_sequence = std::move(param); }
	};

	dxle::sizei32 window_size;
	{
		DxLib::GetWindowSize(&window_size.width, &window_size.height);//@todo dxlibex
		if (window_size != last_window_size) {
			reset_window_size();
		}
	}

	if (results.size() != Data::GetIns()[selecting].GetScoresSet().size()) {
		reset_Scroll();
	}

	if (GetWindowActiveFlag())
	{
		auto old_selecting = selecting;
		//問題選択
		update_SelectProblem();
		//スクロール
		update_Scroll();
		//メニュー処理
		set_next(update_Menu());
		//結果処理
		DEBUG_NOTE;

		//問題の変更確認
		if (selecting != old_selecting) {
			reset_Scroll();
		}
	}

	last_window_size = window_size;

	return next_sequence;
}

void Show_Score::draw() const
{
	//タイトル表示//スコア表示
	draw_problem_state();

	//結果表示
	DEBUG_NOTE;
	DxLib::DrawFillBox(menu_space_size, title_space, last_window_size.width, last_window_size.height, dxle::dx_color(dxle::color_tag::white).get());//@todo dxlibex


	//問題選択矢印表示
	draw_SelectProblem();

	//メニュー処理
	draw_Menu();
}

std::unique_ptr<Sequence> Show_Score::update_Menu()
{
	std::unique_ptr<Sequence> next_sequence = nullptr;

	auto& key = KeyInputData::GetIns();
	auto& mouse = Mouse::GetIns();

	if (to_problem.update(mouse.get_now_pos(), mouse.get_now_input() & MOUSE_INPUT_LEFT)) {
		next_sequence = std::make_unique<Contest>(selecting);
	}

	return next_sequence;
}

void Show_Score::draw_Menu() const
{
	to_problem.draw(menu_font);
}

void Show_Score::update_Scroll()
{
	DEBUG_NOTE;
}
void Show_Score::reset_Scroll()
{
	DEBUG_NOTE;
}
void Show_Score::draw_Scroll() const
{
	DEBUG_NOTE;
}

void Show_Score::reset_window_size()
{
	 //スクロール
	reset_Scroll();

	//問題切り替え矢印
	reset_problemselect();
}
