#pragma once
#include "Submission.h"

struct problem_dir_set {
	size_t problem_num;
	const dxle::tstring& problem_root_dir;
	const dxle::tstring& log_root_dir;
	const dxle::tstring& problem_name;
};
class test_Local final
{
private:
	size_t problem_num;
	dxle::tstring problem_directory;
	dxle::tstring log_user_directory;
	dxle::tstring cppfile_full_name;
public:
	test_Local(const problem_dir_set& prob_dir, const dxle::tstring& user_name, dxle::tstring cppfile_full_name_);
	//スコアデータを返す
	Submission_Core test_run();
	size_t get_problem_num()const noexcept { return problem_num; }
};
using test_class = test_Local;

class compile_taskmanager final
{
private:
	compile_taskmanager(const compile_taskmanager&) = delete;
	compile_taskmanager& operator=(const compile_taskmanager&) = delete;

//スレッド
	std::atomic_bool is_end;
	std::thread test_thread;

//テスト実行前情報
	using test_info = std::pair<size_t, std::unique_ptr<test_class>>;//first:問題セット番号 second:テスト実行クラス
	std::mutex test_queue_mtx;
	std::deque<test_info> test_queue;//front:pop back:push

//テスト実行結果情報
public:
	struct test_result_info {
		size_t problem_set_num;
		size_t problem_num;
		Submission_Core submission_result;
		test_result_info() {}
		test_result_info(size_t problem_set_num, size_t problem_num, Submission_Core&& submission_result)
			: problem_set_num(problem_set_num), problem_num(problem_num), submission_result(std::move(submission_result))
		{}
	};
private:
	std::mutex test_results_mtx;
	std::deque<test_result_info> test_results;//FIFO (first: pop, last: push)

private:
	compile_taskmanager();
	void Loop();
	void SetEnd() { if (test_thread.joinable()) { is_end = true; test_thread.join(); } }
	~compile_taskmanager();
	static compile_taskmanager& GetIns() {
		static compile_taskmanager ins;
		return ins;
	}
public:
	//テスト実行スレッド起動(明示的に起動したいときのみ使用)
	static void start(){ GetIns(); }

	static void set_test(size_t problemset_num, std::unique_ptr<test_class> tester);
	template<typename FUNC>
	//注意：再入耐性なし
	//@param processer: void(test_result_info&&)
	static void process_test_result(FUNC&& processer);
};
template<typename FUNC>
void compile_taskmanager::process_test_result(FUNC&& processer)
{
	auto& ins = GetIns();
	size_t length = 0;
	{
		std::lock_guard<std::mutex> lock(ins.test_results_mtx);
		length = ins.test_results.size();
	}
	for (size_t i = 0; i < length; ++i)
	{
		decltype(ins.test_results)::value_type res;
		{
			std::lock_guard<std::mutex> lock(ins.test_results_mtx);
			res = std::move(ins.test_results[i]);
		}
		processer(std::move(res));
	}
	if (0 < length) {
		std::lock_guard<std::mutex> lock(ins.test_results_mtx);
		ins.test_results.erase(ins.test_results.begin(), ins.test_results.begin() + length);
	}
}
