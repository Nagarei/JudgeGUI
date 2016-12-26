#ifndef PARENT_MODE
#include "Score_detail.h"
#include "Show_Score.h"
#include "Contest.h"
#include "Submission.h"
#include "dx_utility.h"
#include "Mouse.h"
#include "SetClipboardText.h"
#include "popup.h"

constexpr TCHAR EOF_STR[] = _T("[EOF]");
constexpr size_t EOF_STR_SIZE = sizeof(EOF_STR) / sizeof(EOF_STR[0]) - 1;//NULL文字を除く

namespace {
	constexpr int32_t data_height = 30;
	enum class show_data_enum : int32_t {//（個々の入出力以外で）表示するデータの種類
		user_name, time,/*prob_name,*/ type, score,
		ENUM_SIZE
	};
	constexpr int32_t basic_data_heght_total = data_height*(int32_t)(show_data_enum::ENUM_SIZE);
	//全体に対する比率
	constexpr int32_t min_leftspace_width = 10;
	constexpr int32_t min_typename_width = 184;//項目幅
	constexpr int32_t min_type_width = 81;
	constexpr int32_t min_time_width = 88;
	constexpr int32_t min_memory_width = 113;
	constexpr int32_t min_value_width = min_type_width + min_time_width + min_memory_width;//値幅
	constexpr int32_t min_middle_total_width = min_typename_width + min_value_width;
	constexpr int32_t min_total_width = min_leftspace_width + min_middle_total_width;

	constexpr int32_t rightspace_width = 20;//固定値

	constexpr int32_t linenum_space = 60;//行番号の行の幅
	constexpr int32_t main_font_handle_size = 20;
	constexpr int32_t box_frame_thickness = 2;//淵付きボックスの淵の大きさ

	constexpr int32_t type_icon_width = 30;
	static_assert(type_icon_width < min_type_width, "");

