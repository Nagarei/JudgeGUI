#pragma once
#include "Data.h"
#include "ScrollBar.h"
#include "Button.h"

class Contest final : public Sequence
{
private:
	int title_font;
	int main_font;

	int selecting;
	dxle::pointi problem_pos;

	bool problem_load_finished;
	dxle::sizei32 last_window_size;
	ScrollBar scrollbar_v;
	ScrollBar scrollbar_h;
	Button to_result;
	//Button to_submit;
	std::vector<Button> samples;
public:
	Contest();
	~Contest();

	std::unique_ptr<Sequence> update()override;
	void draw()const override;
private:
	void SetWindowTitle();

	void update_SelectProblem();
	void draw_SelectProblem()const;

	void update_Scroll();
	void reset_Scroll();//��肪�ς�B��ۂ̃X�N���[���̍Čv�Z
	void draw_Scroll()const;
};
