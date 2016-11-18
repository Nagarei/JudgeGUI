#pragma once
#include "useful_func_and_class.h"
#include "Script.h"

class Score final
{
public:
	uint32_t use_memory;
	uint32_t use_time;
	enum class Type_T{ AC,WA,TLE,MLE,RE }type;
	TCHAR input_name[20];//入力ファイルの名前
};
class Submission final
{
public:
	enum class Type_T { normal, CE, IE };
private:
	Type_T type;
	dxle::tstring source_name;
	std::vector<Score> scores;
	dxle::tstring user_name;
	DxLib::DATEDATA submit_time;
public:
	Submission() = default;
	Submission(Type_T type_, dxle::tstring source_name_, std::vector<Score> scores_,
		dxle::tstring user_name_, DxLib::DATEDATA submit_time_)
		:type(type_), source_name(std::move(source_name_)), scores(std::move(scores_)),
		 user_name(std::move(user_name_)), submit_time(std::move(submit_time_))
	{}
	Submission(const Submission&) = default;
	Submission(Submission&&) = default;

	const dxle::tstring& get_user_name()const { return user_name; }
	Type_T get_type()const { return type; }
	const dxle::tstring& get_source_name()const { return source_name; }
	const std::vector<Score>& get_scores()const { return scores; }
	const DxLib::DATEDATA& get_submit_time()const { return submit_time; }
};
std::pair<std::array<TCHAR, 10>, dxle::rgb> get_result_type_fordraw(const Score& );
std::pair<std::array<TCHAR, 10>, dxle::rgb> get_result_type_fordraw(const Submission& );

class Problem final
{
private:
	const int max_score = 0;
	const uint32_t sample_num = 0;
	const std::vector<std::pair<int, size_t>> partial_scores;//部分点　first: 得点、　second：どこまでの問題か(<=)(入力ファイルの番号)
	const dxle::tstring name;
	int32_t my_socre = 0;
public:
	struct init_error{};
	//@param path:末尾に\又は/
	Problem(dxle::tstring path, const TCHAR* name)noexcept(false);
	Problem(const Problem&) = default;
	Problem(Problem&&) = default;

	//スレッドセーフ
	const dxle::tstring& GetName()const { return name; }
	//メインスレッドからのみ呼び出し可
	int32_t GetScore()const { return my_socre; }
	//スレッドセーフ
	int GetMaxScore()const { return max_score; }
	//メインスレッドからのみ呼び出し可
	void AddSubmission(const Submission& new_data);
	//メインスレッドからのみ呼び出し可
	void ClearSubmissionCache() { my_socre = 0; }

	//メインスレッドからのみ呼び出し可
	//iの入力が何点相当か調べる
	int32_t GetScore_single(const Submission& )const;

	//メインスレッドからのみ呼び出し可
	void ReloadSubmission();
	//メインスレッドからのみ呼び出し可
	std::vector<Submission> LoadSubmissionAll()const;

	//スレッドセーフ
	int GetSampleNum()const { return sample_num; }
};
struct Script_Data {
	std::vector<std::unique_ptr<Script::Script>> script;
	dxle::sizeui32 size;
};
class Data final : boost::noncopyable
{
private:
	const dxle::tstring user_name;
	const bool is_contest_mode;

	const dxle::tstring log_directory;//InitProblem以外で変更しないこと（それによってスレッドセーフにしている為）
	const dxle::tstring problems_directory;//InitProblem以外で変更しないこと（それによってスレッドセーフにしている為）
	const std::vector<Problem> problems;//InitProblem以外で要素数を変更しないこと（それによってスレッドセーフにしている為）
	std::vector<Script_Data> problems_text;//問題文のキャッシュ

	//問題文の非同期読み込み
	enum class Load_State{file_open, loading, end}load_state;
	static constexpr uint32_t loading_max_num_div2 = 10;//問題のキャッシュをどこまでとるか[view-LMND2,view+LMND2]までとるLMND2=loading_max_num_div2
	int viewing_problem;//最後に読み込みが要請された問題（index）
	int now_loding_problem;//今読み込み中(loading)の問題（index）
	tifstream problem_file;//今読み込み中(loading)の問題（file-stream）
	dxle::pointui32 problem_text_next_start_pos;//今loadingの問題の次に描画を始める座標(X==0&&y==problem_text_newlinw_start_yのとき新しい行)
	dxle::sizeui32 problem_text_total_size;//今loadingの問題の最大サイズ
	//スクリプト
	std::vector<std::unique_ptr<Script::Script>> problem_script_temp;
	dxle::tstring script_raw_temp;//解析中のスクリプト文字列

	void InitBuildProblemText();
	void BuildProblemText();
private:
	Data();
	Data(const Data&) = delete;
	~Data();
public:
	static Data& GetIns(){
		static Data ins; return ins;
	}
	void ReloadSubmission();//提出データの読み込み
	void InitProblem(dxle::tstring problems_directory, dxle::tstring log_directory, dxle::tstring user_name, bool is_contest_mode);//初回呼び出し限定！
	const dxle::tstring& get_user_name()const { return user_name; }//スレッドセーフ
	bool get_is_contest_mode()const { return this->is_contest_mode; }//スレッドセーフ
	void SetBuildProblemText(size_t index);
	void update();
	
	//@return true:finish
	bool IsLoadFinesed(int index)const {
		return !problems_text[index].script.empty();
	}
	void DrawExtendProblem(int index, const dxle::pointi32& pos, double extend_rate)const;
	void DrawProblem(int index, const dxle::pointi32& pos)const {
		DrawExtendProblem(index, pos, 1.0);
	}
	dxle::sizeui32 GetProblemSize(int index)const {
		return problems_text[index].size;
	}
	void ClearProblemsCache();
	//スレッドセーフ
	const dxle::tstring& GetProblemsDirectory()const { return problems_directory; }
	//スレッドセーフ
	const dxle::tstring& GetLogRootDirectory()const { return log_directory; }
	//スレッドセーフ
	const Problem& operator[](size_t i)const{
		return problems[i];
	}
	//スレッドセーフ
	Problem& operator[](size_t i){
		return const_cast<std::vector<Problem>&>(problems)[i];
	}
	//スレッドセーフ
	size_t size()const { return problems.size(); }

	dxle::tstring GetLoadingProblemDir()const {
		return GetProblemsDirectory() + problems[now_loding_problem].GetName() + _T('/');
	}

private:
	//problemsのScoreの更新キャッシュ
	static std::mutex new_scores_mtx;
	static std::vector<std::pair<size_t, Submission>> new_scores;//FIFO (first: pop, last: push)
	void update_ScoresSet();
public:
	static void AddScoresSet_threadsafe(size_t problem_num, Submission new_scores);
};
