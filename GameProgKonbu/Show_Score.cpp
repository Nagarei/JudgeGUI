#include "Show_Score.h"
#include "other_usefuls.h"
#include "Mouse.h"
#include "KeyInputData.h"
#include "Contest.h"
#include "Score_detail.h"

namespace
{
	namespace submit {
		constexpr int32_t height = 30;
		//全体に対する比率
		constexpr int32_t min_leftspace_width = 10;
		constexpr int32_t min_sbumittime_width = 120;
		constexpr int32_t min_problemname_width = 170;
		constexpr int32_t min_user_width = 120;
		constexpr int32_t min_score_width = 50;
		constexpr int32_t min_type_width = 35;
		constexpr int32_t min_total_width = min_leftspace_width + min_sbumittime_width + min_problemname_width
			+ min_user_width + min_score_width + min_type_width;
		constexpr int32_t rightspace_width = 20;//固定値

		constexpr dxle::rgb out_back_color{ 249,249,249 };
		constexpr dxle::rgb on_back_color{ 249,249,0 };
		constexpr dxle::rgb edge_color{ 221,221,221 };
	}

}

Show_Score::Show_Score(int selecting_)
	: Sequence_Commom(selecting_)
	, menu_font(DxLib::CreateFontToHandle(_T("ＭＳ ゴシック"), 16, 2))
	, submissions_font(DxLib::CreateFontToHandle(_T("ＭＳ ゴシック"), 20, 5))
	, show_myscore_only(Data::GetIns().get_is_contest_mode())
	, last_submissions_size(0)
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

	get_submissions_copy();
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

	dxle::sizei32 window_size = My_GetWindowSize();
	if (window_size != last_window_size) {
		reset_window_size();
	}

	auto old_selecting = selecting;
	//問題選択
	update_SelectProblem();
	//結果処理//スクロール
	set_next(update_Submit());
	//メニュー処理
	set_next(update_Menu());

	//リロード処理
	if (KeyInputData::GetIns().GetNewKeyInput(KEY_INPUT_F5)) {
		Data::GetIns().LoadSubmissionAll();
		get_submissions_copy();
		reset_Scroll();
	}
	//問題の変更確認
	if (selecting != old_selecting) {
		get_submissions_copy();
		reset_Scroll();
	}

	if (last_submissions_size != Data::GetIns()[selecting].GetSubmissionSet().size()) {
		get_submissions_copy();
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
	if (submissions_button.empty()) { return nullptr; }
	if (scrollbar.update())
	{
		//ボタンの位置変更
		reset_button_area();
	}
	else
	{
		//ボタン入力
		auto& mouse = Mouse::GetIns();
		for (size_t i = 0; i < submissions_button.size(); ++i)
		{
			if (submissions_button[i].update(mouse.get_now_pos(), mouse.get_now_input() & MOUSE_INPUT_LEFT)) {
				return std::make_unique<Score_detail>(selecting, submissions_index[i]);
			}
		}
	}
	return nullptr;
}
void Show_Score::reset_button_area()
{
	//ボタンの位置変更
	dxle::pointi32 pos1{ menu_space_size, title_space };
	dxle::sizei32 page_size = My_GetWindowSize();
	page_size -= pos1;
	pos1 -= scrollbar.get_value();//ボタン表示エリアの左上座標
	int32_t submit_area_width = std::max(page_size.width - submit::rightspace_width, submit::min_total_width);
	int32_t left_space = submit_area_width * submit::min_leftspace_width / submit::min_total_width;
	int32_t button_width = submit_area_width - left_space;

	for (size_t i = 0; i < submissions_button.size(); ++i)
	{
		submissions_button[i].set_on_color(submit::on_back_color, submit::edge_color, submit::on_back_color);
		submissions_button[i].set_out_color(submit::out_back_color, submit::edge_color, submit::out_back_color);
		submissions_button[i].set_area(pos1 + dxle::sizei32{ left_space, submit::height*(i + 1) }, { button_width, submit::height });
	}
}
void Show_Score::reset_Scroll()
{
	submissions_button.resize(submissions_index.size());
	if (submissions_button.empty()) { return; }

	dxle::pointi32 pos1{ menu_space_size, title_space };
	dxle::sizei32 page_size = My_GetWindowSize();
	page_size -= pos1;
	//スクロールバー調整
	assert(0 < page_size.height && 0 < page_size.width);
	scrollbar.reset(static_cast<dxle::sizeui32>(page_size), { std::max(page_size.width - submit::rightspace_width, submit::min_total_width) , submit::height + submit::height*submissions_button.size() });

	//ボタン調整
	reset_button_area();
}
void Show_Score::draw_Submit() const
{
	DxLib::DrawFillBox(menu_space_size, title_space, last_window_size.width, last_window_size.height, dxle::dx_color(dxle::color_tag::white).get());//@todo dxlibex
	if (submissions_index.empty())
	{
		int y = (last_window_size.height - title_space - 30) / 2;
		DrawStringCenter({ menu_space_size,y }, _T("提出がありません"), dxle::color_tag::black, submissions_font, last_window_size.width - menu_space_size);
		return;
	}
	{
		const int32_t draw_area_width = std::max(last_window_size.width - menu_space_size - submit::rightspace_width, submit::min_total_width);
		const auto& prob = Data::GetIns()[selecting];
		for (size_t i = 0; i < submissions_index.size(); ++i)
		{
			submissions_button[i].draw();

			const auto& score = prob.GetSubmissionSet()[submissions_index[i]];
			dxle::pointi32 pos1{ menu_space_size, title_space + submit::height*(i+1) };
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
			DrawStringCenter2(pos1, _T("%d"), dxle::color_tag::black, submissions_font, draw_area, prob.GetScore_single(submissions_index[i]));
			//タイプ
			set_next_area(submit::min_type_width);
			{
				auto&& type_draw = get_result_type_fordraw(score);
				dxle::DrawBox(pos1 + dxle::sizei{3, 1}, pos1 + draw_area - dxle::sizei{ 0, 1 }, type_draw.second, true);
				dxle::DrawBox(pos1 + dxle::sizei{3, 1}, pos1 + draw_area - dxle::sizei{ 0, 1 }, dxle::color_tag::black, false);
				DrawStringCenter2(pos1, type_draw.first.data(), dxle::color_tag::black, submissions_font, draw_area);
			}
			//右スペース
			//set_next_area(submit::min_rightspace_width);//余計な線が入ってしまうので

			DxLib::SetDrawAreaFull();
		}

		//ガイド
		{
			dxle::pointi32 pos1{ menu_space_size, title_space };
			pos1.x -= scrollbar.get_value().width;

			//背景表示
			[](dxle::pointi pos1, dxle::sizei size) {
				dxle::pointi pos2 = pos1 + size;
				DxLib::DrawFillBox(pos1.x, pos1.y, pos2.x, pos2.y, dxle::dx_color(submit::out_back_color).get());
				DxLib::DrawLineBox(pos1.x, pos1.y, pos2.x, pos2.y, dxle::dx_color(submit::edge_color).get());
			}(dxle::pointi{ submissions_button[0].get_area().first.x, title_space }, submissions_button[0].get_area().second);

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
			DrawStringCenter2(pos1, _T("提出日時"), dxle::color_tag::black, submissions_font, draw_area);
			//問題名
			set_next_area(submit::min_problemname_width);
			DrawStringCenter2(pos1, _T("問題名"), dxle::color_tag::black, submissions_font, draw_area);
			//ユーザー名
			set_next_area(submit::min_user_width);
			DrawStringCenter2(pos1, _T("ユーザ名"), dxle::color_tag::black, submissions_font, draw_area);
			//スコア
			set_next_area(submit::min_score_width);
			DrawStringCenter2(pos1, _T("得点"), dxle::color_tag::black, submissions_font, draw_area);
			//タイプ
			set_next_area(submit::min_type_width);
			DrawStringCenter2(pos1, _T("状態"), dxle::color_tag::black, submissions_font, draw_area);
			//右スペース

			DxLib::SetDrawAreaFull();
		}
	}

	scrollbar.draw();
}

void Show_Score::get_submissions_copy()
{
	auto& submissions = Data::GetIns()[selecting].GetSubmissionSet();
	last_submissions_size = submissions.size();
	submissions_index.resize(last_submissions_size);
	std::iota(submissions_index.rbegin(), submissions_index.rend(), 0u);//新しいのを前にするため、逆順
	if (show_myscore_only) {
		submissions_index.erase(
			std::remove_if(submissions_index.begin(), submissions_index.end(),
				[&submissions](const size_t& i) { return submissions[i].get_user_name() != Data::GetIns().get_user_name(); })
			, submissions_index.end());
	}
}

void Show_Score::reset_window_size()
{
	 //スクロール
	reset_Scroll();

	//問題切り替え矢印
	reset_problemselect();
}
