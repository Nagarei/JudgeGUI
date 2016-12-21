#pragma once
#include "Submission.h"

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
	static void set_test(size_t problem_num, const dxle::tstring& cppfile_full_name);
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
#if 0
		auto& data = Data::GetIns();
		if (data.get_problemset_num() != ins.test_results[i].problem_set_num) {
			continue;
		}
		auto& ns = ins.new_submissions[i].new_submission;
		auto& prob_num = ins.new_submissions[i].problem_num;
		auto type_draw = get_result_type_fordraw(ns);
		popup::set(_T("結果が出ました："_ts) + type_draw.first.data(), type_draw.second, dxle::color_tag::black, 3000);
		WaitJudgeQueue::pop(prob_num);
		data[prob_num].AddSubmission(std::move(ns));
#endif
	}
	if (0 < length) {
		std::lock_guard<std::mutex> lock(ins.test_results_mtx);
		ins.test_results.erase(ins.test_results.begin(), ins.test_results.begin() + length);
	}
}
