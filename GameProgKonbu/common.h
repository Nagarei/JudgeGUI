#pragma once
#include "Data.h"
#include "Button.h"
#include "FormatToString.h"

constexpr int title_space = 50;
constexpr int arrow_width = 20;
constexpr int button_edge_size = 2;
constexpr int menu_space_size = 100;
constexpr int menu_button_height = 50;
constexpr int scrollbar_size = 17;

class Sequence_Commom
{
protected:
	int title_font;
	int score_font;
	int selecting;
	Button arrow[2];//left,right
	dxle::sizei32 last_window_size;

protected:
	Sequence_Commom(int selecting);
	Sequence_Commom(const Sequence_Commom&) = delete;
	Sequence_Commom& operator=(const Sequence_Commom&) = delete;
	~Sequence_Commom() { DeleteFontToHandle(title_font); DeleteFontToHandle(score_font); }

	void draw_problem_state()const;//タイトル、スコア
	void update_SelectProblem();
	void draw_SelectProblem()const;
	void SetWindowTitle()
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
	void reset_problemselect() {
		dxle::sizei32 window_size;
		DxLib::GetWindowSize(&window_size.width, &window_size.height);//@todo dxlibex
		//問題切り替え矢印
		//arrow[0].set_area({ 0,0 }, { arrow_width, title_space });
		arrow[1].set_area({ window_size.width - arrow_width,0 }, { arrow_width , title_space });
	}
};
