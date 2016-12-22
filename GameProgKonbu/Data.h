#pragma once
#include "MyTchar.h"
#include "Script.h"
#include "Submission.h"

class Submission_old final
{
public:
	using Type_T = Submission_Core::Type_T;
private:
	Submission_Core core;
	dxle::tstring code_path;
	dxle::tstring user_name;
public:
	Submission_old() = default;
	Submission_old(Submission_Core core, dxle::tstring code_path, dxle::tstring user_name)
		: core(std::move(core)), code_path(std::move(code_path)) , user_name(std::move(user_name))
	{}
	Submission_old(const Submission_old&) = default;
	Submission_old(Submission_old&&) = default;
	Submission_old& operator=(const Submission_old&) = default;
	Submission_old& operator=(Submission_old&&) = default;
	static Submission_old MakeWJ(time_t);

	const dxle::tstring& get_user_name()const { return user_name; }
	const dxle::tstring& get_source_name()const { return code_path; }
	Type_T get_type()const { return core.get_type(); }
	const std::vector<Score>& get_scores()const { return core.get_scores(); }
	const Submission_Core& get_core()const { return core; }
	DxLib::DATEDATA get_submit_time()const;
};

std::pair<std::array<TCHAR, 10>, dxle::rgb> get_result_type_fordraw(const Score& );
std::pair<std::array<TCHAR, 10>, dxle::rgb> get_result_type_fordraw(const Submission_Core& );

class Problem final
{
private:
	int max_score = 0;
	const uint32_t sample_num = 0;
	std::vector<std::pair<int, size_t>> partial_scores;//部分点　first: 得点、　second：どこまでの問題か(<=)(入力ファイルの番号)
	const dxle::tstring name;
	int32_t my_socre = 0;
public:
	//@param path:末尾に\又は/
	Problem(const TCHAR* name)noexcept(false);
	Problem(const Problem&) = default;
	Problem(Problem&&) = default;

	//スレッドセーフ
	const dxle::tstring& GetName()const { return name; }
	//メインスレッドからのみ呼び出し可
	int32_t GetScore()const { return my_socre; }
	//メインスレッドからのみ呼び出し可
	int GetMaxScore()const { return max_score; }
	//メインスレッドからのみ呼び出し可
	void AddSubmission(const Submission_Core& new_data);
	//メインスレッドからのみ呼び出し可
	void ClearSubmissionCache() { my_socre = 0; }

	//メインスレッドからのみ呼び出し可
	//iの入力が何点相当か調べる
	int32_t GetScore_single(const Submission_Core& )const;

	//メインスレッドからのみ呼び出し可
	void ReloadSubmission();
	//メインスレッドからのみ呼び出し可
	void ReloadPartialScores();
	//メインスレッドからのみ呼び出し可
	std::vector<Submission_old> LoadSubmissionAll()const;

	//スレッドセーフ
	int GetSampleNum()const { return sample_num; }
};
struct Script_Data {
	std::vector<std::unique_ptr<Script::Script>> script;
	dxle::sizeui32 size;
};
class Data final
{
private:
	dxle::tstring user_name;
	bool is_contest_mode;
	size_t problem_set_num = 0;

	dxle::tstring log_directory;
	dxle::tstring problems_directory;
	std::vector<Problem> problems;
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
	Data& operator=(const Data&) = delete;
	~Data();
public:
	static Data& GetIns(){
		static Data ins; return ins;
	}
	void ReloadSubmission();//提出データの読み込み
	void DeleteProblem();
	void InitProblem(dxle::tstring problems_directory, dxle::tstring log_directory, dxle::tstring user_name, bool is_contest_mode);
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
	const dxle::tstring& GetProblemsDirectory()const { return problems_directory; }
	const dxle::tstring& GetLogRootDirectory()const { return log_directory; }
	const Problem& operator[](size_t i)const{
		return problems[i];
	}
	Problem& operator[](size_t i){
		return const_cast<std::vector<Problem>&>(problems)[i];
	}
	size_t size()const { return problems.size(); }
	size_t get_problemset_num()const { return problem_set_num; }

	dxle::tstring GetLoadingProblemDir()const {
		return GetProblemsDirectory() + problems[now_loding_problem].GetName() + _T('/');
	}
};
