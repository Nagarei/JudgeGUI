#include "test.h"
#include "Data.h"
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#define LOG_SOURCE_NAME _T("source.cpp")
#define LOG_COMPILE_NAME _T("compile.txt")
#define LOG_RESULT_NAME _T("result.txt")

namespace
{
	TCHAR* my_itoa(uint32_t n, TCHAR buf[]) {
		auto iter = buf;
		//uint32_t::max 4294967295
		uint32_t div = 1000000000;
		for (; 0 < div; div /= 10) {
			auto num = (n / div) % 10;
			if (iter != buf || num != 0) {
				*iter++ = static_cast<TCHAR>(num + _T('0'));
			}
		}
		*iter = '\0';
		return buf;
	};
	template<size_t N>
	void my_strcpy(TCHAR (&buf)[N], const dxle::tstring str) {
		auto iter = std::begin(buf), iter_end = std::end(buf);
		auto str_iter = std::begin(str), str_iter_end = std::end(str);
		while (str_iter != str_iter_end && iter != iter_end) {
			*iter++ = *str_iter++;
		}
	};
	dxle::tstring SerchFolderNum(dxle::tstring serch_path)
	{
		uint32_t i = 0;
		dxle::tstring result;
		TCHAR buf[20];
		for (; i < std::numeric_limits<uint32_t>::max(); ++i)
		{
			if (PathIsDirectory((result = serch_path + my_itoa(i, buf) + _T('\\')).c_str()) == FALSE) {
				break;
			}
		}
		return result;
	}
	void RunTest(dxle::tstring log_directory, dxle::tstring input_directory)
	{
		if (!log_directory.empty() && log_directory.back() != '\\' && log_directory.back() != '/') {
			log_directory.push_back('/');
		}
		if (2 <= log_directory.size() && log_directory[1] != _T(':')) {
			//"judge++"����̑��΃p�X��
			log_directory.insert(0, _T("../"));
		}
		if (!input_directory.empty() && input_directory.back() != '\\' && input_directory.back() != '/') {
			input_directory.push_back('/');
		}
		if (2 <= input_directory.size() && input_directory[1] != _T(':')) {
			//"judge++"����̑��΃p�X��
			input_directory.insert(0, _T("../"));
		}

		//�p�����[�^�̐ݒ�
		dxle::tstring param;
		param += _T(" /c-out ") + log_directory + LOG_COMPILE_NAME;
		param += _T(" /problem ") + input_directory;
		param += _T(" /source ") + log_directory + LOG_SOURCE_NAME;
		{
			tifstream ifs(input_directory + _T("overview.txt"));
			dxle::tstring c_option;
			std::getline(ifs, c_option);
			uint32_t time_limit, memory_limit;
			ifs >> time_limit >> memory_limit;
			param += _T(" /c-option \"c_option\"");
			TCHAR itoa_buf[20];
			param += _T(" /t-limit "_ts) + my_itoa(time_limit, itoa_buf);
			param += _T(" /m-limit "_ts) + my_itoa(memory_limit, itoa_buf);
		}
		param += _T('>') + log_directory + LOG_RESULT_NAME;
		//�N��
		ShellExecute(NULL, NULL, _T("Judge++\\Judge++.exe"), param.c_str(), _T("Judge++"), SW_HIDE);
	}
	//���ʂ̉��
	Scores BuildScores(dxle::tstring log_directory)
	{
		//�R���p�C�����b�Z�[�W�̎擾
		dxle::tstring compile_message;
		{
			tifstream ifs(log_directory + LOG_COMPILE_NAME);
			if (!ifs.fail()) {
				dxle::tstring buf;
				while (!ifs.eof())
				{
					std::getline(ifs, buf);
					compile_message += buf;
					compile_message += '\n';
				}
			}
		}
		//���ʂ̉��
		tifstream ifs(log_directory + LOG_RESULT_NAME);
		if (ifs.fail()) {
			//IE
			return Scores(Scores::Type_T::IE, log_directory + LOG_SOURCE_NAME
				, std::vector<Score>{}, std::move(compile_message));
		}
		dxle::tstring buf;
		//CE�`�F�b�N
		std::getline(ifs, buf);
		if (buf == _T("CE")) {
			//CE
			return Scores(Scores::Type_T::CE, log_directory + LOG_SOURCE_NAME
				, std::vector<Score>{}, std::move(compile_message));
		}
		//���ʂ��擾
		uint32_t counter = 1;
		std::vector<Score> score_temp;
		TCHAR itoa_buf[20];
		auto get_score = [&](std::basic_istream<TCHAR>& is) {//@return true:error
			score_temp.emplace_back();
			my_strcpy(score_temp.back().input_name, _T("input"_ts) + my_itoa(counter, itoa_buf) + _T(".txt"));
			is >> score_temp.back().use_memory >> score_temp.back().use_time >> buf; is.ignore(-1, _T('\n'));
			if (buf == _T("AC")) { score_temp.back().type = Score::Type_T::AC; }
			else if (buf == _T("WA")) { score_temp.back().type = Score::Type_T::WA; }
			else if (buf == _T("TLE")) { score_temp.back().type = Score::Type_T::TLE; }
			else if (buf == _T("MLE")) { score_temp.back().type = Score::Type_T::MLE; }
			else if (buf == _T("RE")) { score_temp.back().type = Score::Type_T::RE; }
			else {
				score_temp.pop_back();
				//IE
				return true;
			}
			++counter;
			return false;
		};
		{//���߂̈�s���������ǂݍ���ł��܂����̂ŕʏ���
			std::basic_stringstream<TCHAR> ss(buf);
			if (get_score(ss)) {
				return Scores(Scores::Type_T::IE, log_directory + LOG_SOURCE_NAME
					, std::move(score_temp), std::move(compile_message));
			}
		}
		while (!ifs.eof()) {
			if (get_score(ifs)) {
				return Scores(Scores::Type_T::IE, log_directory + LOG_SOURCE_NAME
					, std::move(score_temp), std::move(compile_message));
			}
		}

		return Scores(Scores::Type_T::normal, log_directory + LOG_SOURCE_NAME
			, std::move(score_temp), std::move(compile_message));
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
		//�e�X�g�o�b�t�@���m�F
		{
			std::lock_guard<std::mutex> lock(test_queue_mtx);
			if (!test_queue.empty())
			{
				test = std::move(test_queue.front());
				test_queue.pop_front();
			}
		}
		//���sor�ҋ@
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
	//���O�t�H���_�̊m��
	dxle::tstring problem_irectory = Data::GetIns().GetProblemsDirectory() + Data::GetIns()[problem_num].GetName() + _T('/');
	dxle::tstring log_directory = SerchFolderNum(problem_irectory);

	//�𓚂̏����o��
	CopyFile(cppfile_full_name.c_str(), (log_directory + LOG_SOURCE_NAME).c_str(), FALSE);

	//�W���b�W
	RunTest(log_directory, problem_irectory);

	//���ʂ̉��
	//�X�R�A�f�[�^���Z�b�g
	Data::AddScoresSet_threadsafe(problem_num, BuildScores(log_directory));

	return;
}
