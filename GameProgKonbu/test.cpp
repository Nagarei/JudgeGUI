﻿#include "test.h"
#include "Data.h"
#include "GetNumfileNum.h"
#include "Unsafe_My_func.h"
#include "SubmissionLog.h"
#include "WaitJudge.h"
#include "popup.h"

//メインスレッドでカレントディレクトリが変更される為
//相対パス使用禁止！！！

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
	while (!is_end)
	{
		std::unique_ptr<test_class> test;
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
		if (test) {
			auto&& new_submission = test->test_run();
			std::lock_guard<std::mutex> lock(new_submissions_mtx);
			new_submissions.emplace_back(std::move(new_submission));
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
	ins.test_queue.emplace_back(std::make_unique<test_class>(problem_num, cppfile_full_name));
}

void compile_taskmanager::update()
{
	auto& ins = GetIns();
	size_t length = 0;
	{
		std::lock_guard<std::mutex> lock(ins.new_submissions_mtx);
		length = ins.new_submissions.size();
	}
	for (size_t i = 0; i < length; ++i)
	{
		std::lock_guard<std::mutex> lock(ins.new_submissions_mtx);

		auto& ns = ins.new_submissions[i];
		auto type_draw = get_result_type_fordraw(ns.second);
		popup::set(_T("結果が出ました："_ts) + type_draw.first.data(), type_draw.second, dxle::color_tag::black, 3000);
		WaitJudgeQueue::pop(ns.first);
		Data::GetIns()[ns.first].AddSubmission(std::move(ns.second));
	}
	if(0 < length){
		std::lock_guard<std::mutex> lock(ins.new_submissions_mtx);
		ins.new_submissions.erase(ins.new_submissions.begin(), ins.new_submissions.begin() + length);
	}
}






//------------------------------------test_Local-------------------------------------------//

#define LOG_TEMP_FOLDER_NAME _T("__TEMP/")


test_Local::test_Local(size_t problem_num_, dxle::tstring cppfile_full_name_)
	: problem_num(problem_num_)
	, cppfile_full_name(cppfile_full_name_)
{
}

std::pair<size_t, Submission> test_Local::test_run()
{
	//ログフォルダの確保
	dxle::tstring problem_directory = Data::GetIns().GetProblemsDirectory() + Data::GetIns()[problem_num].GetName() + _T('/');
	//CreateDirectory(problem_directory.c_str(), NULL);
	dxle::tstring log_directory;
	dxle::tstring log_user_directory;
	{
		dxle::tstring log_problem_directory = Data::GetIns().GetLogRootDirectory() + Data::GetIns()[problem_num].GetName() + _T('/');
		CreateDirectory(log_problem_directory.c_str(), NULL);
		log_user_directory = std::move(log_problem_directory) + Data::GetIns().get_user_name() + _T('/');
		CreateDirectory(log_user_directory.c_str(), NULL);
		log_directory = log_user_directory + LOG_TEMP_FOLDER_NAME;
		CreateDirectory(log_directory.c_str(), NULL);
	}

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
	return{ problem_num , BuildScores(log_directory, Data::GetIns().get_user_name()) };
}
