#include "Show_Score.h"
#include "other_usefuls.h"
#include "Mouse.h"
#include "KeyInputData.h"
#include "Contest.h"

namespace
{
	namespace submit {
		constexpr int32_t height = 30;
		//全体に対する比率
		constexpr int32_t min_leftspace_width = 30;
		constexpr int32_t min_sbumittime_width = 100;
		constexpr int32_t min_problemname_width = 200;
		constexpr int32_t min_user_width = 150;
		constexpr int32_t min_score_width = 50;
		constexpr int32_t min_type_width = 30;
		constexpr int32_t min_rightspace_width = 30;
		constexpr int32_t min_total_width = min_leftspace_width + min_sbumittime_width + min_problemname_width
			+ min_user_width + min_score_width + min_type_width + min_rightspace_width;

		constexpr dxle::rgb out_back_color{ 249,249,249 };
		constexpr dxle::rgb on_back_color{ 249,249,0 };
		constexpr dxle::rgb edge_color{ 221,221,221 };
	}

}

Show_Score::Show_Score(int selecting_)
	: Sequence_Commom(selecting_)
	, menu_font(DxLib::CreateFontToHandle(_T("ＭＳ ゴシック"), 16, 2))
	, submissions_font(DxLib::CreateFontToHandle(_T("ＭＳ ゴシック"), 20, 2))
{
	scrollbar.set_pos({ menu_space_size, title_space });

	to_problem.set_area({ 0, title_space }, { menu_space_size , menu_button_height });
	to_problem.set_str(_T("問題文"));

	{
		dxle::rgb out_back_color{ 154, 130, 0 };
		dxle::rgb on_back_color{ 0, 197, 30 };
		dxle::rgb out_edge_color = dxle::color_tag::white;
		dxle::rgb on_edge_color = dxle::color_tag::white;
		dxle::rgb on_string_color = dxle::color_tag::white;
		dxle::rgb out_string_color = dxle::color_tag::white;
		to_problem.set_on_color(on_back_color, on_edge_color, on_string_color);
		to_problem.set_out_color(out_back_color, out_edge_color, out_string_color);
	}

	reset_Scroll();
}

Show_Score::~Show_Score()
{
	DeleteFontToHandle(menu_font);
	DeleteFontToHandle(submissions_font);
}

std::unique_ptr<Sequence> Show_Score::update()
{
	if (!GetWindowActiveFlag()) {
		return nullptr;
	}

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

	auto old_selecting = selecting;
	//問題選択
	update_SelectProblem();
	//結果処理//スクロール
	set_next(update_Submit());
	//メニュー処理
	set_next(update_Menu());

	//問題の変更確認
	if (selecting != old_selecting) {
		reset_Scroll();
	}

	if (submissions.size() != Data::GetIns()[selecting].GetScoresSet().size()) {
		reset_Scroll();
	}

	last_window_size = window_size;

	return next_sequence;
}

void Show_Score::draw() const
{
	//結果表示
	draw_Submit();

	//タイトル表示//スコア表示
	draw_problem_state();

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
	DxLib::DrawFillBox(0, title_space, menu_space_size, last_window_size.height, dxle::dx_color(dxle::color_tag::black).get());//@todo dxlibex
	to_problem.draw(menu_font);
}

