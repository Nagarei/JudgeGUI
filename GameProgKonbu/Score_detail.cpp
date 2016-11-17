#include "Score_detail.h"
#include "Show_Score.h"
#include "Contest.h"
#include "SubmissionLog.h"
#include "other_usefuls.h"
#include "Mouse.h"

namespace{
	constexpr int32_t data_height = 30;
	enum class show_data_enum : int32_t{//（個々の入出力以外で）表示するデータの種類
		user_name, time,/*prob_name,*/ type, score,
		ENUM_SIZE
	};
	constexpr int32_t basic_data_heght_total = data_height*(int32_t)(show_data_enum::ENUM_SIZE);
	//全体に対する比率
	constexpr int32_t min_leftspace_width = 10;
	constexpr int32_t min_typename_width = 138;//項目幅
	constexpr int32_t min_type_width = 61;
	constexpr int32_t min_time_width = 66;
	constexpr int32_t min_memory_width = 85;
	constexpr int32_t min_value_width = min_type_width + min_time_width + min_memory_width;//値幅
	constexpr int32_t min_middle_total_width = min_typename_width + min_value_width;
	constexpr int32_t min_total_width = min_leftspace_width + min_middle_total_width;

	constexpr int32_t rightspace_width = 20;//固定値

	constexpr int32_t linenum_space = 60;//行番号の行の幅
	constexpr int32_t main_font_handle_size = 20;
	constexpr int32_t box_frame_thickness = 2;//淵付きボックスの淵の大きさ

