#pragma once

class test_Local final
{
private:
	size_t problem_num;
	dxle::tstring cppfile_full_name;
public:
	test_Local(size_t problem_num, dxle::tstring cppfile_full_name_);
	void test_run();
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
	compile_taskmanager();
	void Loop();
	void SetEnd() { if (test_thread.joinable()) { is_end = true; test_thread.join(); } }
	~compile_taskmanager();
	static compile_taskmanager& GetIns() {
		static compile_taskmanager ins;
		return ins;
	}
public:
	static void set_test(size_t problem_num, const dxle::tstring& cppfile_full_name) {
		auto& ins = GetIns();
		std::lock_guard<std::mutex> lock(ins.test_queue_mtx);
		ins.test_queue.emplace_back(std::make_unique<test_class>(problem_num, cppfile_full_name));
	}
};
