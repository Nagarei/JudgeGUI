#pragma once
#include "Data.h"
#include "ScrollBar2.h"
#include "Button.h"
#include "common.h"
あああ;
class Score_detail final : public Sequence, private Sequence_Commom
{
private:
	int menu_font;
	int main_font;

	ScrollBar2 scrollbar;
	Button to_problem;
	Button to_submissions;

	size_t submissions_index;
	//Button source_on_off;
	dxle::tstring source_str_raw;
	dxle::sizei32 source_size;
public:
	Score_detail(int selecting, size_t submissions_index);
	~Score_detail();

	std::unique_ptr<Sequence> update()override;
	void draw()const override;
//----------------追記だよ！！11/15夜
private:
	Button copy_code;//まだcppには書いてないよ！！
	uint32_t source_line_num;
	//dxle::sizei32 source_size;//行カウントを含む//未実装だよ！！

	std::unique_ptr<Sequence> update_Menu();
	void draw_Menu()const;
};
