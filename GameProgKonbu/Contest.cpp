﻿#include "Contest.h"
#include "useful_func_and_class.h"
#include "other_usefuls.h"
#include "Mouse.h"
#include "test.h"
#include "fps.h"
namespace
{
	const int title_space = 50;
	const int arrow_width = 20;
	const int button_edge_size = 2;
	const int menu_space_size = 100;
	const int menu_button_height = 50;
	const int scrollbar_size = 17;
}
Contest::Contest()
	: title_font(DxLib::CreateFontToHandle(_T("MS UI Gothic"), 30, 2))
	, main_font (DxLib::CreateFontToHandle(_T("MS UI Gothic"), 16, 2))
	, menu_font (DxLib::CreateFontToHandle(_T("MS UI Gothic"), 16, 2))
	, selecting(0)
	, problem_load_finished(false)
	, extend_rate(1.0)
{
	DxLib::SetDragFileValidFlag(TRUE);
	DxLib::DragFileInfoClear();
	SetWindowTitle();

	int window_x, window_y;
	DxLib::GetWindowSize(&window_x, &window_y);//@todo dxlibex

	arrow[0].set_area({ 0,0 }, { arrow_width, title_space });
	arrow[1].set_area({ window_x - arrow_width,0 }, { arrow_width , title_space });

	to_result.set_area({ 0, title_space }, { menu_space_size , menu_button_height });
	to_result.set_str(_T("結果"));
	to_submit.set_area({ 0, title_space + menu_button_height }, { menu_space_size , menu_button_height });
	to_submit.set_str(_T("提出"));

	dxle::rgb out_back_color{ 154, 130, 0 };
	dxle::rgb on_back_color{ 0, 197, 30 };
	dxle::rgb out_edge_color = dxle::color_tag::white;
	dxle::rgb on_edge_color = dxle::color_tag::white;
	dxle::rgb on_string_color = dxle::color_tag::white;
	dxle::rgb out_string_color = dxle::color_tag::white;

	arrow[0].set_on_color(on_back_color, on_edge_color, on_string_color);
	arrow[1].set_on_color(on_back_color, on_edge_color, on_string_color);
	to_result.set_on_color(on_back_color, on_edge_color, on_string_color);
	to_submit.set_on_color(on_back_color, on_edge_color, on_string_color);

	arrow[0].set_out_color(out_back_color, out_edge_color, out_string_color);
	arrow[1].set_out_color(out_back_color, out_edge_color, out_string_color);
	to_result.set_out_color(out_back_color, out_edge_color, out_string_color);
	to_submit.set_out_color(out_back_color, out_edge_color, out_string_color);
}
Contest::~Contest()
{
	DxLib::SetDragFileValidFlag(FALSE);
	DeleteFontToHandle(title_font);
	DeleteFontToHandle(main_font);
	DeleteFontToHandle(menu_font);
}


std::unique_ptr<Sequence> Contest::update()
{
	dxle::sizei32 window_size;
	{
		DxLib::GetWindowSize(&window_size.width, &window_size.height);//@todo dxlibex
		if (window_size != last_window_size) {
			reset_window_size();
		}
	}

	if (1 <= DxLib::GetDragFileNum())
	{
		auto str_buf = std::make_unique<TCHAR[]>(GetDragFilePath(nullptr)+1);
		GetDragFilePath(str_buf.get());
		DxLib::DragFileInfoClear();
		DxLib::SetDragFileValidFlag(TRUE);
		compile_taskmanager::set_test(selecting, str_buf.get());
	}

	if (GetWindowActiveFlag())
	{
		//問題選択
		update_SelectProblem();
		//問題スクロール
		update_Scroll();
		//メニュー処理
		update_Menu();
	}

	if (!problem_load_finished) {
		problem_load_finished = Data::GetIns().IsLoadFinesed(selecting);
		if (problem_load_finished) {
			reset_Scroll();
		}
	}

	DxLib::GetWindowSize(&last_window_size.width, &last_window_size.height);//@todo dxlibex

	return nullptr;
}
void Contest::draw()const
{
	const auto& problems = Data::GetIns();
	int window_x, window_y;
	DxLib::GetWindowSize(&window_x, &window_y);//@todo dxlibex

	//タイトル表示
	DrawStringCenter({ 0,0 }, problems[selecting].GetName().c_str(), dxle::color_tag::white, title_font, window_x);
	//スコア表示
	DrawStringCenter({ 0,32 }, _T("%d/%d"), dxle::color_tag::white, main_font, window_x, problems[selecting].GetScore(), problems[selecting].GetMaxScore());

	if (problem_load_finished)
	{
		//問題表示
		DxLib::SetDrawArea(menu_space_size, title_space, window_x, window_y);
		DxLib::DrawFillBox(menu_space_size, title_space, window_x, window_y, dxle::dx_color(dxle::color_tag::white).get());//@todo dxlibex
		Data::GetIns().DrawExtendProblem(selecting, problem_pos, extend_rate);
		DxLib::SetDrawAreaFull();

		//スクロール系表示
		draw_Scroll();
	}
	else
	{
		int y = (window_y - 30) / 2;
		DrawStringCenter({ 0,y }, _T("Now Loading"), dxle::color_tag::white, title_font, window_x);
	}

	//問題選択矢印表示
	draw_SelectProblem();

	//メニュー処理
	draw_Menu();
}

