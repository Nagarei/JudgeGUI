#include "Contest.h"
#include "useful_func_and_class.h"
#include "other_usefuls.h"
#include "Mouse.h"
#include "test.h"
namespace
{
	const int title_space = 50;
	const int side_space_size = 20;
	const int menu_space_size = 100;
}
Contest::Contest()
	: title_font(DxLib::CreateFontToHandle(_T("MS UI Gothic"), 30, 2))
	, main_font (DxLib::CreateFontToHandle(_T("MS UI Gothic"), 16, 2))
	, selecting(0)
{
	DxLib::SetDragFileValidFlag(TRUE);
	SetWindowTitle();
}
Contest::~Contest()
{
	DxLib::SetDragFileValidFlag(FALSE);
	DeleteFontToHandle(title_font);
	DeleteFontToHandle(main_font);
}


std::unique_ptr<Sequence> Contest::update()
{
	int window_x, window_y;
	DxLib::GetWindowSize(&window_x, &window_y);//@todo dxlibex

	if (1 <= GetDragFileNum())
	{
		auto str_buf = std::make_unique<TCHAR[]>(GetDragFilePath(nullptr)+1);
		GetDragFilePath(str_buf.get());
		DragFileInfoClear();
		SetDragFileValidFlag(TRUE);
		compile_taskmanager::set_test(selecting, str_buf.get());
	}

	if (GetWindowActiveFlag())
	{
		//問題選択
		update_SelectProblem();
		//問題スクロール
		update_Scroll();
	}

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

	if (problems.IsLoadFinesed(selecting))
	{
		//問題表示
		DxLib::SetDrawArea(menu_space_size, title_space, window_x, window_y);
		DxLib::DrawFillBox(menu_space_size, title_space, window_x, window_y, dxle::dx_color(dxle::color_tag::white).get());//@todo dxlibex
		Data::GetIns().DrawProblem(selecting, problem_pos);
		DxLib::SetDrawAreaFull();
	}
	else
	{
		int y = (window_y - 30) / 2;
		DrawStringCenter({ 0,y }, _T("Now Loading"), dxle::color_tag::white, title_font, window_x);
	}

	//スクロール系表示
	draw_Scroll();

	//問題選択矢印表示
	draw_SelectProblem();
	DrawToLeftArrow2(0, title_space / 2, side_space_size, dxle::color_tag::yellow);
	DrawToRightArrow2(window_x, title_space / 2, side_space_size, dxle::color_tag::yellow);
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

	//@todo dxlibex
	while(!mouse.click_log_is_empty())
	{
		auto click_state = mouse.click_log_front(); mouse.click_log_pop();
		if (click_state.type & MOUSE_INPUT_LEFT)
		{
			if (0 <= click_state.pos.x && click_state.pos.x < side_space_size) {
				//左
				if (selecting <= 0) {
					selecting = problems.size();
				}
				--selecting;
			}
			else if (window_x - side_space_size <= click_state.pos.x && click_state.pos.x < window_x) {
				//右
				++selecting;
				if (problems.size() <= (unsigned)selecting) {
					selecting = 0;
				}
			}
		}
	}

	//キーボード入力チェック

	if (key.GetDirectionKeyInput(key.KEY_LEFT)) {
		//左
		if (selecting <= 0) {
			selecting = problems.size();
		}
		--selecting;
	}
	else if (key.GetDirectionKeyInput(key.KEY_RIGHT)) {
		//右
		++selecting;
		if (problems.size() <= (unsigned)selecting) {
			selecting = 0;
		}
	}

	problems.SetBuildProblemText(selecting);
}

void Contest::draw_SelectProblem() const
{
	DEBUG_NOTE;
}

void Contest::update_Scroll()
{
	DEBUG_NOTE;
	problem_pos.x = menu_space_size;
	problem_pos.y = title_space;
	return;
	
	auto problem_size = Data::GetIns().GetProblemSize(selecting);
	//上下
	if()
}

void Contest::draw_Scroll() const
{
	DEBUG_NOTE;
}
