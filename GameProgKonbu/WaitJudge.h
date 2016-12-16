#pragma once

class WaitJudgeQueue
{
private:
	//backに入れて、frontから出す
	std::unordered_map<int, std::deque<time_t>> wait_judge_list;//problem_num, submission_time
	WaitJudgeQueue() {}
	~WaitJudgeQueue(){}
public:
	static WaitJudgeQueue& GetIns() {
		static WaitJudgeQueue ins; return ins;
	}
	static void clear_all() {
		GetIns().wait_judge_list.clear();
	}
	static void push(size_t problem_num) {
		GetIns().wait_judge_list[problem_num].push_back(time(nullptr));
	}
	static void pop(size_t problem_num) {
		GetIns().wait_judge_list[problem_num].pop_front();
	}
	static const auto& waiting_list(size_t problem_num) {
		return GetIns().wait_judge_list[problem_num];
	}
};