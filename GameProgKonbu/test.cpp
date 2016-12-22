﻿#include "test.h"
#include "Submission.h"
#include "GetNumfileNum.h"
#include "Unsafe_My_func.h"

namespace
{
	dxle::tstring SerchFolderNum(dxle::tstring serch_path)
	{
		TCHAR buf[20];
		return serch_path + my_itoa(get_numdirectry_num(serch_path, _T(""), 0) + 1, buf) + _T('\\');
	}
}

compile_taskmanager::compile_taskmanager()
	:is_end(false)
{
	test_thread = std::thread([this]() { Loop(); });
}

void compile_taskmanager::Loop()
{
	//サブスレッドで動作

	while (!is_end)
	{
		test_info test;
		//テストバッファを確認
		{
			std::lock_guard<std::mutex> lock(test_queue_mtx);
			if (!test_queue.empty())
			{
				test = std::move(test_queue.front());
				test_queue.pop_front();
			}
		}
		//実行or待機
		if (test.second) {
			auto&& new_submission = test.second->test_run();
			std::lock_guard<std::mutex> lock(test_results_mtx);
			test_results.emplace_back(test.first, test.second->get_problem_num(), std::move(new_submission));
		}
		else {
			std::this_thread::sleep_for(std::chrono::milliseconds(17));
		}
	}
}

compile_taskmanager::~compile_taskmanager()
{
	SetEnd();
}

void compile_taskmanager::set_test(size_t problem_num, const dxle::tstring & cppfile_full_name)
{
	auto& ins = GetIns();
	WaitJudgeQueue::push(problem_num);
	std::lock_guard<std::mutex> lock(ins.test_queue_mtx);
	ins.test_queue.emplace_back(Data::GetIns().get_problemset_num(), std::make_unique<test_class>(problem_num, cppfile_full_name));
}


//------------------------------------test_Local-------------------------------------------//

#define LOG_TEMP_FOLDER_NAME _T("__TEMP/")


test_Local::test_Local(size_t problem_num_, dxle::tstring cppfile_full_name_)
	: problem_num(problem_num_)
	, cppfile_full_name(cppfile_full_name_)
{
	const auto& data = Data::GetIns();
	const auto& problem_name = data[problem_num].GetName();
	
	problem_directory = data.GetProblemsDirectory() + problem_name + _T('/');
	//CreateDirectory(problem_directory.c_str(), NULL);
	
	auto log_problem_directory = data.GetLogRootDirectory() + problem_name + _T('/');
	CreateDirectory(log_problem_directory.c_str(), NULL);
	log_user_directory = std::move(log_problem_directory) + data.get_user_name() + _T('/');
	CreateDirectory(log_user_directory.c_str(), NULL);
}

Submission_Core test_Local::test_run()
{
	//サブスレッドで動作

	//ログフォルダの確保
	dxle::tstring log_directory = log_user_directory + LOG_TEMP_FOLDER_NAME;
	CreateDirectory(log_directory.c_str(), NULL);

	//ジャッジ
	RunTest(log_directory, problem_directory, cppfile_full_name);

	//ナンバリングされたログフォルダに移動
	{
		auto true_log_directory = SerchFolderNum(log_user_directory);
		MoveFile(log_directory.c_str(), true_log_directory.c_str());
		log_directory = std::move(true_log_directory);
	}

	//結果の解析
	//スコアデータを返す
	return BuildScores(log_directory);
}
