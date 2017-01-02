#pragma once
#include "Submission.h"

constexpr unsigned username_length = 20;

struct Server_info {
	int port_num;
	DxLib::IPDATA ipdata;
};
void WriteServerInfo(const Server_info& data);
//@return first: trueで有効
std::pair<bool, Server_info> ReadServerInfo();

namespace Send_Data
{
	constexpr uint32_t net_version = 1;
	enum class Type : uint8_t { init_VERSION, init_V1, init_submissionlog, Submit, test_result, receve_error };
	struct init_VERSION
	{
		//後方互換が致命的に崩れるので、絶対に変更しないこと
		uint32_t version;
	};
	struct init_V1
	{
		TCHAR user_name[username_length];
	};
	struct Submit_head
	{
		uint32_t problem_num;
		uint32_t source_length;//NULL終端文字を除く
	};
	struct test_result
	{
		uint32_t problem_num;
		uint32_t source_length;//NULL終端文字を除く
	};
	struct receve_error {};
}

constexpr auto CONTEST_PROBLEM_DIR = _T("\\\\SERVER-P\\Server\\WorkSpace\\競技プログラミング\\contest\\");
