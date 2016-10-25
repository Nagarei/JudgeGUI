#pragma once
#include "Data.h"
#include "ScrollBar.h"

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
	void reset_Scroll();//問題が変わった際のスクロールの再計算
	void draw_Scroll()const;
};
