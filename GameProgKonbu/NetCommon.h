#pragma once

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
	enum class Type : uint8_t { init_VERSION, init_V1, Submit };
	struct init_VERSION
	{
		//後方互換が致命的に崩れるので、絶対に変更しないこと
		uint32_t version;
	};
	struct Submit_head
	{
		size_t problem_num;
		TCHAR user_name[username_length];
		uint32_t length;//NULL終端文字を除く
	};
}

constexpr auto CONTEST_PROBLEM_DIR = _T("\\\\SERVER-PC\Server\\WorkSpace\\競技プログラミング\\contest\\");
