
#include "NetCommon.h"
#include "MyTchar.h"

#define SERVER_INFO_PATH _T("\\\\SERVER-PC\\Server\\WorkSpace\\競技プログラミング\\contest\\server_info.txt")

void WriteServerInfo(const Server_info & data)
{
	tofstream ofs(SERVER_INFO_PATH);
	if (!ofs) { throw std::runtime_error("faild: save server_info"); }
	ofs << data.port_num << _T('\n')
		<< (int)data.ipdata.d1 << _T(' ') << (int)data.ipdata.d2 << _T(' ')
		<< (int)data.ipdata.d3 << _T(' ') << (int)data.ipdata.d4;
}

//@return first: trueで有効
std::pair<bool, Server_info> ReadServerInfo()
{
	Server_info data;
	tifstream ifs(SERVER_INFO_PATH);
	if (!ifs) { return{ false, data }; }
	ifs >> data.port_num;
	int buf;
	ifs >> buf; data.ipdata.d1 = (uint8_t)buf;
	ifs >> buf; data.ipdata.d2 = (uint8_t)buf;
	ifs >> buf; data.ipdata.d3 = (uint8_t)buf;
	ifs >> buf; data.ipdata.d4 = (uint8_t)buf;
	return{ true, data };
}
