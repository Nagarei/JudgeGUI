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
	Submission_Core(Type_T type_, std::vector<Score> scores_, time_t submit_time_)
		:type(type_), scores(std::move(scores_)), submit_time(submit_time_)
	{}
	Submission_Core(const Submission_Core&) = default;
	Submission_Core(Submission_Core&&) = default;
	Submission_Core& operator=(const Submission_Core&) = default;
	Submission_Core& operator=(Submission_Core&&) = default;

	Type_T get_type()const { return type; }
	const std::vector<Score>& get_scores()const { return scores; }
	time_t get_submit_time()const { return submit_time; }
private:
	Type_T type;
	std::vector<Score> scores;
	time_t submit_time;
};



//結果の解析
//@param log_directory 出力するフォルダの「絶対パス」
//@param input_directory 入力があるフォルダの「絶対パス」
//@param cppfile_full_name 入力cppファイルの「絶対パス」
void RunTest(dxle::tstring log_directory, dxle::tstring input_directory, const dxle::tstring& cppfile_full_name);
//結果の解析
//@param log_directory 出力があるフォルダの「絶対パス」
Submission_Core BuildScores(dxle::tstring log_directory);

const TCHAR* get_compile_out_filename()noexcept;
const TCHAR* get_source_filename()noexcept;
std::array<TCHAR, 20> get_input_name(uint32_t i)noexcept;
