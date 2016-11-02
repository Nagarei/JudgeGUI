#include "common.h"
#include "KeyInputData.h"
#include "Mouse.h"

void Sequence_Commom::draw_problem_state() const
{
	const auto& problems = Data::GetIns();
	int window_x, window_y;
	DxLib::GetWindowSize(&window_x, &window_y);//@todo dxlibex

	//�^�C�g���\��
	DrawStringCenter({ 0,0 }, problems[selecting].GetName().c_str(), dxle::color_tag::white,
		title_font, window_x);
	//�X�R�A�\��
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

	//�}�E�X���̓`�F�b�N

	if (arrow[0].update(mouse.get_now_pos(), mouse.get_now_input() & MOUSE_INPUT_LEFT)) {
		//��
		if (selecting <= 0) {
			selecting = problems.size();
		}
		--selecting;
		SetWindowTitle();
	}
	if (arrow[1].update(mouse.get_now_pos(), mouse.get_now_input() & MOUSE_INPUT_LEFT)) {
		//�E
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
				//��
				if (selecting <= 0) {
					selecting = problems.size();
				}
				--selecting;
				SetWindowTitle();
			}
			else if (window_x - side_space_size <= click_state.pos.x && click_state.pos.x < window_x) {
				//�E
				++selecting;
				if (problems.size() <= (unsigned)selecting) {
					selecting = 0;
				}
				SetWindowTitle();
			}
		}
	}
#endif

	//�L�[�{�[�h���̓`�F�b�N
	if (key.GetKeyInput(KEY_INPUT_LCONTROL) || key.GetKeyInput(KEY_INPUT_RCONTROL))
	{
		if (key.GetDirectionKeyInput(key.KEY_LEFT)) {
			//��
			if (selecting <= 0) {
				selecting = problems.size();
			}
			--selecting;
			SetWindowTitle();
		}
		else if (key.GetDirectionKeyInput(key.KEY_RIGHT)) {
			//�E
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

