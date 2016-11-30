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

	ScrollBar2 scrollbar;
	Button to_problem;
	bool show_myscore_only;
	size_t last_WJ_submissions_size;
	std::vector<Submission> submissions_data;
	std::vector<Button> submission_state_index;
	std::vector<size_t> submissions_index;
	std::vector<Button> submissions_button;
	struct{
		int last_state_type;
		enum{up, down}direction;
	}sort_state;
public:
	Show_Score(int selecting);
	~Show_Score();

	std::unique_ptr<Sequence> update()override;
	void draw()const override;
private:

	std::unique_ptr<Sequence> update_Menu();
	void draw_Menu()const;

	std::unique_ptr<Sequence> update_Submit();
	void reset_button_area();//ボタンの再計算
	void reset_Scroll();//問題が変わった際のスクロール(とボタン)の再計算
	void draw_Submit()const;

	void get_submissions_copy();
	void reset_window_size();

	void run_sort(int type);
};