void Contest::SetWindowTitle()
{
	auto& problems = Data::GetIns();
	DxLib::SetMainWindowText(ToStringEx(
		problems[selecting].GetName(),
		_T(" ("),
		problems[selecting].GetScore(),
		_T('/'),
		problems[selecting].GetMaxScore(),
		_T(')')
	).c_str());
}

void Contest::update_SelectProblem()
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
		problem_load_finished = false;
	}
	if (arrow[1].update(mouse.get_now_pos(), mouse.get_now_input() & MOUSE_INPUT_LEFT)) {
		//右
		++selecting;
		if (problems.size() <= (unsigned)selecting) {
			selecting = 0;
		}
		SetWindowTitle();
		problem_load_finished = false;
	}
#if 0
	//@todo dxlibex
	while(!mouse.click_log_is_empty())
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
				problem_load_finished = false;
			}
			else if (window_x - side_space_size <= click_state.pos.x && click_state.pos.x < window_x) {
				//右
				++selecting;
				if (problems.size() <= (unsigned)selecting) {
					selecting = 0;
				}
				SetWindowTitle();
				problem_load_finished = false;
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
			problem_load_finished = false;
		}
		else if (key.GetDirectionKeyInput(key.KEY_RIGHT)) {
			//右
			++selecting;
			if (problems.size() <= (unsigned)selecting) {
				selecting = 0;
			}
			SetWindowTitle();
			problem_load_finished = false;
		}
	}

	problems.SetBuildProblemText(selecting);
}

void Contest::draw_SelectProblem() const
{
	int window_x, window_y;
	DxLib::GetWindowSize(&window_x, &window_y);//@todo dxlibex
	arrow[0].draw();
	DrawToLeftArrow2(button_edge_size, title_space / 2, arrow_width - button_edge_size*2, dxle::color_tag::yellow);
	arrow[1].draw();
	DrawToRightArrow2(window_x - button_edge_size, title_space / 2, arrow_width - button_edge_size*2, dxle::color_tag::yellow);
}

