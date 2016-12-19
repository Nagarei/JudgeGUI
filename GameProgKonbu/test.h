#pragma once
#include "Data.h"

class test_Local final
{
private:
	size_t problem_num;
	dxle::tstring problem_directory;
	dxle::tstring log_user_directory;
	dxle::tstring cppfile_full_name;
public:
	test_Local(size_t problem_num, dxle::tstring cppfile_full_name_);
	//スコアデータを返す
	Submission test_run();
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
		Submission new_submission;
		test_result_info() {}
		test_result_info(size_t problem_set_num, size_t problem_num, Submission&& new_submission)
			: problem_set_num(problem_set_num), problem_num(problem_num), new_submission(std::move(new_submission))
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
	static void set_test(size_t problem_num, const dxle::tstring& cppfile_full_name);
	//注意：再入耐性なし
	static void process_test_result(std::function<void(test_result_info&&)>& processer);
	//注意：再入耐性なし
	static void process_test_result(const std::function<void(test_result_info&&)>& processer) {
		auto processer_ = processer;
		return process_test_result(processer_);
	}
	//注意：再入耐性なし
	static void process_test_result(std::function<void(test_result_info&&)>&& processer) {
		return process_test_result(processer);
	}
};
