#include "test.h"
#include "Data.h"
#include "GetNumfileNum.h"
#include "Unsafe_My_func.h"
#include "SubmissionLog.h"

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
	test_thread = std::thread([this](){ Loop(); });
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
			test->test_run();
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

test_Local::test_Local(size_t problem_num_, dxle::tstring cppfile_full_name_)
	: problem_num(problem_num_)
	, cppfile_full_name(cppfile_full_name_)
{
}

void test_Local::test_run()
{
	//ログフォルダの確保
	dxle::tstring problem_directory = Data::GetIns().GetProblemsDirectory() + Data::GetIns()[problem_num].GetName() + _T('/');
	//CreateDirectory(problem_directory.c_str(), NULL);
	dxle::tstring log_user_directory = Data::GetIns().GetLogRootDirectory() + Data::GetIns()[problem_num].GetName() + _T('/')
											+ Data::GetIns().get_user_name() + _T('/');
	CreateDirectory(log_user_directory.c_str(), NULL);
	dxle::tstring log_directory = SerchFolderNum(log_user_directory);
	CreateDirectory(log_directory.c_str(), NULL);

	//ジャッジ
	RunTest(log_directory, problem_directory, cppfile_full_name);

	//結果の解析
	//スコアデータをセット
	Data::AddScoresSet_threadsafe(problem_num, BuildScores(log_directory, Data::GetIns().get_user_name()));

	return;
}
