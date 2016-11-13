
#include "Data.h"
#include "other_usefuls.h"
#include "GetNumfileNum.h"
#include "SubmissionLog.h"
#include "popup.h"

std::mutex Data::new_scores_mtx;
std::vector<std::pair<size_t, Scores>> Data::new_scores;//FIFO (first: pop, last: push)
namespace {
}

void Data::InitProblem(dxle::tstring problems_directory_, dxle::tstring log_directory_, dxle::tstring user_name_)
{
#ifdef _DEBUG
	//初回しか呼ばれないので、problemsのスレッドセーフを保証できる
	//下手にいじらないこと
	static bool is_finish = false;
	assert(is_finish == false);
	is_finish = true;
#endif

	const_cast<dxle::tstring&>(user_name) = std::move(user_name_);

	if (!problems_directory_.empty() && problems_directory_.back() != '/' && problems_directory_.back() != '\\'){
		problems_directory_.push_back('/');
	}
	problems_directory_ += _T("Problems/");
	{//絶対パスに
		TCHAR buf[MAX_PATH * 3];
		GetFullPathName(problems_directory_.c_str(), sizeof(buf) / sizeof(buf[0]), buf, nullptr);
		problems_directory_ = buf;
	}
	const_cast<dxle::tstring&>(problems_directory) = std::move(problems_directory_);

	if (!log_directory_.empty() && log_directory_.back() != '/' && log_directory_.back() != '\\'){
		log_directory_.push_back('/');
	}
	log_directory_ += _T("Problems/");
	{//絶対パスに
		TCHAR buf[MAX_PATH * 3];
		GetFullPathName(log_directory_.c_str(), sizeof(buf) / sizeof(buf[0]), buf, nullptr);
		log_directory_ = buf;
	}
	const_cast<dxle::tstring&>(log_directory) = std::move(log_directory_);

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

	LoadSubmissionAll();//提出データの初回読み込み
}

void Data::update()
{
	BuildProblemText();
	update_ScoresSet();
}

void Data::InitBuildProblemText()
{
	//font_boldface = DxLib::CreateFontToHandle(_T("MS UI Gothic"), 20, 9);//太字
	//font_h1       = DxLib::CreateFontToHandle(_T("MS UI Gothic"), 40, 9);//大文字、太字

	ClearProblemsCash();
}
void Data::ClearProblemsCash()
{
	for (auto& i : problems_text) {
		i.size.width = i.size.height = 0;
		i.script.clear();
	}
	load_state = Load_State::end;
}

void Data::BuildProblemText()
{
	if (problems.empty()) { return; }
	const int load_time = 10;//ms
	auto start_time = DxLib::GetNowCount();
	do
	{
		switch (load_state)
		{
		case Data::Load_State::file_open: {
			problem_file.close();
			problem_text_next_start_pos.x = Script::linestart_space; problem_text_next_start_pos.y = 0;
			problem_text_total_size.width = problem_text_total_size.height = 0;
			problem_script_temp.clear();
			script_raw_temp.clear();
			problem_file.open(problems_directory + problems[now_loding_problem].GetName() + _T("/Statement.txt"));
			load_state = Load_State::loading;
			//初回読み込み処理（先頭に@をつけない）
			std::getline(problem_file, script_raw_temp, _T('@'));//'@'がでるまで読み込む
		}
			break;
		case Data::Load_State::loading:
			if(script_raw_temp.empty() == false) {
				//スクリプト解析
				problem_script_temp.emplace_back(
					Script::build_script(script_raw_temp)
				);
				//大きさチェック
				Script::advance_script(problem_script_temp.back(), problem_text_next_start_pos,
					problem_text_total_size.height, 1.0, &problem_text_total_size.width, nullptr);
			}
			else {
				//スクリプト読み込み
				if (problem_file.fail() || problem_file.eof()) {
					if (problem_script_temp.empty()) {
						script_raw_temp = _T("問題を読み込めませんでした。\nF5で再読み込みできます。");//ダミー登録
					}
					else{
						//読み込み終了
						problem_text_total_size.width = std::max(problem_text_total_size.width, problem_text_next_start_pos.x);
						problems_text[now_loding_problem].script = std::move(problem_script_temp);
						problems_text[now_loding_problem].size = std::move(problem_text_total_size);
						load_state = Load_State::end;
						break;
					}
				}
				else
				{
					std::getline(problem_file, script_raw_temp, _T('@'));//'@'がでるまで読み込む
					script_raw_temp.insert(script_raw_temp.begin(), _T('@'));//@追加
				}
			}
			break;
		case Data::Load_State::end: {
			//次読み込むのを探す
			//次のロードに
			const auto problem_num = problems.size();
			//まだ、読み込んでないのを全探査
			int b_index = viewing_problem;//デクリメントしていく
			int a_index = viewing_problem-1;//インクリメントしていく
			uint32_t find_count = 0;
			for (; find_count < loading_max_num_div2; ++find_count)
			{
				++a_index;
				a_index %= problem_num;
				if (IsLoadFinesed(a_index) == false) {
					now_loding_problem = a_index; break;
				}

				--b_index;
				if (b_index < 0) { b_index += problem_num; }
				if (IsLoadFinesed(b_index) == false) {
					now_loding_problem = b_index; break;
				}
			}
			if (find_count < loading_max_num_div2) {
				//新規読み込みに遷移
				load_state = Load_State::file_open;
			}
		}
			break;
		default:
			assert(false);
			return;//読み込み中断
		}

		//一定時間たったら抜ける
	} while ((DxLib::GetNowCount() - start_time) < load_time);
}
void Data::LoadSubmissionAll()
{
	dxle::tstring problem_user_directory;
	dxle::tstring problem_directory;
	//すべての問題のデータをロード
	for (auto& prob : const_cast<std::vector<Problem>&>(problems))
	{
		problem_directory = log_directory + prob.GetName() + _T('/');

		//すべてのユーザーのデータをロード
		DxLib::FILEINFO fi;
		DWORD_PTR hFind = (DWORD_PTR)-1;
		FINALLY([&]() {if (hFind != -1) { FileRead_findClose(hFind); }});
		hFind = DxLib::FileRead_findFirst((problem_directory + _T('*')).c_str(), &fi);
		if (hFind == -1) { continue; }
		do
		{
			if (fi.DirFlag == TRUE)
			{
				if (fi.Name[0] != '.')
				{
					//ユーザー名fi.Nameのユーザーの記録を取得
					problem_user_directory = problem_directory + fi.Name + _T('/');
					auto log_num = get_numdirectry_num(problem_user_directory, _T(""), 0);
					if (log_num == (uint32_t)(-1)) { continue; }
					TCHAR buf[20];
					for (uint32_t i = 0; i <= log_num; ++i)
					{
						prob.AddScores(BuildScores(problem_user_directory + my_itoa(i, buf) + _T('/'), fi.Name));
					}
				}
			}
		} while (DxLib::FileRead_findNext(hFind, &fi) == 0);
	}
}
Data::Data()
	: load_state(Load_State::end)
{
}
Data::~Data()
{}
void Data::SetBuildProblemText(size_t index)
{
	viewing_problem = index;
}
void Data::DrawExtendProblem(int index, const dxle::pointi32& pos, double extend_rate)const
{
	const auto& script = problems_text[index].script;
	assert(!script.empty());
	uint32_t next_line_start = 0;
	dxle::pointui32 next_start_pos{ Script::linestart_space, next_line_start };
	for (auto& scr : script)
	{
		//描画&サイズ計算
		Script::advance_script(scr, next_start_pos, next_line_start, extend_rate, nullptr, &pos);
	}
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
	scores_set.emplace_back(std::move(new_data));
	my_socre = std::max(my_socre, GetScore_single(scores_set.size() - 1));
}

