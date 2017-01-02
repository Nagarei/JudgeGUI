#pragma once
#include "Sequence.h"
#include "NetCommon.h"

class NetInit final : public Sequence
{
private:
	int port_input_handle;
	Server_info server_info;
	bool is_error;
public:
	NetInit();
	~NetInit();
	std::unique_ptr<Sequence> update()override;
	void draw()const override;
};
class NetMain final : public Sequence
{
private:
	Server_info server_info;
	std::unordered_map<int, std::array<TCHAR, username_length>> user_names;
	struct connection_info {
		int handle;
		enum class Type{wait_version, wait_initdata, wait_submit}type;
	};
	std::vector<connection_info> connections;//接続済み
private:
	//@return ファイル名
	dxle::tstring WriteTempFile(const TCHAR* str_raw, uint32_t length);
	//データの受け取りに失敗した際のコールバック
	void set_fail_receve(const TCHAR* info);
public:
	NetMain(Server_info server_info);
	~NetMain();
	std::unique_ptr<Sequence> update()override;
	void draw()const override;
};
