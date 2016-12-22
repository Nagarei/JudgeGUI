
#include "Data.h"
#include "my_utility.h"
#include "GetNumfileNum.h"
#include "Submission.h"
#include "popup.h"
#include "WaitJudge.h"

namespace {
}

void Data::DeleteProblem()
{
	WaitJudgeQueue::clear_all();
	++problem_set_num;
}
void Data::InitProblem(dxle::tstring problems_directory_, dxle::tstring log_directory_, dxle::tstring user_name_, bool is_contest_mode_)
{
	user_name = std::move(user_name_);
	is_contest_mode = std::move(is_contest_mode_);

	if (problems_directory_.empty()) {
		problems_directory_ = _T("./");
	}
	else if (problems_directory_.back() != '/' && problems_directory_.back() != '\\'){
		problems_directory_.push_back('/');
	}
	{//絶対パスに
		TCHAR buf[MAX_PATH * 3];
		GetFullPathName(problems_directory_.c_str(), sizeof(buf) / sizeof(buf[0]), buf, nullptr);
		problems_directory_ = buf;
	}
	problems_directory = std::move(problems_directory_);

	if (log_directory_.empty()) {
		log_directory_ = _T("./");
	}
	else if (log_directory_.back() != '/' && log_directory_.back() != '\\'){
		log_directory_.push_back('/');
	}
	{//絶対パスに
		TCHAR buf[MAX_PATH * 3];
		GetFullPathName(log_directory_.c_str(), sizeof(buf) / sizeof(buf[0]), buf, nullptr);
		log_directory_ = buf;
	}
	log_directory = std::move(log_directory_);

	DxLib::FILEINFO fi;
	DWORD_PTR hFind = (DWORD_PTR)-1;
	FINALLY([&]() {if (hFind != -1) { FileRead_findClose(hFind); }});

	hFind = DxLib::FileRead_findFirst((problems_directory + _T('*')).c_str(), &fi);
	problems.clear();
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
				problems.emplace_back(fi.Name);
			}
		}
	} while (DxLib::FileRead_findNext(hFind, &fi) == 0);
	if (problems.empty()) {
		throw std::exception("no problem is found");
	}
	problems_text.resize(problems.size());

	InitBuildProblemText();

	ReloadSubmission();//提出データの初回読み込み
}

void Data::update()
{
	BuildProblemText();
}

void Data::InitBuildProblemText()
{
	//font_boldface = DxLib::CreateFontToHandle(_T("ＭＳ ゴシック"), 20, 9);//太字
	//font_h1       = DxLib::CreateFontToHandle(_T("ＭＳ ゴシック"), 40, 9);//大文字、太字
	ClearProblemsCache();
}
void Data::ClearProblemsCache()
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
			problem_file.open(problems_directory + problems[now_loding_problem].GetName() + _T("/Statement.txt"), std::ios::in | std::ios::binary);
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
				if (problem_file.fail()) {
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
					script_raw_temp.clear();
					std::getline(problem_file, script_raw_temp, _T('@'));//'@'がでるまで読み込む
					if (!problem_file.fail()) {
						script_raw_temp.insert(script_raw_temp.begin(), _T('@'));//@追加
					}
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
void Data::ReloadSubmission()
{
	//すべての問題のデータをロード
	for (auto& prob : const_cast<std::vector<Problem>&>(problems))
	{
		prob.ReloadSubmission();
	}
}
Data::Data()
	: is_contest_mode(false)
	, load_state(Load_State::end)
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
Problem::Problem(const TCHAR* pronlem_name)
	:name(pronlem_name)
{
	ReloadPartialScores();
	const_cast<uint32_t&>(sample_num) = get_numfile_num(Data::GetIns().GetProblemsDirectory() + name + _T("/sample_in"), _T(".txt"), 1);
	if(sample_num == (uint32_t)(-1)){
		const_cast<uint32_t&>(sample_num) = 0;
	}
}

void Problem::AddSubmission(const Submission_Core& new_data)
{
	my_socre = std::max(my_socre, GetScore_single(new_data));
}

int32_t Problem::GetScore_single(const Submission_Core& data) const
{
	if (data.get_type() != Submission_Core::Type_T::normal) {
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
void Problem::ReloadSubmission()
{
	ClearSubmissionCache();

	//自分のデータをロード
	dxle::tstring user_name = Data::GetIns().get_user_name();
	dxle::tstring problem_user_directory =
		Data::GetIns().GetLogRootDirectory() +
		this->GetName() + _T('/') +
		user_name + _T('/');
	auto log_num = get_numdirectry_num(problem_user_directory, _T(""), 0);
	if (log_num == (uint32_t)(-1)) { return; }
	dxle::tstring prb_dir;
	prb_dir.reserve(problem_user_directory.size() + 20);
	for (uint32_t i = 0; i <= log_num; ++i)
	{
		this->AddSubmission(BuildScores(prb_dir));
	}
}
void Problem::ReloadPartialScores()
{
	//部分点情報取得
	max_score = 0;
	partial_scores.resize(0);
	tifstream ifs(Data::GetIns().GetProblemsDirectory() + name + _T("/partial_scores.txt"));
	if (ifs.fail()) {
		MessageBox(GetMainWindowHandle(), (_T("問題["_ts) + name + _T("]のpartial_scores.txtがないため、問題を読み込めません")).c_str(), _T("警告"), MB_OK);
		partial_scores.emplace_back(0,0);
		return;
	}
	while (!ifs.fail())
	{
		partial_scores.emplace_back();
		ifs >> partial_scores.back().first >> partial_scores.back().second;
		//partial_scores.back().second -= 1;//番号->index
		ifs.ignore(-1, '\n');
		if (ifs.fail() && (partial_scores.back().first == 0 && partial_scores.back().second == 0)) {
			//読み込み終了
			partial_scores.pop_back();
			break;
		}
		if (2 <= partial_scores.size()) {
			auto i = partial_scores.rbegin();
			if (i->second <= (i + 1)->second) {
				MessageBox(GetMainWindowHandle(), (_T("問題["_ts) + name + _T("]のpartial_scores.txtの書式が間違っています\n入力の指定は昇順にしてください。")).c_str(), _T("警告"), MB_OK);
				partial_scores.pop_back();
				continue;
			}
		}
		max_score += partial_scores.back().first;
	}
	//自分の点数更新
	ReloadSubmission();
}
std::vector<Submission_old> Problem::LoadSubmissionAll()const
{
	std::vector<Submission_old> submissions;
	auto problem_directory = Data::GetIns().GetLogRootDirectory() + this->GetName() + _T('/');
	dxle::tstring problem_user_directory;

	//すべてのユーザーのデータをロード
	DxLib::FILEINFO fi;
	DWORD_PTR hFind = (DWORD_PTR)-1;
	FINALLY([&]() {if (hFind != -1) { FileRead_findClose(hFind); }});
	hFind = DxLib::FileRead_findFirst((problem_directory + _T('*')).c_str(), &fi);
	if (hFind == -1) {  }
	else
	{
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
						auto submission_dir = problem_user_directory + my_itoa(i, buf) + _T('/');
						submissions.emplace_back(
							BuildScores(submission_dir),
							submission_dir + get_source_filename(),
							fi.Name
						);
					}
				};
			}
		} while (DxLib::FileRead_findNext(hFind, &fi) == 0);
	}
	return submissions;
}

