﻿
#include "Data.h"
#include "other_usefuls.h"
#include "GetNumfileNum.h"

std::mutex Data::new_scores_mtx;
std::vector<std::pair<size_t, Scores>> Data::new_scores;//FIFO (first: pop, last: push)
namespace {
	const int linestart_space = 2;
}

void Data::InitProblem(dxle::tstring path, dxle::tstring user_name_)
{
#ifdef _DEBUG
	//初回しか呼ばれないので、problemsのスレッドセーフを保証できる
	//下手にいじらないこと
	static bool is_finish = false;
	assert(is_finish == false);
	is_finish = true;
#endif

	if (!path.empty() && path.back() != '/' && path.back() != '\\'){
		path.push_back('/');
	}
	path += _T("Problems/");
	{//絶対パスに
		TCHAR buf[MAX_PATH * 3];
		GetFullPathName(path.c_str(), sizeof(buf) / sizeof(buf[0]), buf, nullptr);
		path = buf;
	}
	const_cast<dxle::tstring&>(problems_directory) = std::move(path);

	DxLib::FILEINFO fi;
	DWORD_PTR hFind = (DWORD_PTR)-1;
	FINALLY([&]() {if (hFind != -1) { FileRead_findClose(hFind); }});

	hFind = DxLib::FileRead_findFirst((problems_directory + _T('*')).c_str(), &fi);
	if (hFind == -1) {
		throw std::exception("no problem is found");
	}

	do
	{
		if (fi.DirFlag == TRUE)
		{
			if (fi.Name[0] != '.')
			{
				if (problems.size() >= INT_MAX) {
					MessageBox(GetMainWindowHandle(), _T("問題数数が多すぎます。"), _T("警告"), MB_OK);
					break;//ファイルが多すぎ
				}
				auto& problems_ = const_cast<std::vector<Problem>&>(problems);
				try {
					problems_.emplace_back(problems_directory, fi.Name);
				}
				catch (Problem::init_error) {
					//読み込み失敗
					problems_.pop_back();
				}
			}
		}
	} while (DxLib::FileRead_findNext(hFind, &fi) == 0);
	if (problems.empty()) {
		throw std::exception("no problem is found");
	}
	problems_text.resize(problems.size());

	InitBuildProblemText();

	const_cast<dxle::tstring&>(user_name) = std::move(user_name_);
}

void Data::update()
{
	BuildProblemText();
	update_ScoresSet();
}

void Data::InitBuildProblemText()
{
	font_normal   = DxLib::CreateFontToHandle(_T("MS UI Gothic"), 20, 1);
	font_boldface = DxLib::CreateFontToHandle(_T("MS UI Gothic"), 20, 9);//太字
	font_h1       = DxLib::CreateFontToHandle(_T("MS UI Gothic"), 40, 9);//大文字、太字

	ClearProblemsCash();
}
void Data::ClearProblemsCash()
{
	for (auto& i : problems_text) {
		i.delete_this();
	}
	load_state = Load_State::file_open;
	text_total_size = 0;
	now_loding_problem = 0;
	problem_file.close();
	problem_text_next_start_pos.x = problem_text_next_start_pos.y = 0;
	problem_text_total_size.width = problem_text_total_size.height = 0;
	problem_script.clear();
}

