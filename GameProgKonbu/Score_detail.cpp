#include "Score_detail.h"
#include "Show_Score.h"
#include "Contest.h"

using int32_t = int;
namespace{
	constexpr int32_t data_height = 30;
	enum class show_data_enum : int32_t{//（個々の入出力以外で）表示するデータの種類
		time,/*prob_name,*/user_name, score,type,
		ENUM_SIZE
	};
	constexpr int32_t basic_data_heght_total = data_height*(int32_t)(show_data_enum::ENUM_SIZE);
	//全体に対する比率
	constexpr int32_t min_leftspace_width = 30;
	constexpr int32_t min_inputname_width = 193;
	constexpr int32_t min_type_width = 85;
	constexpr int32_t min_time_width = 134;
	constexpr int32_t min_memory_width = 171;
	constexpr int32_t min_middle_total_width = min_inputname_width+ min_type_width + min_time_width + min_memory_width;
	constexpr int32_t min_rightspace_width = 30;
	constexpr int32_t min_total_width = min_leftspace_width + min_middle_total_width + min_rightspace_width;

	constexpr dxle::rgb box_back_color{ 249,249,249 };
	constexpr dxle::rgb box_edge_color{ 221,221,221 };
}
Score_detail::Score_detail(int selecting_, size_t submissions_index_)
	: Sequence_Commom(selecting_)
	, menu_font(DxLib::CreateFontToHandle(_T("ＭＳ ゴシック"), 16, 2))
	, main_font(DxLib::CreateFontToHandle(_T("ＭＳ ゴシック"), 20, 2))
	, submissions_index(submissions_index_)
{
	//メインの表示部分
	scrollbar.set_pos({ menu_space_size, title_space });
	//ソース
	{
		tifstream ifs(Data::GetIns()[selecting].GetSubmissionSet()[submissions_index].get_source_name());
		if (ifs.bad()) {
			source_str_raw = _T("読み込みに失敗しました");
		}
		else {
			ifs.seekg(0, std::ios::end);
			auto str_len = ifs.tellg();
			auto str_buf = std::make_unique<TCHAR[]>(str_len);
			ifs.seekg(0, std::ios::beg);
			ifs.read(str_buf.get(), str_len);
			source_str_raw.resize(str_len*2);
			//source_str_raw = str_buf.get();
			for(auto iter = str_buf.get(); *iter != _T('\0'); ++iter){
				if(*iter == _T('\t')){
					for (size_t i = 0; i < 4; ++i) {
						source_str_raw.push_back(_T(' '));
					}
				}
				else{
					source_str_raw.push_back(*iter);
				}
			}
		}
		DxLib::GetDrawStringSizeToHandle(&source_size.width, &source_size.height, &source_line_num,
			source_str_raw.c_str(), source_str_raw.size(), main_font);//@todo dxlibex
	}
	//
	scrollbar.reset();

	to_problem.set_area({ 0, title_space }, { menu_space_size , menu_button_height });
	to_problem.set_str(_T("問題文"));
	to_submissions.set_area({ 0, title_space+ menu_button_height }, { menu_space_size , menu_button_height });
	to_submissions.set_str(_T("結果"));

	{
		dxle::rgb out_back_color{ 154, 130, 0 };
		dxle::rgb on_back_color{ 0, 197, 30 };
		dxle::rgb out_edge_color = dxle::color_tag::white;
		dxle::rgb on_edge_color = dxle::color_tag::white;
		dxle::rgb on_string_color = dxle::color_tag::white;
		dxle::rgb out_string_color = dxle::color_tag::white;
		to_problem.set_on_color(on_back_color, on_edge_color, on_string_color);
		to_problem.set_out_color(out_back_color, out_edge_color, out_string_color);
		to_submissions.set_on_color(on_back_color, on_edge_color, on_string_color);
		to_submissions.set_out_color(out_back_color, out_edge_color, out_string_color);
	}
}

Score_detail::~Score_detail()
{
	DeleteFontToHandle(menu_font);
	DeleteFontToHandle(main_font);
}