std::pair<Submission_old::Type_T, Score::Type_T> get_result_type(const Submission_old& scores)
{
	std::pair<Submission_old::Type_T, Score::Type_T> result{ scores.get_type() , Score::Type_T::AC };
	if (result.first == Submission_old::Type_T::normal) {
		//secondの決定
		auto iter = std::find_if_not(scores.get_scores().begin(), scores.get_scores().end()
			, [](const Score& s) {return s.type == Score::Type_T::AC; });
		if (iter == scores.get_scores().end()) {
			//AC
		}
		else {
			//AC以外
			result.second = iter->type;
		}
	}
	return result;
}
std::pair<std::array<TCHAR, 10>, dxle::rgb> get_result_type_fordraw(const Score& score)
{
	std::array<TCHAR, 10> str;
	dxle::rgb color;
#define SET_grts_(message) DxLib::strcpy_sDx(str.data(), str.size(), _T(#message))
	switch (score.type)
	{
	case Score::Type_T::AC:
		SET_grts_(AC);
		color = dxle::rgb{ 70,136,71 };
		break;
	case Score::Type_T::WA:
		SET_grts_(WA);
		color = dxle::rgb{ 248,148,6 };
		break;
	case Score::Type_T::TLE:
		SET_grts_(TLE);
		color = dxle::rgb{ 248,148,6 };
		break;
	case Score::Type_T::MLE:
		SET_grts_(MLE);
		color = dxle::rgb{ 248,148,6 };
		break;
	case Score::Type_T::RE:
		SET_grts_(RE);
		color = dxle::rgb{ 248,148,6 };
		break;
	default:
		assert(false);
		SET_grts_(IE);
		color = dxle::color_tag::white;
		break;
	}
	return{ str,color };
#undef SET_grts_
}

std::pair<std::array<TCHAR, 10>, dxle::rgb> get_result_type_fordraw(const Submission_Core& scores)
{
	std::array<TCHAR, 10> str;
	dxle::rgb color;
#define SET_grts_(message) DxLib::strcpy_sDx(str.data(), str.size(), _T(#message))
	switch (scores.get_type())
	{
	case Submission_old::Type_T::normal: {
		auto iter = std::find_if_not(scores.get_scores().begin(), scores.get_scores().end()
			, [](const Score& s) {return s.type == Score::Type_T::AC; });
		if (iter == scores.get_scores().end()) {
			SET_grts_(AC);
			color = dxle::rgb{70,136,71};
		}
		else {
			//再帰じゃなくてScoreのオーバーロードを呼び出す
			return get_result_type_fordraw(*iter);
		}
	}
		break;
	case Submission_old::Type_T::CE:
		SET_grts_(CE);
		color = dxle::color_tag::red;
		break;
	case Submission_old::Type_T::IE:
		SET_grts_(IE);
		color = dxle::color_tag::white;
		break;
	case Submission_old::Type_T::WJ:
		SET_grts_(WJ);
		color = dxle::color_tag::gray;
		break;
	default:
		assert(false);
		SET_grts_(IE);
		color = dxle::color_tag::white;
		break;
	}
	return{ str,color };
#undef SET_grts_
}

Submission_old Submission_old::MakeWJ(time_t time)
{
	return Submission_old{
		Submission_Core{Submission_Core::Type_T::WJ, {}, time},
		{}, Data::GetIns().get_user_name()
	};
}

DxLib::DATEDATA Submission_old::get_submit_time() const
{
	tm local_time;
	auto time = core.get_submit_time();
	localtime_s(&local_time, &time);
	DxLib::DATEDATA dx_date;
	dx_date.Year = local_time.tm_year + 1900;
	dx_date.Mon  = local_time.tm_mon + 1;
	dx_date.Day  = local_time.tm_mday;
	dx_date.Hour = local_time.tm_hour;
	dx_date.Min  = local_time.tm_min;
	dx_date.Sec  = local_time.tm_sec;
	return dx_date;
}
