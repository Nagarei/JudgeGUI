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
	enum class Type : uint8_t { init_VERSION, init_V1, Submit, test_result };
	struct init_VERSION
	{
		//後方互換が致命的に崩れるので、絶対に変更しないこと
		uint32_t version;
	};
	struct Submit_head
	{
		uint32_t problem_num;
		TCHAR user_name[username_length];
		uint32_t length;//NULL終端文字を除く
	};
}

constexpr auto CONTEST_PROBLEM_DIR = _T("\\\\SERVER-P\\Server\\WorkSpace\\競技プログラミング\\contest\\");
