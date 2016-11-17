#pragma once
#include "Sequence.h"
#include "Data.h"
#include "ScrollBar2.h"
#include "Button.h"
#include "common.h"

class Score_detail final : public Sequence, private Sequence_Commom
{
private:
	int menu_font;
	int main_font;

	ScrollBar2 scrollbar;
	Button to_problem;
	Button to_submissions;
	Button copy_code;//まだcppには書いてないよ！！

	size_t submissions_index;
	//Button source_on_off;
	dxle::tstring source_str_raw;
	dxle::sizei32 source_size;//行カウントを含むようになる予定
	int source_line_num;
	dxle::tstring compile_str_raw;
	dxle::sizei32 compile_size;
private:
	void reset_scrolled_obj();

	std::unique_ptr<Sequence> update_Menu();
	void draw_Menu()const;

	dxle::sizeui32 get_display_total_size();
	void reset_Scroll();

	void reset_window_size(){ reset_Scroll(); }
public:
	Score_detail(int selecting, size_t submissions_index);
	~Score_detail();

	std::unique_ptr<Sequence> update()override;
	void draw()const override;
};