void Data::BuildProblemText()
{
	const int load_time = 10;//ms
	auto start_time = DxLib::GetNowCount();
	do
	{
		switch (load_state)
		{
		case Data::Load_State::file_open:{
			problem_script.clear();
			text_total_size = 0;
			problem_text_next_start_pos.x = linestart_space; problem_text_next_start_pos.y = 0;
			problem_text_total_size.width = problem_text_total_size.height = 0;
			problem_text_newlinw_start_y = 0;
			problem_file.open(problems_directory + problems[now_loding_problem].GetName() + _T("/Statement.txt"));
			if (problem_file.fail() || problem_file.eof()) {
				//読み込みエラー
				problem_script.emplace_back(_T("問題を読み込めませんでした。"));
				problem_script.emplace_back(1, _T('\n'));
				problem_script.emplace_back(_T("F5で再読み込みできます。"));
				load_state = Load_State::size_checking;
				problem_script_iter = problem_script.begin();
				break;
			}
			load_state = Load_State::loading;
			//初回読み込み処理（先頭に@をつけない）
			dxle::tstring str_temp;
			std::getline(problem_file, str_temp, _T('@'));//'@'がでるまで読み込む
			//行分割
			for (auto iter_end = str_temp.end(), before_iter = str_temp.begin(), iter = std::find(before_iter, iter_end, _T('\n'));
				iter != iter_end; before_iter = iter+1, iter = std::find(before_iter, iter_end, _T('\n'))) {

				problem_script.emplace_back(before_iter, iter);
				problem_script.emplace_back(1, _T('\n'));
			}
		}
			break;
		case Data::Load_State::loading:
			if (problem_file.eof()) {
				problem_file.close();
				if (problem_script.empty()) {
					problem_script.emplace_back(_T("問題を読み込めませんでした。"));
					problem_script.emplace_back(1, _T('\n'));
					problem_script.emplace_back(_T("F5で再読み込みできます。"));
				}
				load_state = Load_State::size_checking;
				problem_script_iter = problem_script.begin();
				break;
			}
			//1区画(@から@まで)を読み込む
			{
				dxle::tstring temp_str;
				std::getline(problem_file, temp_str, _T('@'));//'@'がでるまで読み込む
				temp_str.insert(temp_str.begin(), _T('@'));//読み飛ばした@を追加
				//行分割
				for (auto iter_end = temp_str.end(), before_iter = temp_str.begin(), iter = std::find(before_iter, iter_end, _T('\n'));
					iter != iter_end; before_iter = iter+1, iter = std::find(before_iter, iter_end, _T('\n'))) {

					problem_script.emplace_back(before_iter, iter);
					problem_script.emplace_back(1, _T('\n'));
				}
			}
			break;
		case Data::Load_State::size_checking: {
			if (problem_script_iter == problem_script.end()) {
				//サイズ更新
				problem_text_total_size.width = std::max(problem_text_total_size.width, problem_text_next_start_pos.x);
				problem_text_total_size.height = problem_text_newlinw_start_y;
				//キャッシュサイズ計算
				text_total_size += problem_text_total_size.width * problem_text_total_size.height;
				if (false) {
					DEBUG_NOTE;//キャッシュサイズが規定値を超えた場合の動作
				}
				//描画位置巻き戻し
				problem_text_next_start_pos.x = linestart_space;
				problem_text_next_start_pos.y = problem_text_newlinw_start_y = 0;
				//描画に遷移
				load_state = Load_State::drawing;
				problems_text[now_loding_problem] = dxle::MakeScreen(problem_text_total_size);
				problems_text[now_loding_problem].draw_on_this([&problem_text_total_size = problem_text_total_size]() {
					DxLib::SetDrawAreaFull();
					DxLib::DrawFillBox(0, 0, problem_text_total_size.width, problem_text_total_size.height
						, dxle::dx_color(dxle::color_tag::white).get());//@todo dxlibex
				});
				problem_script_iter = problem_script.begin();
				break;
			}

			//スクリプトを一行解析

			DEBUG_NOTE;//スクリプト完全無視
			//クラス形式で書き直したほうが良いと思う
			
			if(problem_script_iter->empty()){}
			else if (false)
			{
				//画像描画
			}
			else if (problem_script_iter->front() == _T('\n'))
			{
				//改行

				problem_text_total_size.width = std::max(problem_text_total_size.width, problem_text_next_start_pos.x);

				//復帰
				problem_text_next_start_pos.x = linestart_space;
				if (problem_text_next_start_pos.y == problem_text_newlinw_start_y) {
					problem_text_newlinw_start_y += 22;
					problem_text_next_start_pos.y = problem_text_newlinw_start_y;
				}
				else {
					problem_text_next_start_pos.y = problem_text_newlinw_start_y;
				}
			}
			else
			{
				//文字描画
				int font = font_normal;

				//サイズ取得
				dxle::sizei temp_size;
				DxLib::GetDrawStringSizeToHandle(&temp_size.width, &temp_size.height, nullptr,
					problem_script_iter->c_str(), problem_script_iter->size(), font);//@todo dxlibex
				//サイズ更新
				problem_text_next_start_pos.x += temp_size.width;
				problem_text_newlinw_start_y = std::max(problem_text_newlinw_start_y, problem_text_next_start_pos.y + temp_size.height);
			}

			++problem_script_iter;
		}
			break;
		case Data::Load_State::drawing: {
			if (problem_script_iter == problem_script.end())
			{
				//次のロードに
				const auto problem_num = problems.size();
				//まだ、読み込んでないのを全探査
				int b_index = viewing_problem;//デクリメントしていく
				int a_index = viewing_problem;//インクリメントしていく
				for(;;)
				{
					++a_index;
					a_index %= problem_num;
					if (problems_text[a_index].valid() == false) {
						now_loding_problem = a_index; break;
					}
					if (b_index == a_index) { break; }

					--b_index;
					if (b_index < 0) { b_index += problem_num; }
					if (problems_text[b_index].valid() == false) {
						now_loding_problem = b_index; break;
					}
					if (b_index == a_index) { break; }
				}
				if (b_index != a_index) {
					//新規読み込みに遷移
					load_state = Load_State::file_open;
				}
				else {
					//読み込み終了
					load_state = Load_State::end;
				}
				break;
			}

			//スクリプトを一行解析

			DEBUG_NOTE;//スクリプト完全無視

			if (problem_script_iter->empty()) {}
			else if (false)
			{
				//画像描画
			}
			else if (problem_script_iter->front() == _T('\n'))
			{
				//改行 //復帰
				problem_text_next_start_pos.x = linestart_space;
				if (problem_text_next_start_pos.y == problem_text_newlinw_start_y) {
					problem_text_newlinw_start_y += 22;
					problem_text_next_start_pos.y = problem_text_newlinw_start_y;
				}
				else {
					problem_text_next_start_pos.y = problem_text_newlinw_start_y;
				}
			}
			else
			{
				//文字描画
				int font = font_normal;
				dxle::dx_color color = dxle::color_tag::black;

				//サイズ取得
				dxle::sizei temp_size;
				DxLib::GetDrawStringSizeToHandle(&temp_size.width, &temp_size.height, nullptr,
					problem_script_iter->c_str(), problem_script_iter->size(), font);//@todo dxlibex
				//描画
				problems_text[now_loding_problem].draw_on_this([this,color,font]() {
					DxLib::SetDrawAreaFull();
					DxLib::DrawStringToHandle(problem_text_next_start_pos.x, problem_text_next_start_pos.y,
						problem_script_iter->c_str(), color.get(), font);
				});
				//サイズ更新
				problem_text_next_start_pos.x += temp_size.width;
				problem_text_newlinw_start_y = std::max(problem_text_newlinw_start_y, problem_text_next_start_pos.y + temp_size.height);
			}


			++problem_script_iter;
		}
			break;
		case Data::Load_State::end:
			return;//読み込み終了
		default:
			assert(false);
			return;//読み込み中断
		}

		//一定時間たったら抜ける
	} while ((DxLib::GetNowCount() - start_time) < load_time);
}
Data::Data()
	: load_state(Load_State::end)
{
}
Data::~Data()
{
	DeleteFontToHandle(font_normal);
	DeleteFontToHandle(font_boldface);
	DeleteFontToHandle(font_h1);
}
void Data::SetBuildProblemText(size_t index)
{
	DEBUG_NOTE;
	viewing_problem = index;
}