void Contest::update_Menu()
{
	auto& key = KeyInputData::GetIns();
	auto& mouse = Mouse::GetIns();

	if (to_result.update(mouse.get_now_pos(), mouse.get_now_input() & MOUSE_INPUT_LEFT)) {
		DEBUG_NOTE;
	}
	if (to_submit.update(mouse.get_now_pos(), mouse.get_now_input() & MOUSE_INPUT_LEFT)) {

		//ファイルを開く為の設定用構造体
		OPENFILENAME ofn;
		memset(&ofn, NULL, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		//ここに指定ファイルの絶対パスが代入される
		TCHAR szFile[MAX_PATH*2] = _T("");

		//親ウィンドウのハンドル
		ofn.hwndOwner = DxLib::GetMainWindowHandle();
		//表示させるファイルのフィルタリング
		ofn.lpstrFilter = _T(
			"Cppファイル(*.cpp *.cxx)\0*.cpp;*.cxx\0"
			"全てのファイル(*.*)\0*.*\0"
			);
		//パスを代入する配列
		ofn.lpstrFile = szFile;
		//パスの最大文字数
		ofn.nMaxFile = MAX_PATH * 2;
		//ファイル名で拡張子が指定されなかった場合に追加する文字列
		ofn.lpstrDefExt = _T(".cpp");
		//ウィンドウの名前
		ofn.lpstrTitle = _T("提出するC++コードの選択");

		//カレントディレクトリを保存（GetOpenFileNameはカレントディレクトリをいじる）
		TCHAR old_current_directory[MAX_PATH * 3];
		GetCurrentDirectory(sizeof(old_current_directory) / sizeof(old_current_directory[0]), old_current_directory);
		//MessageBoxなみに処理を止めるので注意
		auto open_state = GetOpenFileName(&ofn);
		//カレントディレクトリを戻す
		SetCurrentDirectory(old_current_directory);
		if (open_state == 0) {
			//ファイルオープンに失敗
		}
		else {
			//成功
			compile_taskmanager::set_test(selecting, szFile);
		}

	}
}

void Contest::draw_Menu() const
{
	to_result.draw(menu_font);
	to_submit.draw(menu_font);
}

void Contest::update_Scroll()
{
	if (!problem_load_finished) {
		problem_pos.x = menu_space_size;
		problem_pos.y = title_space;
		return;
	}

	dxle::sizei32 window_size;
	DxLib::GetWindowSize(&window_size.width, &window_size.height);//@todo dxlibex

	auto& key = KeyInputData::GetIns();
	auto& mouse = Mouse::GetIns();

	//拡大/縮小入力
	if (key.GetKeyInput(KEY_INPUT_LCONTROL) || key.GetKeyInput(KEY_INPUT_RCONTROL))
	{
		auto old_extend_rate = extend_rate;
		extend_rate += mouse.get_now_wheel() * 10.0 / 100.0;
		if (key.GetKeyInput(KEY_INPUT_0) || key.GetKeyInput(KEY_INPUT_NUMPAD0)) {
			extend_rate = 1.0;
		}
		if (old_extend_rate != extend_rate) {
			reset_Scroll();
			return;
		}
	}
	//スクロール
	uint32_t keyinput = 0;
	int32_t v_wheel = mouse.get_now_wheel() * -25;
	int32_t h_wheel = mouse.get_now_H_wheel() * -25;
	if (key.GetKeyInput(KEY_INPUT_LSHIFT) || key.GetKeyInput(KEY_INPUT_RSHIFT)) {
		std::swap(v_wheel, h_wheel);
	}
	DEBUG_NOTE;//key_input
	scrollbar_v.update(GetFrameTime(),
		mouse.get_now_pos() - dxle::pointi32{ window_size.width - scrollbar_size, title_space },
		v_wheel, mouse.get_now_input() & MOUSE_INPUT_LEFT, keyinput);
	keyinput = 0;
	DEBUG_NOTE;//key_input
	scrollbar_h.update(GetFrameTime(),
		mouse.get_now_pos() - dxle::pointi32{ menu_space_size, window_size.height - scrollbar_size },
		h_wheel, mouse.get_now_input() & MOUSE_INPUT_LEFT, keyinput);

	//値セット
	problem_pos.x = menu_space_size - scrollbar_h.get_value();
	problem_pos.y = title_space - scrollbar_v.get_value();

}

void Contest::reset_Scroll()
{
	if (!problem_load_finished) { return; }

	auto prob_size = Data::GetIns().GetProblemSize(selecting) * extend_rate;
	dxle::sizei32 page_size;
	DxLib::GetWindowSize(&page_size.width, &page_size.height);//@todo dxlibex

	dxle::sizei32 window_size = page_size;

	page_size.height -= title_space;
	page_size.width -= menu_space_size;

	//バーの有効無効でpageサイズが変わってくるのに注意
	scrollbar_v.set_bar_state(prob_size.height, page_size.height, false);
	bool bar_v_p_all_active = scrollbar_v.is_active();
	if (bar_v_p_all_active) {
		page_size.width -= scrollbar_size;
	}
	scrollbar_h.set_bar_state(prob_size.width, page_size.width, true);
	if (scrollbar_h.is_active()) {
		page_size.height -= scrollbar_size;
		scrollbar_v.set_bar_state(prob_size.height, page_size.height, false);
		if (!bar_v_p_all_active && scrollbar_v.is_active()) {
			//縦のバーが有効になった=>page_sizeの再計算発生
			page_size.width -= scrollbar_size;
			scrollbar_h.set_bar_state(prob_size.width, page_size.width, true);
		}
	}

	//バーの描画サイズセット
	scrollbar_v.set_bar_size({ scrollbar_size, window_size.height - title_space - (scrollbar_h.is_active() ? scrollbar_size : 0) });
	scrollbar_h.set_bar_size({ window_size.width - menu_space_size - (scrollbar_v.is_active() ? scrollbar_size : 0) , scrollbar_size});

}

void Contest::draw_Scroll() const
{
	scrollbar_v.draw(dxle::pointi32{ last_window_size.width - scrollbar_size, title_space });
	scrollbar_h.draw(dxle::pointi32{ menu_space_size, last_window_size.height - scrollbar_size });
	if (scrollbar_v.is_active() && scrollbar_h.is_active()) {
		DxLib::DrawFillBox(last_window_size.width - scrollbar_size, last_window_size.height - scrollbar_size
			, last_window_size.width, last_window_size.height, DxLib::GetColor(230, 231, 232));
	}
}

void Contest::reset_window_size()
{
	dxle::sizei32 window_size;
	DxLib::GetWindowSize(&window_size.width, &window_size.height);//@todo dxlibex

	//スクロール
	reset_Scroll();

	//問題切り替え矢印
	//arrow[0].set_area({ 0,0 }, { arrow_width, title_space });
	arrow[1].set_area({ window_size.width - arrow_width,0 }, { arrow_width , title_space });
}
