
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
	load_state = Load_State::file_open;

	DEBUG_NOTE; return;
}

void Data::BuildProblemText()
{
	const int load_time = 10;//ms
	auto start_time = DxLib::GetNowCount();
	do
	{
		switch (load_state)
		{
		case Data::Load_State::file_open:
			problem_file.open(problems_directory + problems[now_loding_problem].GetName() + _T("/Statement.txt"));
			if (problem_file.fail()) {
				//読み込みエラー
				const TCHAR*const str = _T("問題を読み込めませんでした。\nF5で再読み込みできます。");//仮テキスト
				DxLib::GetDrawStringSizeToHandle(&problem_text_total_size.width, &problem_text_total_size.height, nullptr, str, -1, font_normal);//@todo dxlibex
				problem_script.emplace_back(str);
				load_state = Load_State::drawing;
				break;
			}
			//初回読み込み
			if (problem_file.eof()) {
				problem_file.close();
				load_state = Load_State::drawing;
				break;
			}
			problem_script.emplace_back();
			std::getline(problem_file, problem_script.back(), _T('@'));//'@'がでるまで読み込む
			//fall
		case Data::Load_State::loading:
			//1区画(@から@まで)を読み込む
			if (problem_file.eof()) {
				problem_file.close();
				load_state = Load_State::drawing;
				break;
			}
			{
				dxle::tstring temp_str;
				std::getline(problem_file, temp_str, _T('@'));//'@'がでるまで読み込む
				problem_script.emplace_back(_T('@') + std::move(temp_str));
			}
			break;
		case Data::Load_State::drawing:
			DEBUG_NOTE;
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