	constexpr dxle::rgb box_back_color{ 249,249,249 };
	constexpr dxle::rgb box_on_back_color{ 249,249,0 };
	constexpr dxle::rgb box_edge_color{ 221,221,221 };
}
Score_detail::Score_detail(int selecting_, Submission_old&& submission_)
	: Sequence_Commom(selecting_)
	, menu_font(DxLib::CreateFontToHandle(_T("ＭＳ ゴシック"), 16, 2))
	, main_font(DxLib::CreateFontToHandle(_T("ＭＳ ゴシック"), main_font_handle_size, 2))
	, submission(submission_)
{
	SetFontLineSpaceToHandle(main_font_handle_size + 5, main_font);
	//メインの表示部分
	scrollbar.set_pos({ menu_space_size, title_space });
	//ソース
	{
		tifstream ifs(submission.get_source_name(), std::ios::in | std::ios::binary);
		if (ifs.bad()) {
			source_str = _T("読み込みに失敗しました");
		}
		else {
			ifs.seekg(0, std::ios::end);
			auto str_len = ifs.tellg();
			if (0 < str_len)
			{
				auto str_buf = std::make_unique<TCHAR[]>(str_len);
				str_buf[0] = _T('\0');
				ifs.seekg(0, std::ios::beg);
				ifs.read(str_buf.get(), str_len);//NULL終端文字はつかないので注意！！
				source_str.reserve(str_len * 2);
				//source_str = str_buf.get();
				for (auto iter = str_buf.get(), iter_end = str_buf.get() + str_len; iter != iter_end; ++iter) {
					if (*iter == _T('\t')) {
						for (size_t i = 0; i < 4; ++i) {
							source_str.push_back(_T(' '));
						}
					}
					else {
						source_str.push_back(*iter);
					}
				}
			}
			source_str += EOF_STR;
		}
		DxLib::GetDrawStringSizeToHandle(&source_size.width, &source_size.height, &source_line_num,
			source_str.c_str(), source_str.size(), main_font);//@todo dxlibex
		source_size.height += box_frame_thickness * 2;//ボックスの淵分
		source_size.width += box_frame_thickness * 2;//ボックスの淵分
		source_size.width += linenum_space;
	}
	//コンパイルメッセージ
	{
#if 0
		//BOMなしUTF-8（clangの出力）指定で読み込む
		FILE* fp = NULL;
		_tfopen_s(&fp,
			(submission.get_source_name() + _T("/../") + get_compile_out_filename()).c_str()
			, _T("r,ccs=UTF-8"));//clangの出力はUTF-8
		if (fp == NULL) {
			compile_str = _T("読み込みに失敗しました");
		}
		else {
			fseek(fp, 0, SEEK_END);
			auto size = ftell(fp);
			if (0 < size) {
				auto buf = std::make_unique<TCHAR[]>(size + 1);
				fseek(fp, 0, SEEK_SET);
				buf[size] = _T('\0');
				fread((void*)buf.get(), sizeof(TCHAR), size + 1, fp);
				compile_str = buf.get();
			}
			compile_str += EOF_STR;
}
#else
		//gccはANSI
		tifstream ifs(
			(submission.get_source_name() + _T("/../") + get_compile_out_filename()).c_str(),
			std::ios::in | std::ios::binary);
		if (ifs.bad()) {
			compile_str = _T("読み込みに失敗しました");
		}
		else {
			ifs.seekg(0, std::ios::end);
			auto str_len = ifs.tellg();
			if (0 < str_len)
			{
				auto str_buf = std::make_unique<TCHAR[]>((size_t)(str_len)+1);
				str_buf[0] = _T('\0');
				ifs.seekg(0, std::ios::beg);
				ifs.read(str_buf.get(), str_len);//NULL終端文字はつかないので注意！！
				str_buf[str_len] = _T('\0');
				compile_str.reserve(str_len * 2);
				compile_str = str_buf.get();
			}
			compile_str += EOF_STR;
		}
#endif
		DxLib::GetDrawStringSizeToHandle(&compile_size.width, &compile_size.height, nullptr,
			compile_str.c_str(), compile_str.size(), main_font);//@todo dxlibex
		compile_size.height += box_frame_thickness * 2;//ボックスの淵分
		compile_size.width += box_frame_thickness * 2;//ボックスの淵分
	}
	reset_Scroll();
	reset_scrolled_obj();

	//コピーボタン
	copy_code.set_str(_T("ソースコード    [コピー]"));
	copy_compile.set_str(_T("コンパイルメッセージ   [コピー]"));
	copy_code.set_on_color(box_on_back_color, box_edge_color, dxle::color_tag::black);
	copy_code.set_out_color(box_back_color, dxle::color_tag::black, dxle::color_tag::black);
	copy_compile.set_on_color(box_on_back_color, box_edge_color, dxle::color_tag::black);
	copy_compile.set_out_color(box_back_color, dxle::color_tag::black, dxle::color_tag::black);
	//reset_scrolled_obj();

	//メニュー配置
	to_problem.set_area({ 0, title_space }, { menu_space_size , menu_button_height });
	to_problem.set_str(_T("問題文"));
	to_submissions.set_area({ 0, title_space + menu_button_height }, { menu_space_size , menu_button_height });
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

	//リロード処理
	if (KeyInputData::GetIns().GetNewKeyInput(KEY_INPUT_F5)) {
		Data::GetIns()[selecting].ReloadPartialScores();
	}
	//コピペボタン処理
	update_copybutton();
	//スクロール
	if (scrollbar.update(false)) {
		reset_scrolled_obj();
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
	dxle::pointi32 pos1{ menu_space_size + left_space_width, title_space };
	pos1 -= scrollbar.get_value();
	//コード表示
	{
		dxle::sizei32 source_size_temp{ copy_code.get_area().second.width, source_size.height };

		copy_code.draw(main_font);
		pos1.y += data_height;
		DxLib::SetDrawArea(menu_space_size, pos1.y, pos1.x + source_size_temp.width, pos1.y + source_size_temp.height);
		//背景
		dxle::DrawBox(pos1, pos1 + source_size_temp, dxle::rgb{ 245,245,245 }, true);
		//行番号&行仕切り線
		for (int i = 0; i < source_line_num; ++i)
		{
			const auto height = source_size_temp.height / source_line_num;
			auto y = pos1.y + source_size_temp.height * i / source_line_num;
			DxLib::DrawLine(pos1.x, y, pos1.x + source_size_temp.width, y, dxle::dx_color(box_edge_color).get());
			DrawStringRight({ pos1.x + box_frame_thickness,y }, _T("%d"), dxle::color_tag::black,
				main_font, { linenum_space - box_frame_thickness, height }, i + 1);
		}
		//行番号とコードの区切りの縦線
		DxLib::DrawLine(pos1.x + linenum_space, pos1.y,
			pos1.x + linenum_space, pos1.y + source_size_temp.height, dxle::dx_color(box_edge_color).get());
		//コード
		DxLib::DrawStringToHandle(pos1.x + box_frame_thickness + linenum_space, pos1.y + box_frame_thickness,
			source_str.c_str(), dxle::dx_color(dxle::color_tag::black).get(), main_font);
		//枠
		dxle::DrawBox(pos1, pos1 + source_size_temp, dxle::color_tag::black, false);
		DxLib::SetDrawAreaFull();
		pos1.y += source_size_temp.height;
	}
	pos1.y += data_height;
	//コンパイルメッセージ表示
	{
		dxle::sizei32 compile_size_temp{ copy_compile.get_area().second.width, compile_size.height };

		copy_compile.draw(main_font);
		pos1.y += data_height;
		DxLib::SetDrawArea(menu_space_size, pos1.y, pos1.x + compile_size_temp.width, pos1.y + compile_size_temp.height);
		DrawBoxWithFrame(pos1, pos1 + compile_size_temp, dxle::rgb{ 245,245,245 }, dxle::color_tag::black);
		DxLib::DrawStringToHandle(pos1.x + box_frame_thickness, pos1.y + box_frame_thickness,
			compile_str.c_str(), dxle::dx_color(dxle::color_tag::black).get(), main_font);
		DxLib::SetDrawAreaFull();
		pos1.y += compile_size_temp.height;
	}
	pos1.y += data_height;
	//基本情報表示
	{
		const auto old_posy = pos1.y;
		const int32_t typename_width = draw_area_width*min_typename_width / min_total_width;
		const int32_t value_width = draw_area_width*min_value_width / min_total_width;
		const auto& problem = Data::GetIns()[selecting];
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
			int32_t x1 = pos2_x + (value_width - type_icon_width) / 2;
			auto for_draw = get_result_type_fordraw(submission.get_core());
			DxLib::DrawFillBox(x1, pos1.y, x1 + type_icon_width, pos1.y + data_height, dxle::dx_color(for_draw.second).get());
			DrawStringCenter2({ x1, pos1.y }, for_draw.first.data(), dxle::color_tag::black, main_font, { type_icon_width, data_height });
			pos1.y += data_height;
		}
		//得点
		my_draw_box_WF();
		draw_subject(_T("得点"));
		draw_value(_T("%d"), problem.GetScore_single(submission.get_core()));
		pos1.y += data_height;

		//仕切り線
		DxLib::DrawLine(pos2_x, old_posy, pos2_x, pos1.y, dxle::dx_color(box_edge_color).get());
	}
	pos1.y += data_height;
	//個々の入力表示
	{
		const auto old_posy = pos1.y;
		const int32_t typename_width = draw_area_width*min_typename_width / min_total_width;
		const int32_t type_width = draw_area_width*min_type_width / min_total_width;
		const int32_t time_width = draw_area_width*min_time_width / min_total_width;
		const int32_t memory_width = draw_area_width*min_memory_width / min_total_width;
		const int32_t middle_total_width = draw_area_width*min_middle_total_width / min_total_width;
		const auto& scores = submission.get_scores();
		int32_t pos_x = pos1.x;
		int32_t width = 0;
		auto my_draw_box_WF = [&pos1, &middle_total_width]() {
			DrawBoxWithFrame(pos1, { pos1.x + middle_total_width, pos1.y + data_height }, box_back_color, box_edge_color);
		};
		auto draw_str = [&pos_x, &pos1, &width, font = this->main_font](const TCHAR* str, auto... args) {
			DrawStringCenter2({ pos_x,pos1.y }, str, dxle::color_tag::black, font, { width, data_height }, std::forward<decltype(args)>(args)...);
		};
		auto set_next = [&pos_x, &old_posy, &pos1, &width](int32_t new_width) {
			pos_x += width; width = new_width;
			DxLib::SetDrawArea(pos_x, old_posy, pos_x + width, pos1.y + data_height);//縦線を引く為に少し多めにyをとっておく
		};
		//項目
		my_draw_box_WF();
		set_next(typename_width);   draw_str(_T("入力"));
		set_next(type_width);       draw_str(_T("状態"));
		set_next(time_width);       draw_str(_T("実行時間"));
		set_next(memory_width);     draw_str(_T("メモリ使用量"));
		pos1.y += data_height;
		//値
		for (auto iter_begin = scores.cbegin(), iter = iter_begin, iter_end = scores.cend(); iter != iter_end; ++iter)
		{
			auto& i = *iter;
			DxLib::SetDrawAreaFull();
			my_draw_box_WF();
			pos_x = pos1.x;
			width = 0;
			set_next(typename_width); draw_str(get_input_name((iter - iter_begin)+1).data());
			{//状態
				set_next(type_width);
				int32_t x1 = pos_x + (type_width - type_icon_width) / 2;
				auto for_draw = get_result_type_fordraw(i);
				DxLib::DrawFillBox(x1, pos1.y, x1 + type_icon_width, pos1.y + data_height, dxle::dx_color(for_draw.second).get());
				DrawStringCenter2({ x1, pos1.y }, for_draw.first.data(), dxle::color_tag::black, main_font, { type_icon_width, data_height });
			}
			set_next(time_width);   draw_str(ToStringEx(i.use_time, _T(" ms")).c_str());
			set_next(memory_width); draw_str(ToStringEx(i.use_memory, _T(" KB")).c_str());
			pos1.y += data_height;
		}

		//仕切り線
		auto draw_v_line = [&pos_x, &old_posy, &pos1, &width]() {
			DxLib::DrawLine(pos_x, old_posy, pos_x, pos1.y, dxle::dx_color(box_edge_color).get());
		};
		pos_x = pos1.x;
		width = 0;
		set_next(typename_width);                 //draw_str(_T("入力"));
		set_next(type_width);       draw_v_line();//draw_str(_T("状態"));
		set_next(time_width);       draw_v_line();//draw_str(_T("実行時間	"));
		set_next(memory_width);     draw_v_line();//draw_str(_T("メモリ使用量"));

		DxLib::SetDrawAreaFull();
	}
	//末端空白
	pos1.y += data_height;

	//スクロール系表示
	scrollbar.draw();

	//タイトル表示//スコア表示
	draw_problem_state();

	//メニュー処理
	draw_Menu();
}