//@param path:末尾に\又は/
Problem::Problem(dxle::tstring path, const TCHAR* pronlem_name)
	:name(pronlem_name)
{
	path += name;
	path += '/';
	//部分点情報取得
	tifstream ifs(path + _T("partial_scores.txt"));
	if (ifs.fail()) {
		MessageBox(GetMainWindowHandle(), (_T("問題["_ts) + name + _T("]のpartial_scores.txtがないため、問題を読み込めません")).c_str(), _T("警告"), MB_OK);
		throw init_error{};//読み込み失敗
	}
	while (!ifs.eof())
	{
		auto& partial_scores_ = const_cast<std::vector<std::pair<int, size_t>>&>(partial_scores);
		partial_scores_.emplace_back();
		ifs >> partial_scores_.back().first >> partial_scores_.back().second;
		//partial_scores.back().second -= 1;//番号->index
		ifs.ignore(-1, '\n');
		if (2 <= partial_scores.size()) {
			auto i = partial_scores.rbegin();
			if (i->second <= (i + 1)->second) {
				MessageBox(GetMainWindowHandle(), (_T("問題["_ts) + name + _T("]のpartial_scores.txtの書式が間違っています\n入力の指定は昇順にしてください。")).c_str(), _T("警告"), MB_OK);
				partial_scores_.pop_back();
				continue;
			}
		}
		const_cast<int&>(max_score) += partial_scores.back().first;
	}
				
	const_cast<uint32_t&>(sample_num) = get_numfile_num(path + _T("sample_in"), _T(".txt"), 1);
	if(sample_num == (uint32_t)(-1)){
		const_cast<uint32_t&>(sample_num) = 0;
	}
}

void Problem::AddScores(Scores && new_data)
{
	if (new_data.get_type() != Scores::Type_T::normal) {
		scores_set.emplace_back(std::move(new_data));
		return;
	}
	//得点の更新
	int temp_score = 0;
	size_t i = 0;
	auto& new_raw_scores = new_data.get_scores();
	for (auto& partial : partial_scores) {
		size_t end_n = std::min(new_raw_scores.size(), partial.second);
		if (std::all_of(new_raw_scores.begin() + i, new_raw_scores.begin() + end_n,
			[](const Score& s) { return s.type == Score::Type_T::AC; })) {
			temp_score += partial.first;
		}
		i = end_n;
	}
	if (my_socre < temp_score) {
		my_socre = temp_score;
	}
	//scoreの登録
	scores_set.emplace_back(std::move(new_data));
}

void Data::update_ScoresSet()
{
	std::lock_guard<std::mutex> lock(new_scores_mtx);
	for (auto& i : new_scores)
	{
		(*this)[i.first].AddScores(std::move(i.second));
	}
	new_scores.clear();
}

void Data::AddScoresSet_threadsafe(size_t problem_num, Scores param_new_scores)
{
	std::lock_guard<std::mutex> lock(new_scores_mtx);
	new_scores.emplace_back(problem_num, std::move(param_new_scores));
}