int32_t Problem::GetScore_single(size_t scores_set_index) const
{
	const auto& data = scores_set[scores_set_index];
	if (data.get_type() != Scores::Type_T::normal) {
		return 0;
	}
	int32_t temp_score = 0;
	size_t i = 0;
	auto& new_raw_scores = data.get_scores();
	for (auto& partial : partial_scores) {
		size_t end_n = std::min(new_raw_scores.size(), partial.second);
		if (std::all_of(new_raw_scores.begin() + i, new_raw_scores.begin() + end_n,
			[](const Score& s) { return s.type == Score::Type_T::AC; })) {
			temp_score += partial.first;
		}
		i = end_n;
	}

	return temp_score;
}

void Data::update_ScoresSet()
{
	std::lock_guard<std::mutex> lock(new_scores_mtx);
	for (auto& i : new_scores)
	{
		auto type_str = get_result_type_str(i.second);
		popup::set(_T("結果が出ました："_ts) + type_str.data(),
			dxle::tstring(type_str.data()) == _T("AC"_ts) ? dxle::color_tag::green : dxle::color_tag::magenta);
		(*this)[i.first].AddScores(std::move(i.second));
	}
	new_scores.clear();
}

void Data::AddScoresSet_threadsafe(size_t problem_num, Scores param_new_scores)
{
	std::lock_guard<std::mutex> lock(new_scores_mtx);
	new_scores.emplace_back(problem_num, std::move(param_new_scores));
}

std::array<TCHAR, 10> get_result_type_str(const Scores& scores)
{
	std::array<TCHAR, 10> str;
#define SET_grts_(message) DxLib::strcpy_sDx(str.data(), str.size(), _T(#message))
	switch (scores.get_type())
	{
	case Scores::Type_T::normal: {
		auto iter = std::find_if_not(scores.get_scores().begin(), scores.get_scores().end()
			, [](const Score& s) {return s.type == Score::Type_T::AC; });
		if (iter == scores.get_scores().end()) {
			SET_grts_(AC);
		}
		else {
			switch (iter->type)
			{
				break;
			case Score::Type_T::WA:
				SET_grts_(WA);
				break;
			case Score::Type_T::TLE:
				SET_grts_(TLE);
				break;
			case Score::Type_T::MLE:
				SET_grts_(MLE);
				break;
			case Score::Type_T::RE:
				SET_grts_(RE);
				break;
			case Score::Type_T::AC:
			default:
				assert(false);
				SET_grts_(IE);
				break;
			}
		}
	}
		break;
	case Scores::Type_T::CE:
		SET_grts_(CE);
		break;
	case Scores::Type_T::IE:
		SET_grts_(IE);
		break;
	default:
		assert(false);
		SET_grts_(IE);
		break;
	}
	return str;
#undef SET_grts_
}
