#pragma once
#include "Data.h"
#include "ScrollBar.h"
#include "Button.h"
#include "common.h"

class Show_Score final : public Sequence, private Sequence_Commom
{
private:
	int menu_font;
	int result_font;

	dxle::sizei32 last_window_size;
	ScrollBar scrollbar_v;
	Button to_problem;
	std::vector<Button> results;
public:
	Show_Score(int selecting);
	~Show_Score();

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