	constexpr dxle::rgb box_back_color{ 249,249,249 };
	constexpr dxle::rgb box_edge_color{ 221,221,221 };
}
Score_detail::Score_detail(int selecting_, size_t submissions_index_)
	: Sequence_Commom(selecting_)
	, menu_font(DxLib::CreateFontToHandle(_T("ＭＳ ゴシック"), 16, 2))
	, main_font(DxLib::CreateFontToHandle(_T("ＭＳ ゴシック"), main_font_handle_size, 2))
	, submissions_index(submissions_index_)
{
	SetFontLineSpaceToHandle(main_font_handle_size + 5, main_font);
	auto& data_ins = Data::GetIns();
	auto& submission = data_ins[selecting].GetSubmissionSet()[submissions_index];
	//メインの表示部分
	scrollbar.set_pos({ menu_space_size, title_space });
	//ソース
	{
		tifstream ifs(submission.get_source_name());
		if (ifs.bad()) {
			source_str_raw = _T("読み込みに失敗しました");
		}
		else {
			ifs.seekg(0, std::ios::end);
			auto str_len = ifs.tellg();
			if (0 < str_len)
			{
				auto str_buf = std::make_unique<TCHAR[]>(str_len);
				str_buf[0] = _T('\0');
				ifs.seekg(0, std::ios::beg);
				ifs.read(str_buf.get(), str_len);
				source_str_raw.reserve(str_len * 2);
				//source_str_raw = str_buf.get();
				for (auto iter = str_buf.get(); *iter != _T('\0'); ++iter) {
					if (*iter == _T('\t')) {
						for (size_t i = 0; i < 4; ++i) {
							source_str_raw.push_back(_T(' '));
						}
					}
					else {
						source_str_raw.push_back(*iter);
					}
				}
			}
			source_str_raw += _T("[EOF]");
		}
		DxLib::GetDrawStringSizeToHandle(&source_size.width, &source_size.height, &source_line_num,
			source_str_raw.c_str(), source_str_raw.size(), main_font);//@todo dxlibex
		source_size.height += box_frame_thickness*2;//ボックスの淵分
		source_size.width += box_frame_thickness * 2;//ボックスの淵分
		source_size.width += linenum_space;
	}
	//コンパイルメッセージ
	{
		tifstream ifs(submission.get_source_name() + _T("/../") + get_compile_out_filename());
		if (ifs.bad()) {
			compile_str_raw = _T("読み込みに失敗しました");
		}
		else {
			ifs.seekg(0, std::ios::end);
			auto str_len = ifs.tellg();
			if (0 < str_len)
			{
				auto str_buf = std::make_unique<TCHAR[]>(str_len);
				str_buf[0] = _T('\0');
				ifs.seekg(0, std::ios::beg);
				ifs.read(str_buf.get(), str_len);
				compile_str_raw.reserve(str_len * 2);
				//source_str_raw = str_buf.get();
				for (auto iter = str_buf.get(); *iter != _T('\0'); ++iter) {
					if (*iter == _T('\t')) {
						for (size_t i = 0; i < 4; ++i) {
							compile_str_raw.push_back(_T(' '));
						}
					}
					else {
						compile_str_raw.push_back(*iter);
					}
				}
			}
			compile_str_raw += _T("[EOF]");
		}
		DxLib::GetDrawStringSizeToHandle(&compile_size.width, &compile_size.height, nullptr,
			compile_str_raw.c_str(), compile_str_raw.size(), main_font);//@todo dxlibex
		compile_size.height += box_frame_thickness * 2;//ボックスの淵分
		compile_size.width += box_frame_thickness * 2;//ボックスの淵分
	}
	reset_Scroll();

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


std::unique_ptr<Sequence> Score_detail::update()
{
	if (!GetWindowActiveFlag()) {
		return nullptr;
	}

	std::unique_ptr<Sequence> next_sequence = nullptr;
	auto set_next = [&next_sequence](std::unique_ptr<Sequence>&& param) {
		if (param) { next_sequence = std::move(param); }
	};

	dxle::sizei32 window_size = My_GetWindowSize();
	if (window_size != last_window_size) {
		reset_window_size();
	}

	if (scrollbar.update())
	{
		//コピペボタン処理
	}
	//メニュー処理
	set_next(update_Menu());

	last_window_size = window_size;
	return next_sequence;
}

void Score_detail::draw() const
{
	dxle::DrawBox({ 0,0 }, last_window_size, dxle::color_tag::white, true);

	const int32_t draw_area_width = std::max(last_window_size.width - menu_space_size - rightspace_width, min_total_width);
	const int32_t left_space_width = draw_area_width*min_leftspace_width / min_total_width;
	dxle::pointi32 pos1{ menu_space_size + +left_space_width, title_space };
	pos1 -= scrollbar.get_value();
	//コード表示
	{
		pos1.y += data_height;
		DxLib::SetDrawArea(menu_space_size, pos1.y, pos1.x + source_size.width, pos1.y + source_size.height);
		//背景
		dxle::DrawBox(pos1, pos1 + source_size, dxle::rgb{ 245,245,245 }, true);
		//行番号&行仕切り線
		for (int i = 0; i < source_line_num; ++i)
		{
			const auto height = source_size.height / source_line_num;
			auto y = pos1.y + source_size.height * i / source_line_num;
			DxLib::DrawLine(pos1.x, y, pos1.x + source_size.width, y, dxle::dx_color(box_edge_color).get());
			DrawStringRight({ pos1.x + box_frame_thickness,y }, _T("%d"), dxle::color_tag::black,
				main_font, { linenum_space - box_frame_thickness, height }, i+1);
		}
		//行番号とコードの区切りの縦線
		DxLib::DrawLine(pos1.x + linenum_space, pos1.y,
						pos1.x + linenum_space, pos1.y + source_size.height, dxle::dx_color(box_edge_color).get());
		//コード
		DxLib::DrawStringToHandle(pos1.x + box_frame_thickness + linenum_space, pos1.y + box_frame_thickness,
			source_str_raw.c_str(), dxle::dx_color(dxle::color_tag::black).get(), main_font);
		//枠
		dxle::DrawBox(pos1, pos1 + source_size, dxle::color_tag::black, false);
		DxLib::SetDrawAreaFull();
		pos1.y += source_size.height;
	}
	//コンパイルメッセージ表示
	{
		pos1.y += data_height;
		DxLib::SetDrawArea(menu_space_size, pos1.y, pos1.x + compile_size.width, pos1.y + compile_size.height);
		DrawBoxWithFrame(pos1, pos1 + compile_size, dxle::rgb{ 245,245,245 }, dxle::color_tag::black);
		DxLib::DrawStringToHandle(pos1.x + box_frame_thickness, pos1.y + box_frame_thickness,
			compile_str_raw.c_str(), dxle::dx_color(dxle::color_tag::black).get(), main_font);
		DxLib::SetDrawAreaFull();
		pos1.y += compile_size.height;
	}
	pos1.y += compile_size.height;
	//基本情報表示
	{
		const auto old_posy = pos1.y;
		const int32_t typename_width = draw_area_width*min_typename_width / min_total_width;
		const int32_t value_width = draw_area_width*min_value_width / min_total_width;
		const auto& problem = Data::GetIns()[selecting];
		const auto& submission = problem.GetSubmissionSet()[submissions_index];
		int32_t pos1_x = pos1.x;
		int32_t pos2_x = pos1_x + typename_width;
		int32_t pos3_x = pos2_x + value_width;
		auto my_draw_box_WF = [&pos1, &pos1_x, &pos3_x]() {
			DrawBoxWithFrame({ pos1_x,pos1.y }, { pos3_x, pos1.y + data_height }, box_back_color, box_edge_color);
		};
		auto draw_subject = [&typename_width, &pos1, &pos1_x, font = this->main_font](const TCHAR* str) {
			DrawStringCenter2({ pos1_x,pos1.y }, str, dxle::color_tag::black, font, { typename_width, data_height });
		};
		auto draw_value = [&value_width, &pos1, &pos2_x, font = this->main_font](const TCHAR* str, auto... args) {
			DrawStringCenter2({ pos2_x,pos1.y }, str, dxle::color_tag::black, font, { value_width, data_height }, std::forward<decltype(args)>(args)...);
		};
		//ユーザ名
		my_draw_box_WF();
		draw_subject(_T("ユーザ名"));
		draw_value(submission.get_user_name().c_str());
		pos1.y += data_height;
		//時間
		my_draw_box_WF();
		{
			const auto& stime = submission.get_submit_time();
			draw_subject(_T("提出日時"));
			draw_value(_T("%d/%d/%d[%d:%d:%d]"), stime.Year, stime.Mon, stime.Day, stime.Hour, stime.Min, stime.Sec);
			pos1.y += data_height;
		}
		//状態
		my_draw_box_WF();
		{
			draw_subject(_T("状態"));
			constexpr int32_t width = 30;
			int32_t x1 = pos2_x + (value_width - width) / 2;
			auto for_draw = get_result_type_fordraw(submission);
			DxLib::DrawFillBox(x1, pos1.y, x1 + width, pos1.y + data_height, dxle::dx_color(for_draw.second).get());
			DrawStringCenter2({ x1, pos1.y }, for_draw.first.data(), dxle::color_tag::black, main_font, { width, data_height });
			pos1.y += data_height;
		}
		//得点
		my_draw_box_WF();
		draw_subject(_T("得点"));
		draw_value(_T("%d"), problem.GetScore_single(submissions_index));
		pos1.y += data_height;

		//仕切り線
		DxLib::DrawLine(pos2_x, old_posy, pos2_x, pos1.y, dxle::dx_color(box_edge_color).get());
	}

	//スクロール系表示
	scrollbar.draw();

	//タイトル表示//スコア表示
	draw_problem_state();

	//メニュー処理
	draw_Menu();
}

std::unique_ptr<Sequence> Score_detail::update_Menu()
{
	std::unique_ptr<Sequence> next_sequence = nullptr;

	auto& mouse = Mouse::GetIns();

	if (to_problem.update(mouse.get_now_pos(), mouse.get_now_input() & MOUSE_INPUT_LEFT)) {
		next_sequence = std::make_unique<Contest>(selecting);
	}
	if (to_submissions.update(mouse.get_now_pos(), mouse.get_now_input() & MOUSE_INPUT_LEFT)) {
		next_sequence = std::make_unique<Show_Score>(selecting);
	}

	return next_sequence;
}

void Score_detail::draw_Menu() const
{
	DxLib::DrawFillBox(0, title_space, menu_space_size, last_window_size.height, dxle::dx_color(dxle::color_tag::black).get());//@todo dxlibex
	to_problem.draw(menu_font);
	to_submissions.draw(menu_font);
}

dxle::sizeui32 Score_detail::get_display_total_size()
{
	auto& submission = Data::GetIns()[selecting].GetSubmissionSet()[submissions_index];

	dxle::sizeui32 result;
	//height
	result.height += data_height;
	result.height += source_size.height;
	result.height += data_height;
	result.height += compile_size.height;
	result.height += data_height;
	result.height += basic_data_heght_total;
	result.height += data_height;
	result.height += data_height;
	result.height += data_height * submission.get_scores().size();
	//width
	int page_size_width = My_GetWindowSize().width;
	result.width = std::max(page_size_width - menu_space_size - rightspace_width, min_total_width);

	return result;
}

void Score_detail::reset_Scroll()
{
	auto object_size = get_display_total_size();
	dxle::sizei32 page_size = My_GetWindowSize();
	page_size.height -= title_space;
	page_size.width -= menu_space_size;
	assert(0 < page_size.height && 0 < page_size.width);
	scrollbar.reset(static_cast<dxle::sizeui32>(page_size), object_size);
}