void Score_detail::draw() const
{
	int32_t pos1_y = data_height;
	//コード表示
	{
		DxLib::SetDrawArea(menu_space_size, pos1_y, menu_space_size + source_size.width, pos1_y + source_size.height);
		DxLib::DrawFillBox(menu_space_size, pos1_y, menu_space_size + source_size.width, pos1_y + source_size.height, dxle::dx_color(dxle::rgb{ 245,245,245 }).get());
		DxLib::DrawStringToHandle(menu_space_size, pos1_y, source_str_raw.c_str(), dxle::dx_color(dxle::color_tag::black).get(), main_font);
		//GetFontLineSpaceToHandle;
		DxLib::SetDrawAreaFull();
		pos1_y += source_size.height;
	}
	//基本情報表示
	{
		const auto& problem = Data::GetIns()[selecting];
		const auto& submission = problem.GetSubmissionSet()[submissions_index];
		auto draw_box_withframe = [](dxle::pointi pos1, dxle::pointi pos2, dxle::dx_color_param color) {
			DxLib::DrawFillBox(pos1.x, pos1.y, pos2.x, pos2.y, color.get());
			DxLib::DrawLineBox(pos1.x, pos1.y, pos2.x, pos2.y, dxle::dx_color(dxle::color_tag::black).get());
		};
		int32_t pos1_x = menu_space_size + left_space;
		int32_t pos2_x = pos1_x + 項目幅;
		int32_t pos3_x = pos2_x + 値幅;
		auto my_draw_box_WF = [&draw_box_withframe, &pos1_y, &pos1_x, &pos3_x]() {
			draw_box_withframe({ pos1_x,pos1_y }, { pos3_x, pos1_y + data_height }, dxle::rgb{ 249,249,249 });
		};
		auto draw_subject = [&項目幅, &pos1_y, &pos1_x, font = this->main_font](const TCHAR* str) {
			DrawStringRight({ pos1_x,pos1_y }, str, dxle::color_tag::black, font, { 項目幅, data_height });
		};
		auto draw_value = [&値幅, &pos1_y, &pos2_x, font = this->main_font](const TCHAR* str, auto... args) {
			DrawStringCenter2({ pos2_x,pos1_y }, str, dxle::color_tag::black, font, { 値幅, data_height }, std::forward<decltype(args)>(args)...);
		};
		//ユーザ名
		my_draw_box_WF();
		draw_subject(_T("ユーザ名"));
		draw_value(submission.get_user_name().c_str());
		pos1_y += data_height;
		//時間
		{
			const auto& stime = submission.get_submit_time();
			draw_subject(_T("提出日時"));
			draw_value(_T("%d/%d/%d[%d:%d:%d]"), stime.Year, stime.Mon, stime.Day, stime.Hour, stime.Min, stime.Sec);
			pos1_y += data_height;
		}
		//状態
		{
			draw_subject(_T("状態"));
			constexpr int32_t width = 30;
			int32_t x1 = pos2_x + (値幅 - width) / 2;
			auto for_draw = get_result_type_fordraw(submission);
			DxLib::DrawFillBox(x1, pos1_y, x1 + width, pos1_y + data_height, dxle::dx_color(for_draw.second).get());
			DrawStringCenter2({ x1, pos1_y }, for_draw.first.c_str(), for_draw.second, main_font, { width, pos1_y });
			pos1_y += data_height;
		}
		//得点
		draw_subject(_T("得点"));
		draw_value(_T("%d"), problem.GetScore_single(submissions_index));
		pos1_y += data_height;

		//仕切り線
		DxLib::DrawLine(pos2_x, data_height + source_size.height, pos2_x, pos1_y, dxle::dx_color(dxle::color_tag::black).get());
	}

	//タイトル表示//スコア表示
	draw_problem_state();

	//メニュー処理
	draw_Menu();
}

void Score_detail::draw_Menu() const
{
	DxLib::DrawFillBox(0, title_space, menu_space_size, last_window_size.height, dxle::dx_color(dxle::color_tag::black).get());//@todo dxlibex
	to_problem.draw(menu_font);
	to_submissions.draw(menu_font);
}
