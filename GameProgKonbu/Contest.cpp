#include "Contest.h"
#include "useful_func_and_class.h"
#include "other_usefuls.h"
#include "Mouse.h"
#include "test.h"
#include "fps.h"
namespace
{
	const int title_space = 50;
	const int side_space_size = 20;
	const int menu_space_size = 100;
	const int scrollbar_size = 17;
}
Contest::Contest()
	: title_font(DxLib::CreateFontToHandle(_T("MS UI Gothic"), 30, 2))
	, main_font (DxLib::CreateFontToHandle(_T("MS UI Gothic"), 16, 2))
	, selecting(0)
	, problem_load_finished(false)
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
		Data::GetIns().DrawProblem(selecting, problem_pos);
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
	DEBUG_NOTE;
}

void Contest::update_Scroll()
{
	if (!problem_load_finished) {
		problem_pos.x = menu_space_size;
		problem_pos.y = title_space;
		return;
	}

	//windowサイズ更新チェック
	dxle::sizei32 window_size;
	{
		DxLib::GetWindowSize(&window_size.width, &window_size.height);//@todo dxlibex
		if (window_size != last_window_size) {
			reset_Scroll();
			return;
		}
	}
	//スクロール
	auto& key = KeyInputData::GetIns();
	auto& mouse = Mouse::GetIns();

	uint32_t keyinput = 0;
	DEBUG_NOTE;//key_input
	scrollbar_v.update(GetFrameTime(),
		mouse.get_now_pos() - dxle::pointi32{ window_size.width - scrollbar_size, title_space },
		mouse.get_now_wheel() * -25, mouse.get_now_input() & MOUSE_INPUT_LEFT, keyinput);
	keyinput = 0;
	DEBUG_NOTE;//key_input
	scrollbar_h.update(GetFrameTime(),
		mouse.get_now_pos() - dxle::pointi32{ menu_space_size, window_size.height - scrollbar_size },
		mouse.get_now_H_wheel() * -25, mouse.get_now_input() & MOUSE_INPUT_LEFT, keyinput);

	//値セット
	problem_pos.x = menu_space_size - scrollbar_h.get_value();
	problem_pos.y = title_space - scrollbar_v.get_value();

}

void Contest::reset_Scroll()
{
	assert(problem_load_finished);
	auto prob_size = Data::GetIns().GetProblemSize(selecting);
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
