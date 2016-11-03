#pragma once
#include "Data.h"
#include "ScrollBar.h"
#include "Button.h"
#include "common.h"

class Contest final : public Sequence, private Sequence_Commom
{
private:
	int loading_font;
	int menu_font;

	dxle::pointi problem_pos;

	bool problem_load_finished;
	ScrollBar scrollbar_v;
	ScrollBar scrollbar_h;
	double extend_rate;//拡大/縮小率
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
	void draw_Scroll()const;

	void reset_window_size();
};