void Score_detail::reset_scrolled_obj()
{
	dxle::sizei32 window_size = My_GetWindowSize();

	//ポップアップ
	reset_popup();

	//コピーボタン
	const int32_t draw_area_width = std::max(window_size.width - menu_space_size - rightspace_width, min_total_width);
	const int32_t left_space_width = draw_area_width*min_leftspace_width / min_total_width;
	const int32_t middle_total_width = draw_area_width*min_middle_total_width / min_total_width;
	dxle::pointi32 pos1{ menu_space_size + left_space_width, title_space };
	pos1 -= scrollbar.get_value();
	//copy_code
	copy_code.set_area(pos1, { std::max(middle_total_width, source_size.width), data_height });
	pos1.y += data_height;
	pos1.y += source_size.height;
	pos1.y += data_height;
	//copy_compile
	copy_compile.set_area(pos1, { std::max(middle_total_width, compile_size.width), data_height });
}

void Score_detail::update_copybutton()
{
	auto& mouse = Mouse::GetIns();

	if (copy_code.update(mouse.get_now_pos(), mouse.get_now_input() & MOUSE_INPUT_LEFT)) {
		//クリップボードにコピー
		My_SetClipboardText(source_str.substr(0, source_str.size() - EOF_STR_SIZE));
	}
	if (copy_compile.update(mouse.get_now_pos(), mouse.get_now_input() & MOUSE_INPUT_LEFT)) {
		My_SetClipboardText(compile_str.substr(0, compile_str.size() - EOF_STR_SIZE));
	}
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

dxle::sizeui32 Score_detail::get_display_total_size()const
{
	dxle::sizeui32 result;
	//height
	result.height += data_height;
	result.height += source_size.height;
	result.height += data_height;
	result.height += data_height;
	result.height += compile_size.height;
	result.height += data_height;
	result.height += basic_data_heght_total;
	result.height += data_height;
	result.height += data_height;
	result.height += data_height * submission.get_scores().size();
	result.height += data_height;
	//width
	int page_size_width = My_GetWindowSize().width;
	const int32_t draw_area_width = std::max(page_size_width - menu_space_size - rightspace_width, min_total_width);
	const int32_t left_space_width = draw_area_width*min_leftspace_width / min_total_width;
	result.width = std::max<int32_t>({ draw_area_width,
		left_space_width + source_size.width + rightspace_width,
		left_space_width + compile_size.width + rightspace_width });

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

	reset_scrolled_obj();
}

#endif // !PARENT_MODE
