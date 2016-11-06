#include "Contest.h"
#include "useful_func_and_class.h"
#include "other_usefuls.h"
#include "Mouse.h"
#include "test.h"
#include "fps.h"
#include "Show_Score.h"
namespace
{
}
Contest::Contest(int selecting_)
	: Sequence_Commom(selecting_)
	, loading_font(DxLib::CreateFontToHandle(_T("MS UI Gothic"), 32, 2))
	, menu_font (DxLib::CreateFontToHandle(_T("MS UI Gothic"), 16, 2))
	, problem_load_finished(false)
{
	scrollbar.set_pos({ menu_space_size, title_space });

	DxLib::SetDragFileValidFlag(TRUE);
	DxLib::DragFileInfoClear();

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

	to_result.set_on_color(on_back_color, on_edge_color, on_string_color);
	to_submit.set_on_color(on_back_color, on_edge_color, on_string_color);

	to_result.set_out_color(out_back_color, out_edge_color, out_string_color);
	to_submit.set_out_color(out_back_color, out_edge_color, out_string_color);
}
Contest::~Contest()
{
	DxLib::SetDragFileValidFlag(FALSE);
	DeleteFontToHandle(loading_font);
	DeleteFontToHandle(menu_font);
}

std::unique_ptr<Sequence> Contest::update()
{
	if (1 <= DxLib::GetDragFileNum())
	{
		auto str_buf = std::make_unique<TCHAR[]>(GetDragFilePath(nullptr)+1);
		GetDragFilePath(str_buf.get());
		DxLib::DragFileInfoClear();
		DxLib::SetDragFileValidFlag(TRUE);
		compile_taskmanager::set_test(selecting, str_buf.get());
	}
	if (!GetWindowActiveFlag()) {
		return nullptr;
	}

	std::unique_ptr<Sequence> next_sequence = nullptr;

	dxle::sizei32 window_size;
	{
		DxLib::GetWindowSize(&window_size.width, &window_size.height);//@todo dxlibex
		if (window_size != last_window_size) {
			reset_window_size();
		}
	}

	{
		auto old_selecting = selecting;
		//問題選択
		update_SelectProblem();
		//問題スクロール
		update_Scroll();
		//メニュー処理
		next_sequence = update_Menu();

		//問題の変更確認
		if (selecting != old_selecting) {
			problem_load_finished = false;
		}
		Data::GetIns().SetBuildProblemText(selecting);
	}

	if (!problem_load_finished) {
		problem_load_finished = Data::GetIns().IsLoadFinesed(selecting);
		if (problem_load_finished) {
			reset_Scroll();
		}
	}

	DxLib::GetWindowSize(&last_window_size.width, &last_window_size.height);//@todo dxlibex

	return next_sequence;
}
void Contest::draw()const
{
	//タイトル、スコア標示
	draw_problem_state();

	if (problem_load_finished)
	{
		//問題表示
		DxLib::SetDrawArea(menu_space_size, title_space, last_window_size.width, last_window_size.height);
		DxLib::DrawFillBox(menu_space_size, title_space, last_window_size.width, last_window_size.height,
			dxle::dx_color(dxle::color_tag::white).get());//@todo dxlibex
		Data::GetIns().DrawExtendProblem(selecting, problem_pos, scrollbar.get_extend_rate());
		DxLib::SetDrawAreaFull();

		//スクロール系表示
		scrollbar.draw();
	}
	else
	{
		int y = (last_window_size.height - 30) / 2;
		DrawStringCenter({ 0,y }, _T("Now Loading"), dxle::color_tag::white, loading_font, last_window_size.width);
	}

	//問題選択矢印表示
	draw_SelectProblem();

	//メニュー処理
	draw_Menu();
}

std::unique_ptr<Sequence> Contest::update_Menu()
{
	std::unique_ptr<Sequence> next_sequence = nullptr;

	auto& key = KeyInputData::GetIns();
	auto& mouse = Mouse::GetIns();

	if (to_result.update(mouse.get_now_pos(), mouse.get_now_input() & MOUSE_INPUT_LEFT)) {
		next_sequence = std::make_unique<Show_Score>(selecting);
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

	return next_sequence;
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

	scrollbar.update();

	//値セット
	problem_pos = dxle::pointi32{ menu_space_size, title_space }
					- scrollbar.get_value();
}

void Contest::reset_Scroll()
{
	if (!problem_load_finished) { return; }

	auto prob_size = Data::GetIns().GetProblemSize(selecting);
	dxle::sizei32 page_size;
	DxLib::GetWindowSize(&page_size.width, &page_size.height);//@todo dxlibex
	page_size.height -= title_space;
	page_size.width -= menu_space_size;
	scrollbar.reset(page_size, prob_size);
}

void Contest::reset_window_size()
{
	//スクロール
	reset_Scroll();

	//問題切り替え矢印
	reset_problemselect();
}
