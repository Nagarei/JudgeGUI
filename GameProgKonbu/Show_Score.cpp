#include "Show_Score.h"
#include "other_usefuls.h"

Show_Score::Show_Score()
	: menu_font(DxLib::CreateFontToHandle(_T("MS UI Gothic"), 16, 2))
{
}

Show_Score::~Show_Score()
{
	DeleteFontToHandle(menu_font);
}

std::unique_ptr<Sequence> Show_Score::update()
{
	dxle::sizei32 window_size;
	{
		DxLib::GetWindowSize(&window_size.width, &window_size.height);//@todo dxlibex
		if (window_size != last_window_size) {
			reset_window_size();
		}
	}

	DEBUG_NOTE;
	if (results.size() != 今の結果の数) {
		reset_window_size();
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
}

void Show_Score::draw() const
{
	const auto& problems = Data::GetIns();
	int window_x, window_y;
	DxLib::GetWindowSize(&window_x, &window_y);//@todo dxlibex

	//タイトル表示//スコア表示
	draw_problem_state();

	//問題選択矢印表示
	draw_SelectProblem();

	//メニュー処理
	draw_Menu();
}

void Show_Score::update_Menu()
{
	あああ;
}
