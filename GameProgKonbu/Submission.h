#pragma once

class Score final
{
public:
	uint32_t use_memory;
	uint32_t use_time;
	enum class Type_T : uint8_t { AC, WA, TLE, MLE, RE }type;
};
class Submission_Core final
{
public:
	enum class Type_T : uint8_t { normal, CE, IE, WJ };
	Submission_Core() = default;
	Submission_Core(Type_T type_, std::vector<Score> scores_)
		:type(type_), scores(std::move(scores_))
	{}
	Submission_Core(const Submission_Core&) = default;
	Submission_Core(Submission_Core&&) = default;
	Submission_Core& operator=(const Submission_Core&) = default;
	Submission_Core& operator=(Submission_Core&&) = default;

	Type_T get_type()const { return type; }
	const std::vector<Score>& get_scores()const { return scores; }
private:
	Type_T type;
	std::vector<Score> scores;
};
class Submission final
{
public:
	using Type_T = Submission_Core::Type_T;
private:
	Submission_Core core;
	uint32_t submit_num;
	dxle::tstring user_name;
	DxLib::DATEDATA submit_time;
public:
	Submission() = default;
	Submission(Type_T type_, uint32_t submit_num, std::vector<Score> scores_,
		dxle::tstring user_name_, DxLib::DATEDATA submit_time_)
		:core(type_, std::move(scores_)), submit_num(submit_num)
		user_name(std::move(user_name_)), submit_time(std::move(submit_time_))
	{}
	Submission(const Submission&) = default;
	Submission(Submission&&) = default;
	Submission& operator=(const Submission&) = default;
	Submission& operator=(Submission&&) = default;
	static Submission MakeWJ(time_t);

	const dxle::tstring& get_user_name()const { return user_name; }
	Type_T get_type()const { return core.get_type(); }
	const std::vector<Score>& get_scores()const { return core.get_scores(); }
	const DxLib::DATEDATA& get_submit_time()const { return submit_time; }
};



//結果の解析
//@param log_directory 出力するフォルダの「絶対パス」
//@param input_directory 入力があるフォルダの「絶対パス」
//@param cppfile_full_name 入力cppファイルの「絶対パス」
void RunTest(dxle::tstring log_directory, dxle::tstring input_directory, const dxle::tstring& cppfile_full_name);
//結果の解析
//@param log_directory 出力があるフォルダの「絶対パス」
Submission BuildScores(dxle::tstring log_directory, dxle::tstring user_name);

const TCHAR* get_compile_out_filename()noexcept;
std::array<TCHAR, 20> get_input_name(size_t i)noexcept;
