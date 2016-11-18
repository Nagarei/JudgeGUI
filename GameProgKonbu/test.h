#pragma once
#include "Data.h"

class test_Local final
{
private:
	size_t problem_num;
	dxle::tstring cppfile_full_name;
public:
	test_Local(size_t problem_num, dxle::tstring cppfile_full_name_);
	//スコアデータを返す
	std::pair<size_t, Submission> test_run();
};
using test_class = test_Local;

class compile_taskmanager final : boost::noncopyable
{
private:
	std::atomic_bool is_end;
	std::thread test_thread;
	std::mutex test_queue_mtx;
	std::deque<std::unique_ptr<test_class>> test_queue;//front:pop back:push
private:
	//Submissionの更新キャッシュ
	std::mutex new_submissions_mtx;
	std::vector<std::pair<size_t, Submission>> new_submissions;//FIFO (first: pop, last: push)

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
	static void update();
};
