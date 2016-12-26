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
	std::vector<int> NewHandles;//新規接続
	std::vector<int> NetHandles;//接続済み
private:
	//@return ファイル名
	dxle::tstring WriteTempFile(const TCHAR* str_raw, uint32_t length);
public:
	NetMain(Server_info server_info);
	~NetMain();
	std::unique_ptr<Sequence> update()override;
	void draw()const override;
};
