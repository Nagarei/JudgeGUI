#pragma once
#include "Data.h"
#include "ScrollBar2.h"
#include "Button.h"
#include "common.h"

class Contest final : public Sequence, private Sequence_Commom
{
private:
	int loading_font;
	int menu_font;

	dxle::pointi32 problem_pos;

	bool problem_load_finished;
	ScrollBar2 scrollbar;
	Button to_result;
	Button to_submit;
	std::vector<Button> samples;
public:
	Contest(int selecting);
	~Contest();

	std::unique_ptr<Sequence> update()override;
	void draw()const override;
private:

	std::unique_ptr<Sequence> update_Menu();
	void draw_Menu()const;

	void update_Scroll();
	void reset_Scroll();//問題が変わった際のスクロールの再計算

	void reset_window_size();
};
