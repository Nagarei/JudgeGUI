
#include "Submission.h"
#include "GetNumfileNum.h"
#include "my_utility.h"
#include "MyTchar.h"

#define LOG_SOURCE_NAME _T("source.cpp")
#define LOG_COMPILE_NAME _T("compile.txt")
#define LOG_TEMPBAT_NAME _T("__temp.bat")
#define LOG_RESULT_NAME _T("result.txt")
#define TEST_INPUT_NAME_PRE _T("input")
#define TEST_INPUT_NAME_POST _T(".txt")


const TCHAR* get_compile_out_filename()noexcept {
	return LOG_COMPILE_NAME;
}

std::array<TCHAR, 20> get_input_name(uint32_t i) noexcept
{
	std::array<TCHAR, 20> str; 
	auto strcpy2 = [](TCHAR* buf, const TCHAR* str) {
		while (*str != '\0') { *buf++ = *str++; }
		return buf;
	};
	auto iter = str.data();
	iter = strcpy2(iter, TEST_INPUT_NAME_PRE);
	my_itoa(i, iter);
	while (*iter != '\0') { ++iter; }
	*strcpy2(iter, TEST_INPUT_NAME_POST) = '\0';
	return str;
}


void RunTest(dxle::tstring log_directory, dxle::tstring input_directory, const dxle::tstring& cppfile_full_name)
{
	//引数調整
	if (log_directory.empty()) {
		log_directory = _T("./");
	}
	if (log_directory.back() != _T('\\') && log_directory.back() != _T('/')) {
		log_directory.push_back(_T('/'));
	}

	if (input_directory.empty()) {
		input_directory = _T("./");
	}
	if (input_directory.back() == _T('\\')) {
		input_directory.pop_back();//"～\"とやると、"の閉じがエスケープされて、次の引数を巻き込んでしまう
	}
	if (input_directory.back() != _T('\\') && input_directory.back() != _T('/')) {
		input_directory.push_back(_T('/'));
	}

	//解答の書き出し
	CopyFile(cppfile_full_name.c_str(), (log_directory + LOG_SOURCE_NAME).c_str(), FALSE);

	//パラメータの設定
	dxle::tstring param;
	param += _T(" /c-out \"") + log_directory + LOG_COMPILE_NAME + _T("\"");
	param += _T(" /problem \"") + input_directory + _T("\"");
	param += _T(" /source \"") + log_directory + LOG_SOURCE_NAME + _T("\"");
	param += _T(" /tempoutdir \"") + log_directory + _T("\"");
	{
		tifstream ifs(input_directory + _T("overview.txt"));
		if (ifs)
		{
			dxle::tstring c_option;
			uint32_t time_limit, memory_limit;

			std::getline(ifs, c_option);
			ifs >> time_limit >> memory_limit;

			if (!c_option.empty()) {
				param += _T(" /c-option \"") + c_option + _T("\"");
			}
			TCHAR itoa_buf[20];
			param += _T(" /t-limit "_ts) + my_itoa(time_limit, itoa_buf);
			param += _T(" /m-limit "_ts) + my_itoa(memory_limit, itoa_buf);
		}
	}
	param += _T(" /stdout \"") + log_directory + LOG_RESULT_NAME + _T('\"');
	//起動
	{
		PROCESS_INFORMATION pi;
		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		auto temp_param = std::make_unique<TCHAR[]>(param.size()+1);
		StrCpy(temp_param.get(), param.c_str());
		if (
			CreateProcess(_T("Judge++/Judge++.exe"), temp_param.get(),
				NULL, NULL, FALSE, CREATE_NO_WINDOW,
				NULL, _T("Judge++"), &si, &pi)
			) {
			CloseHandle(pi.hThread);
			WaitForSingleObject(pi.hProcess, INFINITE);
			CloseHandle(pi.hProcess);
		}
	}
}
//結果の解析
Submission BuildScores(dxle::tstring log_directory, dxle::tstring user_name)
{
#if 0
	//コンパイルメッセージの取得
	dxle::tstring compile_message;
	{
		tifstream ifs(log_directory + LOG_COMPILE_NAME);
		if (!ifs.fail()) {
			dxle::tstring buf;
			std::getline(ifs, buf);
			while (ifs)
			{
				compile_message += buf;
				compile_message += '\n';
				std::getline(ifs, buf);
			}
		}
	}
#endif
	//提出時間取得
	DATEDATA sbumit_time;
	sbumit_time.Year = sbumit_time.Mon = sbumit_time.Day = sbumit_time.Hour = sbumit_time.Min = sbumit_time.Sec = 0;
	{
		auto file_handle = FileRead_createInfo((log_directory + LOG_RESULT_NAME).c_str());
		if (file_handle != -1) {
			//読み込み成功
			FINALLY([&file_handle]() { DxLib::FileRead_deleteInfo(file_handle); });
			//ワイルドカード使ってないので一つしかないはず
			FILEINFO Buffer;
			Buffer.CreationTime.Year = Buffer.CreationTime.Mon = Buffer.CreationTime.Day
				= Buffer.CreationTime.Hour = Buffer.CreationTime.Min = Buffer.CreationTime.Sec = 0;
			DxLib::FileRead_getInfo(0, &Buffer, file_handle);
			sbumit_time = Buffer.CreationTime;
		}
	}

	//結果の解析
	tifstream ifs(log_directory + LOG_RESULT_NAME);
	if (ifs.fail()) {
		//IE
		return Submission(Submission::Type_T::IE, log_directory + LOG_SOURCE_NAME
			, std::vector<Score>{}, std::move(user_name), std::move(sbumit_time));
	}
	dxle::tstring buf;
	//CEチェック
	std::getline(ifs, buf);
	if (buf == _T("CE")) {
		//CE
		return Submission(Submission::Type_T::CE, log_directory + LOG_SOURCE_NAME
			, std::vector<Score>{}, std::move(user_name), std::move(sbumit_time));
	}
	//結果を取得
	uint32_t counter = 1;
	std::vector<Score> score_temp;
	TCHAR itoa_buf[20];
	auto get_score = [&](std::basic_istream<TCHAR>& is) {//@return true:error
		score_temp.emplace_back();
		my_strcpy(score_temp.back().input_name, _T("input"_ts) + my_itoa(counter, itoa_buf) + _T(".txt"));
		buf.resize(0);
		is >> score_temp.back().use_memory >> score_temp.back().use_time >> buf; is.ignore(-1, _T('\n'));
		if (is.fail() && (buf.empty())) {
			//読み込み終わった
			score_temp.pop_back();
			return false;
		}
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
	if(ifs || !buf.empty()){//初めの一行だけもう読み込んでしまったので別処理
		std::basic_stringstream<TCHAR> ss(buf);
		if (get_score(ss)) {
			return Submission(Submission::Type_T::IE, log_directory + LOG_SOURCE_NAME
				, std::move(score_temp), std::move(user_name), std::move(sbumit_time));
		}
	}
	while (ifs) {
		if (get_score(ifs)) {
			return Submission(Submission::Type_T::IE, log_directory + LOG_SOURCE_NAME
				, std::move(score_temp), std::move(user_name), std::move(sbumit_time));
		}
	}

	return Submission(Submission::Type_T::normal, log_directory + LOG_SOURCE_NAME
		, std::move(score_temp), std::move(user_name), std::move(sbumit_time));
}
