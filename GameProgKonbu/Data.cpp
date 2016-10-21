
#include "Data.h"
#include "other_usefuls.h"

std::mutex Data::new_scores_mtx;
std::vector<std::pair<size_t, Scores>> Data::new_scores;//FIFO (first: pop, last: push)

void Data::InitProblem(dxle::tstring path)
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
	problems_text.clear();
	load_state = Load_State::file_open;
	text_total_size = 0;
	viewing_problem = 0;
	now_loding_problem = 0;
	problem_file.close();
	problem_text_temp_end_pos.x = problem_text_temp_end_pos.y = 0;
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
			problem_file.open(problems_directory + problems[now_loding_problem].GetName() + _T("/Statement.txt"));
			if (problem_file.fail()) {
				//読み込みエラー
				problem_script.emplace_back(_T("問題を読み込めませんでした。"));
				problem_script.emplace_back(_T('\n'));
				problem_script.emplace_back(_T("F5で再読み込みできます。"));
				load_state = Load_State::size_checking;
				problem_script_iter = problem_script.begin();
				break;
			}
			//ファイルチェック
			if (problem_file.eof()) {
				problem_file.close();
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
				iter != iter_end; before_iter = iter, iter = std::find(before_iter, iter_end, _T('\n'))) {

				problem_script.emplace_back(before_iter, iter);
				problem_script.emplace_back(_T('\n'));
			}
			if (problem_file.eof()) {
				problem_file.close();
				load_state = Load_State::size_checking;
				problem_script_iter = problem_script.begin();
			}
		}
			break;
		case Data::Load_State::loading:
			//1区画(@から@まで)を読み込む
			{
				dxle::tstring temp_str;
				std::getline(problem_file, temp_str, _T('@'));//'@'がでるまで読み込む
				temp_str.insert(temp_str.begin(), _T('@'));//読み飛ばした@を追加
				//行分割
				for (auto iter_end = temp_str.end(), before_iter = temp_str.begin(), iter = std::find(before_iter, iter_end, _T('\n'));
					iter != iter_end; before_iter = iter, iter = std::find(before_iter, iter_end, _T('\n'))) {

					problem_script.emplace_back(before_iter, iter);
					problem_script.emplace_back(_T('\n'));
				}
			}
			if (problem_file.eof()) {
				problem_file.close();
				load_state = Load_State::size_checking;
				problem_script_iter = problem_script.begin();
				break;
			}
			break;
		case Data::Load_State::size_checking: {
			//スクリプトを一行解析

			DEBUG_NOTE;//スクリプト完全無視
			
			if (false)
			{
				//画像描画
			}
			else if (problem_script_iter->front() == _T('\n'))
			{
				//改行

				problem_text_total_size.width = std::max(problem_text_total_size.width, problem_text_next_start_pos.x);

				//復帰
				problem_text_next_start_pos.x = 0;
				problem_text_next_start_pos.y = problem_text_newlinw_start_y;
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

				++problem_script_iter;
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
					problem_text_next_start_pos.x = 0;
					problem_text_next_start_pos.y = problem_text_newlinw_start_y = 0;
					//描画に遷移
					load_state = Load_State::drawing;
					problems_text[now_loding_problem] = dxle::MakeScreen(problem_text_total_size);
					problem_script_iter = problem_script.begin();
				}
#if 0
				下のは前の;
				dxle::sizei temp_size;
				auto enter_index = problem_script_iter->find_first_of(_T('\n'));
				if (enter_index == dxle::tstring::npos) { enter_index = problem_script_iter->size(); }
				//追記処理
				if (problem_text_next_start_pos.x != 0) {
					//改行前までの文字を作る
					FINALLY(([enter_index, problem_script_iter = problem_script_iter]() {
						(*problem_script_iter)[enter_index] = _T('\n');
					}));
					(*problem_script_iter)[enter_index] = _T('\0');

					DxLib::GetDrawStringSizeToHandle(&temp_size.width, &temp_size.height, nullptr,
						problem_script_iter->c_str(), enter_index, font);//@todo dxlibex

					problem_text_next_start_pos.x += temp_size.width;
					problem_text_total_size.width = std::max(problem_text_total_size.width, problem_text_next_start_pos.x);
					problem_text_total_size.height = problem_text_newlinw_start_y =
						std::max(problem_text_newlinw_start_y, problem_text_next_start_pos.y + temp_size.height);
				}
				ここから書いてない;
				改行でも分割すべきかもしれない;
				//サイズチェック
				dxle::sizei temp_size;
				DxLib::GetDrawStringSizeToHandle(&temp_size.width, &temp_size.height, nullptr,
					problem_script_iter->c_str(), problem_script_iter->size(), font_normal);//@todo dxlibex
				//描画画面拡張処理
				if (problem_text_total_size.width < temp_size.width || ) {
					DEBUG_NOTE;
				}
#endif
			}
		}
			break;
		case Data::Load_State::drawing: {
			//スクリプトを一行解析

			DEBUG_NOTE;//スクリプト完全無視

			if (false)
			{
				//画像描画
			}
			else if (problem_script_iter->front() == _T('\n'))
			{
				//改行 //復帰
				problem_text_next_start_pos.x = 0;
				problem_text_next_start_pos.y = problem_text_newlinw_start_y;
			}
			else
			{
				//文字描画
				int font = font_normal;
				dxle::dx_color color;

				//サイズ取得
				dxle::sizei temp_size;
				DxLib::GetDrawStringSizeToHandle(&temp_size.width, &temp_size.height, nullptr,
					problem_script_iter->c_str(), problem_script_iter->size(), font);//@todo dxlibex
				//描画
				DxLib::DrawStringToHandle(problem_text_next_start_pos.x, problem_text_next_start_pos.y,
					problem_script_iter->c_str(), color.get(), font);
				//サイズ更新
				problem_text_next_start_pos.x += temp_size.width;
				problem_text_newlinw_start_y = std::max(problem_text_newlinw_start_y, problem_text_next_start_pos.y + temp_size.height);

				++problem_script_iter;
				if (problem_script_iter == problem_script.end()) {
					//次のロードに
					次のロードに;
					const auto problem_num = problems.size();
					if (problem_num == 1) {
						load_state = Load_State::end;
					}
					//まだ、読み込んでないのを全探査
					int b_index = viewing_problem;//デクリメントしていく
					int a_index = (viewing_problem+1) % problem_num;//インクリメントしていく
					while (b_index != a_index)
					{

					}

				}
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
Data::~Data()
{
	DeleteFontToHandle(font_normal);
	DeleteFontToHandle(font_boldface);
	DeleteFontToHandle(font_h1);
}
void Data::SetBuildProblemText(size_t index)
{
	DEBUG_NOTE;
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