std::unique_ptr<Sequence> Show_Score::update_Submit()
{
	if (submissions.empty()) { return nullptr; }
	if (scrollbar.update())
	{
		//ボタンの位置変更
		dxle::sizei32 page_size;
		dxle::pointi32 pos1{ menu_space_size, title_space };
		DxLib::GetWindowSize(&page_size.width, &page_size.height);//@todo dxlibex
		page_size -= pos1;
		pos1 -= scrollbar.get_value();//ボタン表示エリアの左上座標
		int32_t submit_area_width = std::max(page_size.width, submit::min_total_width);
		int32_t left_space = submit_area_width * submit::min_leftspace_width / submit::min_total_width;
		int32_t button_width = submit_area_width - left_space - submit_area_width * submit::min_rightspace_width / submit::min_total_width;
		
		for (size_t i = 0; i < submissions.size(); ++i)
		{
			submissions[i].set_area(pos1 + dxle::sizei32{ left_space, submit::height*i } , { button_width, submit::height });
		}
	}
	else
	{
		//ボタン入力
		auto& mouse = Mouse::GetIns();
		for (size_t i = 0; i < submissions.size(); ++i)
		{
			if (submissions[i].update(mouse.get_now_pos(), mouse.get_now_input() & MOUSE_INPUT_LEFT)) {
				DEBUG_NOTE;
			}
		}
	}
	return nullptr;
}
void Show_Score::reset_Scroll()
{
	submissions.resize(Data::GetIns()[selecting].GetScoresSet().size());
	if (submissions.empty()) { return; }

	dxle::sizei32 page_size;
	constexpr dxle::pointi32 pos1{ menu_space_size, title_space };
	DxLib::GetWindowSize(&page_size.width, &page_size.height);//@todo dxlibex
	page_size -= pos1;
	int32_t submit_area_width = std::max(page_size.width, submit::min_total_width);
	int32_t left_space = submit_area_width * submit::min_leftspace_width / submit::min_total_width;
	int32_t button_width = submit_area_width - left_space - submit_area_width * submit::min_rightspace_width / submit::min_total_width;

	//ボタン調整
	for (size_t i = 0; i < submissions.size(); ++i)
	{
		using namespace submit;
		submissions[i].set_on_color(on_back_color, edge_color, on_back_color);
		submissions[i].set_out_color(out_back_color, edge_color, out_back_color);
		submissions[i].set_area(pos1 + dxle::sizei32{ left_space, submit::height*i }, { button_width, submit::height });
	}
	//スクロールバー調整
	assert(0 < page_size.height && 0 < page_size.width);
	scrollbar.reset(static_cast<dxle::sizeui32>(page_size), { std::max(page_size.width, submit::min_total_width) , submit::height*submissions.size() });
}
void Show_Score::draw_Submit() const
{
	DxLib::DrawFillBox(menu_space_size, title_space, last_window_size.width, last_window_size.height, dxle::dx_color(dxle::color_tag::white).get());//@todo dxlibex
	if (submissions.empty())
	{
		int y = (last_window_size.height - title_space - 30) / 2;
		DrawStringCenter({ menu_space_size,y }, _T("提出がありません"), dxle::color_tag::black, submissions_font, last_window_size.width - menu_space_size);
		return;
	}

	{
		const auto& prob = Data::GetIns()[selecting];
		const int32_t draw_area_width = std::max(last_window_size.width - menu_space_size, submit::min_total_width);
		for (size_t i = 0; i < submissions.size(); ++i)
		{
			submissions[i].draw();

			const auto& score = prob.GetScoresSet()[i];
			dxle::pointi32 pos1{ menu_space_size, title_space + submit::height*i };
			pos1 -= scrollbar.get_value();
			dxle::sizei32 draw_area{ 0, submit::height };
			auto set_next_area = [&pos1, &draw_area, &draw_area_width](int32_t min_width) {
				pos1.x += draw_area.width;
				draw_area.width = std::max(min_width, draw_area_width*min_width / submit::min_total_width);
				DxLib::SetDrawArea(pos1.x, pos1.y, pos1.x + draw_area.width, pos1.y + draw_area.height);
				DxLib::DrawLine(pos1.x, pos1.y, pos1.x, pos1.y + submit::height, dxle::dx_color(submit::edge_color).get());
			};
			//左スペース
			//set_next_area(submit::min_leftspace_width);//余計な線が入ってしまうので
			draw_area.width = std::max(submit::min_leftspace_width, draw_area_width*submit::min_leftspace_width / submit::min_total_width);
			//提出時間
			set_next_area(submit::min_sbumittime_width);
			{
				const auto& stime = score.get_submit_time();
				DrawStringRight(pos1,
					ToStringEx(stime.Year%1000,_T('/'), stime.Mon, _T('/'), stime.Day, _T('['), stime.Hour, _T(':'), stime.Min, _T(']')).c_str(),
					dxle::color_tag::black, submissions_font, draw_area);
			}
			//問題名
			set_next_area(submit::min_problemname_width);
			DrawStringCenter2(pos1, prob.GetName().c_str(), dxle::color_tag::black, submissions_font, draw_area);
			//ユーザー名
			set_next_area(submit::min_user_width);
			DrawStringCenter2(pos1, score.get_user_name().c_str(), dxle::color_tag::black, submissions_font, draw_area);
			//スコア
			set_next_area(submit::min_score_width);
			DrawStringCenter2(pos1, _T("%d"), dxle::color_tag::black, submissions_font, draw_area, prob.GetScore_single(i));
			//タイプ
			set_next_area(submit::min_type_width);
			DrawStringCenter2(pos1, get_result_type_str(score).data(), dxle::color_tag::black, submissions_font, draw_area);
			//右スペース
			//set_next_area(submit::min_rightspace_width);//余計な線が入ってしまうので

			DxLib::SetDrawAreaFull();
		}
	}

	scrollbar.draw();
}

void Show_Score::reset_window_size()
{
	 //スクロール
	reset_Scroll();

	//問題切り替え矢印
	reset_problemselect();
}
