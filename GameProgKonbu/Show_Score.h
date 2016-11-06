#pragma once
#include "Data.h"
#include "ScrollBar2.h"
#include "Button.h"
#include "common.h"

class Show_Score final : public Sequence, private Sequence_Commom
{
private:
	int menu_font;
	int submissions_font;

	dxle::sizei32 last_window_size;
	ScrollBar2 scrollbar;
	Button to_problem;
	std::vector<Button> submissions;
public:
	Show_Score(int selecting);
	~Show_Score();

	std::unique_ptr<Sequence> update()override;
	void draw()const override;
private:

	std::unique_ptr<Sequence> update_Menu();
	void draw_Menu()const;

	std::unique_ptr<Sequence> update_Submit();
	void reset_Scroll();//問題が変わった際のスクロール(とボタン)の再計算
	void draw_Submit()const;

	void reset_window_size();
};
