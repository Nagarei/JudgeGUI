#pragma once
#include "Data.h"

class Contest final : public Sequence
{
private:
	int title_font;
	int main_font;

	int selecting;
	dxle::pointi problem_pos;
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
	void draw_Scroll()const;
};
